#include "ekf.h"
#include "Includes.h"
#include "declareFunctions.h"

double gyroQ[3] = {1, 1, 1};
double accQ[3] = {1, 1, 1};
double magQ[3] = {1, 1, 1};
msr_t initMsr;

static void getNoise(statCovariant_t out, state_t s, double deltaSec);
static void getJobian(JMatrix_t J, state_t s, msr_t init);

// project prev to a next state
// it's from a diffrentiate equation, only when deltaT is
// small enough this works.
// IF deltaT is large, Picard method is needed
void stateExtraPolot(state_t out, state_t prev, sencorData_t gyroData,
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

void cvExtraPolot(statCovariant_t out, statCovariant_t in, sencorData_t gyro,
                  double deltaSec, state_t state) {
  double wx = gyro[0] / 2 * deltaSec, wy = gyro[1] / 2 * deltaSec,
         wz = gyro[2] / 2 * deltaSec;

  double F[10][10] = {
      {0, -wx, -wy, -wz},
      {wx, 0, wz, -wy},
      {wy, -wz, 0, wx},
      {wz, wy, -wx, 0},
      {0, 0, 0, 0, 1},
      {0, 0, 0, 0, 0, 1},
      {0, 0, 0, 0, 0, 0, 1},
      {0, 0, 0, 0, 0, 0, 0, 1},
      {0, 0, 0, 0, 0, 0, 0, 0, 1},
      {0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
  };
  double outTmp[10][10];
  mul((double *)F, (double *)in, false, (double *)outTmp, 10, 10, STDIM);
  tran((double *)F, 10, 10);
  double outTmp1[10][10];
  mul((double *)outTmp, (double *)F, false, (double *)outTmp1, 10, 10, STDIM);

  // add a process noise
  // the quaternion part is first-order approximation
  double Q[10][10];
  getNoise(Q, state, deltaSec);

  add((double *)outTmp1, (double *)Q, (double *)out, 10, 10, 10);
}

void kalmanGain(kalmanGain_t KG, statCovariant_t P, msrCovariant_t R,
                obvMatrix_t obv, sencorData_t gyro, double deltaSec,
                state_t stat) {

  JMatrix_t F;
  getJobian(F, stat, initMsr);
  double tmp[10][10], tmp1[10][10];
  mul((double *)F, (double *)P, false, (double *)tmp, MSRDIM, STDIM, STDIM);
  tran((double *)F, MSRDIM, STDIM);
  mul((double_t *)tmp, (double *)F, false, (double *)tmp1, MSRDIM, STDIM,
      MSRDIM);
  add((double *)tmp1, (double *)R, (double *)tmp, MSRDIM, MSRDIM, MSRDIM);
  inv((double *)tmp, MSRDIM);

  mul((double *)F, (double *)tmp, false, (double *)tmp1, STDIM, MSRDIM, MSRDIM);
  mul((double *)P, (double *)tmp1, false, (double *)KG, STDIM, STDIM, MSRDIM);
}

void stateUPD(state_t out, state_t stat, obvMatrix_t obv, kalmanGain_t KG,
              msr_t msr) {
  double q0 = stat[0], q1 = stat[1], q2 = stat[2], q3 = stat[3];
  normalize(stat, stat, 4);
  double tmp[3];
  double R[3][3] = {{q0 * q0 + q1 * q1 - q2 * q2 - q3 * q3,
                     2 * q1 * q2 - 2 * q0 * q3, 2 * q1 * q3 + 2 * q0 * q2},
                    {2 * q1 * q2 + 2 * q0 * q3,
                     q0 * q0 - q1 * q1 + q2 * q2 - q3 * q3,
                     2 * q2 * q3 - 2 * q0 * q1},
                    {2 * q1 * q3 - 2 * q0 * q2, 2 * q2 * q3 + 2 * q0 * q1,
                     q0 * q0 - q1 * q1 - q2 * q2 + q3 * q3}};
  msr_t msrPrd;
  mul((double *)R, initMsr, false, tmp, 3, 3, 1);
  memcpy(msrPrd, tmp, 3 * sizeof(double));

  mul((double *)R, &initMsr[3], false, tmp, 3, 3, 1);
  memcpy(msrPrd, tmp, 3 * sizeof(double));
}

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

static void getNoise(statCovariant_t out, state_t s, double deltaSec) {
  double approxi[4][3] = {{s[0], -s[3], s[2]},
                          {s[3], s[0], -s[1]},
                          {-s[2], s[1], s[0]},
                          {-s[1], -s[2], -s[3]}};
  double cov[3][3] = {
      {gyroQ[0], 0, 0},
      {0, gyroQ[1], 0},
      {0, 0, gyroQ[2]},
  };

  double tmp[4][3];
  mul((double *)approxi, (double *)cov, false, (double *)tmp, 4, 3, 3);
  tran((double *)approxi, 4, 3);
  double tmp1[4][4];
  mul((double *)tmp, (double *)approxi, false, (double *)tmp1, 4, 3, 4);
  scale((double *)tmp1, deltaSec * deltaSec / 4, 4, 4);

  insert((double *)tmp1, (double *)out, 4, 4, 10, 0, 0);

  cov[0][0] = accQ[0];
  cov[1][1] = accQ[1];
  cov[2][2] = accQ[2];

  insert((double *)cov, (double *)out, 3, 3, 10, 4, 4);

  cov[0][0] = magQ[0];
  cov[1][1] = magQ[1];
  cov[2][2] = magQ[2];

  insert((double *)cov, (double *)out, 3, 3, 10, 7, 7);
}

// init EKF type

// state_t stat;
// statCovariant_t P;
// void (*f)(state_t stat);
// JMatrix_t F;
// statCovariant_t Q;
// double deltaSec;
// msr_t m0;
// msr_t m;

void initEKF(EKF_t now) {
  now.deltaSec = 0.1;
  // TODO: now.m0 = getMSR();
  // TODO: now.stat = init();
  getNoise(now.Q, now.stat, now.deltaSec);
  /* now.f = &stateExtraPolot; */
  /* now.P = ; */
}
