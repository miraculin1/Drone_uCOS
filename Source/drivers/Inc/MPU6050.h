#ifndef __MPU6050_H
#define __MPU6050_H

#include "IIC.h"
#include "ekf.h"
void initMPU6050();
void GyroRadpSData(float out[3], float *bias);
void GyroDpSData(float out[3], float *bias);
void AccGData(float out[3], float *bias);

void enBypass();
void disenBypass();
uint8_t whoami();

extern float GyroLSBPerDegree, AccLSBPerG;
#endif
