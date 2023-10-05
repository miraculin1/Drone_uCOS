#include "cali.h"
#include "Includes.h"
#include "declareFunctions.h"

static void ellipse(void (*fp)(double *, double *), double *bais);

// repsent the state of the cali
// bit2-acc bit1-mag bit0-gyro
uint8_t CALIBDONE = 0b110;

// ellipse vec(O) vec(S)
double accBias[6] = {-0.033726262708522053, 0.0088630975845296577,
                     0.0069665471089496861, 1.0067846186785343,
                     1.0036822097709142,    1.0066589186384178};
double magBias[6] = {-222.14524931519642, 74.825429233358093,
                     269.44834375493491, 454.18431313304973,
                     467.95234004152985, 459.20061975143091};
// vec(O)
double gyroBias[3] = {0};

// r = 1 - (ax - ox)sx^2 - (ay - oy)sy^2 - (az - oz)sz^2
// using least square
void caliAcc() {
  double tmp[6] = {0};
  ellipse(AccGData, tmp);
  double x0, y0, z0, A, B, C;
  x0 = -tmp[2] / 2;
  y0 = -tmp[3] / 2 / tmp[0];
  z0 = -tmp[4] / 2 / tmp[1];
  A = sqrt(x0 * x0 + tmp[0] * y0 * y0 + tmp[1] * z0 * z0 - tmp[5]);
  B = A / sqrt(tmp[0]);
  C = A / sqrt(tmp[1]);

  accBias[0] = x0;
  accBias[1] = y0;
  accBias[2] = z0;
  accBias[3] = A;
  accBias[4] = B;
  accBias[5] = C;
  CALIBDONE |= 0x1 << 2;
}

void caliMag() {
  double tmp[6] = {0};
  ellipse(MagmGuassData, tmp);
  double x0, y0, z0, A, B, C;
  x0 = -tmp[2] / 2;
  y0 = -tmp[3] / 2 / tmp[0];
  z0 = -tmp[4] / 2 / tmp[1];
  A = sqrt(x0 * x0 + tmp[0] * y0 * y0 + tmp[1] * z0 * z0 - tmp[5]);
  B = A / sqrt(tmp[0]);
  C = A / sqrt(tmp[1]);

  magBias[0] = x0;
  magBias[1] = y0;
  magBias[2] = z0;
  magBias[3] = A;
  magBias[4] = B;
  magBias[5] = C;
  CALIBDONE |= 0x1 << 1;
}

// sample when sensor stays stable, simple subtract this
void caliGyro() {
  printf("Gyro calibration start in 3 sec...\n");
  OSTimeDlyHMSM(0,0,3,0);
  printf("now\n");

  double tmp[3];
  for (int i = 0; i < SAMPLE_NUM; ++i) {
    GyroDpSData(tmp, NULL);
    for (int dim = 0; dim < 3; ++dim) {
      gyroBias[dim] += tmp[dim];
    }
  }
  for (int dim = 0; dim < 3; ++dim) {
    gyroBias[dim] /= 10;
  }
  CALIBDONE |= 0x1;
}

static void DataInput(double *data, double *m, double *right) {
  // helper to accumlate the matrix
  double tmp[7] = {data[1] * data[1], data[2] * data[2], data[0],
                   data[1],           data[2],           1,
                   -data[0] * data[0]};

  for (int row = 0; row < 6; ++row) {
    for (int col = 0; col < 6; ++col) {
      m[row * 6 + col] += tmp[row] * tmp[col];
    }
  }

  for (int row = 0; row < 6; ++row) {
    right[row] += tmp[6] * tmp[row];
  }
}

static void ellipse(void (*msrFunc)(double *, double *), double *bais) {
  enum TYPE_T { acc, mag };
  enum TYPE_T type;
  double M[6 * 6] = {0};
  double right[6] = {0};

  int sampleLimit;

  double res[6];

  double data[3];
  if (msrFunc == AccGData) {
    sampleLimit = SAMPLE_NUM;
    type = acc;
  } else if (msrFunc == MagmGuassData) {
    sampleLimit = SAMPLE_NUM * 100;
    type = mag;
  } else {
    return;
  }

  if (type == mag) {
    printf("mag start in 2 sec\n");
    OSTimeDlyHMSM(0, 0, 2, 0);
    printf("starting\n");
  }

  for (int i = 0; i < sampleLimit; ++i) {
    if (type == acc) {
      printf("#%d msr in 5 sec, KEEP STAYBLE...\n", i);
      OSTimeDlyHMSM(0, 0, 5, 0);
    } else {
      OSTimeDlyHMSM(0, 0, 0, 50);
    }
    msrFunc(data, NULL);
    DataInput(data, M, right);
    if (type == acc) {
      printf("#%d, done\n", i);
    }
  }

  if (type == mag) {
    printf("mag done\n");
  }

  for (int i = 0; i < 6 * 6; ++i) {
    M[i] /= sampleLimit;
  }

  for (int i = 0; i < 6; ++i) {
    right[i] /= sampleLimit;
  }

  linsolve(M, res, right, 6, 1);
  for (int i = 0; i < 6; ++i) {
    bais[i] = res[i];
  }
}
