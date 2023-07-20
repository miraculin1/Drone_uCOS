#ifndef __MY_MATH_h
#define __MY_MATH_h

#include <math.h>

typedef double quaternion_t[4];
typedef double sencorData_t[3];
typedef double DCM_t[3][3];
void normalize(double *out, double *v, int dim);
void vecCrossProd(double *out, double *v1, double *v2);
void DCM2quat(quaternion_t out, DCM_t R);
void quat2DCM(quaternion_t q, DCM_t R);
void DCMTrans(double out[3], DCM_t R, double vect[3]);
void squrMxVec(double *out, double **M, double *v, int dim);

#endif
