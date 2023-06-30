#ifndef __HMC5883_H
#define __HMC5883_H

#include "stm32f4xx.h"

void HMCWrite(uint8_t tar, uint8_t data);
void initHMC();
void HMCReadData(int16_t out[3]);

#endif
