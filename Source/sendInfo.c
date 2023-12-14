#include "Includes.h"
#include "pid.h"
#include <math.h>
static bool ekf = false;
static bool use = false;
static bool rec = false;
static bool thro = false;
static bool pid = false;
static bool gyro = false;
static bool inner = false;
void SendInfo() {
  OSTaskSuspend(PriSendinfo);
  while (1) {
    if (thro) {
      for (int i = 0; i < 4; i++) {
        printf("%d ", fourMotorG[i]);
      }
      printf("\n");
    }
    if (rec) {
      for (int i = 0; i < 9; ++i) {
        printf("%4d ", recData.chs[i]);
      }
      printf("\n");
    }
    if (ekf) {
      printf("%f, %f %f\n", ypr[0] / M_PI * 180, ypr[1] / M_PI * 180,
             ypr[2] / M_PI * 180);
    }
    if (gyro) {
      printf("%f, %f %f\n", gyroRate[0] / M_PI * 180, gyroRate[1] / M_PI * 180,
             gyroRate[2] / M_PI * 180);
    }
    if (use) {
      printf("EKF:%.2fms CON:%.2fms CPU:%d\n", (float)EKFdeltatick / 100, (float)CONdeltatick / 100, OSCPUUsage);
    }
    if (pid) {
      printf("pos: ");
      for (int i = 0; i < 3; ++i) {
        printf("%.2f ", posPID.control[i]);
      }
      printf("\n");

      printf("rate: ");
      for (int i = 0; i < 3; ++i) {
        printf("%.2f ", ratePID.control[i]);
      }
      printf("\n");
    }
    if (inner) {
      printf("%.2f %.2f %.2f\n", posPID.control[0], posPID.control[1], posPID.control[2]);
      printf("%.2f %.2f %.2f\n", gyroRate[0], gyroRate[1], gyroRate[2]);
    }

    OSTimeDlyHMSM(0, 0, 0, 100);
  }
}

void shellSendInfo(int argc, char *argv[]) {
  if (argc == 1) {
    OS_TCB tmp;
    OSTaskQuery(PriSendinfo, &tmp);
    if (tmp.OSTCBStat & OS_STAT_SUSPEND) {
      OSTaskResume(PriSendinfo);
      printf("[INFO] log on\n");
    } else {
      OSTaskSuspend(PriSendinfo);
      printf("[INFO] log off\n");
    }
  } else {
    if (strcmp(argv[1], "ekf") == 0) {
      ekf ^= true;
    } else if (strcmp(argv[1], "use") == 0) {
      use ^= true;
    } else if (strcmp(argv[1], "rec") == 0) {
      rec ^= true;
    } else if (strcmp(argv[1], "thro") == 0) {
      thro ^= true;
    } else if (strcmp(argv[1], "pid") == 0) {
      pid ^= true;
    } else if (strcmp(argv[1], "gyro") == 0) {
      gyro ^= true;
    } else if (strcmp(argv[1], "inner") == 0) {
      inner ^= true;
    } else {
      printf("bad pram\n");
    }
  }
}
