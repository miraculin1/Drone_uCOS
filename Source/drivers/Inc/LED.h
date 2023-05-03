#ifndef __LED_H
#define __LED_H
#include <stdint.h>
#define RCC_Base 0x40023800
#define RCC_ENR *(uint32_t *)(RCC_Base + 0x30)

#define GPIOA_Base 0x40020000
#define GPIOA_MODER *(uint32_t *)(GPIOA_Base + 0x00)
#define GPIOA_OType *(uint32_t *)(GPIOA_Base + 0x04)
#define GPIOA_Speed *(uint32_t *)(GPIOA_Base + 0x08)
#define GPIOA_PPMode *(uint32_t *)(GPIOA_Base + 0x0C)
#define GPIOA_OData *(uint32_t *)(GPIOA_Base + 0x14)

void initLED();
void LED_OFF();
void LED_ON();
#endif
