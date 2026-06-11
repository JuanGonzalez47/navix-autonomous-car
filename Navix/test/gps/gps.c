#include "gps.h"
#include "hardware/uart.h"
#include "hardware/irq.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

// Buffers volátiles para la interrupción
volatile char raw_buffer[GPS_BUFFER_SIZE];
volatile uint16_t buffer_idx = 0;
volatile bool linea_lista = false;

volatile DatosGPS g_datos_gps = {0.0, 0.0, false, false};

// --- 1. MANEJADOR DE INTERRUPCIÓN (Segundo plano absoluto) ---
void on_uart_gps_rx() {
    while (uart_is_readable(UART_GPS)) {
        char c = uart_getc(UART_GPS);
        
        if (!linea_lista) {
            if (c == '\n' || buffer_idx >= (GPS_BUFFER_SIZE - 1)) {
                raw_buffer[buffer_idx] = '\0'; // Cierre de cadena
                linea_lista = true;            // Bloqueamos buffer hasta que Core1 lo lea
            } else if (c != '\r') {            // Ignoramos el retorno de carro
                raw_buffer[buffer_idx++] = c;
            }
        }
    }
}

// --- 2. INICIALIZACIÓN ---
void gps_init(void) {
    uart_init(UART_GPS, BAUD_GPS);
    gpio_set_function(PIN_GPS_TX, GPIO_FUNC_UART);
    gpio_set_function(PIN_GPS_RX, GPIO_FUNC_UART);

    // Desactivar buffers FIFO de hardware para procesar byte por byte en la ISR
    uart_set_fifo_enabled(UART_GPS, false);

    // Configurar y habilitar la interrupción por RX
    int UART_IRQ = (UART_GPS == uart0) ? UART0_IRQ : UART1_IRQ;
    irq_set_exclusive_handler(UART_IRQ, on_uart_gps_rx);
    irq_set_enabled(UART_IRQ, true);
    uart_set_irq_enables(UART_GPS, true, false); // Habilitar RX, deshabilitar TX
}

// --- 3. POLLING DE PROCESAMIENTO (Se ejecuta en Core 1) ---
// Parsea sentencias simplificadas $GPRMC o $GPGGA.
bool gps_procesar_buffer(void) {
    if (!linea_lista) return false; // No hay datos nuevos que procesar

    char local_str[GPS_BUFFER_SIZE];
    
    // Desactivar interrupción un instante para copiar el buffer de forma segura
    uart_set_irq_enables(UART_GPS, false, false);
    strncpy(local_str, (char*)raw_buffer, GPS_BUFFER_SIZE);
    buffer_idx = 0;
    linea_lista = false;
    uart_set_irq_enables(UART_GPS, true, false);

    // Buscamos la sentencia recomendada mínima ($GPRMC o $GNRMC dependiendo de la constelación)
    if (strstr(local_str, "RMC") != NULL) {
        // Ejemplo simplificado de parseo por comas utilizando strtok
        // Formato estándar: $GPRMC,hhmmss.ss,A,llll.ll,a,yyyyy.yy,a,...
        // Donde 'A' es Alerta/Activo (Válido) y 'V' es Vacío (Inválido)
        
        char *token = strtok(local_str, ",");
        int campo = 0;
        char status = 'V';
        char *lat_str = NULL;
        char *lat_dir = NULL;
        char *lon_str = NULL;
        char *lon_dir = NULL;

        while (token != NULL) {
            campo++;
            if (campo == 2) status = token[0]; // Campo de estatus: A=Válido, V=Inválido
            if (campo == 4) lat_str = token;   // Latitud en formato DDMM.MMMM
            if (campo == 5) lat_dir = token;   // N o S
            if (campo == 6) lon_str = token;   // Longitud en formato DDDMM.MMMM
            if (campo == 7) lon_dir = token;   // E o W
            token = strtok(NULL, ",");
        }

        if (status == 'A' && lat_str && lon_str) {
            // Conversión rápida de formato NMEA (Grados y Minutos) a Grados Decimales
            double raw_lat = strtod(lat_str, NULL);
            int degrees_lat = (int)(raw_lat / 100);
            double minutes_lat = raw_lat - (degrees_lat * 100);
            double dec_lat = degrees_lat + (minutes_lat / 60.0);
            if (lat_dir && lat_dir[0] == 'S') dec_lat = -dec_lat;

            double raw_lon = strtod(lon_str, NULL);
            int degrees_lon = (int)(raw_lon / 100);
            double minutes_lon = raw_lon - (degrees_lon * 100);
            double dec_lon = degrees_lon + (minutes_lon / 60.0);
            if (lon_dir && lon_dir[0] == 'W') dec_lon = -dec_lon;

            // Guardamos en la variable global
            g_datos_gps.latitud = dec_lat;
            g_datos_gps.longitud = dec_lon;
            g_datos_gps.fijado = true;
            g_datos_gps.nuevo_dato = true;
            return true;
        } else {
            g_datos_gps.fijado = false; // El GPS está reportando que no tiene satélites aún
        }
    }
    return false;
}