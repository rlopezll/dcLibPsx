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

} SDC_Render;

typedef enum {
    RENDER_MODE_NTCS,
    RENDER_MODE_PAL
} EDC_Mode;

typedef enum {
    POLIGON_VERTEX,
    POLIGON_VERTEX_COLOR,
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
    void*     vertexs;
    u_short*  indices;
    const TIM_IMAGE* tim;
    u_short   numIndices;
    u_short   numVertices;
    EDC_PolygonVertexType polygonVertexType;
    
} SDC_Mesh3D;

void dcRender_Init(SDC_Render* render, int width, int height, CVECTOR bgColor, int orderingTableLength/* = 4096*/, int bytesPrimitives/* = 8192*/, EDC_Mode mode );
void dcRender_SwapBuffers(SDC_Render* render);

void dcRender_LoadTexture(TIM_IMAGE* tim, long* texture);
void dcRender_DrawSpriteRect(SDC_Render* render, const TIM_IMAGE *tim, const SVECTOR *pos, const RECT *rect, const DVECTOR *uv, const CVECTOR *color);
void dcRender_DrawMesh(SDC_Render* render,  SDC_Mesh3D* mesh, MATRIX* transform, const CVECTOR* color);

#endif