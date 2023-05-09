#ifndef __TRACE_H
#define __TRACE_H

#define ITM_LOCK (*((volatile uint32_t*)0xe0000fb0))
#define DBGMCU_CR (*((volatile uint32_t*)0xe0042004))
#define ITM_CR (*((volatile uint32_t*)0xe0000e80))
#define ITM_PRI (*((volatile uint32_t*)0xe0000e40))
#define ITM_EN (*((volatile uint32_t*)0xe0000e00))
#define DEMCR (*((volatile uint32_t*)0xe000edfc))

int write(int file, char *ptr, int len);

#endif
