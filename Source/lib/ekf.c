#include "ekf.h"
#include "Includes.h"
#include "declareFunctions.h"

EKF_T ekftmp;
EKF_T *const ekf = &ekftmp;
static void LPF(double *acc, double *data, double alpha);

const double *const quatOut = ekf->x;
double ATT_RATE = 200;

// read data from three sensors
static void getMsr(EKF_T *ekf) {
  static bool init = false;
  static double accA[3] = {0}, accGyro[3] = {0};
  double datatmp[3];
  if (!init) {
    for (int i = 0; i < 10; ++i) {
      AccGData(datatmp, accBias);
      LPF(accA, datatmp, 0.1);
      /* GyroDpSData(datatmp, gyroBias); */
      /* LPF(accGyro, datatmp, 0.01); */
    }
    init = true;
  }
  AccGData(ekf->z, accBias);
  MagmGuassData(ekf->z + 3, magBias);
  GyroRadpSData(datatmp, gyroBias);
  LPF(accGyro, datatmp, 0.01);
  copy(accGyro, ekf->u, 3, 1);
}

// used to gen state vec according to the new msr
void msr2State(EKF_T *ekf) {
  // quaternion gen
  double dcm[3][3];
  vecCrossProd(dcm[0], &ekf->z[0], &ekf->z[3]);
  vecCrossProd(dcm[1], dcm[0], &ekf->z[0]);
  vecCrossProd(dcm[2], dcm[0], dcm[1]);
  normalize(dcm[0], 3);
  normalize(dcm[1], 3);
  normalize(dcm[2], 3);
  DCM2quat(ekf->x, dcm);
}

void magBase(EKF_T *ekf) {

  // quaternion init
  double dcm[3][3];
  vecCrossProd(dcm[0], &ekf->z[0], &ekf->z[3]);
  vecCrossProd(dcm[1], dcm[0], &ekf->z[0]);
  vecCrossProd(dcm[2], dcm[0], dcm[1]);
  normalize(dcm[0], 3);
  normalize(dcm[1], 3);
  normalize(dcm[2], 3);
  DCM2quat(ekf->x, dcm);

  // initalize the magBase for later h(x)
  // TODO: figure out the way the init magbase
  double qConj[4];
  double qTmp[4], qTmp1[4];
  double v[4];
  quatConj(ekf->x, qConj);
  vec2Quat(ekf->z + 3, v);

  quatMulQuat(ekf->x, v, qTmp);
  quatMulQuat(qTmp, qConj, qTmp1);
  quat2Vec(qTmp1, ekf->magBase);

  normalize(ekf->magBase, 3);
}

// 1st
// update the quaternion(x) by gyro data
void predictX(EKF_T *ekf) {
  double dq[4];
  double qu[4];
  vec2Quat(ekf->u, qu);
  quatMulQuat(ekf->x, qu, dq);
  for (int i = 0; i < ST_DIM; i++) {
    ekf->x[i] += dq[i] / (2 * ATT_RATE);
  }
}

// 2nd
// need jacobian matrix
void preidctP(EKF_T *ekf) {
  double wx = ekf->u[0], wy = ekf->u[1], wz = ekf->u[2];

  double F[ST_DIM * ST_DIM] = {1,
                               -wx / (2 * ATT_RATE),
                               -wy / (2 * ATT_RATE),
                               -wz / (2 * ATT_RATE),
                               wx / (2 * ATT_RATE),
                               1,
                               wz / (2 * ATT_RATE),
                               -wy / (2 * ATT_RATE),
                               wy / (2 * ATT_RATE),
                               -wz / (2 * ATT_RATE),
                               1,
                               wx / (2 * ATT_RATE),
                               wz / (2 * ATT_RATE),
                               wy / (2 * ATT_RATE),
                               -wx / (2 * ATT_RATE),
                               1};

  // TODO: get a Q
  double Q[ST_DIM * ST_DIM] = {0.001, 0, 0,    0, 0, 0.001, 0, 0,
                               0,    0, 0.001, 0, 0, 0,    0, 0.001};
  double tmp[ST_DIM * ST_DIM];
  double tmp1[ST_DIM * ST_DIM];
  mul(F, ekf->P, false, tmp, ST_DIM, ST_DIM, ST_DIM);
  tran(F, 4, 4);
  mul(tmp, F, false, tmp1, ST_DIM, ST_DIM, ST_DIM);
  add(tmp1, Q, ekf->P, ST_DIM, ST_DIM, ST_DIM);
}

