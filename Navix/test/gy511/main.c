#include <stdio.h>
#include "pico/stdlib.h"
#include "gy511.h"

int main() {
    stdio_init_all();
    
    // Trampa de velocidad 
    while (!stdio_usb_connected()) {
        sleep_ms(100);
    }
    sleep_ms(1000);

    printf("--- PRUEBA UNITARIA: BRÚJULA GY-511 (I2C1) ---\n");
    
    gy511_init();
    printf("[Sistema] Módulo I2C inicializado correctamente.\n");

    while (true) {
        // --- POLLING I2C ---
        float heading = gy511_leer_heading();
        
        printf("[Brújula] Orientación: %.2f grados\n", heading);
        sleep_ms(200); 
    }
}