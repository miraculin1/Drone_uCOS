#ifndef __HMC5883_H
#define __HMC5883_H

#include "stm32f4xx.h"

#include "ekf.h"
void HMCWrite(uint8_t tar, uint8_t data);
void initHMC();
void MagmGuassData(double dest[3], double *bias);
extern double HMCmGaussPerLSB;

#define MAGCALSAMPLES 300

#endif
