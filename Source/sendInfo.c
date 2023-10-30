#include "Includes.h"
/* static void magBaseTest(EKF_T *ekf) { */
/*  */
  /* void msr2State(EKF_T *ekf); */
  /* msr2State(ekf); */
/*  */
  /* [> // print out the quaternion <] */
  /* [> for (int i = 0; i < 4; i++) { <] */
  /* [> printf("%.2f ", ekf->x[i]); <] */
  /* [> } <] */
  /* [> printf("   "); <] */
/*  */
  /* float v[4], conx[4]; */
  /* float qtmp[4]; */
  /* quatConj(ekf->x, conx); */
  /* vec2Quat(ekf->magBase, v); */
  /* quatMulQuat(conx, v, qtmp); */
  /* quatMulQuat(qtmp, ekf->x, v); */
/*  */
  /* for (int i = 1; i < 4; i++) { */
    /* printf("%.2f ", v[i]); */
  /* } */
  /* printf("\n"); */
/*  */
  /* printf("%.2f ", ekf->z[3]); */
  /* printf("%.2f ", ekf->z[4]); */
  /* printf("%.2f, %.1f\n", ekf->z[5], vecMod(3, ekf->z + 3)); */
  /* printf("============\n"); */
/* } */
/*  */
/* static void accBaseTest(EKF_T *ekf) { */
  /* void msr2State(EKF_T *ekf); */
  /* msr2State(ekf); */
/*  */
  /* float v[4], conx[4]; */
  /* float qtmp[4]; */
  /* quatConj(ekf->x, conx); */
  /* vec2Quat(ekf->z, v); */
  /* quatMulQuat(ekf->x, v, qtmp); */
  /* quatMulQuat(qtmp, conx, v); */
/*  */
  /* for (int i = 1; i < 4; i++) { */
    /* printf("%.2f ", v[i]); */
  /* } */
  /* printf("\n"); */
/*  */
  /* printf("%.2f ", ekf->z[0]); */
  /* printf("%.2f ", ekf->z[1]); */
  /* printf("%.2f, %.1f\n", ekf->z[2], vecMod(3, ekf->z)); */
  /* printf("============\n"); */
/* } */
/*  */
/* static void printMagForPlot() { */
  /* float data[3]; */
  /* MagmGuassData(data, magBias); */
  /* printf("%.2f ", data[0]); */
  /* printf("%.2f ", data[1]); */
  /* printf("%.2f\n", data[2]); */
/* } */

void SendInfo() {
  while (1) {
    /* printf("%f, %f %f\n", pToYPR[0], pToYPR[1], pToYPR[2]); */
    /* printf("%d, %d\n", deltatick, OSCPUUsage); */
    /* printf("%.2f %.2f %.2f %d, %d\n", yprOut[0], yprOut[1], yprOut[2], deltatick, OSCPUUsage); */

    /* for (int i = 0; i < 4; ++i) { */
      /* printf("%.2f ", quatOut[i]); */
    /* } */
    /* printf("\n"); */
    /* print(ekftmp.P, 4, 4); */
    /* printf("========\n"); */

    OSTimeDlyHMSM(0, 0, 1, 0);
  }
}
