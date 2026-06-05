#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/sync.h"
#include "pico/platform.h"
#include "radar.h"
#include "servo.h"

bool servo_timer_callback(struct repeating_timer *t) {
    servo_update_sweep();
    return true;
}

int main() {
    stdio_init_all();
    
    // Inicializamos ambos subsistemas
    servo_init();
    radar_init();
    
    // Timer para el movimiento del servo (20ms)
    struct repeating_timer servo_timer;
    add_repeating_timer_ms(20, servo_timer_callback, NULL, &servo_timer);
    
    sleep_ms(3000); 
    printf("Sistema Servo-Radar iniciado...\n");

    while (true) {
        // Polling del radar
        if (radar_esta_listo()) {
            float dist = radar_leer_cm();
            // Obtenemos el ángulo usando el getter definido en servo.c
            float angulo = servo_get_angle();
            
            printf("Angulo: %.1f - Distancia: %.2f cm\n", angulo, dist);
        } else {
            __wfi(); 
        }
    }
}