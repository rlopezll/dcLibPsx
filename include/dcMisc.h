
#ifndef _DC_MISC_H
#define _DC_MISC_H

struct SDC_Mesh3D;
struct SDC_Render;
struct SDC_Camera;

SDC_Mesh3D* dcMisc_generateSphereMesh(long radius, unsigned latDivs, unsigned longDivs);
long dcMisc_noise(long x, long y, long z);
long dcMisc_fbm(long x, long y, long z);

void dcMisc_DrawAxis(SDC_Render* render, SDC_Camera* camera);
//SDC_Mesh3D* dcMisc_generateGridMesh();


#endif /* _DC_MISC_H */
