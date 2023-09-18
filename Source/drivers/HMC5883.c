#include "Includes.h"
#define HMCAdd 0x3c

double HMCmGaussPerLSB = 0;

// 8-avg 15Hz normal measurement continuos
// need to delay at least 65ms before write in
void initHMC() {
  OSTimeDly(10);
  HMCWrite(0x00, 0x70);
  HMCWrite(0x01, 0x00);
  HMCWrite(0x02, 0x00);
  HMCmGaussPerLSB = 0.73;
}
void HMCWrite(uint8_t tarreg, uint8_t data) {
  IIC_WriteData(HMCAdd, tarreg, data);
}

/*
 * HMC 当只读取了一个寄存器的值之后，会锁住所有寄存器，
 * 直到所有寄存器都被读取。
 * 或者模式发生变化
 */

// rewrite the Read
void HMCRead(uint8_t tar, uint8_t *out) {
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

void MagRawData(int16_t out[3]) {
  uint8_t raw[6];
  IICBurstRead(HMCAdd, 0x03, 6, raw);

  for (int i = 0; i < 3; i++) {
    out[i] = (raw[2 * i] << 8) | raw[2 * i + 1];
  }
  uint16_t tmp = out[1];
  out[1] = out[2];
  out[2] = tmp;
}

void MagGuassData(double dest[3], Bias_t bias) {
  int16_t data[3];
  MagRawData(data);
  for (int i = 0; i < 3; i++) {
    dest[i] = ((double)data[i] * HMCmGaussPerLSB / 1000 + bias[i]);
  }
}

// TODO: implimant the RLS algorisim to get a "online" calibration
void HMCHardCal(EKF_t *now) {
  static double lamda = 0.98;
  double theta[4] = {0};
  double P[4][4];

  for (int i = 0; i < MAGCALSAMPLES; ++i) {
    // take single sample
    int16_t data[4];
    MagRawData(data);
    data[3] = 1;

    // calculate the prediction error
    int e = data[0] * data[0] + data[1] * data[1] + data[2] * data[2];
    for (int dim = 0; dim < 3; ++dim) {
      e -= data[dim] * theta[dim];
    }

    // TODO: calculate RLS gain
  }
}
