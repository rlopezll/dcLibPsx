#include "dcCamera.h"
#include "dcMath.h"

void dcCamera_SetCameraPositionVector(SDC_Camera* camera, const VECTOR* pos) {
    camera->position = *pos;
}

void dcCamera_SetCameraPosition(SDC_Camera* camera, int x, int y, int z) {
    camera->position.vx = x;
    camera->position.vy = y;
    camera->position.vz = z;
}

void dcCamera_LookAt(SDC_Camera* camera, const VECTOR* at) {
    // Vector that defines the 'up' direction of the camera
    SVECTOR up = { 0, -ONE, 0 };
    
    // Divide out fractions of camera coordinates
    VECTOR eye;
    eye.vx = camera->position.vx;
    eye.vy = camera->position.vy;
    eye.vz = camera->position.vz;

    SVECTOR zaxis;
	SVECTOR xaxis;
	SVECTOR yaxis;

	VECTOR taxis = {at->vx - eye.vx, at->vy - eye.vy, at->vz - eye.vz};
	VectorNormalS(&taxis, &zaxis);
    taxis = dcMath_Cross(&up, &zaxis );
	VectorNormalS(&taxis, &xaxis);
	taxis = dcMath_Cross(&zaxis, &xaxis);
	VectorNormalS(&taxis, &yaxis);

	camera->viewMatrix.m[0][0] = xaxis.vx;	camera->viewMatrix.m[1][0] = yaxis.vx;	camera->viewMatrix.m[2][0] = zaxis.vx;
	camera->viewMatrix.m[0][1] = xaxis.vy;	camera->viewMatrix.m[1][1] = yaxis.vy;	camera->viewMatrix.m[2][1] = zaxis.vy;
	camera->viewMatrix.m[0][2] = xaxis.vz;	camera->viewMatrix.m[1][2] = yaxis.vz;	camera->viewMatrix.m[2][2] = zaxis.vz;

	VECTOR pos = {-eye.vx, -eye.vy, -eye.vz};
    VECTOR vec = {0,0,0};
    ScaleMatrixL(&camera->viewMatrix, &camera->viewScale);
	ApplyMatrixLV(&camera->viewMatrix, &pos, &vec);
	TransMatrix(&camera->viewMatrix, &vec);
}

void dcCamera_SetScreenResolution(SDC_Camera* camera, int width, int height) {
    const short realWidth = 320;
    const short realHeight = 240;
    const short realAspect = dcMath_Division(realWidth,realHeight); 
    const short currAspect = dcMath_Division(width,height); 
    const short delta = dcMath_Division(currAspect, realAspect);
    camera->viewScale.vx = ONE;
    camera->viewScale.vy = dcMath_Division(ONE,delta);
    camera->viewScale.vz = ONE;
}

void dcCamera_ApplyCameraTransform(SDC_Camera* camera, MATRIX* matrix, MATRIX* outMatrix) 
{
    CompMatrixLV(&camera->viewMatrix, matrix, outMatrix);
}

