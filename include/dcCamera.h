#ifndef _DC_CAMERA_
#define _DC_CAMERA_

#include <stdlib.h>
#include <libgte.h>

typedef struct {
    VECTOR  position;
    VECTOR  viewScale;
    MATRIX  viewMatrix;

} SDC_Camera;

void dcCamera_SetScreenResolution(SDC_Camera* camera, int width, int height);
void dcCamera_SetCameraPositionVector(SDC_Camera* camera, const VECTOR* pos);
void dcCamera_SetCameraPosition(SDC_Camera* camera, int x, int y, int z);
void dcCamera_LookAt(SDC_Camera* camera, const VECTOR* at);
void dcCamera_ApplyCameraTransform(SDC_Camera* camera, MATRIX* matrix, MATRIX* outMatrix);

#endif