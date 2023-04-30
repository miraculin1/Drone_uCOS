#ifndef __MAIN_H
#define __MAIN_H

#ifndef OS_MASTER_FILE
#include <ucos_ii.h>
#endif

void initProcessor();
extern OS_MEM *stkpool;
void test();

#endif
