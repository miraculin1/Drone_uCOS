#ifndef __INCLUDES_H
#define __INCLUDES_H

#ifndef OS_MASTER_FILE

#include <ucos_ii.h>
#endif
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
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
#include "cali.hpp"
#include "sendInfo.h"
#include "ekf.hpp"
#include "shell.h"
#include "control.h"

#include "main.h"

#include "mymath.h"

#define STK_SIZE 2000
#define STK_CNT 7

#endif
