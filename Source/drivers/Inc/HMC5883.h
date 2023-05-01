#ifndef __HMC5883_H
#define __HMC5883_H

#include "stm32f4xx.h"

#define HMCAdd 0x3c

void HMCWrite(uint8_t tar, uint8_t data);
void initHMC();
void HMCReadData(uint16_t *x, uint16_t *y, uint16_t *z);
void HMCReadByte(uint8_t tar, uint8_t *out);

#endif
