#ifndef __IIC_H
#define __IIC_H
#include "stm32f4xx.h"
#include <stdint.h>

void sendAdress();
void initIIC();
int IIC1_CheckStatus(uint16_t s1, uint16_t s2);
void IIC_WriteData(uint8_t SlaveAdd, uint8_t WriteAddr, uint8_t data);
void IIC_ReadData(uint32_t SlaveAdd, uint32_t tar, uint8_t *out);
void IICOR(uint32_t addr, uint32_t tarreg, uint8_t orval);
void IICNAND(uint32_t addr, uint32_t tarreg, uint8_t nandval);

void IICBurstRead(uint32_t addr, uint32_t startReg, uint32_t cnt,
                  uint8_t *datas);
#endif
