#include "os_cpu.h"

void __setPenSV() {
  // 0xE000 ED00 is the base add of SCB registers
  *((uint32_t *)0xE000ED04) |= (0b1 << 28);
}

OS_STK *OSTaskStkInit(void (*task)(void *pd), void *pdata, OS_STK *ptos,
                      INT16U opt) {
  OS_STK *p_stk = ptos;
  // must aligned to 8byte on entry of a AAPCS function
  // all the C compiler will always result a AAPCS comforming function
  // according to this document
  // https://github.com/ARM-software/abi-aa/blob/
  p_stk = (OS_STK *)((uint32_t)p_stk & 0xFFFFFFF8);

  // TODO now need to push all the need stack frame info

  return p_stk;
}
