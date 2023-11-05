#include "Includes.h"
#include <math.h>
void SendInfo() {
  OSTaskSuspend(PriSendinfo);
  while (1) {
    printf("%d, %d, %d, %d, %d, %d, %d\n", recData.linedUp, recData.chs[0], recData.chs[1], recData.chs[2],recData.chs[3], recData.chs[4], recData.chs[5]);
    /* printf("%f, %f %f\n", ypr[0] / M_PI * 180, ypr[1] / M_PI * 180, */
           /* ypr[2] / M_PI * 180); */
    /* printf("%d, %d\n", deltatick, OSCPUUsage); */
    /* printf("%.2f %.2f %.2f %d, %d\n", yprOut[0], yprOut[1], yprOut[2],
     * deltatick, OSCPUUsage); */

    /* for (int i = 0; i < 4; ++i) { */
    /* printf("%.2f ", quatOut[i]); */
    /* } */
    /* printf("\n"); */
    /* print(ekftmp.P, 4, 4); */
    /* printf("========\n"); */
    OSTimeDlyHMSM(0, 0, 1, 0);
  }
}

void logToggle() {
  OS_TCB tmp;
  OSTaskQuery(PriSendinfo, &tmp);
  if (tmp.OSTCBStat & OS_STAT_SUSPEND) {
    OSTaskResume(PriSendinfo);
  } else {
    OSTaskSuspend(PriSendinfo);
  }
}
