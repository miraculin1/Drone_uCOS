#include "control.h"
#include "REC.h"
#include "ekf.hpp"
#include "main.h"
#include "motor.h"
#include "pid.h"
#include "ucos_ii.h"
#include <math.h>
#include <stdint.h>
#include <stdio.h>
const float yawGain = 0.5, pitchGain = 0.3, rollGain = 0.3;
const float MAXTHRO = 0.8;
uint32_t CONdeltatick;

const float throThre = 0.6;
// TODO: need check the num and set rotation
//     [y]
// [0+]  [3-]
//   |    |
//  [board]   [x]
//   |    |
// [1-]  [2+]
void yaw(float control, uint16_t *fourMotor) {
  fourMotor[0] -= control;
  fourMotor[2] -= control;

  fourMotor[1] += control;
  fourMotor[3] += control;
}

void pitch(float control, uint16_t *fourMotor) {
  fourMotor[1] -= control;
  fourMotor[2] -= control;

  fourMotor[0] += control;
  fourMotor[3] += control;
}

void roll(float control, uint16_t *fourMotor) {
  fourMotor[2] -= control;
  fourMotor[3] -= control;

  fourMotor[0] += control;
  fourMotor[1] += control;
}

void getWantedYPR(float yprRAD[3]) {
  yprRAD[0] =
      -((float)recData.chs[3] - 1500) / 500 * yawGain * SYS_DELTASEC + ypr[0];
  yprRAD[1] = -((float)recData.chs[1] - 1500) / 500 * pitchGain;
  yprRAD[2] = ((float)recData.chs[0] - 1500) / 500 * rollGain;
}

// reciever data, rate pid output contol
void powerDistri(const float *pidControl, uint16_t *fourMotor) {
  if (!pidControl) {
    printf("bad in\n");
    return;
  }
  // 1000 refed as 100%
  // set basic throtle according to the threshold
  int thro = recData.chs[2] - 1000;
  if (thro < 0) {
    thro = 0;
  }
  thro = thro > 1000 * throThre ? 1000 * throThre : thro;
  for (int i = 0; i < 4; ++i) {
    fourMotor[i] = thro;
  }
  /* // do position transition */
  yaw(pidControl[0], fourMotor);
  pitch(pidControl[1], fourMotor);
  roll(pidControl[2], fourMotor);
  for (int i = 0; i < 4; ++i) {
    if (fourMotor[i] > 1000 * MAXTHRO) {
      fourMotor[i] = 1000 * MAXTHRO;
      printf("[WARN] motor%d over load\n", i);
    } else if (fourMotor[i] < 0) {
      fourMotor[i] = 0;
      printf("[WARN] motor%d under load\n", i);
    }
  }
  setThro(fourMotor);
}

bool safe(float *tar, float *ypr) {
  for (int i = 0; i < 3; ++i) {
    if (fabsf(tar[i] - ypr[i]) > 0.7) {
      return false;
    }
  }
  return true;
}

void taskControl() {
  static int lastInterTick;
  static int initcnt = 100;
  static uint8_t cnt;
  cnt = initcnt;
  float tar[PID_DIM];
  float control[3];
  initbothPID();

  while (1) {
    if (cnt == initcnt) {
      lastInterTick = OSTime;
    }
    if (recData.valid) {
      getWantedYPR(tar);
      PID(tar, ypr, control);
      if (!safe(tar, ypr)) {
        RCC->APB1ENR &= ~(0b1 << 1);
        printf("[WARN] unsafe attitude\n");
      } else {
        powerDistri(control, fourMotorG);
      }
    }
    if (cnt > 0) {
      cnt--;
    } else {
      CONdeltatick = OSTime - lastInterTick;
      // cout << (x.inverse().matrix().eulerAngles(2, 1, 0) * (180 / PI))
      // .transpose()
      // << "," << float(deltatick) / OS_TICKS_PER_SEC / initcnt * 1000
      // << "ms" << int(OSCPUUsage) << "%" << endl;
      cnt = initcnt;
    }
    OSTimeDlyHMSM(0, 0, 0, SYS_DELTASEC * 1000);
  }
}

void setTopThro() {
  OSTaskSuspend(PriUpdThro);
  uint16_t motor[4] = {1000, 1000, 1000, 1000};
  setThro(motor);
}

void setBotThro() {
  uint16_t motor[4] = {0, 0, 0, 0};
  setThro(motor);
  OSTaskResume(PriUpdThro);
}
void motorCalThro() {
  setTopThro();
  OSTimeDlyHMSM(0, 0, 3, 400);
  setBotThro();
}

