#ifdef __cplusplus
extern "C" {
#endif

#ifndef _EKF_HPP
#define _EKF_HPP

void attitudeEST();
extern int EKFdeltatick;

extern float ypr[3];
extern float gyroRate[3];

#endif

#ifdef __cplusplus
}
#endif
