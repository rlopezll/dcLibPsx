#ifndef _DC_RENDER_
#define _DC_RENDER_

#include <stdlib.h>
#include <libgte.h>
#include <libgpu.h>
#include <libgs.h>

typedef struct {

    int       width;
    int       height;

    u_long    *orderingTable[2];
    u_char    *primitives[2];
    u_char    *nextPrimitive;
    CVECTOR    bgColor;

    int        orderingTableCount;
    int        bytesPrimitives;
    
    DISPENV    displayEnvironment[2];
    DRAWENV    drawEnvironment[2];

    int        doubleBufferIndex;

    MATRIX     lightDirMatrix;
    MATRIX     lightColorMatrix;
    CVECTOR    ambientColor;

} SDC_Render;

typedef enum {
    RENDER_MODE_NTCS,
    RENDER_MODE_PAL
} EDC_Mode;

typedef enum {
    POLIGON_VERTEX,
    POLIGON_VERTEX_COLOR,
    POLIGON_VERTEX_NORMAL,  //Gouraud-shaded (TODO)
    POLIGON_VERTEX_COLOR_NORMAL,  //Gouraud-shaded (TODO)
    POLIGON_VERTEX_TEXTURED,
    POLIGON_VERTEX_TEXTURED_COLOR, //Gouraud-shaded
    POLIGON_VERTEX_TEXTURED_NORMAL //Gouraud-shaded (TODO)
} EDC_PolygonVertexType;


typedef struct {
    SVECTOR position;
} SDC_Vertex; // POLIGON_VERTEX

typedef struct {
    SVECTOR position;
    CVECTOR color;
} SDC_VertexColor; // POLIGON_VERTEX_COLOR

typedef struct {
    SVECTOR position;
    SVECTOR normal;
} SDC_VertexNormal; // POLIGON_VERTEX_NORMAL

typedef struct {
    SVECTOR position;
    SVECTOR normal;
    CVECTOR color;
} SDC_VertexColorNormal; // POLIGON_VERTEX_COLOR_NORMAL

typedef struct {
    SVECTOR position;
    short	u;
    short	v;
} SDC_VertexTextured; // POLIGON_VERTEX_TEXTURED

typedef struct {
    SVECTOR position;
    CVECTOR color;
    short	u;
    short	v;
} SDC_VertexColorTextured; // POLIGON_VERTEX_TEXTURED_COLOR

typedef struct {
    SVECTOR position;
    SVECTOR normal;
    short	u;
    short	v;
} SDC_VertexTexturedNormal; // POLIGON_VERTEX_TEXTURED_NORMAL

typedef struct {
    void*     vertexs;
    u_short*  indices;
    const TIM_IMAGE* tim;
    u_short   numIndices;
    u_short   numVertices;
    EDC_PolygonVertexType polygonVertexType;
    
} SDC_Mesh3D;

typedef struct
{
	TIM_IMAGE* tim;
    CVECTOR    constantColor;
    short int  backFaceCullMode; // 0: no culling, 1: cull backface, -1: cull front face
    u_short    bLighting : 1;
    u_short    bWireFrame : 1;
    u_short    bUseConstantColor : 1;

} SDC_DrawParams;

void dcRender_Init(SDC_Render* render, int width, int height, CVECTOR bgColor, int orderingTableLength/* = 4096*/, int bytesPrimitives/* = 8192*/, EDC_Mode mode );
void dcRender_SwapBuffers(SDC_Render* render);

void dcRender_LoadTexture(TIM_IMAGE* tim, u_long* texture);
void dcRender_DrawSpriteRect(SDC_Render* render, const TIM_IMAGE *tim, short x, short y, short w, short h, const DVECTOR *uv, const CVECTOR *color);
void dcRender_DrawMesh(SDC_Render* render,  SDC_Mesh3D* mesh, MATRIX* transform, SDC_DrawParams* drawParams );
void dcRender_DrawLine(SDC_Render* render, SVECTOR* v0, SVECTOR* v1, MATRIX* transform, CVECTOR* color, u_short segments);

/********** Light helper functions **********/

/**
 * Resets light matrices.
*/
void dcRender_ResetLights(SDC_Render* render);

/**
 * Sets parameters of light [lightIdx] and updates GTE light matrices accordingly. Note that only 3 lights are suported by
 * GTE hardware.
 * 
 * @param render global render state object
 * @param lightIdx index of the light to update, must be >= 0 and < 3
 * @param lightDirection direction vector of the light rays
 * @param lightColor light color, each component goes from 0 to 4096 (normal psx decimal format)
 * 
*/
void dcRender_SetLight(SDC_Render* render, u_short lightIdx, const SVECTOR* lightDirection, const SVECTOR* lightColor);

/**
 * Sets ambient color for lighting calculations. 
*/
void dcRender_SetAmbientColor(SDC_Render* render, CVECTOR* ambientColor);

#endif