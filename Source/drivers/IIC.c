#include "IIC.h"
#include <stdint.h>

int IIC1_CheckStatus(uint16_t s1, uint16_t s2) {
  if (((IIC1_SR1 & s1) == s1) && ((IIC1_SR2 & s2) == s2))
    return 1;
  return 0;
}


// using pb8 SCL
// pb9 SDL
void initIIC() {
  // enable clock
  RCC_AHB1 |= (0x1 << 1);
  RCC_APB1 |= (0x1 << 21);

  // set GPIOB
  GPIOB_MODER &= ~(0x3 << (8 * 2));
  GPIOB_MODER &= ~(0x3 << (9 * 2));
  GPIOB_MODER |= (0x2 << (8 * 2));
  GPIOB_MODER |= (0x2 << (9 * 2));

  GPIOB_OTYPER |= (0x1 << 8);
  GPIOB_OTYPER |= (0x1 << 9);

  GPIOB_OSPEEDR |= (0x3 << (2 * 8));
  GPIOB_OSPEEDR |= (0x3 << (2 * 9));

  GPIOB_PUPDR &= ~(0x3 << (2 * 8));
  GPIOB_PUPDR &= ~(0x3 << (2 * 9));
  GPIOB_PUPDR |= (0x1 << 8);
  GPIOB_PUPDR |= (0x1 << 9);

  GPIOB_AF_H &= ~(0xf << (4 * 0));
  GPIOB_AF_H &= ~(0xf << (4 * 1));
  GPIOB_AF_H |= (0x4 << (4 * 0));
  GPIOB_AF_H |= (0x4 << (4 * 1));

  // set IIC
  IIC1_CR2 &= ~(0x3f);
  IIC1_CR2 |= (42);

  IIC1_CCR &= ~(0x1 << 15);
  IIC1_CCR |= (210);

  IIC1_TRISE |= (43);

  IIC1_CR1 |= (0x1);
}

/*
 * IIC 对于每一步的 SRx 的变换在手册里有详细的描述，那么如果
 * 不按照那个顺序去检查（或者访问）相关寄存器会导致错误
 */
void IIC_WriteData(uint8_t SlaveAdd, uint8_t WriteAddr, uint8_t data) {
  IIC1_CR1 |= (0x1 << 8);

  /* IIC_EVENT_MASTER_MODE_SELECT:
          BUSY, MSL and SB flag */
  while (!IIC1_CheckStatus(0x0001, 0x0003))
    ;

  // call slave address
  IIC1_DR = SlaveAdd; // 11010000

  /* IIC_EVENT_MASTER_TRANSMITTER_MODE_SELECTED:
          BUSY, MSL, ADDR, TXE and TRA flags */
  while (!IIC1_CheckStatus(0x0082, 0x0007))
    ;
  // slave register address
  IIC1_DR = WriteAddr;

  /* IIC_EVENT_MASTER_BYTE_TRANSMITTED:
          TRA, BUSY, MSL, TXE and BTF flags */
  while (!IIC1_CheckStatus(0x0084, 0x0007))
    ;

  IIC1_DR = data;
  while (!IIC1_CheckStatus(0x0084, 0x0007))
    ;

  // set stop
  IIC1_CR1 |= (0x1 << 9);
}

void IIC_ReadData(uint32_t SlaveAdd, uint32_t tar, uint8_t *out) {
  while (IIC1_CheckStatus(0x0000, 0x0002))
    ;

  IIC1_CR1 |= (0x1 << 8);
  // Check SB BUSY MASTER
  while (!IIC1_CheckStatus(0x1, 0x3))
    ;
  IIC1_DR = SlaveAdd;

  // Check ADDR TxE
  while (!IIC1_CheckStatus(0x82, 0x7))
    ;

  IIC1_CR1 |= 0x1;
  IIC1_DR = tar;
  // Check TxE BTF
  while (!IIC1_CheckStatus(0x84, 0x7))
    ;

  IIC1_CR1 |= (0x1 << 8);
  // Check SB BUSY MASTER
  while (!IIC1_CheckStatus(0x1, 0x3))
    ;

  IIC1_DR = SlaveAdd + 1;
  // enable ACK
  IIC1_CR1 |= (0x1 << 10);

  // Check ADDR
  while (!IIC1_CheckStatus(0x2, 0x3))
    ;

  // Check RxNE
  while (!IIC1_CheckStatus(0x1 << 6, 0x3))
    ;

  IIC1_CR1 |= (0x1 << 9);
  // disable ACK
  IIC1_CR1 &= ~(0x1 << 10);
  *out = IIC1_DR;
}
