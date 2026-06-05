#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/sync.h"
#include "pico/platform.h"
#include "radar.h"

int main() {
    stdio_init_all();
    radar_init();
    
    sleep_ms(3000); 
    printf("Iniciando mediciones de distancia...\n");

    while (true) {
        if (radar_esta_listo()) {
            float dist = radar_leer_cm();
            printf("Distancia detectada: %.2f cm\n", dist);
        } else {
            __wfi(); 
        }
    }
}