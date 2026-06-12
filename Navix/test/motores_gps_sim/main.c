#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include "pico/stdlib.h"
#include "pico/multicore.h"
#include "hardware/sync.h"

// Módulos del sistema
#include "gps.h"
#include "gy511.h"
#include "bluetooth.h"
#include "motores.h"

// Constantes matemáticas
#define TO_RAD (M_PI / 180.0)
#define TO_DEG (180.0 / M_PI)
#define RADIO_TIERRA 6371000.0

#define MODO_SIMULACION true

typedef struct
{
    bool gps_fijado;
    bool meta_activa;
    float distancia;
    float error_giro;
} NavData;

volatile NavData g_nav_data = {false, false, 0.0, 0.0};
spin_lock_t *nav_lock;

// --- FUNCIONES MATEMÁTICAS ---
float calcular_distancia(double lat1, double lon1, double lat2, double lon2)
{
    double dLat = (lat2 - lat1) * TO_RAD;
    double dLon = (lon2 - lon1) * TO_RAD;
    double a = sin(dLat / 2) * sin(dLat / 2) +
               cos(lat1 * TO_RAD) * cos(lat2 * TO_RAD) *
                   sin(dLon / 2) * sin(dLon / 2);
    double c = 2 * atan2(sqrt(a), sqrt(1 - a));
    return RADIO_TIERRA * c;
}

float calcular_rumbo(double lat1, double lon1, double lat2, double lon2)
{
    double dLon = (lon2 - lon1) * TO_RAD;
    double lat1_rad = lat1 * TO_RAD;
    double lat2_rad = lat2 * TO_RAD;

    double y = sin(dLon) * cos(lat2_rad);
    double x = cos(lat1_rad) * sin(lat2_rad) - sin(lat1_rad) * cos(lat2_rad) * cos(dLon);

    float rumbo = atan2(y, x) * TO_DEG;
    if (rumbo < 0)
        rumbo += 360.0;
    return rumbo;
}

float calcular_error_giro(float heading_actual, float target_bearing)
{
    float error = target_bearing - heading_actual;
    if (error > 180.0f)
        error -= 360.0f;
    else if (error < -180.0f)
        error += 360.0f;
    return error;
}
// CORE 1: SENSORES, TELEMETRÍA Y MATEMÁTICA
void core1_entry(void)
{
    // Inicialización de periféricos en el núcleo asignado
    gps_init();
    gy511_init();
    bt_init();

    // Variables de navegación interna
    double meta_lat = 0.0;
    double meta_lon = 0.0;
    bool local_meta_activa = false;
    uint32_t ultimo_envio_telemetria = 0;

    char bt_tx_buffer[128];
    static char bt_buffer[64];
    static int idx = 0;

    while (true)
    {
        gps_procesar_buffer();

        // 1. Recepción de comandos por Bluetooth
        while (uart_is_readable(UART_BT))
        {
            char c = uart_getc(UART_BT);
            if (c == '\n' || c == '\r')
            {
                if (idx > 0)
                {
                    bt_buffer[idx] = '\0';
                    if (strncmp(bt_buffer, "META,", 5) == 0)
                    {
                        char *token = strtok(bt_buffer, ",");
                        token = strtok(NULL, ",");
                        if (token)
                            meta_lat = strtod(token, NULL);
                        token = strtok(NULL, ",");
                        if (token)
                            meta_lon = strtod(token, NULL);

                        local_meta_activa = true;
                        uart_puts(UART_BT, "[Navix] Meta aceptada. Calculando ruta...\r\n");
                    }
                    idx = 0;
                }
            }
            else if (idx < 63)
            {
                bt_buffer[idx++] = c;
            }
        }

        // 2. Selección de origen: Coordenadas Reales vs Simuladas
        double lat_actual = g_datos_gps.latitud;
        double lon_actual = g_datos_gps.longitud;
        bool gps_listo = g_datos_gps.fijado;

        if (MODO_SIMULACION)
        {
            // Coordenada estática simulada dentro del laboratorio
            lat_actual = 6.268525;
            lon_actual = -75.568116;
            gps_listo = true;
        }

        float distancia_calc = 0.0;
        float error_giro_calc = 0.0;

        // 3. Algoritmo matemático Go-To-Goal
        if (local_meta_activa && gps_listo)
        {
            float heading_actual = gy511_leer_heading();
            float target_bearing = calcular_rumbo(lat_actual, lon_actual, meta_lat, meta_lon);
            distancia_calc = calcular_distancia(lat_actual, lon_actual, meta_lat, meta_lon);
            error_giro_calc = calcular_error_giro(heading_actual, target_bearing);

            if (distancia_calc < 2.0)
            {
                local_meta_activa = false;
                uart_puts(UART_BT, "[Navix] ¡DESTINO ALCANZADO! Vehiculo detenido.\r\n");
            }
            else if (to_ms_since_boot(get_absolute_time()) - ultimo_envio_telemetria > 1500)
            {
                // Identificación de la acción real basándose en motores cruzados
                char accion_str[15];
                if (error_giro_calc > 25.0)
                {
                    strcpy(accion_str, "IZQUIERDA");
                }
                else if (error_giro_calc < -25.0)
                {
                    strcpy(accion_str, "DERECHA");
                }
                else
                {
                    strcpy(accion_str, "ADELANTE");
                }

                // Despliegue de telemetría limpia por Bluetooth hacia la aplicación
                snprintf(bt_tx_buffer, sizeof(bt_tx_buffer),
                         "[Navix] Lat: %.6f, Lon: %.6f | Dist: %.1f m | Accion: %s\r\n",
                         lat_actual, lon_actual, distancia_calc, accion_str);

                uart_puts(UART_BT, bt_tx_buffer);
                ultimo_envio_telemetria = to_ms_since_boot(get_absolute_time());
            }
        }

        // 4. Transferencia segura de estados mediante memoria compartida
        uint32_t save = spin_lock_blocking(nav_lock);
        g_nav_data.gps_fijado = gps_listo;
        g_nav_data.meta_activa = local_meta_activa;
        g_nav_data.distancia = distancia_calc;
        g_nav_data.error_giro = error_giro_calc;
        spin_unlock(nav_lock, save);

        sleep_ms(50);
    }
}

// CORE 0: MÁQUINA DE ESTADOS Y MOTORES

int main()
{
    stdio_init_all();

    // Retardo inicial para estabilización eléctrica general
    sleep_ms(3000);

    motores_init();

    int spin_lock_num = spin_lock_claim_unused(true);
    nav_lock = spin_lock_instance(spin_lock_num);

    multicore_launch_core1(core1_entry);

    NavData local_nav;

    while (true)
    {
        // Sincronización cíclica de las variables de control
        uint32_t save = spin_lock_blocking(nav_lock);
        local_nav = g_nav_data;
        spin_unlock(nav_lock, save);

        // Control de los drivers de potencia (TB6612FNG) con lógica de giro invertida
        if (local_nav.meta_activa && local_nav.gps_fijado)
        {
            if (local_nav.distancia < 2.0)
            {
                motores_detener();
            }
            else
            {
                if (local_nav.error_giro > 25.0)
                {
                    motores_girar_izquierda(10000);
                }
                else if (local_nav.error_giro < -25.0)
                {
                    motores_girar_derecha(10000);
                }
                else
                {
                    motores_adelante(12000);
                }
            }
        }
        else
        {
            motores_detener();
        }

        sleep_ms(20);
    }
}