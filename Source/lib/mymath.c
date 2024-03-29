#include "mymath.h"

void quat2Vec(float *quat, float *vec) {
  for (int i = 0; i < 3; i++) {
    vec[i] = quat[i + 1];
  }
}
void quatConj(float *q, float *conq) {
  conq[0] = q[0];
  for (int i = 1; i < 4; ++i) {
    conq[i] = -q[i];
  }
}

void vec2Quat(float *vec, float *quat) {
  quat[0] = 0;
  for (int i = 0; i < 3; i++) {
    quat[i + 1] = vec[i];
  }
}

static float maxIn4(float a, float b, float c, float d);

float vecMod(int n, float *v) {
  float sum = 0;
  for (int dim = 0; dim < n; dim++) {
    sum += v[dim] * v[dim];
  }
  return sqrtf(sum);
}
void vecCrossProd(float *out, float *u, float *v) {
  out[0] = u[1] * v[2] - u[2] * v[1];
  out[1] = -1 * (u[0] * v[2] - u[2] * v[0]);
  out[2] = u[0] * v[1] - u[1] * v[0];
}

void normalize(float *v, int dim) {
  float sum = 0;
  for (int i = 0; i < dim; i++) {
    sum += v[i] * v[i];
  }
  sum =sqrtf(sum);
  for (int i = 0; i < dim; i++) {
    v[i] = v[i] / sum;
  }
}

void DCM2quat(quaternion_t out, DCM_t R) {
  float q0, q1, q2, q3;
  q0 =sqrtf((1 + R[0][0] + R[1][1] + R[2][2]) / 4);
  q1 =sqrtf((1 + R[0][0] - R[1][1] - R[2][2]) / 4);
  q2 =sqrtf((1 - R[0][0] + R[1][1] - R[2][2]) / 4);
  q3 =sqrtf((1 - R[0][0] - R[1][1] + R[2][2]) / 4);

  float now = maxIn4(q0, q1, q2, q3);
  if (now == q0) {
    out[0] = q0;
    out[1] = (R[2][1] - R[1][2]) / (4 * q0);
    out[2] = (R[0][2] - R[2][0]) / (4 * q0);
    out[3] = (R[1][0] - R[0][1]) / (4 * q0);
  } else if (now == q1) {
    out[0] = (R[2][1] - R[1][2]) / (4 * q1);
    out[1] = q1;
    out[2] = (R[0][1] + R[1][0]) / (4 * q1);
    out[3] = (R[0][2] + R[2][0]) / (4 * q1);
  } else if (now == q2) {
    out[0] = (R[0][2] - R[2][0]) / (4 * q2);
    out[1] = (R[0][1] + R[1][0]) / (4 * q2);
    out[2] = q2;
    out[3] = (R[2][1] + R[1][2]) / (4 * q2);
  } else {
    out[0] = (R[1][0] - R[0][1]) / (4 * q3);
    out[1] = (R[2][1] + R[1][2]) / (4 * q3);
    out[2] = (R[1][2] + R[2][1]) / (4 * q3);
    out[3] = q3;
  }
  normalize(out, 4);
  return;
}

void quat2DCM(quaternion_t q, DCM_t R) {
  normalize(q, 4);
  float q02 = q[0] * q[0], q12 = q[1] * q[1], q22 = q[2] * q[2],
         q32 = q[3] * q[3];

  R[0][0] = q02 + q12 - q22 - q32;
  R[0][1] = 2 * q[1] * q[2] - 2 * q[0] * q[3];
  R[0][2] = 2 * q[1] * q[3] + 2 * q[0] * q[2];

  R[1][0] = 2 * q[1] * q[2] + 2 * q[0] * q[3];
  R[1][1] = q02 - q12 + q22 - q32;
  R[1][2] = 2 * q[2] * q[3] - 2 * q[0] * q[1];

  R[2][0] = 2 * q[1] * q[3] - 2 * q[0] * q[2];
  R[2][1] = 2 * q[2] * q[3] + 2 * q[0] * q[1];
  R[2][2] = q02 - q12 - q22 + q32;
}

static float maxIn4(float a, float b, float c, float d) {
  float max = fmaxf(a, b);
  float max0 = fmaxf(c, d);
  return fmaxf(max, max0);
}

void DCMTrans(float out[3], DCM_t R, float vect[3]) {
  for (int i = 0; i < 3; i++) {
    out[i] = 0;
    for (int j = 0; j < 3; j++) {
      out[i] += R[i][j] * vect[j];
    }
  }
}

void squrMxVec(float *out, float **M, float *v, int dim) {
  for (int i = 0; i < dim; i++) {
    out[i] = 0;
    for (int j = 0; j < dim; j++) {
      out[i] += M[i][j] * v[j];
    }
  }
}

void quatMulQuat(float *q, float *p, float *out) {
  float q0 = q[0], q1 = q[1], q2 = q[2], q3 = q[3];
  float p0 = p[0], p1 = p[1], p2 = p[2], p3 = p[3];
  out[0] = q0 * p0 - q1 * p1 - q2 * p2 - q3 * p3;
  out[1] = q1 * p0 + q0 * p1 + q2 * p3 - q3 * p2;
  out[2] = q2 * p0 + q0 * p2 + q3 * p1 - q1 * p3;
  out[3] = q3 * p0 + q0 * p3 + q1 * p2 - q2 * p1;
}
