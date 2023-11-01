#include "Includes.h"
#include <math.h>
void SendInfo() {
  while (1) {
    printf("%f, %f %f\n", ypr[0] / M_PI * 180, ypr[1] / M_PI * 180, ypr[2] / M_PI * 180);
    /* printf("%d, %d\n", deltatick, OSCPUUsage); */
    /* printf("%.2f %.2f %.2f %d, %d\n", yprOut[0], yprOut[1], yprOut[2], deltatick, OSCPUUsage); */

    /* for (int i = 0; i < 4; ++i) { */
      /* printf("%.2f ", quatOut[i]); */
    /* } */
    /* printf("\n"); */
    /* print(ekftmp.P, 4, 4); */
    /* printf("========\n"); */

    OSTimeDlyHMSM(0, 0, 0, 200);
  }
}