// 3rd
// need jacobian
void calKalGain(EKF_T *ekf) {
  double q0 = ekf->x[0], q1 = ekf->x[1], q2 = ekf->x[2], q3 = ekf->x[3];
  double bx = ekf->magBase[0], by = ekf->magBase[1], bz = ekf->magBase[2];
  // in the ref github repo, pronenewbits/Arduino_AHRS_System, its
  // quaternion is qConj of mine, so thats a bit different
  double H[Z_DIM * ST_DIM] = {
      -q2,
      -q3,
      -q0,
      -q1,
      q1,
      q0,
      -q3,
      -q2,
      -q0,
      q1,
      q2,
      -q3,
      q0 * bx - q3 * by + q2 * bz,
      q1 * bx + q2 * by + q3 * bz,
      -q2 * bx + q1 * by + q0 * bz,
      -q3 * bx - q0 * by + q1 * bz,
      q3 * bx + q0 * by - q1 * bz,
      q2 * bx - q1 * by - q0 * bz,
      q1 * bx + q2 * by + q3 * bz,
      q0 * bx - q3 * by + q2 * bz,
      -q2 * bx + q1 * by + q0 * bz,
      q3 * bx + q0 * by - q1 * bz,
      -q0 * bx + q3 * by - q2 * bz,
      q1 * bx + q2 * by + q3 * bz,
  };
  scale(H, 2, Z_DIM, ST_DIM);
  copy(H, ekf->H, Z_DIM, ST_DIM);
  // below H is ok to ues

  // TODO: get msr cov-M
  double R[Z_DIM * Z_DIM] = {
      0.001, 0, 0, 0,    0, 0, 0, 0.001, 0, 0, 0,    0, 0, 0, 0.001, 0, 0, 0,
      0,     0, 0, 0.01, 0, 0, 0, 0,     0, 0, 0.01, 0, 0, 0, 0,     0, 0, 0.01,
  };

  double HT[Z_DIM * ST_DIM];
  copy(H, HT, Z_DIM, ST_DIM);
  tran(HT, Z_DIM, ST_DIM);

  // make it big enough for all use
  double tmp[8 * 8];
  double Stmp[Z_DIM * Z_DIM];
  double S[Z_DIM * Z_DIM];

  mul(ekf->P, HT, false, tmp, ST_DIM, ST_DIM, Z_DIM);
  mul(H, tmp, false, Stmp, Z_DIM, ST_DIM, Z_DIM);
  add(Stmp, R, S, Z_DIM, Z_DIM, Z_DIM);

  inv(S, Z_DIM);

  mul(tmp, S, false, ekf->K, ST_DIM, Z_DIM, Z_DIM);
}

