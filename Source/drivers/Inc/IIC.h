#ifndef __IIC_H
#define __IIC_H
#include "stm32f4xx.h"
#include <stdint.h>

#define RCC_Base 0x40023800
#define RCC_AHB1 *(uint32_t *)(RCC_Base + 0x30)
#define RCC_APB1 *(uint32_t *)(RCC_Base + 0x40)

#define GPIOB_Base 0x40020400
#define GPIOB_MODER *(uint32_t *)(GPIOB_Base + 0x00)
#define GPIOB_OTYPER *(uint32_t *)(GPIOB_Base + 0x04)
#define GPIOB_OSPEEDR *(uint32_t *)(GPIOB_Base + 0x08)
#define GPIOB_PUPDR *(uint32_t *)(GPIOB_Base + 0x0c)
#define GPIOB_AF_L *(uint32_t *)(GPIOB_Base + 0x20)
#define GPIOB_AF_H *(uint32_t *)(GPIOB_Base + 0x24)

#define IIC1_Base 0x40005400
#define IIC1_CR1 *(uint32_t *)(IIC1_Base + 0x00)
#define IIC1_CR2 *(uint32_t *)(IIC1_Base + 0x04)
#define IIC1_DR *(uint32_t *)(IIC1_Base + 0x10)
#define IIC1_SR1 *(uint32_t *)(IIC1_Base + 0x14)
#define IIC1_SR2 *(uint32_t *)(IIC1_Base + 0x18)
#define IIC1_CCR *(uint32_t *)(IIC1_Base + 0x1c)
#define IIC1_TRISE *(uint32_t *)(IIC1_Base + 0x20)
#define IIC1_FLTR *(uint32_t *)(IIC1_Base + 0x24)

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
