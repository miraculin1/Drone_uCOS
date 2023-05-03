#include "LED.h"

void initLED() {
  RCC_ENR |= 0x01;

  GPIOA_MODER &= ~(0x01 << 2 * 5);
  GPIOA_MODER |= (0x01 << 2 * 5);

  GPIOA_OType &= ~(0x1 << 5);

  GPIOA_Speed &= ~(0x01 << 2 * 5);
  GPIOA_Speed |= (0x01 << 2 * 5);

  GPIOA_PPMode &= ~(0x01 << 2 * 5);
  GPIOA_PPMode |= (0x01 << 2 * 5);
}

void LED_OFF() {
  GPIOA_OData &= ~(0x01 << 5);
}

void LED_ON() {
  GPIOA_OData |= (0x01 << 5);
}
