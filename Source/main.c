#include "Includes.h"
#include "stm32f4xx.h"

OS_STK __stk_Array[STK_CNT * STK_SIZE];
OS_MEM *stkpool;
OS_STK mainStk[STK_SIZE];
OS_STK *topMainSTK = &mainStk[STK_SIZE - 1];
uint8_t m0[20];
uint8_t m1[20];

// TODO: this shall be changed
float SYS_DELTASEC = 0.1f;

enum prioTbl;

/*******************************
 * Table of tasks
 ******************************* * prio|name|note
 * */
void userTaskCreate() {
  OS_STK *pstk;
  INT8U ERROR;

  // CLR make sure stack all zero initalized,
  // stkUSED counts zero enteties
  //
  pstk = OSMemGet(stkpool, &ERROR);
  OSTaskCreateExt(&SendInfo, NULL, &pstk[STK_SIZE - 1], PriSendinfo, PriSendinfo, pstk, STK_SIZE,
                  NULL, OS_TASK_OPT_STK_CHK | OS_TASK_OPT_STK_CLR);
  OSTaskNameSet(PriSendinfo, (unsigned char *)"sendInfo", &ERROR);

  pstk = OSMemGet(stkpool, &ERROR);
  OSTaskCreateExt(&attitudeEST, NULL, &pstk[STK_SIZE - 1], PriAttitude, PriAttitude, pstk, STK_SIZE,
                  NULL, OS_TASK_OPT_STK_CHK | OS_TASK_OPT_STK_CLR);
  OSTaskNameSet(PriAttitude, (unsigned char *)"attitudeEST", &ERROR);

  pstk = OSMemGet(stkpool, &ERROR);
  OSTaskCreateExt(&taskControl, NULL, &pstk[STK_SIZE - 1], PriUpdThro, PriUpdThro, pstk, STK_SIZE,
                  NULL, OS_TASK_OPT_STK_CHK | OS_TASK_OPT_STK_CLR);
  OSTaskNameSet(PriUpdThro, (unsigned char *)"updThro", &ERROR);

  pstk = OSMemGet(stkpool, &ERROR);
  OSTaskCreateExt(&taskShell, NULL, &pstk[STK_SIZE - 1], PriShell, PriShell, pstk, STK_SIZE,
                  NULL, OS_TASK_OPT_STK_CHK | OS_TASK_OPT_STK_CLR);
 OSTaskNameSet(PriShell, (unsigned char *)"shell", &ERROR);
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
  printf("\n\n");
  initIIC();
  initMPU6050();
  initHMC();
  /* initMotor(); */

  initDMA(m0, m1, sizeof(m0));
  caliGyro();
  initRec();
}

void initSystickPsv() {
  // set overall priority group to
  // 4bit preemt, 4bit sub
  NVIC_SetPriorityGrouping(5);
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
