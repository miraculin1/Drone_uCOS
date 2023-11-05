#include "shell.h"
#include "sendInfo.h"
#include "USART.h"
#include <stdint.h>
#include <string.h>

// use to indecate the ready msg from terminal
OS_EVENT *termMsgSem;

#define ARGSIZE 10
#define MAXARGNUM 5
#define ENTRYSIZE 6

typedef struct {
  uint8_t argc;
  char argv[MAXARGNUM][ARGSIZE];
} CMD_T;

typedef struct {
  // max size of ARGSIZE
  char *name;
  void (*fp)(int argc, char **argv);
} Entry_T;

void defaultSehllFunction(int argc, char **argv) {}

void test(int argc, char **argv) { printf("hello\n"); }

const static Entry_T entTbl[6] = {
    {.name = "test", .fp = test},
    {.name = "log", .fp = logToggle},
    {.name = "", .fp = defaultSehllFunction},
    {.name = "", .fp = defaultSehllFunction},
    {.name = "", .fp = defaultSehllFunction},
    {.name = "", .fp = defaultSehllFunction},
};

const char *prompt = "drone $";

// return the recent CMD, end with '\0'
void getCMD(char *buffer) {
  uint8_t errno;
  OSSemPend(termMsgSem, 0, &errno);
  memcpy(buffer, puRecBufInit, puRecBuf - puRecBufInit);
  puRecBuf = puRecBufInit;
}

// parse the command
void parserCMD(CMD_T *out, char *buffer) {
  out->argc = 0;
  char *tmp = strtok(buffer, " ");
  for (; out->argc < MAXARGNUM && tmp != NULL; ++out->argc) {
    strncpy(out->argv[out->argc], tmp, ARGSIZE);
    tmp = strtok(NULL, " ");
  }
}

// excute the corresponding funciton
void excuteCMD(CMD_T *cmd) {
  if (strcmp(cmd->argv[0], "\0") == 0) {
    printf("%s", prompt);
    return;
  }
  for (int i = 0; i < ENTRYSIZE; ++i) {
    if (strcmp(cmd->argv[0], entTbl[i].name) == 0) {
      entTbl[i].fp(cmd->argc, (char **)cmd->argv);
      printf("%s", prompt);
      return;
    }
  }
  printf("cmd not found\n");
  printf("%s", prompt);
}

void initShell() { termMsgSem = OSSemCreate(0); }

void taskShell() {
  char buffer[MAXBUFLEN];
  CMD_T cmd;
  initShell();
  printf("%s", prompt);
  while (1) {
    getCMD(buffer);
    parserCMD(&cmd, buffer);
    excuteCMD(&cmd);
  }
}
