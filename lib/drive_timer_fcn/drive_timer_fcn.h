#ifndef DRIVE_TIMER_H
#define DRIVE_TIMER_H

#include <Arduino.h>
#include <motor_control.h>

extern const int FORWARD_DIR;
extern const int BACKWARD_DIR;
extern const int LEFT_DIR;
extern const int RIGHT_DIR;

extern int direction;
extern const int mtr_power;

void encoder_init(void);
void create_timer(void);

#endif