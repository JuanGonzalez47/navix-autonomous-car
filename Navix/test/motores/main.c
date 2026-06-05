#include <stdio.h>          // Para printf
#include "pico/stdlib.h"     // Para funciones básicas de la Pico
#include "hardware/sync.h"   // Para __wfi() (Wait For Interrupt)
#include "motores.h"        
#include "hardware/irq.h"

// Bandera para indicar que debemos cambiar de movimiento
volatile bool flag_cambiar_movimiento = false;

// Función ISR del Timer
bool timer_callback(struct repeating_timer *t) {
    flag_cambiar_movimiento = true; // Aquí se activa la bandera
    return true; 
}

int main() {
    stdio_init_all();
    motores_init();
    sleep_ms(2000);

    struct repeating_timer timer;
    add_repeating_timer_ms(30000, timer_callback, NULL, &timer);

    while (true) {
        printf("Esperando 30 segundos para la secuencia...\n");
        if (flag_cambiar_movimiento) {
            printf("Cambiando movimiento...\n");
            // Procesamiento en el Main (Polling)
            printf("Ejecutando movimiento adelante...\n");
            motores_adelante(15000); // Ejemplo velocidad
            sleep_ms(2000);
            printf("Deteniendo motores...\n");
            motores_detener();
            sleep_ms(2000);
            printf("Ejecutando movimiento atrás...\n");
            motores_atras(15000);
            sleep_ms(2000);
            printf("Deteniendo motores...\n");
            motores_detener();
            sleep_ms(2000);
            printf("Ejecutando giro derecha...\n");
            motores_girar_derecha(15000);
            sleep_ms(2000);
            printf("Deteniendo motores...\n");
            motores_detener();
            sleep_ms(2000);
            printf("Ejecutando giro izquierda...\n");
            motores_girar_izquierda(15000);
            sleep_ms(2000);
            printf("Finalizo la secuencia...\n");
            motores_detener();
            flag_cambiar_movimiento = false;
        } else {
            // Modo de bajo consumo
            __wfi(); 
        }
    }
}