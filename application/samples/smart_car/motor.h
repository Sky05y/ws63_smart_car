#ifndef MOTOR_H
#define MOTOR_H

#include <stdint.h>

void motor_init(void);
void set_left_speed(uint8_t percent);   // 0~100
void set_right_speed(uint8_t percent);  // 0~100

#endif
