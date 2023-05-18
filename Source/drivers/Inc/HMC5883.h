#ifndef __HMC5883_H
#define __HMC5883_H

#include "stm32f4xx.h"

void HMCWrite(uint8_t tar, uint8_t data);
void initHMC();
void HMCReadData(uint16_t *x, uint16_t *y, uint16_t *z);
void HMCRead(uint8_t tar, uint8_t *out);

#endif
