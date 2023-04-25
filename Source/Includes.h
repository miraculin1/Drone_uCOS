#ifndef __INCLUDES_H
#define __INCLUDES_H

#ifndef OS_MASTER_FILE
#include <ucos_ii.h>
#endif
#include <stdint.h>
#include "stm32f4xx.h"

#define STK_SIZE 128
#define STK_CNT 10

void initProcessor();
extern OS_MEM *stkpool;

#endif