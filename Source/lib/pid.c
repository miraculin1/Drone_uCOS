#include "pid.h"
#include "REC.h"
#include "ekf.hpp"
#include "main.h"
#include "shell.h"
#include "ucos_ii.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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

void initbothPID() {
  float pramOut[PID_DIM][3] = {
      {10, 0, 0},
      {10, 0, 0},
      {10, 0, 0},
  };
  float pramIn[PID_DIM][3] = {
      {10, 0, 0},
      {10, 0, 0},
      {10, 0, 0},
  };
  initPID(&posPID, pramOut);
  initPID(&ratePID, pramIn);
}

void PID(float *tar, float *cur, float *control) {
  float error[PID_DIM];
  for (int i = 0; i < PID_DIM; ++i) {
    error[i] = tar[i] - cur[i];
  }
  updPID(&posPID, error, SYS_DELTASEC);

  for (int i = 0; i < PID_DIM; ++i) {
    error[i] = posPID.control[i] - gyroRate[i];
  }
  updPID(&ratePID, error, SYS_DELTASEC);
  for (int i = 0; i < PID_DIM; ++i) {
    control[i] = ratePID.control[i];
  }
}

// pid pos/rate [dim] p i d
void shellPID(int argc, char *argv[ARGSIZE]) {
  float(*tarPram)[3];
  if (argc == 1) {
    printf("%sPID\n", argv[1]);
    tarPram = posPID.pram;
    for (int i = 0; i < PID_DIM; ++i) {
      printf("%.2f, %.2f, %.2f\n", tarPram[i][0], tarPram[i][1], tarPram[i][2]);
    }
    tarPram = ratePID.pram;
    for (int i = 0; i < PID_DIM; ++i) {
      printf("%.2f, %.2f, %.2f\n", tarPram[i][0], tarPram[i][1], tarPram[i][2]);
    }
    return;
  }
  if (argc < 5) {
    printf("bad pram num\n");
    return;
  }

  if (strcmp(argv[1], "pos") == 0) {
    tarPram = posPID.pram;
  } else if (strcmp(argv[1], "rate") == 0) {
    tarPram = ratePID.pram;
  } else {
    printf("bad pram\n");
    return;
  }

  if (argc == 5) {
    float Pp = atof(argv[2]);
    float Pi = atof(argv[3]);
    float Pd = atof(argv[4]);
    for (int i = 0; i < PID_DIM; ++i) {
      tarPram[i][0] = Pp;
      tarPram[i][1] = Pi;
      tarPram[i][2] = Pd;
    }
    printf("%sPID set\n", argv[1]);
    for (int i = 0; i < PID_DIM; ++i) {
      printf("%.2f, %.2f, %.2f\n", tarPram[i][0], tarPram[i][1], tarPram[i][2]);
    }
  } else {
    int i = atoi(argv[2]);
    float Pp = atof(argv[3]);
    float Pi = atof(argv[4]);
    float Pd = atof(argv[5]);
    tarPram[i][0] = Pp;
    tarPram[i][1] = Pi;
    tarPram[i][2] = Pd;
    printf("%sPID %d set\n", argv[1], i);
    printf("%.2f, %.2f, %.2f\n", tarPram[i][0], tarPram[i][1], tarPram[i][2]);
  }

  printf("good luck\n");
}
