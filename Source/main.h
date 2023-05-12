#ifndef __MAIN_H
#define __MAIN_H

#ifndef OS_MASTER_FILE
#include <ucos_ii.h>
#endif

void initSys();
extern OS_MEM *stkpool;
void initHardware();
void updateThro();
void SendInfo();
void userTaskCreate();
void initSystickPsv();

#endif
