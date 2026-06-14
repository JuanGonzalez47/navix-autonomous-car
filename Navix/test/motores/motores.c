/**
 * @file motores.c
 * @brief Implementación de las señales de potencia y maniobras del vehículo.
 * * Gestiona la configuración directa de los registros del hardware de la 
 * RP2040 para generar las señales PWM y realizar la conmutación de los 
 * puentes H internos del driver TB6612FNG.
 */

#include "motores.h"
#include <stdio.h>

void motores_init() {
    printf("[Init] Iniciando Motores...\n");
    gpio_init(AIN1); gpio_set_dir(AIN1, GPIO_OUT);
    gpio_init(AIN2); gpio_set_dir(AIN2, GPIO_OUT);
    gpio_init(BIN1); gpio_set_dir(BIN1, GPIO_OUT);
    gpio_init(BIN2); gpio_set_dir(BIN2, GPIO_OUT);
    gpio_init(STBY); gpio_set_dir(STBY, GPIO_OUT);
    
    // Configuración PWM
    printf("[Init] Configurando PWM para Motores...\n");
    gpio_set_function(PWMA, GPIO_FUNC_PWM);
    gpio_set_function(PWMB, GPIO_FUNC_PWM);
    
    uint slice_a = pwm_gpio_to_slice_num(PWMA);
    uint slice_b = pwm_gpio_to_slice_num(PWMB);
    pwm_set_wrap(slice_a, 25000); // Frecuencia ~5kHz
    pwm_set_wrap(slice_b, 25000);
    pwm_set_enabled(slice_a, true);
    pwm_set_enabled(slice_b, true);
    
    gpio_put(STBY, 1); // Habilitar driver
    printf("[Init] Motores inicializados.\n");
}

void motores_adelante(uint16_t velocidad) {
    gpio_put(AIN1, 1); gpio_put(AIN2, 0);
    gpio_put(BIN1, 1); gpio_put(BIN2, 0);
    pwm_set_gpio_level(PWMA, velocidad);
    pwm_set_gpio_level(PWMB, velocidad);
}

void motores_detener() {
    gpio_put(AIN1, 0); gpio_put(AIN2, 0);
    gpio_put(BIN1, 0); gpio_put(BIN2, 0);
    pwm_set_gpio_level(PWMA, 0);
    pwm_set_gpio_level(PWMB, 0);
}

void motores_atras(uint16_t velocidad) {
    gpio_put(AIN1, 0); gpio_put(AIN2, 1);
    gpio_put(BIN1, 0); gpio_put(BIN2, 1);
    pwm_set_gpio_level(PWMA, velocidad);
    pwm_set_gpio_level(PWMB, velocidad);
}

void motores_girar_derecha(uint16_t velocidad) {
    // Motor A (Derecho) atrás
    gpio_put(AIN1, 0); gpio_put(AIN2, 1);
    // Motor B (Izquierdo) adelante
    gpio_put(BIN1, 1); gpio_put(BIN2, 0);
    pwm_set_gpio_level(PWMA, velocidad);
    pwm_set_gpio_level(PWMB, velocidad);
}

void motores_girar_izquierda(uint16_t velocidad) {
    // Motor A (Derecho) adelante
    gpio_put(AIN1, 1); gpio_put(AIN2, 0);
    // Motor B (Izquierdo) atrás
    gpio_put(BIN1, 0); gpio_put(BIN2, 1);
    pwm_set_gpio_level(PWMA, velocidad);
    pwm_set_gpio_level(PWMB, velocidad);
}