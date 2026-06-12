#include "gy511.h"
#include <math.h>
#include <stdio.h>

void gy511_init() {
    // Inicializar I2C a 100 kHz (Estándar)
    i2c_init(I2C_PORT, 100 * 1000); 
    gpio_set_function(PIN_SDA, GPIO_FUNC_I2C);
    gpio_set_function(PIN_SCL, GPIO_FUNC_I2C);
    
    // Habilitar las resistencias Pull-Up internas de la Pico
    gpio_pull_up(PIN_SDA);
    gpio_pull_up(PIN_SCL);

    // 1. Despertar el magnetómetro (Modo de conversión continua)
    // Registro 0x02 (MR_REG_M) lo ponemos en 0x00
    uint8_t buf_mode[2] = {0x02, 0x00};
    i2c_write_blocking(I2C_PORT, MAG_ADDRESS, buf_mode, 2, false);
    
    // 2. Configurar la tasa de salida a 30 Hz
    // Registro 0x00 (CRA_REG_M) lo ponemos en 0x14
    uint8_t buf_rate[2] = {0x00, 0x14};
    i2c_write_blocking(I2C_PORT, MAG_ADDRESS, buf_rate, 2, false);
}

float gy511_leer_heading() {
    // Apuntamos al primer registro de datos del magnetómetro (Eje X, byte alto)
    uint8_t reg = 0x03; 
    uint8_t data[6];

    // Le decimos al sensor qué registro queremos leer
    i2c_write_blocking(I2C_PORT, MAG_ADDRESS, &reg, 1, true);
    // Leemos los 6 bytes consecutivos (X_H, X_L, Z_H, Z_L, Y_H, Y_L)
    i2c_read_blocking(I2C_PORT, MAG_ADDRESS, data, 6, false);

    // Unimos los bytes altos (Shift left 8 bits) con los bytes bajos (OR lógico)
    int16_t x = (data[0] << 8) | data[1];
    // int16_t z = (data[2] << 8) | data[3]; // El eje Z se usa si el carro se inclina, por ahora lo ignoramos
    int16_t y = (data[4] << 8) | data[5];

    // --- CALIBRACIÓN DEFINITIVA ---
    // Offsets calculados a partir del giro en tu mesa
    float x_calibrado = (float)x - 245.5;
    float y_calibrado = (float)y - 248.5;

    // Calcular el ángulo (Heading)
    float heading = atan2(y_calibrado, x_calibrado) * (180.0 / M_PI);
    
    // Normalizar a una brújula de 360 grados (Si da negativo, dar la vuelta)
    if (heading < 0) {
        heading += 360.0;
    }
    // ---> ¡AGREGA ESTA LÍNEA! <---
    printf("RAW X: %d | RAW Y: %d\n", x, y);

    return heading;
}