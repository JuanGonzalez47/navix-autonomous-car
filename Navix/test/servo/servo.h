#ifndef SERVO_H
#define SERVO_H

#include "pico/stdlib.h"
#include "hardware/pwm.h"

#define SERVO_PIN 15

void servo_init();
void servo_update_sweep();
float servo_get_angle();

#endif