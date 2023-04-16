@ vim:ft=armv5
  .syntax unified
  .cpu cortex-m4
  .thumb


  .section .text.__enableirq
  .type __enableirq, %function

  // if os is running
  .extern OSRunning
  // task in cpu
  .extern OSTCBCur
  .extern OSTaskSwHook
  .extern TCBHighRdy
  .extern OSPriorCur
  .extern OSPriorHighRdy

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
  ldr sp, =OSStartHighRdy

  // perform a return
  // pop regs pushed by me
  // to return from the interrupt mode pm said to
  // save the EXC_RETURN to pc
  ldmfd sp!, {r4-r11, pc}

.size OSStartHighRdy, .-OSStartHighRdy

// *************************************
// perform a contex switch after a interrupt
// save regs, get new regs
// *************************************
  .section .text.OSCtxSw
  .type OSCtxSw, %function
OSCtxSw:
  // save regs
  stmfd sp!, {r4-r11, lr}
  // save the pushed sp
  ldr r0, =OSTCBCur
  ldr r0, [r0]
  str sp, [r0]

  bl OSTaskSwHook

  // TCBCur = TCBHighRdy
  ldr r0, =OSTCBCur
  ldr r1, =TCBHighRdy
  str r1, [r0]
  // sp = TCBHighRdy->sp
  ldr sp, [r1]
  // OSPriorCur = OSPriorHighRdy
  ldr r0, =OSPriorCur
  ldr r1, =OSPriorHighRdy
  ldr r1, [r1]
  str r1, [r0]

  // restore the regs
  // return from the interrupt
  ldmfd sp!, {r4-r11, pc}

.size OSCtxSw, .-OSCtxSw

  .section .text.OSTickISR
  .type OSTickISR, %function

OSTickISR:
  
  // TODO here to add code 
