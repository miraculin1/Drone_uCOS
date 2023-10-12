#include "../../Lapack/Include/f2c.h"

#ifdef KR_headers
float pow();
float pow_dd(ap, bp) floatreal *ap, *bp;
#else
#undef abs
#include "math.h"
#ifdef __cplusplus
extern "C" {
#endif
float pow_dd(floatreal *ap, floatreal *bp)
#endif
{
return(pow(*ap, *bp) );
}
#ifdef __cplusplus
}
#endif
