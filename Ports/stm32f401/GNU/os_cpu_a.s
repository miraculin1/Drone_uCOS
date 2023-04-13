@ vim:ft=armv5
  .syntax = unified
  .cpu cortex-m4
  .thumb


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

