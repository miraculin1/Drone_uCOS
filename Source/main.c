#include "Includes.h"

// TODO: get mag cal

OS_STK __stk_Array[STK_CNT * STK_SIZE];
OS_MEM *stkpool;
OS_STK mainStk[STK_SIZE * 3];
OS_STK *topMainSTK = &mainStk[STK_SIZE - 1];

/*******************************
 * Table of tasks
 *******************************
 * prio|name|note
 * */
void userTaskCreate() {
  OS_STK *pstk;
  INT8U ERROR;

  pstk = OSMemGet(stkpool, &ERROR);
  OSTaskCreate(&updateThro, NULL, &pstk[STK_SIZE - 1], 1);

  pstk = OSMemGet(stkpool, &ERROR);
  OSTaskCreate(&SendInfo, NULL, &pstk[STK_SIZE - 1], 4);
}

int main() {

  INT8U ERROR;

  while (1) {
    OSInit();

    stkpool =
        OSMemCreate(__stk_Array, STK_CNT, sizeof(OS_STK) * STK_SIZE, &ERROR);

    OS_STK *pstk = OSMemGet(stkpool, &ERROR);
    // MUST
    OSTaskCreate(&initSys, NULL, &pstk[STK_SIZE - 1], 0);

    OSStart();
  }
}

// this func MUST run as first task
// after initalize, it will delet itself
// - Set up systick
// - Set pendSV priority
void initSys() {
  initSystickPsv();

  OSStatInit();
  initHardware();
  userTaskCreate();
  OSTaskDel(OS_PRIO_SELF);
  while (1) {
  }
}

void initHardware() {
  initLED();
  initUSART();
  initIIC();
  initMPU6050();
  initHMC();
  initRec();
  initMotor();
}

void SendInfo() {
  EKF_t ekf;
  initEKF(&ekf);
  while (1) {

    getMSR(ekf.m, ekf.m0);
    for (int i = 0; i < 3; ++i) {
      printf("%.3f ", ekf.m[i]);
    }
    printf("\n");

    OSTimeDly(20);
  }
}

void updateThro() {
  while (1) {
    if (recData.linedUp) {
      fourMotor[0] = recData.chs[2] - 1000;
      setThro(fourMotor);
    }
    OSTimeDly(2);
  }
}

void initSystickPsv() {
  // set overall priority group to
  // 4bit preemt, 4bit sub
  SCB->AIRCR |= (0b101 << 8);
  // set up systick
  // /8 scale
  SysTick->CTRL &= ~(1 << 2);
  SysTick->LOAD = ((SystemCoreClock / 8 / OS_TICKS_PER_SEC) & 0xFFFFFF);
  // enable systick inttrupt
  SysTick->CTRL |= (1 << 1);
  SysTick->CTRL |= 1;

  // pendSV set to lowest priority
  SCB->SHP[9] = 0xFF;
}
