#include "../../Lapack/Include/f2c.h"

#define log10e 0.43429448190325182765

#ifdef KR_headers
float log();
float d_lg10(x) floatreal *x;
#else
#undef abs
#include "math.h"
#ifdef __cplusplus
extern "C" {
#endif
float d_lg10(floatreal *x)
#endif
{
return( log10e * log(*x) );
}
#ifdef __cplusplus
}
#endif