// 4th
// get msr use KalGain to upd predictX, get optimal res
void updX_est(EKF_T *ekf) {
  double q0 = ekf->x[0], q1 = ekf->x[1], q2 = ekf->x[2], q3 = ekf->x[3];
  double bx = ekf->magBase[0], by = ekf->magBase[1], bz = ekf->magBase[2];
  double hx[6] = {
      -2 * q0 * q2 - 2 * q1 * q3,
      -2 * q3 * q2 + 2 * q0 * q1,
      -q0 * q0 - q3 * q3 + q2 * q2 + q1 * q1,
      (q0 * q0 + q1 * q1 - q2 * q2 - q3 * q3) * bx +
          2 * (q1 * q2 - q0 * q3) * by + 2 * (q1 * q3 + q0 * q2) * bz,
      2 * (q1 * q2 + q0 * q3) * bx +
          (q0 * q0 - q1 * q1 + q2 * q2 - q3 * q3) * by +
          2 * (q2 * q3 - q0 * q1) * bz,
      2 * (q1 * q3 - q0 * q2) * bx + 2 * (q2 * q3 + q0 * q1) * by +
          (q0 * q0 - q1 * q1 - q2 * q2 + q3 * q3) * bz,
  };

  getMsr(ekf);

  double ztmp[Z_DIM];
  sub(ekf->z, hx, ztmp, Z_DIM, 1, 1);

  double xtmp[ST_DIM];
  double xtmp1[ST_DIM];
  mul(ekf->K, ztmp, false, xtmp, ST_DIM, Z_DIM, 1);
  add(ekf->x, xtmp, xtmp1, ST_DIM, 1, 1);
  copy(xtmp1, ekf->x, ST_DIM, 1);
}

// 5th
// need jacobian
void updP_est(EKF_T *ekf) {
  double tmp[4 * 4], tmp1[4 * 4];
  mul(ekf->K, ekf->H, false, tmp, ST_DIM, Z_DIM, ST_DIM);
  double I4[4 * 4];
  eye(I4, 4, 4);
  sub(I4, tmp, tmp1, ST_DIM, ST_DIM, ST_DIM);

  mul(tmp1, ekf->P, false, tmp, ST_DIM, ST_DIM, ST_DIM);
  copy(tmp, ekf->P, ST_DIM, ST_DIM);
}

void attitudeEST() {
  // initalize ekf
  getMsr(ekf);
  magBase(ekf);
  // init quaternion
  msr2State(ekf);
  // TODO: convariance matrix need to initalize
  double Ptmp[ST_DIM * ST_DIM] = {0.1, 0, 0,   0, 0, 0.1, 0, 0,
                                  0,   0, 0.1, 0, 0, 0,   0, 0.1};
  copy(Ptmp, ekf->P, ST_DIM, ST_DIM);
  while (1) {
    predictX(ekf);
    preidctP(ekf);
    calKalGain(ekf);
    updX_est(ekf);
    updP_est(ekf);
    normalize(ekf->x, 4);
    OSTimeDlyHMSM(0, 0, 0, 1.0 / ATT_RATE * 1000);
  }
}

void outputDCM() {
  double dcm[3][3];
  double qtmp[4];
  quatConj(ekf->x, qtmp);
  quat2DCM(qtmp, dcm);
  print((double *)dcm, 3, 3);
  printf("\n");
}

void outputForPython() {
  double dcm[3][3];
  quat2DCM(ekf->x, dcm);
  for (int i = 0; i < 3; ++i) {
    for (int j = 0; j < 3; ++j) {
      printf("%.2f ", dcm[i][j]);
    }
  }
  printf("\n");
}

void outputYPR(double *yaw, double *pitch, double *roll) {
  double q0 = ekf->x[0], q1 = ekf->x[1], q2 = ekf->x[2], q3 = ekf->x[3];
  *yaw = atan2(-2 * (q1 * q2 + q0 * q3), q0 * q0 - q1 * q1 + q2 * q2 - q3 * q3) / M_PI * 180;
  *pitch = asin(2 * (q2 * q3 - q0 * q1)) / M_PI * 180;
  *roll = atan2(-2 * (q1 * q3 + q0 * q2), q0 * q0 - q1 * q1 - q2 * q2 + q3 * q3) / M_PI * 180;
}

static void LPF(double *acc, double *data, double alpha) {
  for (int dim = 0; dim < 3; ++dim) {
    acc[dim] = acc[dim] * (1 - alpha) + alpha * data[dim];
  }
}
