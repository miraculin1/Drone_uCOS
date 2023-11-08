#ifndef __MAIN_H
#define __MAIN_H

#ifndef OS_MASTER_FILE
#include <ucos_ii.h>
#endif

void initSys();
extern OS_MEM *stkpool;
void initHardware();
void userTaskCreate();
void initSystickPsv();
extern float SYS_DELTASEC;

extern enum { PriSendinfo = 4, PriAttitude = 2, PriUpdThro = 3, PriShell = 5 } prioTbl;

#endif
