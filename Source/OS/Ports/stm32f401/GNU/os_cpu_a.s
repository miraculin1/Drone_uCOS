  .syntax unified
  .cpu cortex-m4
  .thumb


// varies
  // if os is running
  .extern OSRunning
  // task in cpu
  .extern OSTCBCur
  .extern OSTaskSwHook
  .extern OSTCBHighRdy
  .extern OSIntNesting
  .extern OSPrioCur
  .extern OSPrioHighRdy

// funcs
  .extern OSIntEnter
  .extern OSTimeTick
  .extern OSIntExit
  .extern __setPenSV


  .global OSStartHighRdy
  .global OSIntCtxSw
  .global OSCtxSw
  .global __savePRIMASK
  .global __setPSR
  .global PendSV_Handler
  .global SysTick_Handler

    .section .text.__savePRIMASK
  .type __savePRIMASK, %function

__savePRIMASK:
  mrs r0, primask
  cpsid i
  bx lr

  .size __savePRIMASK, .-__savePRIMASK

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
  strb r1, [r0]

  // load the sp of Highest prio task
  ldr r0, =OSTCBHighRdy
  ldr r0, [r0]
  ldr r0, [r0]

  // perform a return
  // pop regs pushed by me
  // to return from the interrupt mode pm said to
  // save the EXC_RETURN to pc

  ldmfd r0!, {r4-r11, lr}
  mov r8, r0
  ldmfd r8!, {r0-r3}
  ldmfd r8!, {r4-r7}
  msr epsr, r7
  msr psp, r8
  mov r8, #2
  msr control, r8
  isb
  bx r6

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
  // save regs
  ldr r0, =OSTCBCur
  ldr r0, [r0]
  mrs r1, psp
  tst lr, #0x10
  it eq
  vstmdbeq r1!, {s16-s31}
  stmfd r1!, {r4-r11, lr}
  // save the pushed sp
  str r1, [r0]

  bl OSTaskSwHook

  // TCBCur = TCBHighRdy
  ldr r0, =OSTCBCur
  ldr r1, =OSTCBHighRdy
  ldr r1, [r1]
  str r1, [r0]
  // OSPriorCur = OSPriorHighRdy
  ldr r0, =OSPrioCur
  ldr r1, =OSPrioHighRdy
  ldr r1, [r1]
  strb r1, [r0]

  ldr r0, =OSTCBCur // R0->TCBCur->TCB
  ldr r0, [r0] // R0->TCB
  ldr r1, [r0] // R0 = TCB

  // restore the regs
  // return from the interrupt
  ldmfd r1!, {r4-r11, lr}
  tst lr, #0x10
  it eq
  vldmiaeq r1!, {s16-s31}
  msr psp, r1
  bx lr

  .size OSCtxSw, .-OSCtxSw

    .section .text.OSTickISR
  .type OSTickISR, %function

OSTickISR:

  stmfd sp!, {lr}
  bl OSIntEnter
  ldmfd sp!, {lr}
  // systick is most prio so no need turn off int
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
  // 这里保存寄存器并非必要（实际情况下）
  // 因为 neat 方案里的 IntCtxSw 是使用置位 PendSV
  // 所以不会从 OSIntExit 里出去，
  // 所以不需要保存现场
  // uc 手册里提到的方案会导致被 systick 打断的更低
  // 优先级的中断需要等上下文切换目的任务完成才会继续进行
  // 中断的处理

  // i assume no need to clear interrupt device
  // maening no explict need to ack the interrupt

  // no nesting so not enable interrupt here

  stmfd sp!, {lr}
  bl OSTimeTick
  bl OSIntExit
  ldmfd sp!, {lr}

  bx lr


  .size OSTickISR, .-OSTickISR

    .section .text.OSIntCtxSw
  .type OSIntCtxSw, %function

OSIntCtxSw:
  // set PendSV
  stmfd sp!, {lr}
  bl __setPenSV
  ldmfd sp!, {lr}
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
