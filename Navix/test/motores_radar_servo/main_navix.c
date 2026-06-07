#include "pico/stdlib.h"
#include "pico/multicore.h"
#include "hardware/sync.h"
#include "robot_defs.h"
#include "radar.h"
#include "servo.h"
#include "motores.h"
#include "core1_logic.h"
#include <stdio.h>

// --- Variables Globales ---
volatile MapaEntorno g_mapa = {50.0, 50.0, 50.0, false};
volatile EstadoNav g_estado_actual = ST_MOVING;
volatile bool g_flag_multicore_data = false; // Flag para sincronización
spin_lock_t *map_lock; 

uint32_t last_maneuver_time = 0;

int main() {
    stdio_init_all();
    sleep_ms(3000); 
    printf("[Core0] Iniciando sistema de navegación...\n");

    int spin_lock_num = spin_lock_claim_unused(true);
    map_lock = spin_lock_instance(spin_lock_num);
    motores_init(); 
    multicore_launch_core1(core1_entry);
    
    // Mapa local para trabajar
    MapaEntorno local_map;

    while (true) {
        // --- 1. Sincronización de Datos (Snapshot general) ---
        uint32_t save = spin_lock_blocking(map_lock);
        local_map = g_mapa;
        spin_unlock(map_lock, save);

        switch (g_estado_actual) {
            case ST_MOVING:
                servo_sweep_mode = false;    
                servo_set_angle_fixed(90.0);  
                motores_adelante(VELOCIDAD_MOTORES_ADELANTE_ATRAS);
                if (local_map.dist_centro < DISTANCIA_SEGURIDAD) {
                    g_estado_actual = ST_OBSTACLE;
                }
                break;

            case ST_OBSTACLE:
                printf("[Core0] Obstáculo detectado. Preparando escaneo...\n");
                
                // --- RESET DE FLAG Y MAPA ---
                save = spin_lock_blocking(map_lock);
                g_flag_multicore_data = false; // INVALIDAMOS DATOS VIEJOS
                g_mapa.dist_izq = 100.0;       // Resetear a camino libre
                g_mapa.dist_centro = 100.0;
                g_mapa.dist_der = 100.0;
                spin_unlock(map_lock, save);
                // -----------------------------

                servo_set_angle_fixed(0.0);
                sleep_ms(200); 
                motores_detener();
                servo_sweep_mode = true;      
                g_estado_actual = ST_SCANNING;
                break;

            case ST_SCANNING:
                if (servo_get_angle() >= 170.0) {
                    servo_sweep_mode = false; 
                    g_estado_actual = ST_RESET_SERVO; 
                }
                break;

            case ST_RESET_SERVO:
                servo_set_angle_fixed(90.0); 
                sleep_ms(200);
                g_estado_actual = ST_DECIDING;
                break;

            case ST_DECIDING:
                // --- ESPERA ACTIVA DE DATOS FRESCOS ---
                // Si el flag no es true, significa que el Core 1 no ha terminado de llenar el mapa.
                // Nos quedamos bloqueados en este estado hasta que los datos sean nuevos.
                if (!g_flag_multicore_data) {
                    break; // Salimos del switch y volvemos a iterar hasta que g_flag sea true
                }
                
                // Si llegamos aquí, los datos son frescos.
                printf("[Core0] Datos frescos recibidos. Analizando...\n");
                
                if (local_map.dist_der > DISTANCIA_SEGURIDAD) {
                    printf("[Core0] Giro a la derecha despejado. Ejecutando giro...\n");
                    motores_girar_izquierda(VELOCIDAD_MOTORES_GIRO); // para no cruzarlo circuitalmente
                    last_maneuver_time = to_ms_since_boot(get_absolute_time());
                    g_estado_actual = ST_TURNING_RIGHT;
                } else if (local_map.dist_izq > DISTANCIA_SEGURIDAD) {
                    printf("[Core0] Giro a la izquierda despejado. Ejecutando giro...\n");
                    motores_girar_derecha(VELOCIDAD_MOTORES_GIRO); // para no cruzarlo circuitalmente
                    last_maneuver_time = to_ms_since_boot(get_absolute_time());
                    g_estado_actual = ST_TURNING_LEFT;
                } else {
                    printf("[Core0] Ningún giro despejado. Ejecutando maniobra...\n");
                    g_estado_actual = ST_MANEUVER;
                }
                break;

            case ST_TURNING_RIGHT:
            case ST_TURNING_LEFT:
                if ((to_ms_since_boot(get_absolute_time()) - last_maneuver_time) > TIEMPO_GIRO_90_MS) {
                    motores_detener();
                    g_estado_actual = ST_MOVING;
                }
                break;

            case ST_MANEUVER:
                motores_atras(VELOCIDAD_MOTORES_ADELANTE_ATRAS);
                last_maneuver_time = to_ms_since_boot(get_absolute_time());
                g_estado_actual = ST_BACKING_UP;
                break;

            case ST_BACKING_UP:
                if ((to_ms_since_boot(get_absolute_time()) - last_maneuver_time) > TIEMPO_RETROCESO_MS) {
                    motores_detener();
                    g_estado_actual = ST_MOVING;
                }
                break;

            default:
                g_estado_actual = ST_MOVING;
                break;
        }
    }
}