#ifndef _REC_H
#define _REC_H

#include "stm32f4xx.h"
#include "LED.h"

// used to store the raw data from PPM
struct RecData {
  int now;
  // (ch[i] - 1000) / 10 = persentage of power
  // chs defined as:
  // [R] l to r
  // [R] down to up
  // [L] down to up
  // [L] l to r
  // swa
  // swd
  int chs[9];
  uint8_t linedUp;
  uint8_t valid;
};
#include <stdbool.h>

extern struct RecData recData;
void getWantedYPR(float ypr[3]);

// int rawCh[9];
void initRec();
#endif
