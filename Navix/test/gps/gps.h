#ifndef GPS_H
#define GPS_H

#include "pico/stdlib.h"

// --- Nueva configuración de hardware (Sin cruces con motores) ---
#define UART_GPS uart0
#define BAUD_GPS 9600
#define PIN_GPS_TX 12 // TX de la Pico (va conectado al RX del GPS)
#define PIN_GPS_RX 13 // RX de la Pico (va conectado al TX del GPS)

#define GPS_BUFFER_SIZE 128

typedef struct {
    double latitud;
    double longitud;
    bool fijado;        // True si hay satélites
    bool nuevo_dato;    // Bandera para el sistema multicore
} DatosGPS;

extern volatile DatosGPS g_datos_gps;

// Funciones del módulo
void gps_init(void);            // Inicializa UART e interrupciones
bool gps_procesar_buffer(void); // Función de Polling

#endif