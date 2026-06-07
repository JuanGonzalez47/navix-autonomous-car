#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/sync.h"
#include "radar.h"
#include "servo.h"

// Declaramos las variables externas
extern volatile bool g_flag_timer_servo;
extern volatile bool servo_sweep_mode;

int main() {
    stdio_init_all();
    
    // Inicialización
    servo_init();
    radar_init();
    
    sleep_ms(3000); 
    printf("Sistema Servo-Radar iniciado...\n");
    servo_sweep_mode = true;

    while (true) {
        // --- 1. GESTIÓN DEL SERVO ---
        // Chequeamos si el timer disparó la bandera
        if (g_flag_timer_servo) {
            servo_update_sweep();      // Ejecutamos el movimiento
            g_flag_timer_servo = false; // Reset de la bandera
        }

        // --- 2. GESTIÓN DEL RADAR ---
        // Chequeamos si el radar tiene datos nuevos
        if (radar_esta_listo()) {
            float dist = radar_leer_cm();
            float angulo = servo_get_angle();
            
            printf("Angulo: %.1f - Distancia: %.2f cm\n", angulo, dist);
        }

        // --- 3. EFICIENCIA ---
        // Si no hay nada que hacer, dormimos el procesador hasta la próxima interrupción
        if (!g_flag_timer_servo && !radar_esta_listo()) {
            __wfi(); 
        }
    }
}