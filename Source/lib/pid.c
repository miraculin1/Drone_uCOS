#include "pid.h"
#include "REC.h"
#include "ekf.h"

// pid for position outer loop
// output the desired spin speed in rad
static PID_T posPID;
// pid for rategyro inner loop
// output for throtle addjust funcions
static PID_T ratePID;

void initPID(PID_T *ppid, float pram[PID_DIM][3]) {

  for (int i = 0; i < PID_DIM; ++i) {
    for (int j = 0; j < 3; ++j) {
      ppid->pram[i][j] = pram[i][j];
    }
    ppid->intergrator[i] = 0;
  }
}

void updPID(PID_T *ppid, float *error, float deltas) {
  for (int i = 0; i < PID_DIM; ++i) {
    ppid->control[i] = 0;
  }

  // add p notaion
  for (int i = 0; i < PID_DIM; ++i) {
    ppid->control[i] += ppid->pram[i][0] * error[i];
  }

  // add intergrate
  for (int i = 0; i < PID_DIM; ++i) {
    ppid->intergrator[i] += error[i] * deltas;
    ppid->control[i] += ppid->pram[i][1] * ppid->intergrator[i];
  }

  // add derivtive
  for (int i = 0; i < PID_DIM; ++i) {
    float deltaError = error[i] - ppid->lastError[i];
    ppid->control[i] += ppid->pram[i][2] * (deltaError / deltas);
  }

  // update the error for derivtive
  for (int i = 0; i < PID_DIM; ++i) {
    ppid->lastError[i] = error[i];
  }
}

void yaw(float rad) {}

void pitch(float rad) {}

void roll(float rad) {}

void innerPID() {
  float error[PID_DIM];
  for (int i = 0; i < PID_DIM; ++i) {
    error[i] = posPID.control[i] - gyroOut[i];
  }

  updPID(&ratePID, error, PID_DeltaSec);
}

void PID() {
  float pramOut[PID_DIM][3] = {

  };
  float pramIn[PID_DIM][3] = {

  };
  initPID(&posPID, pramOut);
  initPID(&ratePID, pramIn);

  float error[PID_DIM];
  float tar[PID_DIM];
  while (1) {
    getWantedYPR(tar);

    for (int i = 0; i < PID_DIM; ++i) {
      error[i] = tar[i] - yprOut[i];
    }

    updPID(&posPID, error, PID_DeltaSec);
    innerPID();

    yaw(ratePID.control[0]);
    pitch(ratePID.control[1]);
    roll(ratePID.control[2]);
  }
}
