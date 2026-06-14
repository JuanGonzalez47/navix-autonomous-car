/**
 * @file gy511.h
 * @brief Módulo de lectura y control de la brújula magnética (Sensor GY-511 / LSM303).
 * * Este archivo define la configuración del bus I2C para comunicarse con el 
 * magnetómetro LSM303DLHC integrado en el módulo GY-511. Proporciona la 
 * interfaz para inicializar el sensor y leer el ángulo de orientación 
 * espacial (Heading) del vehículo respecto al norte magnético.
 * * @author Equipo Navix
 * @date Junio 2026
 */

#ifndef GY511_H
#define GY511_H

#include "pico/stdlib.h"
#include "hardware/i2c.h"

// --- Configuración I2C ---
#define I2C_PORT i2c1   /**< Puerto I2C utilizado en la RP2040. */
#define PIN_SDA 10      /**< Pin de Datos Seriales (SDA). */
#define PIN_SCL 11      /**< Pin de Reloj Serial (SCL). */

#define MAG_ADDRESS 0x1E /**< Dirección hexadecimal I2C del magnetómetro. */

/**
 * @brief Inicializa el periférico I2C y configura el magnetómetro.
 * * Establece la velocidad del bus I2C a 100 kHz (modo estándar), configura 
 * los pines SDA y SCL con resistencias Pull-Up internas, y escribe en los 
 * registros del sensor para despertarlo (modo continuo) y ajustar su tasa 
 * de muestreo a 30 Hz.
 */
void gy511_init(void);

/**
 * @brief Lee los vectores magnéticos y calcula el rumbo (Heading).
 * * Solicita 6 bytes consecutivos al sensor correspondientes a los ejes X, Z e Y.
 * Aplica una calibración "Hard Iron" mediante offsets precalculados para 
 * centrar los datos magnéticos, y utiliza trigonometría (atan2) para 
 * obtener el ángulo de orientación del chasis.
 * * @return float Ángulo de orientación actual normalizado de 0.0 a 359.9 grados.
 */
float gy511_leer_heading(void);

#endif // GY511_H