
#include "dcCollision.h"
#include "dcMath.h"

long dcCollision_RaySphereInteresct( VECTOR* rayOrigin, SVECTOR* rayDir, VECTOR* sphereCenter, long sphereRadius )
{
    // Note: I'm using shl instead of integer multiply because it's probably faster. But I'm just guessing it.
    
    // Translate sphere to the origin
    VECTOR s = { rayOrigin->vx - sphereCenter->vx, rayOrigin->vy - sphereCenter->vy, rayOrigin->vz - sphereCenter->vz };

    // compute quadratic equation coefficients
    long a = DC_DOT(rayDir, rayDir);
    long b = DC_DOT64(&s, rayDir) << 1; // shl1 instead of x2
    long c = DC_DOT64(&s, &s) - DC_MUL64(sphereRadius, sphereRadius);

    long discriminant = DC_MUL64(b, b) - (DC_MUL64(a, c) << 2); // b^2 - 4ac (shl2 instead of x4)
    if(discriminant < 0)
    {
        return 0;
    }

    long discSqrt = SquareRoot12(discriminant);
    long t1 = DC_DIV( -b - discSqrt, a << 1 );
    long t2 = DC_DIV( -b + discSqrt, a << 1 );
    
    // return closest intersection point which is in front of ray, or closest intersection point if both are behind.
    long minT = DC_MIN(t1, t2);
    return minT >= 0 ? minT : DC_MAX(t1, t2);
}

long dcCollision_RayPlaneInteresct( VECTOR* rayOrigin, SVECTOR* rayDir, SVECTOR* planeNormal, long planeHeight )
{
    return 0;
}

long dcCollision_RayAABBInteresct( VECTOR* rayOrigin, SVECTOR* rayDir, VECTOR* boxHalfSize, VECTOR* boxCenter )
{
    return 0;
}

long dcCollision_RayBOXInteresct( VECTOR* rayOrigin, SVECTOR* rayDir, VECTOR* boxHalfSize, MATRIX* boxTransform )
{
    return 0;
}

long dcCollision_SpheresOverlap( VECTOR* sphere1Center, VECTOR* sphere2center, long sphere1Radius, long sphere2Radius )
{
    return 0;
}
