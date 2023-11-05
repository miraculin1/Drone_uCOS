#include "REC.h"

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
  RCC->AHB1ENR |= (0x1 << 0);

  // AF
  GPIOA->MODER |= (0x2 << (2 * 8));
  // pull up
  GPIOA->PUPDR |= (0x1 << (2 * 8));

  /* // float */
  /* GPIOA_PPMode &= ~(0x3 << (2 * 8)); */

  GPIOA->AFR[1] |= (0x1 << 0);
}

void initRec() {
  PinInit();

  // reset the data
  recData.now = 0;
  recData.linedUp = 0;
  recData.valid = 0;

  RCC->APB2ENR |= (0x1 << 0);
  // en ARR preload
  // not deviding the input running on 84MHz
  TIM1->CR1 |= (0x1 << 7);
  // set input mode TI1 & no filter
  TIM1->CCMR1 &= ~(0x3 << 0);
  TIM1->CCMR1 |= (0x1 << 0);
  // enable rising interrupt
  TIM1->DIER |= (0x1 << 1);

  // CC1P pority is default set to rising edge

  // set freq T=20ms each count represent 1us (0.1% in throttle
  // change)
  TIM1->PSC = 84 - 1;
  TIM1->ARR = 20 * 1000 - 1;
  // NVIC settings
  // enable interrupt
  NVIC_EnableIRQ(TIM1_CC_IRQn);
  /* NVIC->ISER[0] |= (0x1 << 27); */

  // enable counter
  TIM1->CR1 |= (0x1);

  // enable capture
  TIM1->CCER |= (0x1 << 0);
}

// TODO: write this
void getWantedYPR(float yprRAD[3]) {

}
