#ifndef __EKF_h
#define __EKF_h

#include "Includes.h"

// state matrix structure
// q0, q1, q2, q3, bax, bay, baz, bmx, bmy, bmz
#define STDIM 10
typedef double state_t[STDIM];
typedef double statCovariant_t[STDIM][STDIM];
typedef double transMatrix_t[STDIM];

typedef double gyroBias_t[3];

#define MSRDIM 6
typedef double msr_t[MSRDIM];
typedef double msrCovariant_t[MSRDIM][MSRDIM];
typedef double obvMatrix_t[MSRDIM][STDIM];

typedef double kalmanGain_t[STDIM][MSRDIM];
typedef double JMatrix_t[MSRDIM][STDIM];

#endif
