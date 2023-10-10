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
  double x[ST_DIM];
  double z[Z_DIM];
  double u[U_DIM];
  double P[ST_DIM * ST_DIM];
  // best:{-0.042155252543202654, 0.699747026300236, -0.71314587138046004}
  double magBase[3];
  double K[ST_DIM * Z_DIM];
  double H[Z_DIM * ST_DIM];
} EKF_T;

#define INITSAMPLES 10

extern double ATT_RATE;

void msr2State(EKF_T *ekf);
void magBase(EKF_T *ekf);
void attitudeEST();
void outputDCM();
void outputForPython();
void outputYPR(double *yaw, double *pitch, double *roll);

extern const double *const quatOut;
#endif
