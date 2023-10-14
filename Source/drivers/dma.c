#include "dma.h"

void DMAPrep() {
  RCC->AHB1ENR |= (0b1 << 21);

  // select channel
  DMA1_Stream0->CR |= (0b1 << 25);

  // set priority level
  DMA1_Stream0->CR |= (0b10 << 16);

  // selete mem increase
  DMA1_Stream0->CR |= (0b1 << 10);

  // choose direction
  DMA1_Stream0->CR &= ~(0b11 << 6);

  DMA1_Stream0->PAR = 0x40005410;

  // enable tranfer complete interrupt
  DMA1_Stream0->CR |= (0b1 << 4);

  // mem data size and preiph data size all default 8bit


  NVIC_EnableIRQ(DMA1_Stream0_IRQn);
}

void DMAStream0en() {
  DMA1_Stream0->CR |= (0b1);
}
