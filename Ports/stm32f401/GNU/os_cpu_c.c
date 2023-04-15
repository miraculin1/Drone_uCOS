#include "os_cpu.h"
#ifndef OS_MASTER_FILE
#include <ucos_ii.h>
#endif

void __setPenSV() {
  // 0xE000 ED00 is the base add of SCB registers
  *((uint32_t *)0xE000ED04) |= (0b1 << 28);
}


// function exported
//=============================================
/*
 *  stack frame
 *
 *  BY HARDWARE
 *  {align}
 *  psr
 *  pc
 *  lr
 *  r12
 *  r3
 *  r2
 *  r1
 *  r0    BY me
 *        EXC_RETURN
 *        r11
 *        |
 *        r4
 *
 */

OS_STK *OSTaskStkInit(void (*task)(void *pd), void *pdata, OS_STK *ptos,
                      INT16U opt) {
  OS_STK *p_stk = ptos;
  // must aligned to 8byte on entry of a AAPCS function
  // all the C compiler will always result a AAPCS comforming function
  // according to this document
  // https://github.com/ARM-software/abi-aa/blob/
  p_stk = (OS_STK *)((uint32_t)p_stk & 0xFFFFFFF8);

  // xPSR
  // PM said write to the "Thumb" bit will be ignored
  // in other word 0x00000000 shall work too
  (*p_stk--) = (OS_STK)0x01000000;
  (*p_stk--) = (OS_STK)task;
  (*p_stk--) = (OS_STK)OS_TaskReturn;
  // r12
  (*p_stk--) = (OS_STK)0x11111111;
  (*p_stk--) = (OS_STK)0x11111111;
  (*p_stk--) = (OS_STK)0x11111111;
  // r1
  (*p_stk--) = (OS_STK)0x11111111;

  // r0 arg passing
  (*p_stk--) = (OS_STK) * ((uint32_t *)pdata);

  // EXC_RETURN val
  // return to Thread mode and using PSP afterwards
  (*p_stk--) = (OS_STK)0xFFFFFFFD;
  // r11
  (*p_stk--) = (OS_STK)0x11111111;
  (*p_stk--) = (OS_STK)0x11111111;
  (*p_stk--) = (OS_STK)0x11111111;
  (*p_stk--) = (OS_STK)0x11111111;
  (*p_stk--) = (OS_STK)0x11111111;
  (*p_stk--) = (OS_STK)0x11111111;
  (*p_stk--) = (OS_STK)0x11111111;
  // r4
  (*p_stk--) = (OS_STK)0x11111111;

  return p_stk;
}

// all hook function need to be defined
// (NO MATTER USED OR NOT)
// I my opinion, hook is just a interface,
// open up for adding more acts when something happens

// called after create
void OSTaskCreateHook(OS_TCB *ptcb) {
  // empty
}

// called right before remove a task from the active tasks
// d-link list
// @param ptcb is pointing to the deleting tcb
// can be used as a clean up for extended tcb part
void OSTaskDelHook(OS_TCB *ptcb) {
  // empty
}

// when task switch occur, right before switch
// @global param OSTCBCur: old, gonna leave cpu 
// (pointer to task)
// @global param OSTCBHighRdy: new, gonna go in cpu 
// (pointer to task)
void OSTaskSwHook() {
  // empty
}

// every sec called by OSTaskStat()
// track and display the status of tasks and CPU
void OSTaskStatHook(void) {
  // empty
}

// called by the OSTimeTick() right before the tick is
// accturlly processed
void OSTimeTickHook() {
// empty
}

// called by OS_TCBInit() just before the OSTaskCreateHook()
void OSTCBInitHook(OS_TCB *ptcb) {
// empty
}

// called by OS_TaskIdle()
// This function can be implemented to turn the CPU into some
// low power mode, or use LED to indecate how busy the CPU is
void OSTaskIdleHook(void) {
  // TODO add Idle hook
}

// called as soon as enter the OSInit()
// give a clean addon for OS init
void OSInitHookBegin() {
  // empty
}

// like above
void OSInitHookEnd() {
  // empty
}

