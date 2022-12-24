
#ifndef _DC_COLLISION_H
#define _DC_COLLISION_H

#include <stddef.h>
#include <libgte.h>

/* RAY INTERSECTIONS */

long dcCollision_RaySphereInteresct( VECTOR* rayOrigin, SVECTOR* rayDir, VECTOR* sphereCenter, long sphereRadius );
long dcCollision_RayPlaneInteresct( VECTOR* rayOrigin, SVECTOR* rayDir, SVECTOR* planeNormal, long planeHeight );
long dcCollision_RayAABBInteresct( VECTOR* rayOrigin, SVECTOR* rayDir, VECTOR* boxHalfSize, VECTOR* boxCenter );
long dcCollision_RayBOXInteresct( VECTOR* rayOrigin, SVECTOR* rayDir, VECTOR* boxHalfSize, MATRIX* boxTransform );

/* SHAPE OVERLAPS */

long dcCollision_SpheresOverlap( VECTOR* sphere1Center, VECTOR* sphere2center, long sphere1Radius, long sphere2Radius );


#endif /* _DC_COLLISION_H */
