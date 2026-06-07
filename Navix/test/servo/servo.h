#ifndef SERVO_H
#define SERVO_H

#include "pico/stdlib.h"
#include "hardware/pwm.h"

#define SERVO_PIN 15

extern volatile bool g_flag_timer_servo;
extern volatile bool servo_sweep_mode;
extern volatile float current_angle;

void servo_set_angle_fixed(float angle);
void servo_init();
void servo_update_sweep();
float servo_get_angle();
bool servo_timer_callback(struct repeating_timer *t);

#endif