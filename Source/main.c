#include "Includes.h"
#include "stm32f4xx.h"

OS_STK __stk_Array[STK_CNT][STK_SIZE];
OS_MEM *stkpool;
OS_STK mainStk[STK_SIZE];
OS_STK *topMainSTK = &mainStk[STK_SIZE - 1];

int main() {

  INT8U ERROR;

  while (1) {
    OSInit();

    stkpool =
        OSMemCreate(&__stk_Array, STK_CNT, sizeof(OS_STK) * STK_SIZE, &ERROR);

    OS_STK *stk1 = OSMemGet(stkpool, &ERROR);
    // MUST
    OSTaskCreate(&initProcessor, NULL, &stk1[STK_SIZE - 1], 0);

    stk1 = OSMemGet(stkpool, &ERROR);
    OSTaskCreate(&test, NULL, &stk1[STK_SIZE - 1], 2);

    OSStart();
  }
}

// this func MUST run as first task
// after initalize, it will delet itself
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

  initHardware();
  OSTaskDel(OS_PRIO_SELF);
  while (1) {
  }
}

void initHardware() { initLED(); }

void test() {
  while (1) {
    LED_ON();
    OSTimeDly(2);
    LED_OFF();
    OSTimeDly(2);
  }
}
