#include "Includes.h"

void TIM1_CC_IRQHandler() {
  OSIntEnter();
  // reset CNT after each interrupt TIM1->CNT = 0;
  TIM1->CNT = 0;

  // line up when possible
  if (!recData.linedUp) {
    if (TIM1->CCR1 >= 4000) {
      recData.linedUp = 1;
    }
  } else {
    recData.chs[recData.now] = TIM1->CCR1;
    if (recData.now == 8) {
      recData.now = 0;
      // check whether linedUp or not
      if (recData.chs[8] < 3000) {
        recData.linedUp = 0;
      } else {
        recData.valid = 1;
      }
    } else {
      recData.now++;
    }
  }

  OSIntExit();
}

static void dmaHMC() {
  // release sem
  if (dbuf.curVal == -1) {
    // init
    OSSemPost(dbuf.sem0);
  } else if (dbuf.curVal == 0) {
    // 0 ok, written 1
    OSSemPost(dbuf.sem1);
  } else { // 1 ok, written 0
    OSSemPost(dbuf.sem0);
  }

  // init data read for mag
  I2CDMAPrep();
  uint8_t errno;
  // enable dma in i2c
  I2C1->CR2 |= (0b1 << 11);
  I2C1->CR2 |= (0b1 << 12);

  if (dbuf.curVal == -1) {
    // init
    OSSemPend(dbuf.sem0, 0, &errno);
    dbuf.size0 += 6;
    DMA1_Stream0->M0AR = (uint32_t)dbuf.rawbuf0 + 14;
  } else if (dbuf.curVal == 0) {
    // write 1
    OSSemPend(dbuf.sem1, 0, &errno);
    dbuf.size1 += 6;
    DMA1_Stream0->M0AR = (uint32_t)dbuf.rawbuf1 + 14;
  } else {
    // write 0
    OSSemPend(dbuf.sem0, 0, &errno);
    dbuf.size0 += 6;
    DMA1_Stream0->M0AR = (uint32_t)dbuf.rawbuf0 + 14;
  }

  DMA1_Stream0->NDTR = 6;
  DMA1_Stream0->CR |= (0b1);

  uint8_t addr = 0x3c;
  uint8_t startReg = 0x03;
  while (IIC1_CheckStatus(0x0000, 0x0002))
    ;

  I2C1->CR1 |= I2C_CR1_START;
  while (!IIC1_CheckStatus(I2C_SR1_SB, 0x0))
    ;
  I2C1->DR = (addr & ~(0b1));
  while (!IIC1_CheckStatus(I2C_SR1_ADDR, 0x0))
    ;

  I2C1->DR = startReg;
  while (!IIC1_CheckStatus(I2C_SR1_BTF, 0x0))
    ;

  I2C1->CR1 |= I2C_CR1_START;
  while (!IIC1_CheckStatus(I2C_SR1_SB, 0x0))
    ;

  I2C1->CR1 |= I2C_CR1_ACK;
  I2C1->DR = addr | 0b1;
  while (!IIC1_CheckStatus(I2C_SR1_ADDR, 0x0))
    ;

  dbuf.needmag = 0;
}

void DMA1_Stream0_IRQHandler() {
  I2C1->CR1 |= I2C_CR1_STOP;
  I2C1->CR1 |= I2C_CR1_STOP;
  DMA1->LIFCR |= (0b1 << 5);
  if (dbuf.needmag == 1) {
    dmaHMC();
  } else {
    exitDMA();
  }
}

void DMA2_Stream7_IRQHandler() {
  // disable UART dma tx
  USART1->CR3 &= ~(0b1 << 7);
  DMA2->HIFCR |= (0b1 << 27);

  while (!USART1CheckStatus(USART_TC))
    ;
  // disable TE
  USART1->CR1 &= ~(0x1 << 3);
  OSSemPost(uart1Sem);
}

void DMA2_Stream5_IRQHandler() {
  if (DMA2->HISR & (0b1 << 11)) {
    // tc
  } else if (DMA2->HISR & (0b1 << 10)) {
    // ht
  }
}

void USART1_IRQHandler() {
  if (USART1->SR & (0b1 << 5)) {
    char tmp;
    tmp = USART1->DR;
    // rxne
    // read dr to clear flag
    if (puRecBuf - puRecBufInit < MAXBUFLEN) {
      printf("%c", tmp);
      if (tmp == '\n' || tmp == '\r') {
        *puRecBuf++ = '\0';
        OSSemPost(termMsgSem);
      } else {
        *puRecBuf++ = tmp;
      }
    } else {
      // overflow & reset
      puRecBuf = puRecBufInit;
      // read to clear flag
      *puRecBuf = USART1->DR;
    }
  }
}
