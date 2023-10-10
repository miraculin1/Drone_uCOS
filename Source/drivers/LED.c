#include "LED.h"

void initLED() {
  RCC->AHB1ENR |= 0x01;

  GPIOA->MODER &= ~(0x01 << 2 * 5);
  GPIOA->MODER |= (0x01 << 2 * 5);

  GPIOA->OTYPER &= ~(0x1 << 5);

  GPIOA->OSPEEDR &= ~(0x01 << 2 * 5);
  GPIOA->OSPEEDR |= (0x01 << 2 * 5);

  GPIOA->PUPDR &= ~(0x01 << 2 * 5);
  GPIOA->PUPDR |= (0x01 << 2 * 5);
}

void LED_OFF() {
  GPIOA->ODR &= ~(0x01 << 5);
}

void LED_ON() {
  GPIOA->ODR |= (0x01 << 5);
}
