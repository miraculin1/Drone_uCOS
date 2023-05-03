#ifndef __USART_H
#define __USART_H

#include "stm32f4xx.h"
#include <stdint.h>

#define RCC_Base 0x40023800
#define RCC_AHB1 *(uint32_t *)(RCC_Base + 0x30)
#define RCC_APB1 *(uint32_t *)(RCC_Base + 0x40)
#define RCC_APB2 *(uint32_t *)(RCC_Base + 0x44)

#define GPIOB_Base 0x40020400
#define GPIOB_MODER *(uint32_t *)(GPIOB_Base + 0x00)
#define GPIOB_OTYPER *(uint32_t *)(GPIOB_Base + 0x04)
#define GPIOB_OSPEEDR *(uint32_t *)(GPIOB_Base + 0x08)
#define GPIOB_PUPDR *(uint32_t *)(GPIOB_Base + 0x0c)
#define GPIOB_AF_L *(uint32_t *)(GPIOB_Base + 0x20)
#define GPIOB_AF_H *(uint32_t *)(GPIOB_Base + 0x24)

#define USART1_FREQ 84000000
#define USART1_Base 0x40011000
#define USART1_SR *(uint32_t *)(USART1_Base + 0x00)
#define USART1_DR *(uint32_t *)(USART1_Base + 0x04)
#define USART1_BRR *(uint32_t *)(USART1_Base + 0x08)
#define USART1_CR1 *(uint32_t *)(USART1_Base + 0x0c)
#define USART1_CR2 *(uint32_t *)(USART1_Base + 0x10)
#define USART1_CR3 *(uint32_t *)(USART1_Base + 0x14)
#define USART1_GTPR *(uint32_t *)(USART1_Base + 0x18)

#define USART_TxE 0x080
#define USART_TC 0x040
#define USART_RxNE 0x020
#define USART_IDLE 0x010

void USendByte(uint8_t data);
void initUSART();
void USendStr(char *str);
void UReceByte(uint8_t *Byte);
void USendInt(uint32_t data);

#endif
