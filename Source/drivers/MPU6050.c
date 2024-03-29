#include "Includes.h"
#define MPU_ADDR 0xd0

float GyroLSBPerDegree = 0, AccLSBPerG = 0;

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
  // Gyro set to 500degs/sec
  GyroLSBPerDegree = 65.5;
  MPUWrite(0x1b, 0x08);
  // acce set to +- 2g
  AccLSBPerG = 16384;
  MPUWrite(0x1c, 0x00);
  // set direct link to all senser in GY86
  enBypass();
}

void AccRawData(int16_t data[3]) {
  uint8_t errno;
  uint8_t *temp;
  if (dbuf.curVal == 0) {
    OSSemPend(dbuf.sem0, 0, &errno);
    temp = dbuf.rawbuf0;
  } else if (dbuf.curVal == 1) {
    OSSemPend(dbuf.sem1, 0, &errno);
    temp = dbuf.rawbuf1;
  } else {
    OSSemPend(dbuf.sem0, 0, &errno);
    temp = dbuf.rawbuf0;
  }
  for (int i = 0; i < 3; i++) {
    data[i] = 0;
    data[i] |= temp[i * 2];
    data[i] = data[i] << 8;
    data[i] |= temp[i * 2 + 1];
  }
  if (dbuf.curVal == 0) {
    OSSemPost(dbuf.sem0);
  } else if (dbuf.curVal == 1) {
    OSSemPost(dbuf.sem1);
  } else {
    OSSemPost(dbuf.sem0);
  }
}

void AccGData(float *out, float *bias) {
  if (bias == NULL) {
    int16_t data[3];
    AccRawData(data);
    for (int i = 0; i < 3; i++) {
      out[i] = -(float)data[i] / AccLSBPerG;
    }
  } else {
    int16_t data[3];
    AccRawData(data);
    for (int i = 0; i < 3; i++) {
      out[i] = (-(float)data[i] / AccLSBPerG - bias[i]) / bias[3 + i];
    }
  }
}

uint8_t whoami() {
  uint8_t temp;
  IICBurstRead(MPU_ADDR, 0x75, 1, &temp);
  return temp;
}

void GyroRawData(int16_t data[3]) {
  uint8_t errno;
  uint8_t *temp;
  if (dbuf.curVal == 0) {
    OSSemPend(dbuf.sem0, 0, &errno);
    temp = dbuf.rawbuf0 + 8;
  } else if (dbuf.curVal == 1) {
    OSSemPend(dbuf.sem1, 0, &errno);
    temp = dbuf.rawbuf1 + 8;
  } else {
    OSSemPend(dbuf.sem0, 0, &errno);
    temp = dbuf.rawbuf0 + 8;
  }
  for (int i = 0; i < 3; i++) {
    data[i] = (temp[i * 2] << 8) | temp[i * 2 + 1];
  }
  if (dbuf.curVal == 0) {
    OSSemPost(dbuf.sem0);
  } else if (dbuf.curVal == 1) {
    OSSemPost(dbuf.sem1);
  } else {
    OSSemPost(dbuf.sem0);
  }
}

void GyroDpSData(float *out, float *bias) {
  if (bias == NULL || (CALIBDONE & 0b001) == 0) {
  int16_t data[3];
  GyroRawData(data);
  for (int i = 0; i < 3; i++) {
    out[i] = (float)data[i] / GyroLSBPerDegree;
  }
  }

  int16_t data[3];
  GyroRawData(data);
  for (int i = 0; i < 3; i++) {
    out[i] = (float)data[i] / GyroLSBPerDegree - bias[i];
  }
}
void GyroRadpSData(float *out, float *bias) {
  GyroDpSData(out, bias);
  const float dPreRad = 180 / M_PI;
  for (int dim = 0; dim < 3; ++dim) {
    out[dim] /= dPreRad;
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

  uint8_t tmpTest[4];

  MPURead(0x0d, &tmpTest[0]);
  MPURead(0x0e, &tmpTest[1]);
  MPURead(0x0f, &tmpTest[2]);
  MPURead(0x10, &tmpTest[3]);

  float ftx, fty, ftz;

  // GYRO
  xtest = tmpTest[0] & 0x1f;
  ytest = tmpTest[1] & 0x1f;
  ztest = tmpTest[2] & 0x1f;
  GyroRawData(data);
  MPUor(0x1b, 0x7 << 5);
  GyroRawData(data1);
  MPUnand(0x1b, 0x7 << 5);

  uint32_t facti = 25 * 131;
  ftx = xtest != 0 ? facti * powf(1.046, xtest - 1) : 0;
  fty = ytest != 0 ? (-1) * facti * powf(1.046, ytest - 1) : 0;
  ftz = ztest != 0 ? facti * powf(1.046, ztest - 1) : 0;

  float tmp;
  tmp = (data1[0] - data[0]) / ftx;
  if (tmp > 14 || tmp < -14) {
    res |= 1;
    printf("Gx fail\n");
  }

  tmp = (data1[1] - data[1]) / fty;
  if (tmp > 14 || tmp < -14) {
    res |= 1;
    printf("Gy fail\n");
  }

  tmp = (data1[2] - data[2]) / ftz;
  if (tmp > 14 || tmp < -14) {
    res |= 1;
    printf("Gz fail\n");
  }

  // accel
  xtest = (tmpTest[0] >> 3) & 0b11100;
  ytest = (tmpTest[1] >> 3) & 0b11100;
  ztest = (tmpTest[2] >> 3) & 0b11100;
  xtest |= (tmpTest[3] >> 4) & 0b11;
  ytest |= (tmpTest[3] >> 2) & 0b11;
  ztest |= (tmpTest[3] >> 0) & 0b11;

  AccRawData(data);
  MPUor(0x1c, 0x7 << 5);
  AccRawData(data1);
  MPUnand(0x1c, 0x7 << 5);

  float factf1 = 0.34f * 4096;
  float factf2 = 0.92f / 0.34f;
  ftx = xtest != 0 ? factf1 * powf(factf2, (xtest - 1) / 30.0f) : 0;
  fty = ytest != 0 ? factf1 * powf(factf2, (ytest - 1) / 30.0f) : 0;
  ftz = ztest != 0 ? factf1 * powf(factf2, (ztest - 1) / 30.0f) : 0;

  tmp = (data1[0] - data[0]) / ftx;
  if (tmp > 14 || tmp < -14) {
    res |= 2;
    printf("Ax fail\n");
  }

  tmp = (data1[1] - data[1]) / fty;
  if (tmp > 14 || tmp < -14) {
    res |= 2;
    printf("Ay fail\n");
  }

  tmp = (data1[2] - data[2]) / ftz;
  if (tmp > 14 || tmp < -14) {
    res |= 2;
    printf("Az fail\n");
  }

  if (res == 0) {
    printf("======self test passed======\n");
  }
  return res;
}
