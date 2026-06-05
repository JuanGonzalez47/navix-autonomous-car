#ifndef RADAR_H
#define RADAR_H

#include "pico/stdlib.h"

// Pines definidos para el sensor
#define TRIG_PIN 16
#define ECHO_PIN 17

// Inicializa el radar, configura el timer interno y las interrupciones
void radar_init();

// Devuelve 'true' si hay una nueva medición lista para ser procesada
bool radar_esta_listo();

// Devuelve la última distancia medida en cm y limpia la bandera de listo
float radar_leer_cm();

#endif