#ifndef _REC_H
#define _REC_H

#include "stm32f4xx.h"
#include "LED.h"

// used to store the raw data from PPM
struct RecData {
  int now;
  // (ch[i] - 1000) / 10 = persentage of power
  int chs[9];
  int linedUp;
};

extern struct RecData recData;
void getWantedYPR(float ypr[3]);

// int rawCh[9];
void initRec();
#endif
