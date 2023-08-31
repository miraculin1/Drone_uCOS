#ifndef __SYSCALLS_H
#define __SYSCALLS_H

#include <errno.h>
#include "USART.h"
#undef errno
extern int errno;

int _write(int file, char *ptr, int len);
int _read(int file, char *ptr, int len);
int _lseek(int file, int ptr, int dir);
int _isatty(int file);
int _getpid(void);
int _close(int file);
int _fstat();

#endif
