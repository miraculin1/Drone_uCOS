#include "ekf.hpp"
#include "cali.hpp"

extern "C" {
#include "HMC5883.h"
#include "IIC.h"
#include "MPU6050.h"
#include "os_cfg.h"
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

// output yaw pitch roll in rad/s
float ypr[3];
float gyroRate[3];

// use to show how long to finish a attitudeEST
uint32_t lastInterTick;
int deltatick;

using namespace std;
using namespace Eigen;

class EKF {
private:
  // v_r = x * v_b (eigen quaterion mul)
  Quaternionf x;
  // [accX, accY, accZ, magX, magY, magZ]
  Matrix<float, ZDIM, 1> z;
  // [gyroX, gyroY, gyroZ]
  Matrix<float, UDIM, 1> u;
  // covarience matrix for x
  Matrix<float, XDIM, XDIM> P;
  // transition matrix of X, here is linear transition
  Matrix<float, XDIM, XDIM> F;
  Vector3f magBase;
  // kalman gain
  Matrix<float, XDIM, ZDIM> K;
  // 1st time linearlize h(x), Z_predicted = h(x_est)
  Matrix<float, ZDIM, XDIM> H;
  // TODO: update by clocking
  float ATT_RATE = 333;

  void getMsr();
  void initMsr2State();
  void predictX();
  void predictP();
  void calKalGain();
  void updX_est();
  void updP_est();
  void updYPR();

  // low pass filter for mag or acc data
  void LPF(Vector3f &acc, const Vector3f &data, const float &alpha) {
    acc = data * alpha + (1 - alpha) * acc;
  }

public:
  void attitudeEST();
};

// TODO: maybe add threahold for exceeding data
void EKF::getMsr() {
  static const float a4A = 0.1;
  static bool init = false;
  static Vector3f accA = Vector3f::Zero();
  float datatmp[3];
  if (!init) {
    for (int i = 0; i < INITSAMPLES; ++i) {
      IICDMARead();
      AccGData(datatmp, accBias);
      Vector3f tmp = Map<Vector3f>(datatmp, 3, 1);
      LPF(accA, tmp, a4A);
    }
    init = true;
  }
  AccGData(datatmp, accBias);
  Vector3f tmp = Map<Vector3f>(datatmp, 3, 1);
  float norm = tmp.norm();
  if (norm < 1.1 && norm > 0.9) {
    LPF(accA, tmp, a4A);
  }
  z.block<3, 1>(0, 0) = accA;

  MagmGuassData(datatmp, magBias);
  z.block<3, 1>(3, 0) = Map<Vector3f>(datatmp, 3, 1);
  GyroRadpSData(datatmp, gyroBias);
  memcpy(gyroRate, datatmp, sizeof(float) * 3);
  u = Map<Vector3f>(datatmp, 3, 1);
  IICDMARead();
}

// init the x vector to get a faster converge
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

  F << 1, az, -ay, ax, -az, 1, ax, ay, ay, -ax, 1, az, -ax, -ay, -az, 1;

  // NOTE: here eigen's quaterion.coeffs multiply is in [q1 q2 q3 q0] order
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
       -q0 * bx - q3 * by + q2 * bz, -q3 * bx + q0 * by + q1 * bz},
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
  PartialPivLU<Matrix<float, ZDIM, ZDIM>> Slu(S);
  K = tmp * Slu.inverse();
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

// NOTE: not using eigen's eulerangles method since that one always makes yaw in
// range (0 to PI)
void EKF::updYPR() {
  float q0 = x.w(), q1 = -x.x(), q2 = -x.y(), q3 = -x.z();
  ypr[0] =
      atan2f(-2 * (q1 * q2 + q0 * q3), q0 * q0 - q1 * q1 + q2 * q2 - q3 * q3);
  ypr[1] = asinf(2 * (q2 * q3 - q0 * q1));
  ypr[2] =
      atan2f(-2 * (q1 * q3 + q0 * q2), q0 * q0 - q1 * q1 - q2 * q2 + q3 * q3);
}

void EKF::attitudeEST() {
  static int initcnt = 100;
  static uint8_t cnt;
  cnt = initcnt;
  getMsr();
  initMsr2State();
  P << 0.1, 0, 0, 0, 0, 0.1, 0, 0, 0, 0, 0.1, 0, 0, 0, 0, 0.1;
  while (1) {
    if (cnt == initcnt) {
      lastInterTick = OSTime;
    }
    predictX();
    predictP();
    calKalGain();
    updX_est();
    updP_est();
    x.normalize();
    updYPR();
    if (cnt > 0) {
      cnt--;
    } else {
      deltatick = OSTime - lastInterTick;
      // cout << (x.inverse().matrix().eulerAngles(2, 1, 0) * (180 / PI))
      // .transpose()
      // << "," << float(deltatick) / OS_TICKS_PER_SEC / initcnt * 1000
      // << "ms" << int(OSCPUUsage) << "%" << endl;
      cnt = initcnt;
    }
    OSTimeDlyHMSM(0, 0, 0, 3);
  }
}

void attitudeEST() {
  class EKF ekf;
  ekf.attitudeEST();
}
