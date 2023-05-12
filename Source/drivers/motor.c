#include "Includes.h"
uint16_t fourMotro[4];
/*
 * this is the motor file
 * for now only init one motor, but the reset three
 * are exactly the same
 */

// fourMotor[] / 10 is real val
// CCR val ranger from 1000-2000 linearly refer to
// 0%-100% of throtle
void setThro(uint16_t fourMotor[4]) {
  TIM3->CCR1 = fourMotor[0] + 1000;
  TIM3->CCR2 = fourMotor[1] + 1000;
  TIM3->CCR3 = fourMotor[2] + 1000;
  TIM3->CCR4 = fourMotor[3] + 1000;
}

// init all four pins
static void initPins() {
  // enable GPIOC
  RCC_AHB1 |= (0x1 << 2);

  // AF pc6
  GPIOC->MODER &= (0x2 << (2 * 6));
  GPIOC->MODER |= (0x2 << (2 * 6));

  // AF pc7
  GPIOC->MODER &= (0x2 << (2 * 7));
  GPIOC->MODER |= (0x2 << (2 * 7));

  // AF pc8
  GPIOC->MODER &= (0x2 << (2 * 8));
  GPIOC->MODER |= (0x2 << (2 * 8));

  // AF pc9
  GPIOC->MODER &= (0x2 << (2 * 9));
  GPIOC->MODER |= (0x2 << (2 * 9));

  // very speed
  GPIOC_OSPEEDR |= (0x3 << (2 * 6));
  GPIOC_AF_L |= (0x2 << (4 * 6));

  // very speed
  GPIOC_OSPEEDR |= (0x3 << (2 * 7));
  GPIOC_AF_L |= (0x2 << (4 * 7));

  // very speed
  GPIOC_OSPEEDR |= (0x3 << (2 * 8));
  GPIOC_AF_H |= (0x2 << (4 * 0));

  // very speed
  GPIOC_OSPEEDR |= (0x3 << (2 * 9));
  GPIOC_AF_H |= (0x2 << (4 * 1));
}

void initTIM3PWM() {

  // enable clock
  RCC_APB1 |= (0x1) << 1;

  // TIM running on 84MHz
  // 20ms period
  TIM3_PSC |= (84 - 1);
  TIM3_ARR = (20000 - 1);

  // ARR preload
  TIM3_CR1 |= (0x1 << 7);
  TIM3_CR1 &= ~(0x1 << 4);

  // config capture/compare mode
  // pwm5 pin ch1
  // enable preload
  TIM3_CCMR1 |= (0x1 << 3);
  // set mode pwm1
  TIM3_CCMR1 |= (0x6 << 4);

  // pwm6 pin ch2
  TIM3->CCMR1 |= (0x1 << 11);
  TIM3->CCMR1 |= (0x6 << 12);

  // pwm7 pin ch3
  // enable preload
  TIM3_CCMR2 |= (0x1 << 3);
  // set mode pwm1
  TIM3_CCMR2 |= (0x6 << 4);

  // pwm8 pin ch4
  TIM3->CCMR2 |= (0x1 << 11);
  TIM3->CCMR2 |= (0x6 << 12);

  // enable capture/compare
  TIM3_CCER |= (0x1 << 0);

  // reset all four motors to 0 thro
  for (int i = 0; i < 4; i++) {
    fourMotro[i] = 0;
  }
  setThro(fourMotro);


  initPins();
  // enable Counter
  TIM3_CR1 |= (0x1 << 0);
}

void initMotor() {
  initTIM3PWM();
}

void motorCalThro() {
  initTIM3PWM();
  OSTimeDly(10);
  OSTimeDly(100);
  OSTimeDly(100);
}
