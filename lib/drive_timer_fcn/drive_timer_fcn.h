#ifndef DRIVE_TIMER_H
#define DRIVE_TIMER_H

#include <Arduino.h>
#include <motor_control.h>

extern const int mtr_power;
extern int direction;

void encoder_init(void);
void create_timer(void);

#endif