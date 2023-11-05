#include "IIC.h"
#include "stm32f4xx.h"
#include <stdbool.h>
#include <stdint.h>

doubleBuf_t dbuf;

int IIC1_CheckStatus(uint16_t s1, uint16_t s2) {
  if (((I2C1->SR1 & s1) == s1) && ((I2C1->SR2 & s2) == s2))
    return 1;
  return 0;
}

// using pb8 SCL
// pb9 SDL
void initIIC() {
  // enable clock
  RCC->AHB1ENR |= (0x1 << 1);
  RCC->APB1ENR |= (0x1 << 21);

  // set GPIOB
  GPIOB->MODER &= ~(0x3 << (8 * 2));
  GPIOB->MODER &= ~(0x3 << (9 * 2));
  GPIOB->MODER |= (0x2 << (8 * 2));
  GPIOB->MODER |= (0x2 << (9 * 2));

  GPIOB->OTYPER |= (0x1 << 8);
  GPIOB->OTYPER |= (0x1 << 9);

  GPIOB->OSPEEDR |= (0x3 << (2 * 8));
  GPIOB->OSPEEDR |= (0x3 << (2 * 9));

  GPIOB->PUPDR &= ~(0x3 << (2 * 8));
  GPIOB->PUPDR &= ~(0x3 << (2 * 9));
  GPIOB->PUPDR |= (0x1 << 8);
  GPIOB->PUPDR |= (0x1 << 9);

  GPIOB->AFR[1] &= ~(0xf << (4 * 0));
  GPIOB->AFR[1] &= ~(0xf << (4 * 1));
  GPIOB->AFR[1] |= (0x4 << (4 * 0));
  GPIOB->AFR[1] |= (0x4 << (4 * 1));

  // set IIC
  I2C1->CR2 &= ~(0x3f);
  I2C1->CR2 |= (42);

  I2C1->CCR &= ~(0x1 << 15);
  I2C1->CCR |= (210);

  I2C1->TRISE |= (43);

  I2C1->CR1 |= (0x1);
}

/*
 * IIC 对于每一步的 SRx 的变换在手册里有详细的描述，那么如果
 * 不按照那个顺序去检查（或者访问）相关寄存器会导致错误
 */
void IIC_WriteData(uint8_t SlaveAdd, uint8_t WriteAddr, uint8_t data) {
  I2C1->CR1 |= (0x1 << 8);

  /* IIC_EVENT_MASTER_MODE_SELECT:
          BUSY, MSL and SB flag */
  while (!IIC1_CheckStatus(0x0001, 0x0003))
    ;

  // call slave address
  I2C1->DR = SlaveAdd; // 11010000

  /* IIC_EVENT_MASTER_TRANSMITTER_MODE_SELECTED:
          BUSY, MSL, ADDR, TXE and TRA flags */
  while (!IIC1_CheckStatus(0x0082, 0x0007))
    ;
  // slave register address
  I2C1->DR = WriteAddr;

  /* IIC_EVENT_MASTER_BYTE_TRANSMITTED:
          TRA, BUSY, MSL, TXE and BTF flags */
  while (!IIC1_CheckStatus(0x0084, 0x0007))
    ;

  I2C1->DR = data;
  while (!IIC1_CheckStatus(0x0084, 0x0007))
    ;

  // set stop
  I2C1->CR1 |= (0x1 << 9);
}

void IIC_ReadData(uint32_t SlaveAdd, uint32_t tar, uint8_t *out) {
  while (IIC1_CheckStatus(0x0000, 0x0002))
    ;

  I2C1->CR1 |= (0x1 << 8);
  // Check SB BUSY MASTER
  while (!IIC1_CheckStatus(0x1, 0x3))
    ;
  I2C1->DR = SlaveAdd;

  // Check ADDR TxE
  while (!IIC1_CheckStatus(0x82, 0x7))
    ;

  I2C1->CR1 |= 0x1;
  I2C1->DR = tar;
  // Check TxE BTF
  while (!IIC1_CheckStatus(0x84, 0x7))
    ;

  I2C1->CR1 |= (0x1 << 8);
  // Check SB BUSY MASTER
  while (!IIC1_CheckStatus(0x1, 0x3))
    ;

  I2C1->DR = SlaveAdd + 1;
  // enable ACK
  I2C1->CR1 |= (0x1 << 10);

  // Check ADDR
  while (!IIC1_CheckStatus(0x2, 0x3))
    ;

  // Check RxNE
  while (!IIC1_CheckStatus(0x1 << 6, 0x3))
    ;

  I2C1->CR1 |= (0x1 << 9);
  // disable ACK
  I2C1->CR1 &= ~(0x1 << 10);
  *out = I2C1->DR;
}

