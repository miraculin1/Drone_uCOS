#ifndef __MAIN_H
#define __MAIN_H

#ifndef OS_MASTER_FILE
#include <ucos_ii.h>
#endif

void initProcessor();
extern OS_MEM *stkpool;
void initHardware();
void updateThro();
void SendInfo();
void userTaskCreate();
void displayOLED();

#endif
