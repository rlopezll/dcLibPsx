
#ifndef _DC_MATH_H
#define _DC_MATH_H

#define DC_PI 0x7AB8 // 3.1416
#define DC_ONE 0x1000 // 4096 

#define DC_MUL(fixedA, fixedB) ( (fixedA * fixedB) >> 12 )
#define DC_DIV(fixedA, fixedB) ( (fixedA / fixedB) << 12 )


#endif /* _DC_MATH_H */
