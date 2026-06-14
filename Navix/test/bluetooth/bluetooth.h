/**
 * @file bluetooth.h
 * @brief Módulo de comunicación inalámbrica vía Bluetooth (HC-05).
 * * Este archivo define los pines y la configuración de la interfaz UART1 
 * de la Raspberry Pi Pico para comunicarse con el módulo HC-05. 
 * Contiene las funciones para enviar telemetría asíncrona y recibir 
 * coordenadas de navegación (META).
 * * @author Equipo Navix
 * @date Junio 2026
 */

#ifndef BLUETOOTH_H
#define BLUETOOTH_H

#include <stdbool.h>
#include "pico/stdlib.h"

// --- Configuración UART1 para HC-05 ---
#define UART_BT uart1
#define BAUD_BT 9600 // Velocidad de fábrica por defecto del HC-05
#define PIN_BT_TX 8  // TX de la Pico (Va al RX del HC-05)
#define PIN_BT_RX 9  // RX de la Pico (Va al TX del HC-05)

/**
 * @brief Inicializa el periférico UART1 para la comunicación Bluetooth.
 * * Configura los pines de transmisión (TX = 8) y recepción (RX = 9), 
 * y establece la tasa de baudios a 9600 bps para el HC-05.
 */
void bt_init(void);

/**
 * @brief Envía la telemetría actual del vehículo hacia la aplicación móvil.
 * * @param latitud Posición actual en el eje Y (grados decimales).
 * @param longitud Posición actual en el eje X (grados decimales).
 * @param distancia Distancia restante hacia la meta en metros.
 * @param accion Cadena de texto indicando el estado de los motores (ej. "ADELANTE").
 */
void bt_enviar_telemetria(double latitud, double longitud, float distancia, const char* accion);

/**
 * @brief Lee y procesa los comandos entrantes desde el puerto serie.
 * * @param[out] meta_lat Puntero donde se almacenará la latitud recibida.
 * @param[out] meta_lon Puntero donde se almacenará la longitud recibida.
 * @return true Si se recibió y parseó exitosamente una nueva coordenada META.
 * @return false Si no hay datos nuevos o la trama es inválida.
 */
bool bt_procesar_comando(double* meta_lat, double* meta_lon);

#endif // BLUETOOTH_H