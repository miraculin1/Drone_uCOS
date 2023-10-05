#ifndef __MPU6050_H
#define __MPU6050_H

#include "IIC.h"
#include "ekf.h"
void initMPU6050();
void GyroRadpSData(double out[3], double *bias);
void GyroDpSData(double out[3], double *bias);
void AccGData(double out[3], double *bias);

void enBypass();
void disenBypass();
uint8_t whoami();

extern double GyroLSBPerDegree, AccLSBPerG;
#endif
