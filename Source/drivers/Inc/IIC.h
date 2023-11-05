#ifndef __IIC_H
#define __IIC_H
#include <stdint.h>
#include "ucos_ii.h"

void I2CDMAPrep();
void sendAdress();
void initIIC();
int IIC1_CheckStatus(uint16_t s1, uint16_t s2);
void IIC_WriteData(uint8_t SlaveAdd, uint8_t WriteAddr, uint8_t data);
void IIC_ReadData(uint32_t SlaveAdd, uint32_t tar, uint8_t *out);
void IICOR(uint32_t addr, uint32_t tarreg, uint8_t orval);
void IICNAND(uint32_t addr, uint32_t tarreg, uint8_t nandval);

void IICBurstRead(uint32_t addr, uint32_t startReg, uint32_t cnt,
                  uint8_t *datas);

typedef struct {
  // point to most recent data
  int8_t curVal;
  uint8_t needmag;

  void *rawbuf0;
  int32_t size0;
  OS_EVENT *sem0;

  void *rawbuf1;
  int32_t size1;
  OS_EVENT *sem1;

  uint32_t MaxSize;
  OS_EVENT *DMAsem;
} doubleBuf_t;

extern doubleBuf_t dbuf;
void exitDMA();
void initDMA(void *addr0, void *addr1, uint32_t size);
void IICDMARead();
#endif