void IICOR(uint32_t addr, uint32_t tarreg, uint8_t orval) {
  uint8_t tmp;
  IIC_ReadData(addr, tarreg, &tmp);
  tmp |= orval;
  IIC_WriteData(addr, tarreg, tmp);
}

void IICNAND(uint32_t addr, uint32_t tarreg, uint8_t nandval) {
  uint8_t tmp;
  IIC_ReadData(addr, tarreg, &tmp);
  tmp &= ~(nandval);
  IIC_WriteData(addr, tarreg, tmp);
}

void IICBurstRead(uint32_t addr, uint32_t startReg, uint32_t cnt,
                  uint8_t *datas) {
  uint8_t *temp = datas;

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

  while (!IIC1_CheckStatus(I2C_SR1_RXNE, 0x0))
    ;

  *(temp++) = I2C1->DR;
  while (cnt > 1) {
    while (!IIC1_CheckStatus(I2C_SR1_RXNE, 0x0))
      ;
    *(temp++) = I2C1->DR;
    cnt--;
  }
  I2C1->CR1 |= I2C_CR1_STOP;
  I2C1->CR1 &= ~(I2C_CR1_ACK);
  while (!IIC1_CheckStatus(I2C_SR1_RXNE, 0x0))
    ;
  *(temp++) = I2C1->DR;
  cnt--;
}

void I2CDMAPrep() {
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

void initDMA(void *addr0, void *addr1, uint32_t size) {
  dbuf.rawbuf0 = addr0;
  dbuf.size0 = -1;
  dbuf.sem0 = OSSemCreate(1);

  dbuf.rawbuf1 = addr1;
  dbuf.size1 = -1;
  dbuf.sem1 = OSSemCreate(1);

  dbuf.MaxSize = size;
  dbuf.curVal = -1;
  dbuf.needmag = 1;
  dbuf.DMAsem = OSSemCreate(1);
}


// TODO: this code using OS sem, 
// shall be isolated from basic driver
static void enterDMA(uint16_t cnt) {
  uint8_t errno;
  OSSemPend(dbuf.DMAsem, 0, &errno);
  I2CDMAPrep();
  // enable dma in i2c
  I2C1->CR2 |= (0b1 << 11);
  I2C1->CR2 |= (0b1 << 12);

  if (dbuf.curVal == -1) {
    // init
    OSSemPend(dbuf.sem0, 0, &errno);
    dbuf.size0 = cnt;
    DMA1_Stream0->M0AR = (uint32_t)dbuf.rawbuf0;
  } else if (dbuf.curVal == 0) {
    // write 1
    OSSemPend(dbuf.sem1, 0, &errno);
    dbuf.size1 = cnt;
    DMA1_Stream0->M0AR = (uint32_t)dbuf.rawbuf1;
  } else {
    // write 0
    OSSemPend(dbuf.sem0, 0, &errno);
    dbuf.size0 = cnt;
    DMA1_Stream0->M0AR = (uint32_t)dbuf.rawbuf0;
  }

  DMA1_Stream0->NDTR = cnt;
  DMA1_Stream0->CR |= (0b1);
}

// TODO: this code using OS sem, 
// shall be isolated from basic driver
void exitDMA() {
  if (dbuf.curVal == -1) {
    // init
    dbuf.curVal = 0;
    OSSemPost(dbuf.sem0);
  } else if (dbuf.curVal == 0) {
    // 0 ok, written 1
    dbuf.curVal = 1;
    OSSemPost(dbuf.sem1);
  } else { // 1 ok, written 0
    dbuf.curVal = 0;
    OSSemPost(dbuf.sem0);
  }
  I2C1->CR2 &= ~(0b1 << 11);
  I2C1->CR2 &= ~(0b1 << 12);
  dbuf.needmag = 1;
  OSSemPost(dbuf.DMAsem);
}

// TODO: this code using OS sem, 
// shall be isolated from basic driver
void IICDMARead() {
  // set to read MPU data
  uint8_t addr = 0xd0;
  uint8_t startReg = 0x3b;
  uint8_t cnt = 14;
  enterDMA(cnt);
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
}
