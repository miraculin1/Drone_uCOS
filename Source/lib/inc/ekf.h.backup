#ifndef __EKF_h
#define __EKF_h

#include <stdint.h>

// x_t(state vector) = [q0(scaler), q1, q2, q3](v^r = x * v^b * v^-1 )
// u_t(control) = [wx, wy, wz] !!not fixed
// z_t(msr) = [vec(acc), vec(mag)]

// x_t = f(x_t-1, u_t-1, w_t-1) w_t is the "process noise", which is unmodel
// z_t = h(x_t, v_t) v_t is the "measurement noise"
//
// these two function is NOT linear, so when deriving things about the variance
// have the linearize this thing by taking "Jacobian Matrix"

// ref to things above
#define ST_DIM 4
#define Z_DIM 6
#define U_DIM 3

typedef struct {
  float x[ST_DIM];
  float z[Z_DIM];
  float u[U_DIM];
  float P[ST_DIM * ST_DIM];
  // best:{-0.042155252543202654, 0.699747026300236, -0.71314587138046004}
  float magBase[3];
  float K[ST_DIM * Z_DIM];
  float H[Z_DIM * ST_DIM];
  // yaw pithc roll in rad
  float ypr[3];
} EKF_T;

#define INITSAMPLES 10
#define PI 3.14159265359f

extern float ATT_RATE;

void attitudeEST();
void outputDCM();
void outputForPython();
void updYPR(EKF_T *ekf);

extern const float *const quatOut;
extern uint32_t deltatick;
extern const float *const yprOut;
// rad
extern const float *const gyroOut;
#endif
