#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/sync.h"
#include "gps.h"

int main() {
    stdio_init_all();
    
    while (!stdio_usb_connected()) {
        sleep_ms(100);
    }
    sleep_ms(1000);

    printf("--- PRUEBA UNITARIA: MÓDULO GPS (Pines GP12 y GP13) ---\n");
    
    gps_init();
    printf("[Sistema] Inicializado. Esperando coordenadas válidas...\n");

    while (true) {
        // --- POLLING ---
        if (gps_procesar_buffer()) {
            printf("[GPS] FIX OK -> Latitud: %.6f | Longitud: %.6f\n", g_datos_gps.latitud, g_datos_gps.longitud);
        } else {
            __wfi(); 
        }
    }
}