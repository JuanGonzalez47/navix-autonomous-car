/**
 * @file bluetooth.c
 * @brief Implementación de las funciones de comunicación Bluetooth.
 * * Gestiona el acceso al hardware de la RP2040 utilizando la configuración 
 * definida en bluetooth.h para transmitir la telemetría y parsear los 
 * comandos NMEA personalizados del sistema Navix.
 */

#include "bluetooth.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

/**
 * @brief Configura el hardware UART1 de la RP2040.
 */
void bt_init(void) {
    uart_init(UART_BT, BAUD_BT);
    gpio_set_function(PIN_BT_TX, GPIO_FUNC_UART);
    gpio_set_function(PIN_BT_RX, GPIO_FUNC_UART);
}

/**
 * @brief Formatea y transmite la cadena de telemetría a través del bus UART1.
 */
void bt_enviar_telemetria(double latitud, double longitud, float distancia, const char* accion) {
    char bt_tx_buffer[128];
    snprintf(bt_tx_buffer, sizeof(bt_tx_buffer),
             "[Navix] Lat: %.6f, Lon: %.6f | Dist: %.1f m | Accion: %s\r\n",
             latitud, longitud, distancia, accion);
    
    uart_puts(UART_BT, bt_tx_buffer);
}

/**
 * @brief Escucha el buffer circular de UART1 y parsea la trama "META,lat,lon".
 */
bool bt_procesar_comando(double* meta_lat, double* meta_lon) {
    static char bt_buffer[64];
    static int idx = 0;
    bool nueva_meta = false;

    while (uart_is_readable(UART_BT)) {
        char c = uart_getc(UART_BT);
        
        if (c == '\n' || c == '\r') {
            if (idx > 0) {
                bt_buffer[idx] = '\0'; // Finalizar la cadena
                
                // Buscar cabecera de comando entrante
                if (strncmp(bt_buffer, "META,", 5) == 0) {
                    char *token = strtok(bt_buffer, ","); // Saltar "META"
                    
                    token = strtok(NULL, ",");
                    if (token) *meta_lat = strtod(token, NULL);
                    
                    token = strtok(NULL, ",");
                    if (token) *meta_lon = strtod(token, NULL);

                    nueva_meta = true;
                    uart_puts(UART_BT, "[Navix] Meta aceptada. Calculando ruta...\r\n");
                }
                idx = 0; // Reiniciar buffer para la próxima trama
            }
        } else if (idx < 63) {
            bt_buffer[idx++] = c;
        }
    }
    return nueva_meta;
}