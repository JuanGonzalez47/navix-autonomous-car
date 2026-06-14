/**
 * @file motores.h
 * @brief Módulo de control de tracción y dirección (Driver TB6612FNG).
 * * Este archivo define los pines GPIO y las funciones necesarias para 
 * manipular la etapa de potencia del vehículo. Permite controlar la 
 * dirección y velocidad de los dos motores DC utilizando señales PWM 
 * y lógica digital.
 * * @author Equipo Navix
 * @date Junio 2026
 */

#ifndef MOTORES_H
#define MOTORES_H

#include "pico/stdlib.h"
#include "hardware/pwm.h"

// --- Definición de Pines para el Driver TB6612FNG ---
#define AIN1 0    /**< Pin de control de dirección 1 para el Motor A (Derecho). */
#define AIN2 1    /**< Pin de control de dirección 2 para el Motor A (Derecho). */
#define PWMA 2    /**< Pin de control de velocidad (PWM) para el Motor A. */
#define BIN1 3    /**< Pin de control de dirección 1 para el Motor B (Izquierdo). */
#define BIN2 4    /**< Pin de control de dirección 2 para el Motor B (Izquierdo). */
#define PWMB 5    /**< Pin de control de velocidad (PWM) para el Motor B. */
#define STBY 6    /**< Pin de Standby para habilitar o deshabilitar el puente H. */

/**
 * @brief Inicializa los pines GPIO y configura los canales PWM.
 * * Establece los pines de dirección como salidas digitales y configura 
 * los pines PWMA y PWMB para funcionar como canales PWM a una 
 * frecuencia aproximada de 5 kHz. Habilita el pin STBY.
 */
void motores_init();

/**
 * @brief Activa ambos motores para avanzar en línea recta.
 * * @param velocidad Ciclo de trabajo (Duty Cycle) del PWM (0 a 25000).
 */
void motores_adelante(uint16_t velocidad);

/**
 * @brief Activa ambos motores en reversa.
 * * @param velocidad Ciclo de trabajo (Duty Cycle) del PWM (0 a 25000).
 */
void motores_atras(uint16_t velocidad);

/**
 * @brief Detiene completamente el vehículo.
 * * Establece todos los pines de dirección y las señales PWM a 0.
 */
void motores_detener();

/**
 * @brief Ejecuta un giro sobre su propio eje hacia la derecha.
 * * Invierte el giro del motor derecho (A) y avanza el izquierdo (B).
 * * @param velocidad Ciclo de trabajo (Duty Cycle) del PWM (0 a 25000).
 */
void motores_girar_derecha(uint16_t velocidad);

/**
 * @brief Ejecuta un giro sobre su propio eje hacia la izquierda.
 * * Invierte el giro del motor izquierdo (B) y avanza el derecho (A).
 * * @param velocidad Ciclo de trabajo (Duty Cycle) del PWM (0 a 25000).
 */
void motores_girar_izquierda(uint16_t velocidad);

#endif // MOTORES_H