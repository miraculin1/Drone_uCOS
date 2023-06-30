#include "Includes.h"

/* ============
 * predefs
 * ============
 * assume the front side of aircraft is y+ axis
 * - roll is rolling along the y axis
 */

static double translate(int16_t raw, uint32_t scale) {
  double out = (double)raw * scale * 2 / 0xffff;
  return out;
}

void accEuler(float *out) {
  int16_t raw[3];
  AccData(raw);

  float x, y, z;
  // (-1), since the acc output is counter of real acc
  x = (-1) * translate(raw[0], AccScale);
  y = (-1) * translate(raw[1], AccScale);
  z = (-1) * translate(raw[2], AccScale);

  double cosGandX = -z / (sqrt(x * x + z * z));
  double roll = acos(cosGandX);
  if (x < 0) {
    roll *= (-1);
  }
  out[0] = roll;

  double cosGandY = -z / (sqrt(z * z + y * y));
  double pitch = acos(cosGandY);
  if (y > 0) {
    pitch *= (-1);
  }
  out[1] = pitch;
}

// 四元数状态外插方程
// 输入先前状态的四元数，角加速度数据
// 输出预测的四元数
// quaternion[4] = [S, xi, yj, zk]
// sencorData[3] = [x, y, z]
//=============================
// q_new = q_prev + delta_q
// delta_q = 1/2 * q * omega * deltaSec
// omega = some matrix
//=============================
// ref. as quaternion kinematic equation
void quatPred(quaternion prev, quaternion new, sencorData gyroData,
              double deltaSec) {
  double wx, wy, wz;
  wx = gyroData[0];
  wy = gyroData[1];
  wz = gyroData[2];

  double s, i, j, k;
  s = prev[0];
  i = prev[1];
  j = prev[2];
  k = prev[3];

  new[0] = -wx *i - wy *j - wz *k;
  new[1] = wx *s + wz *j - wy *k;
  new[2] = wy *s - wz *i + wx *k;
  new[4] = wz *s + wy *i - wx *k;

  for (int i = 0; i < 4; i++) {
    new[i] *= deltaSec / 2;
  }

  for (int i = 0; i < 4; i++) {
    new[i] += prev[i];
  }

  return;
}

// when initailizing, sencor must stay stable
void measure2quat(quaternion out, sencorData gyro, sencorData mag) { return; }
