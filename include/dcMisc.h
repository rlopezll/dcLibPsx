
#ifndef _DC_MISC_H
#define _DC_MISC_H

struct SDC_Mesh3D;

SDC_Mesh3D* dcMisc_generateSphereMesh(long radius, unsigned latDivs, unsigned longDivs);
long dcMisc_noise(long x, long y, long z);
long dcMisc_fbm(long x, long y, long z);
//SDC_Mesh3D* dcMisc_generateGridMesh();


#endif /* _DC_MISC_H */
