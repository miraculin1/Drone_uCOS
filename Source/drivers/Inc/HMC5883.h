#ifndef __HMC5883_H
#define __HMC5883_H

#include "stm32f4xx.h"

void HMCWrite(uint8_t tar, uint8_t data);
void initHMC();
void MagmGuassData(float dest[3], float *bias);
extern float HMCmGaussPerLSB;

#define MAGCALSAMPLES 300

#endif
