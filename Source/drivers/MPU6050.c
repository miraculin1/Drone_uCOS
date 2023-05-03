#include "Includes.h"
#define SELF_ADD 0xd0

// copied from the answer OK to run
void ref() {
  IIC_WriteData(SELF_ADD, 0x6b, 0x00);
  IIC_WriteData(SELF_ADD, 0x19, 0x00);
  IIC_WriteData(SELF_ADD, 0x1a, 0x06);
  IIC_WriteData(SELF_ADD, 0x1b, 0x18);
  IIC_WriteData(SELF_ADD, 0x1c, 0x00);
}

// Buggy TODO when set the 0x6b 0x80 (reset device)
// MPU6050 wont function untill power off
void initMPU6050() {

  /* // reset device */
  IIC_WriteData(SELF_ADD, 0x6b, 0x80);
  OSTimeDly(10);

  /* IIC_WriteData(SELF_ADD, 0x68, 0x07); */

  // select 8MHz internal clock
  IIC_WriteData(SELF_ADD, 0x6b, 0x00);
  // all axis no standby
  IIC_WriteData(SELF_ADD, 0x6c, 0x00);
  // disable interupts
  IIC_WriteData(SELF_ADD, 0x38, 0x00);
  // DLPF as 20Hz
  IIC_WriteData(SELF_ADD, 0x1a, 0x06);
  IIC_WriteData(SELF_ADD, 0x19, 0x00);
  // Gyro set to 2000degs/sec
  IIC_WriteData(SELF_ADD, 0x1b, 0x18);
  // acce set to +- 2g
  IIC_WriteData(SELF_ADD, 0x1c, 0x00);
  // set direct link to all senser in GY86
  IIC_WriteData(SELF_ADD, 0x37, 0x02);
}

void MPUReadData(uint16_t *x, uint16_t *y, uint16_t *z) {
  uint8_t tmp_h;
  uint8_t tmp_l;
  // read x
  IIC_ReadData(SELF_ADD, 0x3b, &tmp_h);
  IIC_ReadData(SELF_ADD, 0x3c, &tmp_l);
  *x = 0;
  *x |= (tmp_h << 8);
  *x |= (tmp_l);

  // read y
  IIC_ReadData(SELF_ADD, 0x3d, &tmp_h);
  IIC_ReadData(SELF_ADD, 0x3e, &tmp_l);
  *y = 0;
  *y |= (tmp_h << 8);
  *y |= (tmp_l);

  // read z
  IIC_ReadData(SELF_ADD, 0x3f, &tmp_h);
  IIC_ReadData(SELF_ADD, 0x40, &tmp_l);
  *z = 0;
  *z |= (tmp_h << 8);
  *z |= (tmp_l);
}
