#include <stdio.h>
#include "pico/stdlib.h"
#include "gps.h"
#include "motores.h"

int main() {
    // 1. Inicialización de puertos
    stdio_init_all();
    sleep_ms(3000); // Tiempo para que abra el monitor serial (USB)
    
    printf("--- PRUEBA DE INTEGRACIÓN: MOTORES + GPS ---\n");

    // 2. Inicializar periféricos
    motores_init();
    gps_init();
    
    printf("[Sistema] Módulos inicializados correctamente. Esperando ráfagas NMEA...\n");

    // 3. Bucle infinito de la prueba
    while (true) {
        
        // El GPS usa interrupciones en el fondo, pero aquí procesamos el buffer
        if (gps_procesar_buffer()) {
            printf("[GPS-FIX] Lat: %.6f | Lon: %.6f\n", g_datos_gps.latitud, g_datos_gps.longitud);
        }

        // Lógica de prueba simple para los motores basada en el GPS
        // (La mantenemos comentada para que el carro no salga volando apenas encienda, 
        // pero puedes descomentarla cuando lo tengas en el piso o en el aire)
        
        /*
        if (g_datos_gps.fijado) {
            // Si el GPS ya tiene satélites, mueve las llantas suavemente
            motores_adelante(10000); 
        } else {
            // Si está buscando satélites, se queda quieto
            motores_detener();
        }
        */

        // Evita saturar el procesador (Core 0) en esta prueba básica
        sleep_ms(10); 
    }
}