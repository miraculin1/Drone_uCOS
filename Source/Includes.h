#ifndef __INCLUDES_H
#define __INCLUDES_H

#ifndef OS_MASTER_FILE
#include <ucos_ii.h>
#endif
#include <stdint.h>
#include "stm32f4xx.h"

#include "HMC5883.h"
#include "IIC.h"
#include "LED.h"
#include "MPU6050.h"
#include "oled2.h"
#include "REC.h"
#include "TIM.h"
#include "USART.h"

#include "main.h"

#define STK_SIZE 128
#define STK_CNT 10

#endif
