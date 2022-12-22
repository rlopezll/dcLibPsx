#ifndef _DC_UTILS_MATH_
#define _DC_UTILS_MATH_

#include <stdlib.h>
#include <libgte.h>

#define ONE 4096
#define OFFSET_FIXED_VECTOR 12

extern VECTOR VECTOR_UP;
extern VECTOR VECTOR_ZERO;

short   dcUtilsMath_DotProduct(const SVECTOR* v0, const SVECTOR* v1);
VECTOR  dcUtilsMath_Cross(const SVECTOR* v0, const SVECTOR* v1);
SVECTOR dcUtilsMath_CrossS(const SVECTOR* v0, const SVECTOR* v1);
SVECTOR dcUtilsMath_Normalize(const SVECTOR* vector);
long    dcUtilsMath_Division(long value, long dividend);

#endif