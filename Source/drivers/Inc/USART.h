#ifndef __USART_H
#define __USART_H

#include "stm32f4xx.h"
#include <stdint.h>

#define USART1_FREQ 84000000

#define USART_TxE 0x080
#define USART_TC 0x040
#define USART_RxNE 0x020
#define USART_IDLE 0x010

void initUSART();
// void USendByte(const uint8_t data);
// void USendStr(const char *str);
// void USendStrDMA(const char *str);
uint8_t USART1CheckStatus(uint16_t sr);
void os_writeByte(char byte);
void os_witeStr(char *str, int len);
extern char *puRecBuf;
extern char *const puRecBufInit;

#define MAXBUFLEN 100

#include "ucos_ii.h"
extern OS_EVENT *uart1Sem;

#endif
