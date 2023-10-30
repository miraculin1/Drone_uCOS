#include "ekf.hpp"

extern "C" {
#include "HMC5883.h"
#include "IIC.h"
#include "MPU6050.h"
#include "cali.h"
#include "ucos_ii.h"
#include <stdio.h>
}

#include <Eigen/Core>
#include <Eigen/Dense>
#include <Eigen/Geometry>
#include <Eigen/LU>
#include <iostream>

const int XDIM = 4;
const int ZDIM = 6;
const int UDIM = 3;
const int INITSAMPLES = 10;
const float PI = 3.14159265359f;

const float *pToQuat;
const float *pToYPR;

int deltatick;

using namespace std;
using namespace Eigen;

class EKF {
private:
  Quaternionf x;
  Matrix<float, ZDIM, 1> z;
  Matrix<float, UDIM, 1> u;
  Matrix<float, XDIM, XDIM> P;
  Matrix<float, XDIM, XDIM> F;
  Vector3f magBase;
  Matrix<float, XDIM, ZDIM> K;
  Matrix<float, ZDIM, XDIM> H;
  // TODO: update by clocking
  float ATT_RATE = 20;

  void getMsr();
  void initMsr2State();
  void predictX();
  void predictP();
  void calKalGain();
  void updX_est();
  void updP_est();
  void updOutput();

  void LPF(Vector3f &acc, const Vector3f &data, const float &alpha) {
    acc = data * alpha + (1 - alpha) * acc;
  }

public:
  void attitudeEST();
  void getYPR(float ypr[3]);
  float *quatOut;
  float *yprOut;
};

void EKF::getMsr() {
  static const float a4A = 1;
  static bool init = false;
  static Vector3f accA = Vector3f::Zero();
  float datatmp[3];
  if (!init) {
  AccGData(datatmp, accBias);
  accA = Map<Vector3f>(datatmp, 3, 1);
  init = true;
  }
  AccGData(datatmp, accBias);
  Vector3f tmp = Map<Vector3f>(datatmp, 3, 1);
  LPF(accA, tmp, a4A);
  z.block<3, 1>(0, 0) = accA;

  MagmGuassData(datatmp, magBias);
  z.block<3, 1>(3, 0) = Map<Vector3f>(datatmp, 3, 1);
  GyroRadpSData(datatmp, gyroBias);
  u = Map<Vector3f>(datatmp, 3, 1);
  IICDMARead();
}

void EKF::initMsr2State() {
  Matrix3f dcm;
  dcm.row(0) = z.head<3>().cross(z.tail<3>());
  dcm.row(1) = dcm.row(0).transpose().cross(z.head<3>());
  dcm.row(2) = dcm.row(0).transpose().cross(dcm.row(1).transpose());

  dcm.row(0).normalize();
  dcm.row(1).normalize();
  dcm.row(2).normalize();
  // convert dcm to quaternion
  x = dcm;
  x.normalize();
  // geting a magBase
  magBase = x * z.tail<3>();
}

void EKF::predictX() {
  float twiceATT_RATE = 2 * ATT_RATE;
  float ax = u(0) / twiceATT_RATE, ay = u(1) / twiceATT_RATE,
        az = u(2) / twiceATT_RATE;

  F << 1, -ax, -ay, -az, ax, 1, az, -ay, ay, -az, 1, ax, az, ay, -ax, 1;

  x.coeffs() = F * x.coeffs();
}

void EKF::predictP() {

  // TODO: get a Q
  Matrix<float, XDIM, XDIM> Q{
      {0.001, 0, 0, 0}, {0, 0.001, 0, 0}, {0, 0, 0.001, 0}, {0, 0, 0, 0.001}};
  P = F * P;
  P = P * F.transpose();
  P += Q;
}

void EKF::calKalGain() {

  float q0 = x.w(), q1 = x.x(), q2 = x.y(), q3 = x.z();
  float bx = magBase[0], by = magBase[1], bz = magBase[2];
  // in the ref github repo, pronenewbits/Arduino_AHRS_System, its
  // quaternion is qConj of mine, so thats a bit different
  Matrix<float, ZDIM, XDIM> H_tmp{
      {-q3, q0, -q1, q2},
      {-q0, -q3, -q2, -q1},
      {q1, q2, -q3, -q0},
      {q1 * bx + q2 * by + q3 * bz, -q2 * bx + q1 * by - q0 * bz,
       -q3 * bx + q0 * by + q1 * bz, q0 * bx + q3 * by - q2 * bz},
      {q2 * bx - q1 * by + q0 * bz, q1 * bx + q2 * by + q3 * bz,
       - q0 * bx - q3 * by + q2 * bz, -q3 * bx + q0 * by + q1 * bz},
      {q3 * bx - q0 * by - q1 * bz, q0 * bx + q3 * by - q2 * bz,
       q1 * bx + q2 * by + q3 * bz, q2 * bx - q1 * by + q0 * bz},
  };
  H_tmp *= 2;
  // below H is ok to ues
  H = H_tmp;

  // TODO: get msr cov-M
  Matrix<float, ZDIM, ZDIM> R{
      {0.001, 0, 0, 0, 0, 0}, {0, 0.001, 0, 0, 0, 0}, {0, 0, 0.001, 0, 0, 0},
      {0, 0, 0, 0.01, 0, 0},  {0, 0, 0, 0, 0.01, 0},  {0, 0, 0, 0, 0, 0.01},
      // {0.001, 0, 0, 0, 0, 0}, {0, 0.001, 0, 0, 0, 0}, {0, 0, 0.001, 0, 0, 0},
      // {0, 0, 0, 0.01, 0, 0},  {0, 0, 0, 0, 0.01, 0},  {0, 0, 0, 0, 0, 0.01},
  };

  Matrix<float, XDIM, ZDIM> tmp = P * H.transpose();

  Matrix<float, ZDIM, ZDIM> S = H * tmp + R;
  K = tmp * S.inverse();
}

void EKF::updX_est() {
  Matrix<float, 6, 1> hx;
  Vector3f g0(0, 0, -1);
  hx.head<3>() = x.inverse() * g0;
  hx.tail<3>() = x.inverse() * magBase;

  getMsr();

  Matrix<float, 6, 1> y = z - hx;

  x.coeffs() = x.coeffs() + K * y;
}

void EKF::updP_est() {
  Matrix<float, XDIM, XDIM> I = Matrix<float, XDIM, XDIM>::Identity();
  P = (I - K * H) * P;
}

void EKF::updOutput() {
  pToQuat = x.coeffs().data();
  pToYPR = x.matrix().eulerAngles(2, 1, 0).data();
}

void EKF::attitudeEST() {
  static uint8_t initcnt = 100;
  static uint8_t cnt;
  cnt = initcnt;
  static uint32_t lastInterTick;
  IICDMARead();
  getMsr();
  initMsr2State();
  P << 0.1, 0, 0, 0, 0, 0.1, 0, 0, 0, 0, 0.1, 0, 0, 0, 0, 0.1;
  while (1) {
    predictX();
    predictP();
    calKalGain();
    updX_est();
    updP_est();
    x.normalize();
    updOutput();
    if (cnt > 0) {
      cnt--;
    } else {
      deltatick = OSTime - lastInterTick;
      lastInterTick = OSTime;
      cnt = initcnt;
    }
    cout << "======" << endl;
    cout << (x * z.tail<3>()).transpose() << (x * z.head<3>()).transpose() << endl;
    OSTimeDlyHMSM(0, 0, 0, 1000 / ATT_RATE);
  }
}

void attitudeEST() {
  class EKF ekf;
  ekf.attitudeEST();
}
