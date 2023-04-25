#include "main.h"
#include "stm32f4xx.h"
#include "ucos_ii.h"

int main() {

  OS_STK stk1[STK_SIZE];

  while (1) {
    OSInit();

    // MUST
    OSTaskCreate(&initProcessor, NULL, &stk1[STK_SIZE - 1], 0);

    OSStart();
  }
}

// this func MUST run as first task
// - Set up systick
// - Set pendSV priority
void initProcessor() {
  // set overall priority group to
  // 4bit preemt, 4bit sub
  SCB->AIRCR |= (0b101 << 8);
  // set up systick
  SysTick->CTRL &= ~(1 << 2);
  SysTick->LOAD = (105000 & 0xFFFFFF);
  // enable systick inttrupt
  SysTick->CTRL |= (1 << 1);
  SysTick->CTRL |= 1;

  // pendSV set to lowest priority
  SCB->SHP[9] = 0xFF;

  OSTaskDel(OS_PRIO_SELF);
  while (1) {
  }
}
