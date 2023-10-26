#include "ekf.hpp"
#include "Includes.h"
#include <Eigen/Core>
#include <Eigen/Dense>
#include <Eigen/Geometry>
#include <iostream>

const int XDIM = 4;
const int ZDIM = 6;
const int UDIM = 3;

const int INITSAMPLES = 10;
const float PI = 3.14159265359f;

using namespace std;
using namespace Eigen;

class EKF {
private:
  Quaternionf x;
  Matrix<float, ZDIM, 1> z;
  Matrix<float, UDIM, 1> u;
  Matrix<float, XDIM, XDIM> P;
  Vector3f magBase;
  Matrix<float, XDIM, ZDIM> K;
  Matrix<float, ZDIM, XDIM> H;
  // TODO: update by clocking
  float ATT_RATE;
  void getMsr();
  void initMsr2State();
  void predictX();
  void predictP();
  void calKalGain();
  void updX_est();
  void updP_est();

  void LPF(Vector3f acc, Vector3f data, float alpha) {
    acc = data * alpha + (1 - alpha) * acc;
  }

public:
  void getYPR(float ypr[3]);
};

void EKF::getMsr() {
  static const float a4A = 0.4;
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
  z.block(0, 0, 3, 1) = accA;

  MagmGuassData(datatmp, magBias);
  z.block(2, 0, 3, 1) = Map<Vector3f>(datatmp, 3, 1);
  GyroRadpSData(datatmp, gyroBias);
  u = Map<Vector3f>(datatmp, 3, 1);
  IICDMARead();
}

// void EKF::initMsr2State() {
  // Matrix3f dcm;
  // dcm.row(0) = z.block(0, 0, 3, 1).cross(z.block(2, 0, 3, 1));
  // dcm.row(1) = dcm.row(0).transpose().cross(z.block(0, 0, 3, 1));
  // dcm.row(2) = dcm.row(0).transpose().cross(dcm.row(1).transpose());
//
  // dcm.row(0).normalize();
  // dcm.row(1).normalize();
  // dcm.row(2).normalize();
  // // convert dcm to quaternion
  // x = dcm;
  // x.normalize();
  // // geting a magBase
  // magBase = x * z.block(2, 0, 3, 1);
// }
//
// void EKF::predictX() {
// }
//
// void test() {
  // cout << "hello";
// }
