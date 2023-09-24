#ifndef __INCLUDES_H
#define __INCLUDES_H

#ifndef OS_MASTER_FILE

#include <ucos_ii.h>
#include <math.h>
#endif
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <stdio.h>
#include "newlib.h"
#include "stm32f4xx.h"
#include "trace.h"

#include "HMC5883.h"
#include "IIC.h"
#include "LED.h"
#include "MPU6050.h"
#include "oled2.h"
#include "REC.h"
#include "motor.h"
#include "USART.h"

#include "main.h"

#include "mymath.h"
#include "ekf.h"
#include "syscalls.h"

#define STK_SIZE 1536
#define STK_CNT 10

#endif
