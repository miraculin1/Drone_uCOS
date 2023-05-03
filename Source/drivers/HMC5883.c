#include "Includes.h"

// 8-avg 15Hz normal measurement continuos
// need to delay at least 65ms before write in
void initHMC() {
  OSTimeDly(10);
  HMCWrite(0x00, 0x70);
  HMCWrite(0x01, 0xa0);
  HMCWrite(0x02, 0x00);
}
void HMCWrite(uint8_t tar, uint8_t data) { IIC_WriteData(HMCAdd, tar, data); }

/*
 * HMC 当只读取了一个寄存器的值之后，会锁住所有寄存器，
 * 直到所有寄存器都被读取。
 * 或者模式发生变化
 */

// TODO rewrite the Read
void HMCReadByte(uint8_t tar, uint8_t *out) {
  uint8_t RDY = 0;
  while ((RDY & 0x01) == 0) {
    IIC_ReadData(HMCAdd, 0x09, &RDY);
  }
  IIC_ReadData(HMCAdd, tar, out);
}

void HMCJustListen(uint8_t *data) {
  IIC1_CR1 |= (0x1 << 8);
  // Check SB BUSY MASTER
  while (!IIC1_CheckStatus(0x1, 0x3))
    ;

  IIC1_DR = HMCAdd + 1;
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
  *data = IIC1_DR;
}

void HMCReadData(uint16_t *x, uint16_t *y, uint16_t *z) {
  uint8_t raw[6];
  int now = 0;
  // this should set te pointer to begin of data
  // and read the first data
  uint8_t add = 0x03;
  // read into the array
  while (now < 6) {
    HMCReadByte(add, &raw[now]);
    now++;
    add++;
  }

  now = 0;
  *x = 0;
  *x |= (raw[now++] << 8);
  *x |= (raw[now++]);
  *y = 0;
  *y |= (raw[now++] << 8);
  *y |= (raw[now++]);
  *z = 0;
  *z |= (raw[now++] << 8);
  *z |= (raw[now++]);
}
