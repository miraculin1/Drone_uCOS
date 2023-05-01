#include "Includes.h"

struct RecData recData;
/*
 * in total there is 8 chanels in ppm
 */

/*
 * shall using PA8/9/10/11 as input
 * TIM1 CH1/2/3/4
 * TIM1 on APB2
 * we are trying to capture PPM
 */

/*
 * only use ch1 pa8 as input
 */
static void PinInit() {
  // set clock enable
  RCC_AHB1_ENR |= (0x1 << 0);

  // AF
  GPIOA_MODER |= (0x2 << (2 * 8));
  // pull up
  GPIOA_PPMode |= (0x1 << (2 * 8));

  /* // float */
  /* GPIOA_PPMode &= ~(0x3 << (2 * 8)); */

  GPIOA_AF_H |= (0x1 << 0);
}

void initRec() {
  PinInit();

  // reset the data
  recData.now = 0;
  recData.linedUp = 0;

  RCC_APB2_ENR |= (0x1 << 0);
  // en ARR preload
  // not deviding the input running on 84MHz
  TIM1_CR1 |= (0x1 << 7);
  // set input mode TI1 & no filter
  TIM1_CCMR1 &= ~(0x3 << 0);
  TIM1_CCMR1 |= (0x1 << 0);
  // enable rising interrupt
  TIM1_DIER |= (0x1 << 1);

  // CC1P pority is default set to rising edge

  // set freq T=20ms each count represent 1us (0.1% in throttle
  // change)
  TIM1_PSC = 84 - 1;
  TIM1_ARR = 20 * 1000 - 1;
  // NVIC settings
  // enable interrupt
  NVIC_ISER1 |= (0x1 << 27);

  // enable counter
  TIM1_CR1 |= (0x1);

  // enable capture
  TIM1_CCER |= (0x1 << 0);
}

void TIM1_CC_IRQHandler() {
  // reset CNT after each interrupt

  // line up when possible
  if (!recData.linedUp) {
    if (TIM1_CCR1 >= 4000) {
      recData.linedUp = 1;
      LED_ON();
    }
  } else {
    recData.chs[recData.now] = TIM1_CCR1;
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
}
