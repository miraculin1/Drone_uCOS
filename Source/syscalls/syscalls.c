#include "syscalls.h"
#include "USART.h"

/**********************************
 * this func always called by printf
 * good to use
 * P.S. from newlib docs
 ***********************************
 */
int _write(int file, char *ptr, int len) {

  os_witeStr(ptr, len);
  /* int todo; */
  /* for (todo = 0; todo < len; todo++) { */
    /* // redirecting out put to uart or swo */
    /* [> ITM_SendChar(*(ptr++)); <] */
    /* [> os_writeByte(*ptr); <] */
    /* [> USendByte(*ptr); <] */
    /* ptr++; */
  /* } */
  return len;
}

/***************************************
 *
 * IMPORTANT: funcs below are all minimal
 * implementations if need, has to be rewriten
 *
 **************************************
 */
int _read(int file, char *ptr, int len) { return 0; }

//    Set position in a file. Minimal implementation:

int _lseek(int file, int ptr, int dir) { return 0; }

// Send a signal.
int _kill(int pid, int sig) {
  errno = EINVAL;
  return -1;
}

// Query whether output stream is a terminal. For consistency with the other
// minimal implementations, which only support output to stdout
int _isatty(int file) { return 1; }

// Process-ID; this is sometimes used to generate strings unlikely to conflict
// with other processes
int _getpid(void) { return 1; }

//     Close a file. Minimal implementation:

int _close(int file) { return -1; }

// Status of an open file. For consistency with other minimal implementations
// in these examples, all files are regarded as character special devices.
// The sys/stat.h header file required is distributed in the include
// subdirectory for this C library.

int _fstat() { return 0; }

void *_sbrk(intptr_t increment) {
  extern char _end;
  extern char _Min_Heap_Size;
  static char *heap_end;
  char *prev_heap_end;
  if (heap_end == NULL) {
    heap_end = &_end;
  }
  if (heap_end + increment > &_end + (int)&_Min_Heap_Size) {
    return (void *)-1;
  }
  prev_heap_end = heap_end;
  heap_end += increment;

  return (void *)prev_heap_end;
}

void _exit(int a) {
  while (1) {
  }
}
