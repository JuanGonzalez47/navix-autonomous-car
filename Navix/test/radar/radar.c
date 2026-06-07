#include "radar.h"
#include "hardware/gpio.h"
#include "hardware/timer.h"
#include "hardware/sync.h"
#include <stdio.h>

static volatile bool medicion_lista = false;
static volatile uint64_t tiempo_inicio = 0;
static volatile uint64_t tiempo_fin = 0;

bool radar_timer_callback(struct repeating_timer *t) {
    gpio_put(TRIG_PIN, 1);
    busy_wait_us(10);
    gpio_put(TRIG_PIN, 0);
    return true;
}

void radar_echo_callback(uint gpio, uint32_t events) {
    if (events & GPIO_IRQ_EDGE_RISE) {
        tiempo_inicio = time_us_64();
    } else if (events & GPIO_IRQ_EDGE_FALL) {
        tiempo_fin = time_us_64();
        medicion_lista = true;
    }
}

void radar_init() {
    printf("[Init] Iniciando Radar...\n");
    gpio_init(TRIG_PIN);
    gpio_set_dir(TRIG_PIN, GPIO_OUT);
    gpio_put(TRIG_PIN, 0);
    
    gpio_init(ECHO_PIN);
    gpio_set_dir(ECHO_PIN, GPIO_IN);
    gpio_pull_down(ECHO_PIN); 
    
    printf("[Init] Configurando IRQs de Radar...\n");
    gpio_set_irq_enabled_with_callback(ECHO_PIN, GPIO_IRQ_EDGE_RISE | GPIO_IRQ_EDGE_FALL, true, &radar_echo_callback);
    
    static struct repeating_timer radar_timer;
    printf("[Init] Configurando Timer de Radar...\n");
    add_repeating_timer_ms(500, radar_timer_callback, NULL, &radar_timer);
    printf("[Init] Radar finalizado.\n");
}

bool radar_esta_listo() {
    uint32_t status = save_and_disable_interrupts();
    bool estado = medicion_lista;
    restore_interrupts(status);
    return estado;
}

float radar_leer_cm() {
    uint32_t status = save_and_disable_interrupts();
    medicion_lista = false;
    uint64_t inicio = tiempo_inicio;
    uint64_t fin = tiempo_fin;
    restore_interrupts(status);
    
    return ((float)(fin - inicio) * 0.0343f) / 2.0f;
}