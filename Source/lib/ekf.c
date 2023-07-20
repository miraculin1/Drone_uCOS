#include "ekf.h"

// project prev to a next state
// it's from a diffrentiate equation, only when deltaT is
// small enough this works.
// IF deltaT is large, Picard method is needed
void stateExtraPolot(quaternion_t out, quaternion_t prev, sencorData_t gyroData,
                     double deltaSec) {
  // get data
  double wx, wy, wz;
  wx = gyroData[0];
  wy = gyroData[1];
  wz = gyroData[2];

  double omega[4][4] = {
      {0, -wx, -wy, -wz},
      {wx, 0, wz, -wy},
      {wy, -wz, 0, wx},
      {wz, wy, -wx, 0},
  };

  squrMxVec(out, (double **)omega, prev, 4);
  for (int i = 0; i < 4; i++) {
    out[i] *= deltaSec / 2;
  }
  for (int i = 0; i < 4; i++) {
    out[i] += prev[i];
  }

  // copy all bias
  for (int i = 4; i < STDIM; i++) {
    out[i] = prev[i];
  }

  return;
}

void cvExtraPolot(statCovariant_t out, statCovariant_t in) {}

void kalmanGain(kalmanGain_t KG, statCovariant_t stat_cov,
                msrCovariant_t msr_cov, obvMatrix_t obv) {}

void stateUPD(state_t out, state_t prev, obvMatrix_t obv, kalmanGain_t KG,
              msr_t msr) {}

void covUPD(statCovariant_t out, statCovariant_t prev, kalmanGain_t KG,
            obvMatrix_t OM) {}

// we define the obv matrix is transfer the g0 to gb
static void getJobian(JMatrix_t J, state_t s, msr_t init) {
  double q0 = s[0], q1 = s[1], q2 = s[2], q3 = s[3];
  // normalize quaternion
  normalize(s, s, 4);
  JMatrix_t tmp = {
      {// f0
       2 * q0 * init[0] + -2 * q3 * init[1] + 2 * q2 * init[2],
       2 * q1 * init[0] + 2 * q2 * init[1] + 2 * q3 * init[2],
       -2 * q2 * init[0] + 2 * q1 * init[1] + 2 * q0 * init[2],
       -2 * q3 * init[0] + -2 * q0 * init[2] + 2 * q1 * init[3], 1},
      {// f1
       2 * q3 * init[0] + 2 * q0 * init[1] + -2 * q1 * init[2],
       2 * q2 * init[0] + -2 * q1 * init[1] + -2 * q0 * init[2],
       2 * q1 * init[0] + 2 * q2 * init[1] + 2 * q3 * init[2],
       2 * q0 * init[0] + -2 * q3 * init[1] + 2 * q2 * init[2], 0, 1},
      {// f2
       -2 * q2 * init[0] + 2 * q1 * init[1] + 2 * q0 * init[2],
       2 * q3 * init[0] + 2 * q0 * init[1] + -2 * q1 * init[2],
       -2 * q0 * init[0] + 2 * q3 * init[1] + -2 * q2 * init[2],
       2 * q1 * init[0] + 2 * q2 * init[1] + 2 * q3 * init[2], 0, 0, 1

      },
      {// f3
       2 * q0 * init[0] + -2 * q3 * init[1] + 2 * q2 * init[2],
       2 * q1 * init[0] + 2 * q2 * init[1] + 2 * q3 * init[2],
       -2 * q2 * init[0] + 2 * q1 * init[1] + 2 * q0 * init[2],
       -2 * q3 * init[0] + -2 * q0 * init[2] + 2 * q1 * init[3], 0, 0, 0, 1},
      {// f4
       2 * q3 * init[0] + 2 * q0 * init[1] + -2 * q1 * init[2],
       2 * q2 * init[0] + -2 * q1 * init[1] + -2 * q0 * init[2],
       2 * q1 * init[0] + 2 * q2 * init[1] + 2 * q3 * init[2],
       2 * q0 * init[0] + -2 * q3 * init[1] + 2 * q2 * init[2], 0, 0, 0, 0, 1},
      {// f5
       -2 * q2 * init[0] + 2 * q1 * init[1] + 2 * q0 * init[2],
       2 * q3 * init[0] + 2 * q0 * init[1] + -2 * q1 * init[2],
       -2 * q0 * init[0] + 2 * q3 * init[1] + -2 * q2 * init[2],
       2 * q1 * init[0] + 2 * q2 * init[1] + 2 * q3 * init[2], 0, 0, 0, 0, 0,
       1},
  };
  memcpy(J, tmp, sizeof(double) * MSRDIM * STDIM);
}

static void state2Obv(msr_t out, state_t s, msr_t init) {
  DCM_t R;
  quat2DCM(s, R);
  DCMTrans(&out[0], R, &init[0]);
  DCMTrans(&out[3], R, &init[3]);
};
