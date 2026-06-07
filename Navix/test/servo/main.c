#include <stdio.h>
#include <stdint.h>
#include "pico/stdlib.h"
#include "servo.h"
#include "hardware/sync.h"

// Declaramos la variable externa para poder leerla en el main
extern volatile bool g_flag_timer_servo; 
extern volatile bool servo_sweep_mode;

int main() {
    stdio_init_all();
    servo_init(); // Esto ya configura el timer y el PWM
    
    printf("Sistema iniciado, esperando movimiento...\n");
    sleep_ms(2000);
    servo_sweep_mode = true; // Activamos el modo de barrido para ver el movimiento

    while (true) {
        if (g_flag_timer_servo) {
            servo_update_sweep();      // Ejecutamos el movimiento
            g_flag_timer_servo = false; // Reset de la bandera
        }
        
        else{
            __wfi();
        }
    }
}