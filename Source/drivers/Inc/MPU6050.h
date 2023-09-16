#ifndef __MPU6050_H
#define __MPU6050_H

#include "IIC.h"
#include "ekf.h"
void initMPU6050();
void GyroDpSData(double out[3], rawBias_t bias);
void AccGData(double out[3], rawBias_t bias);

void enBypass();
void disenBypass();
uint8_t whoami();

extern double GyroLSBPerDegree, AccLSBPerG;
#endif
