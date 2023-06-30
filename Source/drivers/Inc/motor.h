#ifndef __TIME_H
#define __TIME_H

#include "stm32f4xx.h"

#define RCC_Base 0x40023800
#define RCC_AHB1 *(uint32_t *)(RCC_Base + 0x30)
#define RCC_APB1 *(uint32_t *)(RCC_Base + 0x40)

#define GPIOC_Base 0x40020800
#define GPIOC_MODER *(uint32_t *)(GPIOC_Base + 0x00)
#define GPIOC_OTYPER *(uint32_t *)(GPIOC_Base + 0x04)
#define GPIOC_OSPEEDR *(uint32_t *)(GPIOC_Base + 0x08)
#define GPIOC_PUPDR *(uint32_t *)(GPIOC_Base + 0x0c)
#define GPIOC_AF_L *(uint32_t *)(GPIOC_Base + 0x20)
#define GPIOC_AF_H *(uint32_t *)(GPIOC_Base + 0x24)

#define TIM3FREQ 84000000
#define TIM3_Base 0x40000400
#define TIM3_CR1 *(uint32_t *)(TIM3_Base + 0x00)
#define TIM3_CR2 *(uint32_t *)(TIM3_Base + 0x04)

#define TIM3_CCMR1 *(uint32_t *)(TIM3_Base + 0x18)
#define TIM3_CCMR2 *(uint32_t *)(TIM3_Base + 0x1c)
#define TIM3_CCER *(uint32_t *)(TIM3_Base + 0x20)
#define TIM3_CCR1 *(uint32_t *)(TIM3_Base + 0x34)
#define TIM3_CNT *(uint32_t *)(TIM3_Base + 0x24)
#define TIM3_PSC *(uint32_t *)(TIM3_Base + 0x28)
#define TIM3_ARR *(uint32_t *)(TIM3_Base + 0x2c)
#define TIM3_ *(uint32_t *)(TIM3_Base + 0x)

void initTIM3PWM();
void setThro(uint16_t fourMotor[4]);
void initMotor();
void motorCalThro();
extern uint16_t fourMotor[4];
#endif
