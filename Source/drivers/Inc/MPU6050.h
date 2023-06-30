#ifndef __MPU6050_H
#define __MPU6050_H

#include "IIC.h"
void initMPU6050();
void GyroData(int16_t data[3]);
void AccData(int16_t data[3]);

void enBypass();
void disenBypass();
uint8_t whoami();

extern uint32_t GyroScale, AccScale;
#endif
