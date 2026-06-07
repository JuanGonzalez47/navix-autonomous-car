#ifndef ROBOT_DEFS_H
#define ROBOT_DEFS_H

#include "pico/stdlib.h"
#include "hardware/sync.h"

// --- Estructuras de Datos ---
typedef struct {
    float dist_izq;
    float dist_centro;
    float dist_der;
    bool nuevo_mapa_disponible;
} MapaEntorno;

// --- Máquina de Estados ---
typedef enum { 
    ST_MOVING,         // Servo fijo en 90°
    ST_OBSTACLE,       // Freno total
    ST_SCANNING,   // Barrido 0 a 180 grados
    ST_RESET_SERVO,    // Poner servo en 90°
    ST_DECIDING,       // Analizar datos
    ST_TURNING_RIGHT,  // Giro específico
    ST_TURNING_LEFT,   // Giro específico
    ST_MANEUVER,       // Iniciar retroceso
    ST_BACKING_UP      // Ejecutando retroceso
} EstadoNav;

// --- Constantes del Sistema (Ajustables) ---
#define DISTANCIA_SEGURIDAD 32.0f
#define TIEMPO_GIRO_90_MS   500
#define TIEMPO_RETROCESO_MS 500
#define VELOCIDAD_MOTORES_ADELANTE_ATRAS 11000
#define VELOCIDAD_MOTORES_GIRO 15000

// --- Variables Globales Compartidas (Declaraciones Externas) ---
// La palabra 'extern' es clave aquí. 
// La definición real (sin 'extern') ocurre solo en main_navix.c
extern volatile MapaEntorno g_mapa;
extern volatile EstadoNav g_estado_actual;
extern volatile bool g_flag_multicore_data;

// Cerrojo para evitar condiciones de carrera (Race Conditions)
extern spin_lock_t *map_lock;

#endif // ROBOT_DEFS_H