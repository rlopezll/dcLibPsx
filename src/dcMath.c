#include "dcMath.h"

VECTOR VECTOR_UP = {0,ONE,0};
VECTOR VECTOR_ZERO = {0,0,0};

VECTOR dcMath_Cross(const SVECTOR* v0, const SVECTOR* v1)
{   
    VECTOR cross = {
        ((v0->vy*v1->vz)-(v0->vz*v1->vy))>>12,
	    ((v0->vz*v1->vx)-(v0->vx*v1->vz))>>12,
	    ((v0->vx*v1->vy)-(v0->vy*v1->vx))>>12
    };
    return cross;
}

SVECTOR dcMath_CrossS(const SVECTOR* v0, const SVECTOR* v1)
{
    SVECTOR output;
    VECTOR input = {(((v0->vy*v1->vz)-(v0->vz*v1->vy))>>12),
            (((v0->vz*v1->vx)-(v0->vx*v1->vz))>>12),
            (((v0->vx*v1->vy)-(v0->vy*v1->vx))>>12)
    };

    VectorNormalS(&input, &output);
    return output;
}

SVECTOR dcMath_Normalize(const SVECTOR* vector)
{
    VECTOR input = {vector->vx, vector->vy, vector->vz};
    SVECTOR output;
    VectorNormalS(&input, &output);
    return output;
}

short dcMath_DotProduct(const SVECTOR* v0, const SVECTOR* v1)
{
    return ((v0->vx * v1->vx)>>12) + ((v0->vy * v1->vy)>>12) + ((v0->vz * v1->vz)>>12);
}

long dcMath_Division(long value, long dividend)
{
    return (value<<12)/dividend;
}