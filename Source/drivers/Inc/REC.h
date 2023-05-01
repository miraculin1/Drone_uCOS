#ifndef _REC_H
#define _REC_H

#include "stm32f4xx.h"
#include "LED.h"

#define RCC_Base 0x40023800
#define RCC_AHB1_ENR *(uint32_t *)(RCC_Base + 0x30)
#define RCC_APB2_ENR *(uint32_t *)(RCC_Base + 0x44)

#define GPIOA_Base 0x40020000
#define GPIOA_MODER *(uint32_t *)(GPIOA_Base + 0x00)
#define GPIOA_OType *(uint32_t *)(GPIOA_Base + 0x04)
#define GPIOA_Speed *(uint32_t *)(GPIOA_Base + 0x08)
#define GPIOA_PPMode *(uint32_t *)(GPIOA_Base + 0x0C)
#define GPIOA_OData *(uint32_t *)(GPIOA_Base + 0x14)
#define GPIOA_AF_H *(uint32_t *)(GPIOA_Base + 0x24)

#define TIM1_Base 0x40010000
#define TIM1_CR1 *(uint32_t *)(TIM1_Base + 0x00)
#define TIM1_SMCR *(uint32_t *)(TIM1_Base + 0x08)
#define TIM1_CCMR1 *(uint32_t *)(TIM1_Base + 0x18)
#define TIM1_CCER *(uint32_t *)(TIM1_Base + 0x20)
#define TIM1_PSC *(uint32_t *)(TIM1_Base + 0x28)
#define TIM1_ARR *(uint32_t *)(TIM1_Base + 0x2c)
#define TIM1_DIER *(uint32_t *)(TIM1_Base + 0x0c)
#define TIM1_CCR1 *(uint32_t *)(TIM1_Base + 0x34)
#define TIM1_CNT *(uint32_t *)(TIM1_Base + 0x24)
#define TIM1_ *(uint32_t *)(TIM1_Base + 0x)
#define TIM1_ *(uint32_t *)(TIM1_Base + 0x)

#define NVIC_Base 0xe000e000
#define NVIC_ISER1 *(uint32_t *)(NVIC_Base + 0x100 + 0x4 * 0)
#define NVIC_IABR1 *(uint32_t *)(NVIC_Base + 0x300 + 04 * 0)

// used to store the raw data from PPM
struct RecData {
  int now;
  int chs[9];
  int linedUp;
};

extern struct RecData recData;

// int rawCh[9];
void initRec();
#endif
