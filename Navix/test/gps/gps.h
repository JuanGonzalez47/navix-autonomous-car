#ifndef GPS_H
#define GPS_H

#include "pico/stdlib.h"

// Configuración de Hardware
#define UART_GPS uart1
#define BAUD_GPS 9600
#define PIN_GPS_TX 4
#define PIN_GPS_RX 5

// Tamaño del buffer para almacenar una sentencia NMEA completa (Ej: $GPRMC,...)
#define GPS_BUFFER_SIZE 128

typedef struct {
    double latitud;
    double longitud;
    bool fijado;        // True si tiene señal de satélites válida
    bool nuevo_dato;    // Flag para avisar que hay una nueva coordenada lista
} DatosGPS;

extern volatile DatosGPS g_datos_gps;

void gps_init(void);
bool gps_procesar_buffer(void); // Polling desde Core 1

#endif