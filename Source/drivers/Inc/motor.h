#ifndef __TIME_H
#define __TIME_H

#include "stm32f4xx.h"

#define TIM3FREQ 84000000

void initTIM3PWM();
void setThro(uint16_t fourMotor[4]);
void initMotor();
void motorCalThro();
extern uint16_t fourMotor[4];
#endif
