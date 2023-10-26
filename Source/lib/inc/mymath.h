#ifndef __MY_MATH_h
#define __MY_MATH_h

#include <math.h>

typedef float quaternion_t[4];
typedef float sencorData_t[3];
typedef float DCM_t[3][3];
void normalize(float *v, int dim);
void vecCrossProd(float *out, float *v1, float *v2);
void DCM2quat(quaternion_t out, DCM_t R);
void quat2DCM(quaternion_t q, DCM_t R);
void DCMTrans(float out[3], DCM_t R, float vect[3]);
void squrMxVec(float *out, float **M, float *v, int dim);
float vecMod(int n, float *v);
void quatMulQuat(float *q, float *p, float *out);
void vec2Quat(float *vec, float *quat);
void quatConj(float *q, float *conq);
void quat2Vec(float *quat, float *vec);

#endif
