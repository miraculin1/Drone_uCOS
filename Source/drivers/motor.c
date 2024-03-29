#include "motor.h"
#include "USART.h"
#include <REC.h>
#include <stdbool.h>

uint16_t fourMotorG[4];
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
  if (rotorLocked && recData.chs[4] > 1500) {
    // unlock
    /* RCC->APB1ENR |= (0b1 << 1); */
    initMotor();
    rotorLocked = false;
    Armed = false;
    printf("[INFO] unlocked\n");
  }

  if (!rotorLocked && recData.chs[4] <= 1500) {
    // lockup
    RCC->APB1ENR &= ~(0b1 << 1);
    rotorLocked = true;
    Armed = false;
    printf("[WARNING] locked\n");
  }

  if (Armed && recData.chs[5] > 1500) {
    // stop
    TIM3->CCR1 = 1000;
    TIM3->CCR2 = 1000;
    TIM3->CCR3 = 1000;
    TIM3->CCR4 = 1000;
    Armed = false;
    printf("[WARNING] motor stoped, disarmed\n");
  }

  if (!Armed && !rotorLocked && recData.chs[5] <= 1500 &&
      recData.chs[2] < 1100) {
    // not locked and throtle is min, enable all things
    Armed = true;
    printf("[INFO] motor armed\n");
  }

  if (!Armed) {
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

void disarm(const char *const reason) {
  if (Armed) {
    printf("[WARN]disarm due to %s", reason);
  }
  RCC->APB1ENR &= ~(0b1 << 1);
  rotorLocked = true;
  Armed = false;
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

  TIM3->CCR1 = 1000;
  TIM3->CCR2 = 1000;
  TIM3->CCR3 = 1000;
  TIM3->CCR4 = 1000;

  // enable Counter
  TIM3->CR1 |= (0x1 << 0);
}

void initMotor() { initTIM3PWM(); }
