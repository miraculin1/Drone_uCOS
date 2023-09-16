#include "Includes.h"

void TIM1_CC_IRQHandler() {
  OSIntEnter();
  // reset CNT after each interrupt
  TIM1->CNT = 0;

  // line up when possible
  if (!recData.linedUp) {
    if (TIM1->CCR1 >= 4000) {
      recData.linedUp = 1;
    }
  } else {
    recData.chs[recData.now] = TIM1->CCR1;
    if (recData.now == 8) {
      recData.now = 0;
      // check whether linedUp or not
      if (recData.chs[8] < 3000) {
        recData.linedUp = 0;
      }
    } else {
      recData.now++;
    }
  }

  OSIntExit();
}
