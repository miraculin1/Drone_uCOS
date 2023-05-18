#include "Includes.h"
#define MPU_ADDR 0xd0

static uint8_t selfTest();

static void MPUWrite(uint8_t tarreg, uint8_t data) {
  IIC_WriteData(MPU_ADDR, tarreg, data);
}

static void MPURead(uint8_t tarreg, uint8_t *dataout) {
  IIC_ReadData(MPU_ADDR, tarreg, dataout);
}

static void MPUor(uint8_t tarreg, uint8_t orVal) {
  IICOR(MPU_ADDR, tarreg, orVal);
}

static void MPUnand(uint8_t tarreg, uint8_t nandVal) {
  IICNAND(MPU_ADDR, tarreg, nandVal);
}

static void realconfig();

void enBypass() { MPUor(0x37, 0x01 << 1); }

void disenBypass() { MPUnand(0x37, 0x01 << 1); }

void initMPU6050() {
  selfTest();
  realconfig();
}

void realconfig() {
  /* // reset device */
  MPUWrite(0x6b, 0x80);
  OSTimeDly(10);

  /* IIC_WriteData(SELF_ADD, 0x68, 0x07); */

  // PWR_MGMT_1
  // select clock pll with x gyro
  MPUWrite(0x6b, 0x01);

  // PWR_MGMT_2
  // not low power mode
  MPUWrite(0x6c, 0x00);

  // SMPRT_DIV
  // not divided
  MPUWrite(0x19, 0x00);
  // CONFIG
  // set digital low pass filter to 3
  MPUWrite(0x1a, 0x06);

  // disable interupts
  MPUWrite(0x38, 0x00);
  // Gyro set to 2000degs/sec
  MPUWrite(0x1b, 0x18);
  // acce set to +- 2g
  MPUWrite(0x1c, 0x00);
  // set direct link to all senser in GY86
  enBypass();
}


void AccData(int16_t data[3]) {
  uint8_t temp[6];
  IICBurstRead(MPU_ADDR, 0x3b, 6, temp);
  for (int i = 0; i < 3; i++) {
    data[i] = 0;
    data[i] |= temp[i * 2];
    data[i] = data[i] << 8;
    data[i] |= temp[i * 2 + 1];
  }
}

uint8_t whoami() {
  uint8_t temp;
  IICBurstRead(MPU_ADDR, 0x75, 1, &temp);
  return temp;
}

void GyroData(int16_t data[3]) {
  uint8_t temp[6];
  IICBurstRead(MPU_ADDR, 0x3b, 6, temp);
  for (int i = 0; i < 3; i++) {
    data[i] = 0;
    data[i] |= temp[i * 2];
    data[i] = data[i] << 8;
    data[i] |= temp[i * 2 + 1];
  }
}

// perform a selftest
//
// RETURNS
// 0 as all good
// 1 as GYRO bad
// 2 as ACC bad
// 3 as all bad
static uint8_t selfTest() {
  /* // reset device */
  MPUWrite(0x6b, 0x80);
  OSTimeDly(10);

  // PWR_MGMT_1
  // select clock pll with x gyro
  MPUWrite(0x6b, 0x01);

  // PWR_MGMT_2
  // not low power mode
  MPUWrite(0x6c, 0x00);

  // SMPRT_DIV
  // not divided
  MPUWrite(0x19, 0x00);
  // CONFIG
  // set digital low pass filter to 3
  MPUWrite(0x1a, 0x06);

  // disable interupts
  MPUWrite(0x38, 0x00);
  // Gyro set to 250degs/sec
  MPUWrite(0x1b, 0x10);
  // acce set to +- 8g
  MPUWrite(0x1c, 0x10);
  // set direct link to all senser in GY86
  enBypass();

  int16_t data[3];
  int16_t data1[3];
  uint8_t res = 0;
  uint8_t xtest, ytest, ztest;

  MPURead(0x0d, &xtest);
  MPURead(0x0e, &ytest);
  MPURead(0x0f, &ztest);

  float ftx, fty, ftz;

  // GYRO
  GyroData(data);
  MPUor(0x1b, 0x7 << 5);
  GyroData(data1);
  MPUnand(0x1b, 0x7 << 5);

  uint32_t facti = 25 * 131;
  ftx = (xtest & 0x1f) != 0 ? facti * powf(1.046, (xtest & 0x1f) - 1) : 0;
  fty =
      (ytest & 0x1f) != 0 ? (-1) * facti * powf(1.046, (ytest & 0x1f) - 1) : 0;
  ftz = (ztest & 0x1f) != 0 ? facti * powf(1.046, (ztest & 0x1f) - 1) : 0;

  float tmp;
  tmp = (data1[0] - data[0]) / ftx - 1;
  if (tmp > 14 || tmp < -14) {
    res |= 1;
  }

  tmp = (data1[1] - data[1]) / fty - 1;
  if (tmp > 14 || tmp < -14) {
    res |= 1;
  }

  tmp = (data1[2] - data[2]) / ftz - 1;
  if (tmp > 14 || tmp < -14) {
    res |= 1;
  }

  // accel
  AccData(data);
  MPUor(0x1c, 0x7 << 5);
  AccData(data1);
  MPUnand(0x1c, 0x7 << 5);

  float factf1 = 0.34f * 4096;
  float factf2 = 0.92f / 0.34f;
  ftx = (xtest & 0xe0) != 0
            ? factf1 * powf(factf2, (((xtest & 0xe0) >> 5) - 1) / 30.0f)
            : 0;
  fty = (ytest & 0xe0) != 0
            ? factf1 * powf(factf2, (((ytest & 0xe0) >> 5) - 1) / 30.0f)
            : 0;
  ftz = (ztest & 0xe0) != 0
            ? factf1 * powf(factf2, (((ztest & 0xe0) >> 5) - 1) / 30.0f)
            : 0;

  tmp = (data1[0] - data[0]) / ftx - 1;
  if (tmp > 14 || tmp < -14) {
    res |= 2;
  }

  tmp = (data1[1] - data[1]) / fty - 1;
  if (tmp > 14 || tmp < -14) {
    res |= 2;
  }

  tmp = (data1[2] - data[2]) / ftz - 1;
  if (tmp > 14 || tmp < -14) {
    res |= 2;
  }

  USendStr("++++++++selftest+++++++++++\n");
  USendInt(res);
  USendStr("\n++++++++selftest+++++++++++\n");
  return res;
}
