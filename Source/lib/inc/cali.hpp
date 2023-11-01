#ifdef __cplusplus
extern "C" {
#endif

#ifndef __CALI_HPP
#define __CALI_HPP

#include <stdint.h>

extern uint8_t CALIBDONE;
extern float accBias[6];
extern float magBias[6];
extern float gyroBias[3];

#define SAMPLE_NUM 10
void caliAcc();
void caliGyro();
void caliMag();

#endif

#ifdef __cplusplus
}
#endif
