#include "servo.h"
#include <stdio.h>

volatile bool g_flag_timer_servo = false;
static uint slice_num;
static uint channel;
volatile float current_angle = 90.0;
static float step = 2.0; // Incremento de grados por interrupción
volatile bool servo_sweep_mode = false; // Si es true, el servo hace un barrido continuo. Si es false, se queda fijo en el último ángulo.

bool servo_timer_callback(struct repeating_timer *t) {
    g_flag_timer_servo = true; // Solo avisamos al Core 1 que es hora de moverse
    return true; // true = mantener el timer corriendo
}

void servo_init() {
    printf("[Init] Iniciando Servo...\n");
    gpio_set_function(SERVO_PIN, GPIO_FUNC_PWM);
    slice_num = pwm_gpio_to_slice_num(SERVO_PIN);
    channel = pwm_gpio_to_channel(SERVO_PIN);

    printf("[Init] Configurando PWM para Servo en pin %d (slice %d, channel %d)\n", SERVO_PIN, slice_num, channel);
    pwm_set_clkdiv(slice_num, 64.0f);
    pwm_set_wrap(slice_num, 39062);
    pwm_set_enabled(slice_num, true);

    printf("[Init] Configurando Timer para Servo...\n");
    static struct repeating_timer servo_timer;
    add_repeating_timer_ms(20, servo_timer_callback, NULL, &servo_timer);

    printf("[Init] Servo inicializado.\n");
}

void servo_set_angle_fixed(float angle) {
    // Esta función mueve el servo a un ángulo específico y lo deja ahí
    current_angle = angle;
    uint32_t duty = 1000 + (uint32_t)(((angle - 10.0) / 160.0) * 3800);
    pwm_set_chan_level(slice_num, channel, (uint16_t)duty);
}

void servo_update_sweep() {

    if (!servo_sweep_mode) return;

    current_angle += step;

    // Lógica de rebote
    if (current_angle >= 170.0) {
        current_angle = 170.0;
        step = -1.3;
    } else if (current_angle <= 10.0) {
        current_angle = 10.0;    
        step = 1.3;
    }

    // 1000 = 0.5ms (posición 0°)
    // 4800 = 2.4ms (posición 180°)
    uint32_t duty = 1000 + (uint32_t)(((current_angle - 10.0) / 160.0) * 3800);
    
    pwm_set_chan_level(slice_num, channel, (uint16_t)duty);
}

float servo_get_angle() { 
    return current_angle; 
}