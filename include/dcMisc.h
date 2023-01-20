
#ifndef _DC_MISC_H
#define _DC_MISC_H

#include "dcRender.h"
#include "dcCamera.h"

struct SDC_Camera;

SDC_Mesh3D* dcMisc_generateSphereMesh(long radius, unsigned latDivs, unsigned longDivs);
SDC_Mesh3D* dcMisc_GeneratePlaneMeshXZ(long halfSize, u_long xDivs, u_long zDivs, EDC_PolygonVertexType vFormat );

long dcMisc_Noise(long x, long y, long z);
long dcMisc_Fbm(long x, long y, long z);

void dcMisc_DrawAxis(SDC_Render* render, SDC_Camera* camera);
//SDC_Mesh3D* dcMisc_generateGridMesh();


#endif /* _DC_MISC_H */
