#include "ekf.h"
#include "Includes.h"
#include "declareFunctions.h"

double gyroQ[3] = {1, 1, 1};
double accQ[3] = {1, 1, 1};
double magQ[3] = {1, 1, 1};
msr_t initMsr;

static void getNoise(statCovariant_t out, state_t s, double deltaSec);
static void getJobian(JMatrix_t J, state_t s, msr_t init);
static void state2Obv(msr_t out, state_t s, msr_t init);
static void getMSR(msr_t m);
static void avgMSR(msr_t m);
static void statusInit(EKF_t now);

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
              msr_t msr, msr_t m0) {
  msr_t fx;
  state2Obv(fx, stat, m0);

  msr_t tmpMsr;
  sub(msr, fx, tmpMsr, MSRDIM, 1, 1);
  state_t bias;
  mul((double *)KG, tmpMsr, false, (double *)bias, STDIM, MSRDIM, 1);

  add(stat, bias, out, STDIM, 1, 1);
}

void covUPD(statCovariant_t out, statCovariant_t prev, kalmanGain_t KG,
            obvMatrix_t OM, msr_t base, state_t s) {
  JMatrix_t F;
  getJobian(F, s, base);
  double tmp[STDIM][STDIM];
  double tmp1[STDIM][STDIM];
  mul((double *)KG, (double *)F, false, (double *)tmp, STDIM, MSRDIM, STDIM);

  double I[STDIM][STDIM];
  eye((double *)I, STDIM, STDIM);

  sub((double *)I, (double *)tmp, (double *)tmp1, STDIM, STDIM, STDIM);

  inv((double *)prev, STDIM);
  mul((double *)tmp1, (double *)prev, false, (double *)out, STDIM, STDIM, STDIM);
}

// we define the obv matrix is transfer the g0 to gb
static void getJobian(JMatrix_t J, state_t s, msr_t init) {
  double q0 = s[0], q1 = s[1], q2 = s[2], q3 = s[3];
  // normalize quaternion
  normalize(s, 4);
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
  // init the base m0
  avgMSR(now.m0);
  statusInit(now);
  getNoise(now.Q, now.stat, now.deltaSec);
  getJobian(now.F, now.stat, now.m0);
  // TODO: get the covMatrix done
}

static void avgMSR(msr_t m) {
  uint8_t sampleN = 10;
  msr_t tmp;
  for (int i = 0; i < MSRDIM; i++) {
    m[i] = 0;
  }
  for (int i = 0; i < sampleN; i++) {
    getMSR(tmp);
    for (int i = 0; i < MSRDIM; i++) {
      m[i] += tmp[i] / sampleN;
    }
  }
}

// NOTE: 32768 is pow(2, 15)
static void getMSR(msr_t m) {
  int16_t data[3];
  HMCReadData(data);
  for (int i = 0; i < 3; i++) {
    m[i + 3] = (double)data[i] * HMCScale / 32768;
  }

  AccData(data);
  for (int i = 0; i < 3; i++) {
    m[i + 0] = (double)data[i] * AccScale / 32768;
  }

  GyroData(data);
  for (int i = 0; i < 3; i++) {
    m[i + 6] = (double)data[i] * GyroScale / 32768;
  }
}

// after get the initial measurement, use that to 
// calculate the inital altitude
static void statusInit(EKF_t now) {
  double dcm[3][3];
  vecCrossProd(dcm[0], &now.m0[0], &now.m0[3]);
  vecCrossProd(dcm[1], dcm[0], &now.m0[0]);
  vecCrossProd(dcm[2], dcm[0], dcm[1]);
  normalize(dcm[0], 3);
  normalize(dcm[1], 3);
  normalize(dcm[2], 3);
  DCM2quat(now.stat, dcm);
}
