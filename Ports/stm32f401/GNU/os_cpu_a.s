@ vim:ft=armv5
  .syntax unified
  .cpu cortex-m4
  .fpu softvfp
  .thumb


// varies
  // if os is running
  .extern OSRunning
  // task in cpu
  .extern OSTCBCur
  .extern OSTaskSwHook
  .extern TCBHighRdy
  .extern OSIntNesting
  .extern OSPriorCur
  .extern OSPriorHighRdy

// funcs
  .extern OSIntEnter
  .extern OSTimeTick
  .extern OSIntExit

  .word back2Thread
  .word realBack
  .set back2Thread, 0xffffffff9
  .set realBack, 0xfffffffd

  .global OSStartHighRdy
  .global OSIntCtxSw
  .global OSCtxSw
  .global __enableirq
  .global __disableirq
  .global __getPSR
  .global __setPSR

    .section .text.__enableirq
  .type __enableirq, %function
__enableirq:
  cpsie i
  bx lr
  .size __enableirq, .-__enableirq

    .section .text.__disableirq
  .type __disableirq, %function

__disableirq:
  cpsid i
  bx lr

  .size __disableirq, .-__disableirq

    .section .text.__getPSR
  .type __getPSR, %function

__getPSR:
  mrs r0, PRIMASK
  bx lr

  .size __getPSR, .-__getPSR

    .section .text.__setPSR
  .type __setPSR, %function

__setPSR:
  msr PRIMASK, r0
  bx lr

  .size __setPSR, .-__setPSR


// *************************************
//      START A TASK WHEN FIRST BOOT
// *************************************
    .section .text.OSStartHighRdy
  .type OSStartHighRdy, %function

OSStartHighRdy:
  // call the swhook
  bl OSTaskSwHook

  // OSRunning = 1
  ldr r0, =OSRunning
  mov r1, #1
  str r1, [r0]

  // load the sp of Highest prio task
  ldr r0, =OSTCBHighRdy
  ldr r0, [r0]
  ldr r0, [r0]

  // perform a return
  // pop regs pushed by me
  // to return from the interrupt mode pm said to
  // save the EXC_RETURN to pc

  ldmfd r0!, {r4-r11, lr}
  msr psp, r0
  bx lr

  .size OSStartHighRdy, .-OSStartHighRdy

// *************************************
// perform a contex switch after a interrupt
// save regs, get new regs
// *************************************
    .section .text.OSCtxSw
  .type OSCtxSw, %function
OSCtxSw:
  // check EXEC_RETURN
  // make sure Thread mode use psp all the time
  ldr r0, =back2Thread
  ldr r0, [r0]
  ldr r1, =realBack
  ldr r1, [r1]
  cmp lr, r0
  it eq
  moveq lr, r1
  // save regs
  ldr r0, =OSTCBCur
  ldr r0, [r0]
  mrs r1, psp
  stmfd r1!, {r4-r11, lr}
  // save the pushed sp
  str r1, [r0]

  bl OSTaskSwHook

  // TCBCur = TCBHighRdy
  ldr r0, =OSTCBCur
  ldr r1, =TCBHighRdy
  ldr r1, [r1]
  str r1, [r0]
  // OSPriorCur = OSPriorHighRdy
  ldr r0, =OSPriorCur
  ldr r1, =OSPriorHighRdy
  ldr r1, [r1]
  str r1, [r0]

  ldr r0, =OSTCBCur // R0->TCBCur->TCB
  ldr r0, [r0] // R0->TCB
  ldr r1, [r0] // R0 = TCB

  // restore the regs
  // return from the interrupt
  ldmfd r1!, {r4-r11, lr}
  msr psp, r1
  bx lr

  .size OSCtxSw, .-OSCtxSw

    .section .text.OSTickISR
  .type OSTickISR, %function

OSTickISR:
  ldr r0, =back2Thread
  ldr r0, [r0]
  ldr r1, =realBack
  ldr r1, [r1]
  cmp lr, r0
  it eq
  moveq lr, r1

  bl OSIntEnter
  ldr r0, =OSIntNesting
  ldr r0, [r0]
  cmp r0, #1
  bne NESTING
  b NONESTING

NESTING:
  stmfd sp!, {r4-r11, lr}

  // i assume no need to clear interrupt device
  // maening no explict need to ack the interrupt

  // no nesting so not enable interrupt here

  bl OSTimeTick
  bl OSIntExit

  // no higher task
  ldmfd sp!, {r4-r11, lr}
  bx lr

NONESTING:
  // save regs
  mrs r1, psp
  stmfd r1!, {r4-r11, lr}
  ldr r0, =OSTCBCur
  ldr r0, [r0]
  str r1, [r0]
  // i assume no need to clear interrupt device
  // maening no explict need to ack the interrupt

  // no nesting so not enable interrupt here

  bl OSTimeTick
  bl OSIntExit

  // no higher task
  ldr r0, =OSTCBCur
  ldr r0, [r0]
  str r1, [r0]
  ldmfd r1!, {r4-r11, lr}
  msr psp, r1
  bx lr


  .size OSTickISR, .-OSTickISR

    .section .text.OSIntCtxSw
  .type OSIntCtxSw, %function

OSIntCtxSw:
  bl OSTaskSwHook
  // OSTCBCur = OSTCBHighRdy
  ldr r0, =OSTCBCur
  ldr r1, =OSTCBHighRdy
  ldr r1, [r1]
  str r1, [r0]

  // OSPriorCur = OSPriorHighRdy
  ldr r0, =OSPriorCur
  ldr r1, =OSPriorHighRdy
  ldr r1, [r1]
  str r1, [r0]

  ldr r0, =OSTCBCur
  ldr r0, [r0]
  ldr r1, [r0]
  ldmfd r1!, {r4-r11, lr}
  msr psp, r1
  bx lr

  .size OSIntCtxSw, .-OSIntCtxSw

    .section .text.PendSV_Handler
  .type PendSV_Handler, %function

PendSV_Handler:
  b OSCtxSw

  .size PendSV_Handler, .-PendSV_Handler

    .section .text.SysTick_Handler
  .type SysTick_Handler, %function

SysTick_Handler:
  b OSTickISR

  .size SysTick_Handler, .-SysTick_Handler
