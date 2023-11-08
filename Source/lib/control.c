#include "control.h"
#include "REC.h"
#include "ekf.hpp"
#include "main.h"
#include "motor.h"
#include "pid.h"
#include "ucos_ii.h"
#include <stdint.h>
#include <stdio.h>
const float yawGain = 0.5, pitchGain = 0.3, rollGain = 0.3;

const float throThre = 0.8;
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
    if (fourMotor[i] > 1000) {
      fourMotor[i] = 1000;
      printf("[WARN] motor%d over load\n", i);
    } else if (fourMotor[i] < 0) {
      fourMotor[i] = 0;
      printf("[WARN] motor%d under load\n", i);
    }
  }
  setThro(fourMotor);
}

void taskControl() {
  float tar[PID_DIM];
  float control[3];
  initbothPID();

  while (1) {
    if (recData.valid) {
      getWantedYPR(tar);
      PID(tar, ypr, control);
      powerDistri(control, fourMotorG);
    }
    OSTimeDlyHMSM(0, 0, 0, SYS_DELTASEC * 1000);
  }
}
