#ifndef _DC_RENDER_
#define _DC_RENDER_

#include <stdlib.h>
#include <libgte.h>
#include <libgpu.h>
#include <libgs.h>

//#define PAL_256

#ifdef PAL_256 /* Do we want 256 (PAL) high ? */
 #define SCREEN_X 0 /* Offset display by 0 in X */
 #define SCREEN_Y 24 /* Offset display by 24 in Y */
#else /* Otherwise assume 240 NSTC high */
 #define SCREEN_X 0 /* Offset X by 0. */
 #define SCREEN_Y 28 /* Offset Y by 28 pixels. */
#endif

/*
 *	Multi-purpose TIM image
 */
typedef struct {
	u_long  mode;		/* pixel mode */
	RECT	crect;		/* CLUT rectangle on frame buffer */
	u_long	*caddr;		/* CLUT address on main memory */
	RECT	prect;		/* texture image rectangle on frame buffer */
	u_long	*paddr;		/* texture image address on main memory */
} SDC_TIM_IMAGE;

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
    POLIGON_VERTEX_NORMAL,  //Gouraud-shaded
    POLIGON_VERTEX_COLOR_NORMAL,  //Gouraud-shaded
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
    const SDC_TIM_IMAGE* tim;
    u_short   numIndices;
    u_short   numVertices;
    EDC_PolygonVertexType polygonVertexType;
    
} SDC_Mesh3D;

typedef struct {
    SVECTOR rot; //we are use the rot.pad -> frame
    VECTOR  scale;
} SDC_AnimFrame; // Frame

typedef struct {
    SDC_AnimFrame* frames;
    u_short        numFrames;
} SDC_Animation; // Animation

typedef struct
{
	SDC_TIM_IMAGE* tim;
    CVECTOR    constantColor;
    short int  backFaceCullMode; // 0: no culling, 1: cull backface, -1: cull front face
    u_short    bLighting : 1;
    u_short    bWireFrame : 1;
    u_short    bUseConstantColor : 1;

} SDC_DrawParams;

void dcRender_Init(SDC_Render* render, int width, int height, CVECTOR bgColor, int orderingTableLength/* = 4096*/, int bytesPrimitives/* = 8192*/, EDC_Mode mode );
int  dcRender_SwapBuffers(SDC_Render* render);

void dcRender_ReportPrimitivesSize(SDC_Render* render);

void dcRender_LoadTexture(SDC_TIM_IMAGE* tim, u_long* texture);
void dcRender_DrawSpriteRect(SDC_Render* render, const SDC_TIM_IMAGE *tim, short x, short y, short w, short h, const DVECTOR *uv, const CVECTOR *color);
void dcRender_DrawSpriteRectZ(SDC_Render* render, const SDC_TIM_IMAGE *tim, short x, short y, short w, short h, const DVECTOR *uv, const CVECTOR *color, int otz);
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

/**
 * Return size in bytes of vertex type
*/
u_long dcRender_VertexSize( EDC_PolygonVertexType vertexType );


#endif