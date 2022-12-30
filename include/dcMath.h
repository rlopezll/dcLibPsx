
#ifndef _DC_MATH_H
#define _DC_MATH_H

#include <stdlib.h>
#include <libgte.h>

#define ONE 4096
#define OFFSET_FIXED_VECTOR 12

#define DC_PI 0x7AB8 // 3.1416
#define DC_ONE 0x1000 // 4096 
#define DC_EPSILON 1

extern VECTOR VECTOR_UP;
extern VECTOR VECTOR_ZERO;

#define DC_ONEREV DC_ONE // 360 degrees is represented as 1
#define DC_HALFREV 2048

#define DC_MUL(fixedA, fixedB) ( ((fixedA) * (fixedB)) >> 12 )
#define DC_DIV(fixedA, fixedB) ( ((fixedA) << 12) / (fixedB) )
#define DC_DOT(v1, v2) (DC_MUL((v1)->vx, (v2)->vx) + DC_MUL((v1)->vy, (v2)->vy) + DC_MUL((v1)->vz, (v2)->vz))
#define DC_DOT64(v1, v2) (DC_MUL64((v1)->vx, (v2)->vx) + DC_MUL64((v1)->vy, (v2)->vy) + DC_MUL64((v1)->vz, (v2)->vz))
static long DC_MUL64(long v0, long v1);

#define DC_MAX(a,b) (((a) > (b)) ? (a) : (b))
#define DC_MIN(a,b) (((a) < (b)) ? (a) : (b))
#define DC_LERP(fixedA, fixedB, alpha)( DC_MUL64(fixedA, DC_ONE - (alpha)) + DC_MUL64(fixedB, alpha) )

short   dcMath_DotProduct(const SVECTOR* v0, const SVECTOR* v1);
VECTOR  dcMath_Cross(const SVECTOR* v0, const SVECTOR* v1);
SVECTOR dcMath_CrossS(const SVECTOR* v0, const SVECTOR* v1);
SVECTOR dcMath_Normalize(const SVECTOR* vector);
long    dcMath_Division(long value, long dividend);

/* in-line assembly function for performing multiplication        */
/* operations with 12-bit fractions. Uses what is effectively  */
/* 64-bit maths with both hi and lo result registers to avoid  */
/* overflow bugs when using 32-bit maths.                      */
/*                                                             */
/* Performs ( v0 * v1 )>>12 with 64-bit arithmetic        */
/*                                                             */
static inline long DC_MUL64(long v0, long v1)
{
    long result;
    __asm__ volatile ( \
        "mult   %2, %1;"        /* multiply values (small * large is faster) */\ 
        "mflo   %0;"           /* retrieve the 64-bit result in two regs    */\
        "mfhi   $t0;"                                                          \
        "srl    %0, 12;"       /* equivalent to dividing LO by 4096         */\
        "and    $t0, 0x0fff;"   /* mask HI result to fit in upper 12 bits of */\
        "sll    $t0, 20;"       /* LO result, then shift bits to position    */\
        "or	    %0, $t0;"      /* combine the bits                          */\
        : "=r"( result ) 														   \
        : "r"( v0 ), "r"( v1 ) 
        : "lo", "hi", "t0" 
    );
    return result;
}

#endif /* _DC_MATH_H */
