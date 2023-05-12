#include "Includes.h"
#define MPU_ADDR 0xd0

void MPUWrite(uint8_t tarreg, uint8_t data) {
  IIC_WriteData(MPU_ADDR, tarreg, data);
}

void initMPU6050() {

  /* // reset device */
  MPUWrite(0x6b, 0x80);
  OSTimeDly(10);

  /* IIC_WriteData(SELF_ADD, 0x68, 0x07); */

  // select 8MHz internal clock
  MPUWrite(0x6b, 0x00);
  // all axis no standby
  MPUWrite(0x6c, 0x00);
  // disable interupts
  MPUWrite(0x38, 0x00);
  // DLPF as 20Hz
  MPUWrite(0x1a, 0x06);
  MPUWrite(0x19, 0x00);
  // Gyro set to 2000degs/sec
  MPUWrite(0x1b, 0x18);
  // acce set to +- 2g
  MPUWrite(0x1c, 0x00);
  // set direct link to all senser in GY86
  MPUWrite(0x37, 0x02);
}

void MPUReadData(uint16_t *x, uint16_t *y, uint16_t *z) {
  uint8_t tmp_h;
  uint8_t tmp_l;
  // read x
  IIC_ReadData(MPU_ADDR, 0x3b, &tmp_h);
  IIC_ReadData(MPU_ADDR, 0x3c, &tmp_l);
  *x = 0;
  *x |= (tmp_h << 8);
  *x |= (tmp_l);

  // read y
  IIC_ReadData(MPU_ADDR, 0x3d, &tmp_h);
  IIC_ReadData(MPU_ADDR, 0x3e, &tmp_l);
  *y = 0;
  *y |= (tmp_h << 8);
  *y |= (tmp_l);

  // read z
  IIC_ReadData(MPU_ADDR, 0x3f, &tmp_h);
  IIC_ReadData(MPU_ADDR, 0x40, &tmp_l);
  *z = 0;
  *z |= (tmp_h << 8);
  *z |= (tmp_l);
}
