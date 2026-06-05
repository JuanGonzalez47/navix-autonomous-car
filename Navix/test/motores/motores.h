#ifndef MOTORES_H
#define MOTORES_H

#include "pico/stdlib.h"
#include "hardware/pwm.h"

// Definición de Pines
#define AIN1 0
#define AIN2 1
#define PWMA 2
#define BIN1 3
#define BIN2 4
#define PWMB 5
#define STBY 6

void motores_init();
void motores_adelante(uint16_t velocidad);
void motores_atras(uint16_t velocidad);
void motores_detener();
void motores_girar_derecha(uint16_t velocidad);
void motores_girar_izquierda(uint16_t velocidad);

#endif