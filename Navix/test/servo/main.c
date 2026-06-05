#include <stdio.h>
#include <stdint.h>
#include "pico/stdlib.h"
#include "hardware/sync.h"
#include "servo.h"

// La interrupción ahora es el "motor" del movimiento
bool timer_callback(struct repeating_timer *t) {
    servo_update_sweep(); // conectamos directamente al cambio del angulo segun el apso para el barrido
    return true;
}

int main() {
    stdio_init_all();
    servo_init();
    sleep_ms(2000);

    // Interrupción cada 500ms (frecuencia óptima para servos)
    struct repeating_timer timer;
    add_repeating_timer_ms(20, timer_callback, NULL, &timer);
    while (true) {
        // Este es un evento que debo de manejar dentrod e la interrupcion
        __wfi(); 
    }
}