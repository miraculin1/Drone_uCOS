#include "USART.h"
#include "motor.h"
#include <stdbool.h>
#include <REC.h>

uint16_t fourMotor[4];
bool Armed = false;
bool rotorLocked;

/*
 * this is the motor file
 * for now only init one motor, but the reset three
 * are exactly the same
 */

// fourMotor[] / 10 is real val
// CCR val ranger from 1000-2000 linearly refer to
// 0%-100% of throtle
void setThro(uint16_t fourMotor[4]) {
  // TODO: check the channel
  if (recData.chs[0] == 1000) {
    // lock channel
    initMotor();
    rotorLocked = false;
    printf("[INFO] motor unlocked\n");
  } else {
    RCC->APB1ENR &= ~(0b1 << 1);
    rotorLocked = true;
    printf("[INFO] motor locked\n");
  }

  if (recData.chs[1] == 1000) {
    // stop channel
    TIM3->CCR1 = 1000;
    TIM3->CCR2 = 1000;
    TIM3->CCR3 = 1000;
    TIM3->CCR4 = 1000;
    printf("[WARNING] motor stoped\n");
    Armed = false;
  }

  if (!Armed && fourMotor[1] == 0 && !rotorLocked) {
    // not locked and throtle is min, enable all things
    Armed = true;
    printf("[INFO] motor disarmed\n");
  }
  if (!Armed) {
    printf("[INFO] NOT armed\n");
    TIM3->CCR1 = 1000;
    TIM3->CCR2 = 1000;
    TIM3->CCR3 = 1000;
    TIM3->CCR4 = 1000;
  } else {
    TIM3->CCR1 = fourMotor[0] + 1000;
    TIM3->CCR2 = fourMotor[1] + 1000;
    TIM3->CCR3 = fourMotor[2] + 1000;
    TIM3->CCR4 = fourMotor[3] + 1000;
  }
}

// init all four pins
static void initPins() {
  // enable GPIOC
  RCC->AHB1ENR |= (0x1 << 2);

  // AF pc6
  GPIOC->MODER &= ~(0x3 << (2 * 6));
  GPIOC->MODER |= (0x2 << (2 * 6));

  // AF pc7
  GPIOC->MODER &= ~(0x3 << (2 * 7));
  GPIOC->MODER |= (0x2 << (2 * 7));

  // AF pc8
  GPIOC->MODER &= ~(0x3 << (2 * 8));
  GPIOC->MODER |= (0x2 << (2 * 8));

  // AF pc9
  GPIOC->MODER &= ~(0x3 << (2 * 9));
  GPIOC->MODER |= (0x2 << (2 * 9));

  // very speed
  GPIOC->OSPEEDR |= (0x3 << (2 * 6));
  GPIOC->AFR[0] |= (0x2 << (4 * 6));

  // very speed
  GPIOC->OSPEEDR |= (0x3 << (2 * 7));
  GPIOC->AFR[0] |= (0x2 << (4 * 7));

  // very speed
  GPIOC->OSPEEDR |= (0x3 << (2 * 8));
  GPIOC->AFR[1] |= (0x2 << (4 * 0));

  // very speed
  GPIOC->OSPEEDR |= (0x3 << (2 * 9));
  GPIOC->AFR[1] |= (0x2 << (4 * 1));
}

void initTIM3PWM() {
  initPins();

  // enable clock
  RCC->APB1ENR |= (0x1) << 1;

  // TIM running on 84MHz
  // 20ms period
  TIM3->PSC |= (84 - 1);
  TIM3->ARR = (20000 - 1);

  // ARR preload
  TIM3->CR1 |= (0x1 << 7);
  TIM3->CR1 &= ~(0x1 << 4);

  // config capture/compare mode
  // pwm5 pin ch1
  // enable preload
  TIM3->CCMR1 |= (0x1 << 3);
  // set mode pwm1
  TIM3->CCMR1 |= (0x6 << 4);

  // pwm6 pin ch2
  TIM3->CCMR1 |= (0x1 << 11);
  TIM3->CCMR1 |= (0x6 << 12);

  // pwm7 pin ch3
  // enable preload
  TIM3->CCMR2 |= (0x1 << 3);
  // set mode pwm1
  TIM3->CCMR2 |= (0x6 << 4);

  // pwm8 pin ch4
  TIM3->CCMR2 |= (0x1 << 11);
  TIM3->CCMR2 |= (0x6 << 12);

  // enable capture/compare
  TIM3->CCER |= (0x1 << 0);
  TIM3->CCER |= (0x1 << 4);
  TIM3->CCER |= (0x1 << 8);
  TIM3->CCER |= (0x1 << 12);

  // reset all four motors to 0 thro
  for (int i = 0; i < 4; i++) {
    fourMotor[i] = 0;
  }
  setThro(fourMotor);

  // enable Counter
  TIM3->CR1 |= (0x1 << 0);
}

void initMotor() { initTIM3PWM(); }

void motorCalThro() {
  initTIM3PWM();
  OSTimeDly(10);
  OSTimeDly(100);
  OSTimeDly(100);
}
