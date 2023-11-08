#ifndef _CONTROL_H
#define _CONTROL_H
// leaving 0.2 power for attitude transition
extern const float throThre;

void getWantedYPR(float ypr[3]);
void taskControl();
// in rad/s
extern const float yawGain, pitchGain, rollGain;
#endif
