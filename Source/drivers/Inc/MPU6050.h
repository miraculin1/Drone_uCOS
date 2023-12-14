#ifndef __MPU6050_H
#define __MPU6050_H

#include <stdint.h>
void initMPU6050();
void GyroRadpSData(float *out, float *bias);
void GyroDpSData(float *out, float *bias);
void AccGData(float *out, float *bias);

void enBypass();
void disenBypass();
uint8_t whoami();

extern float GyroLSBPerDegree, AccLSBPerG;
#endif
