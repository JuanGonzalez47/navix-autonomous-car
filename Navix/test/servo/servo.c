#include "servo.h"
#include <stdio.h>

static uint slice_num;
static uint channel;
float current_angle = 0.0;
static float step = 2.0; // Incremento de grados por interrupción

void servo_init() {
    gpio_set_function(SERVO_PIN, GPIO_FUNC_PWM);
    slice_num = pwm_gpio_to_slice_num(SERVO_PIN);
    channel = pwm_gpio_to_channel(SERVO_PIN);

    pwm_set_clkdiv(slice_num, 64.0f);
    pwm_set_wrap(slice_num, 39062);
    pwm_set_enabled(slice_num, true);
}

void servo_update_sweep() {
    current_angle += step;

    // Lógica de rebote
    if (current_angle >= 170.0) {
        current_angle = 170.0;
        step = -2.0;
    } else if (current_angle <= 10.0) {
        current_angle = 10.0;    
        step = 2.0;
    }

    // 1000 = 0.5ms (posición 0°)
    // 4800 = 2.4ms (posición 180°)
    uint32_t duty = 1000 + (uint32_t)(((current_angle - 10.0) / 160.0) * 3800);
    
    pwm_set_chan_level(slice_num, channel, (uint16_t)duty);
}

float servo_get_angle() { 
    return current_angle; 
}