#include "pico/stdlib.h"
#include "gps.h"
#include <stdio.h>

int main() {
    stdio_init_all();
    sleep_ms(3000);
    printf("--- PRUEBA UNITARIA MÓDULO GNSS ---\n");
    
    gps_init();
    printf("UART1 Inicializado a 9600 baud. Esperando ráfagas NMEA...\n");

    while (true) {
        // Ejecutamos la función de procesamiento continuamente (simulando Core 1)
        if (gps_procesar_buffer()) {
            printf("[GPS-FIX] Lat: %.6f | Lon: %.6f\n", g_datos_gps.latitud, g_datos_gps.longitud);
        }

        // Cada 2 segundos imprimimos un estatus general de diagnóstico
        static uint32_t last_print = 0;
        if (to_ms_since_boot(get_absolute_time()) - last_print > 2000) {
            last_print = to_ms_since_boot(get_absolute_time());
            if (!g_datos_gps.fijado) {
                printf("[GPS-WARN] Buscando satélites... (Saca el carro a un espacio abierto)\n");
            }
        }
        
        sleep_ms(10); // Evita saturar la CPU en el test unitario
    }
}