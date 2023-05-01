#ifndef __OS_CPU_H
#define __OS_CPU_H

// data type
// compiler related

#include <stdint.h>

typedef uint8_t BOOLEAN;
typedef uint8_t INT8U;   /* Unsigned  8 bit quantity           */
typedef int8_t INT8S;    /* Signed    8 bit quantity           */
typedef uint16_t INT16U; /* Unsigned 16 bit quantity           */
typedef int16_t INT16S;  /* Signed   16 bit quantity           */
typedef uint32_t INT32U; /* Unsigned 32 bit quantity           */
typedef int32_t INT32S;  /* Signed   32 bit quantity           */
typedef float FP32;      /* Single precision floating point    */
typedef double FP64;     /* Double precision floating point    */

typedef INT32U OS_STK;    /* Each stack entry is 32-bit wide    */
typedef INT32U OS_CPU_SR; /* Define size of CPU status register */

// asm funcs for OS_CRITICAL_METHOD
OS_CPU_SR __savePRIMASK(void);
void __setPSR(INT32U cpu_sr);

void __setPenSV();

// things about processor
// the method 1 and 2 have some problem
#define OS_CRITICAL_METHOD 3

#if OS_CRITICAL_METHOD == 3

#define OS_ENTER_CRITICAL()                                                    \
  { cpu_sr = __savePRIMASK(); }

#define OS_EXIT_CRITICAL()                                                     \
  { __setPSR(cpu_sr); }

#endif

// 1 means grow downwards
#define OS_STK_GROWTH 1

// this shall trigger an interrupt, registers will
// be preserved by irq_handler, so ctxsw only resume a
// ready task
#define OS_TASK_SW() __setPenSV();

void OSStartHighRdy(void);
void OSIntCtxSw(void);
void OSCtxSw(void);
#endif
