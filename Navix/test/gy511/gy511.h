#ifndef GY511_H
#define GY511_H

#include "pico/stdlib.h"
#include "hardware/i2c.h"

// --- Configuración I2C ---
#define I2C_PORT i2c1
#define PIN_SDA 10
#define PIN_SCL 11

// Dirección hexadecimal del magnetómetro LSM303DLHC en el bus I2C
#define MAG_ADDRESS 0x1E 

void gy511_init(void);
float gy511_leer_heading(void);

#endif