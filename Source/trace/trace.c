#include "Includes.h"

/**********************************
 * this func always called by printf
 * good to use
 * P.S. from newlib docs
 ***********************************
 */
int _write(int file, char *ptr, int len) {
  int todo;

  for (todo = 0; todo < len; todo++) {
    // redirecting out put to uart or swo
    ITM_SendChar(*(ptr++));
  }
  return len;
}

