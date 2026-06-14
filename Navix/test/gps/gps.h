/**
 * @file gps.h
 * @brief Módulo de recepción y parseo de tramas GPS (Protocolo NMEA).
 * * Este archivo define la estructura de datos y las funciones necesarias para 
 * comunicarse con un módulo GPS (ej. NEO-6M / L76) a través de la UART0.
 * Utiliza un enfoque híbrido: recepción por interrupciones (ISR) para no perder 
 * bytes y procesamiento por polling en el ciclo principal.
 * * @author Equipo Navix
 * @date Junio 2026
 */

#ifndef GPS_H
#define GPS_H

#include "pico/stdlib.h"

// --- Nueva configuración de hardware (Sin cruces con motores) ---
#define UART_GPS uart0
#define BAUD_GPS 9600
#define PIN_GPS_TX 12 // TX de la Pico (va conectado al RX del GPS)
#define PIN_GPS_RX 13 // RX de la Pico (va conectado al TX del GPS)

#define GPS_BUFFER_SIZE 128

/**
 * @struct DatosGPS
 * @brief Estructura que almacena la información procesada del GPS.
 */
typedef struct {
    double latitud;     /**< Latitud actual en grados decimales (Negativo para el Sur). */
    double longitud;    /**< Longitud actual en grados decimales (Negativo para el Oeste). */
    bool fijado;        /**< True si el módulo tiene conexión con los satélites (Status 'A'). */
    bool nuevo_dato;    /**< Bandera para notificar al sistema multicore que hay nueva lectura. */
} DatosGPS;

/**
 * @brief Variable global compartida que contiene la última ubicación conocida.
 */
extern volatile DatosGPS g_datos_gps;

/**
 * @brief Inicializa el hardware UART y las interrupciones para el GPS.
 * * Configura los pines TX/RX, establece los baudios y desactiva el buffer 
 * FIFO de la UART para obligar a que la interrupción RX se dispare byte a byte.
 */
void gps_init(void);

/**
 * @brief Procesa el buffer de recepción y actualiza las coordenadas globales.
 * * Esta función debe ser llamada en el bucle principal (polling). Copia el 
 * buffer de forma segura (desactivando la ISR momentáneamente) y busca la 
 * sentencia NMEA "RMC". Si la encuentra, convierte las coordenadas a grados 
 * decimales y actualiza `g_datos_gps`.
 * * @return true Si se procesó exitosamente una trama RMC válida y se actualizó la posición.
 * @return false Si el buffer aún no está listo o la trama recibida no es RMC.
 */
bool gps_procesar_buffer(void);

#endif // GPS_H