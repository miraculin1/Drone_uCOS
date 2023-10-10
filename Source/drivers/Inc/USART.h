#ifndef __USART_H
#define __USART_H

#include "stm32f4xx.h"
#include <stdint.h>

#define USART1_FREQ 84000000

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
