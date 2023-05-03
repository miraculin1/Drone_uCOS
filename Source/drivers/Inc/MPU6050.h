#ifndef __MPU6050_H
#define __MPU6050_H

#include "IIC.h"
void initMPU6050();
void MPUReadData(uint16_t *x, uint16_t *y, uint16_t *z);
#endif
