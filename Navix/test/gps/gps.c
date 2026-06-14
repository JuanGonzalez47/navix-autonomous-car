/**
 * @file gps.c
 * @brief Implementación del sistema de recepción y parseo GPS.
 * * Separa la recepción de caracteres en hardware (mediante interrupciones UART) 
 * del pesado cálculo matemático de conversión de coordenadas NMEA a grados 
 * decimales, evitando el bloqueo del núcleo procesador.
 */

#include "gps.h"
#include "hardware/uart.h"
#include "hardware/irq.h"
#include <string.h>
#include <stdlib.h>

volatile char raw_buffer[GPS_BUFFER_SIZE];
volatile uint16_t buffer_idx = 0;
volatile bool linea_lista = false; /**< Bandera de comunicación ISR -> Polling */

volatile DatosGPS g_datos_gps = {0.0, 0.0, false, false};

/**
 * @brief Rutina de Servicio de Interrupción (ISR) para recepción UART.
 * * Se ejecuta automáticamente en el fondo cada vez que llega un byte del GPS.
 * Ensambla los caracteres en `raw_buffer` hasta encontrar un salto de línea 
 * ('\\n'), momento en el cual levanta la bandera `linea_lista`.
 */
void on_uart_gps_rx() {
    while (uart_is_readable(UART_GPS)) {
        char c = uart_getc(UART_GPS);
        
        if (!linea_lista) {
            if (c == '\n' || buffer_idx >= (GPS_BUFFER_SIZE - 1)) {
                raw_buffer[buffer_idx] = '\0'; 
                linea_lista = true; // Le avisa al polling que terminó una línea
            } else if (c != '\r') { 
                raw_buffer[buffer_idx++] = c;
            }
        }
    }
}

void gps_init(void) {
    uart_init(UART_GPS, BAUD_GPS);
    gpio_set_function(PIN_GPS_TX, GPIO_FUNC_UART);
    gpio_set_function(PIN_GPS_RX, GPIO_FUNC_UART);
    
    // Desactivar FIFO para disparar la interrupción byte a byte
    uart_set_fifo_enabled(UART_GPS, false); 

    // Configurar y habilitar la interrupción
    int UART_IRQ = (UART_GPS == uart0) ? UART0_IRQ : UART1_IRQ;
    irq_set_exclusive_handler(UART_IRQ, on_uart_gps_rx);
    irq_set_enabled(UART_IRQ, true);
    uart_set_irq_enables(UART_GPS, true, false); // Solo RX
}

bool gps_procesar_buffer(void) {
    // Si la interrupción no ha terminado de armar la línea, salimos inmediatamente
    if (!linea_lista) return false; 

    char local_str[GPS_BUFFER_SIZE];
    
    // Bloqueamos interrupciones un nanosegundo para copiar el texto sin riesgo
    uart_set_irq_enables(UART_GPS, false, false);
    strncpy(local_str, (char*)raw_buffer, GPS_BUFFER_SIZE);
    buffer_idx = 0;
    linea_lista = false; // Liberamos el buffer para la siguiente interrupción
    uart_set_irq_enables(UART_GPS, true, false);

    // Parseo de la sentencia RMC
    if (strstr(local_str, "RMC") != NULL) {
        char *token = strtok(local_str, ",");
        int campo = 0;
        char status = 'V';
        char *lat_str = NULL, *lat_dir = NULL, *lon_str = NULL, *lon_dir = NULL;

        while (token != NULL) {
            campo++;
            if (campo == 3) status = token[0]; 
            if (campo == 4) lat_str = token;   
            if (campo == 5) lat_dir = token;
            if (campo == 6) lon_str = token;
            if (campo == 7) lon_dir = token;
            token = strtok(NULL, ",");
        }

        if (status == 'A' && lat_str && lon_str) {
            // Conversión NMEA a Grados Decimales
            double raw_lat = strtod(lat_str, NULL);
            int degrees_lat = (int)(raw_lat / 100);
            double dec_lat = degrees_lat + ((raw_lat - (degrees_lat * 100)) / 60.0);
            if (lat_dir && lat_dir[0] == 'S') dec_lat = -dec_lat;

            double raw_lon = strtod(lon_str, NULL);
            int degrees_lon = (int)(raw_lon / 100);
            double dec_lon = degrees_lon + ((raw_lon - (degrees_lon * 100)) / 60.0);
            if (lon_dir && lon_dir[0] == 'W') dec_lon = -dec_lon;

            g_datos_gps.latitud = dec_lat;
            g_datos_gps.longitud = dec_lon;
            g_datos_gps.fijado = true;
            g_datos_gps.nuevo_dato = true;
            return true;
        } else {
            g_datos_gps.fijado = false;
        }
    }
    return false; // Retorna falso si era otra sentencia (ej. GPGGA) o no tenía satélites
}