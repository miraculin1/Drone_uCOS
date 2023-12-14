#ifndef __SHELL_H
#define __SHELL_H

#include "ucos_ii.h"
extern OS_EVENT *termMsgSem;
void taskShell();

#define ARGSIZE 10
#define MAXARGNUM 6
#define ENTRYSIZE 6

#endif
