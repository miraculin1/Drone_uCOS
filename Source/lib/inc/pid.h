#ifndef __PID_H
#define __PID_H

#define PID_DIM 3

typedef struct {
  float lastError[PID_DIM];
  float pram[PID_DIM][3];
  float control[PID_DIM];
  float intergrator[PID_DIM];
} PID_T;


#endif
