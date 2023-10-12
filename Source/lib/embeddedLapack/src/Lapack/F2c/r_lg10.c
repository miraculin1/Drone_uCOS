#include "../../Lapack/Include/f2c.h"

#define log10e 0.43429448190325182765

#ifdef KR_headers
float logf();
float r_lg10(x) real *x;
#else
#undef abs
#include "math.h"
#ifdef __cplusplus
extern "C" {
#endif
float r_lg10(real *x)
#endif
{
return( log10e * logf(*x) );
}
#ifdef __cplusplus
}
#endif
