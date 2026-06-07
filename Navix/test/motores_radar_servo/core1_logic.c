#include "robot_defs.h"
#include "radar.h"
#include "servo.h"
#include "pico/stdlib.h"
#include "hardware/sync.h"

extern volatile bool g_flag_timer_servo;

void core1_entry(void) {
    // 1. Inicialización
    radar_init();
    servo_init();
    
    // Distancia segura por defecto (si no se detecta nada)
    const float DISTANCIA_SEGURA = 100.0; 

    while (true) {
        // --- Manejo del Servo (20ms intervalo) ---
        if (g_flag_timer_servo) {
            servo_update_sweep();
            g_flag_timer_servo = false;
        }

        // --- Manejo del Radar ---
        if (radar_esta_listo()) {
            float dist_raw = radar_leer_cm();
            float angulo = servo_get_angle();
            
            // Si el sensor devuelve 0 o error, tratamos como camino libre (100cm)
            float dist_final = (dist_raw <= 0.0 || dist_raw > 200.0) ? DISTANCIA_SEGURA : dist_raw;

            // --- INICIO DE SECCIÓN CRÍTICA ---
            uint32_t save = spin_lock_blocking(map_lock);

            // Clasificación de sectores
            // Actualizamos solo el sector correspondiente según el ángulo del servo
            if (angulo >= 0 && angulo < 50) {
                g_mapa.dist_izq = dist_final;
            } 
            else if (angulo >= 50 && angulo < 130) {
                g_mapa.dist_centro = dist_final;
            } 
            else if (angulo >= 130 && angulo <= 180) {
                g_mapa.dist_der = dist_final;
            }

            // Marcamos que los datos fueron actualizados
            // Esto permite al Core 0 saber que el mapa es "fresco"
            g_mapa.nuevo_mapa_disponible = true; 
            g_flag_multicore_data = true; 

            spin_unlock(map_lock, save);
            // --- FIN DE SECCIÓN CRÍTICA ---
        }
    }
}