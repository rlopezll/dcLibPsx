/* VERTEX MESHES */

#ifndef _DC_MESH_H
#define _DC_MESH_H

#include <stddef.h>
#include <libgte.h>
#include "dcMemory.h"

/* VERTEX FORMATS */

typedef enum
{
    DCVFMT_POS,
    DCVFMT_POS_COL,
    DCVFMT_POS_COL_NORM,
    DCVFMT_POS_TEX,
    DCVFMT_POS_TEX_NORM,
    DCVFMT_POS_COL_TEX,
    DCVFMT_POS_COL_TEXT_NORM,

    DCVFMT_NUM_FORMATS
    
} EDC_VertexFormat;

typedef struct
{
    SVECTOR pos;

} SDC_PosVtx;

typedef struct
{
    SVECTOR pos;
    CVECTOR color;

} SDC_PosColVtx;

typedef struct
{
    SVECTOR pos;
    CVECTOR color;
    SVECTOR normal;

} SDC_PosColNormVtx;

typedef struct
{
    SVECTOR pos;
    DVECTOR uv;

} SDC_PosTexVtx;

typedef struct
{
    SVECTOR pos;
    DVECTOR uv;
    SVECTOR normal;

} SDC_PosTexNormVtx;

typedef struct
{
    SVECTOR pos;
    CVECTOR color;
    DVECTOR uv;

} SDC_PosColTexVtx;

typedef struct
{
    SVECTOR pos;
    CVECTOR color;
    DVECTOR uv;
    SVECTOR normal;

} SDC_PosColTexNormVtx;

/* MESH */

/* generic mesh type for any vertex format, then we'll have a function pointer table for each vtx type */
typedef struct
{
    EDC_VertexFormat vtxFormat;
    
    int     numVertices;
    int     numIndices;
    void*   vtxData;
    int*    indexData;

} SDC_Mesh;


//void dcMesh_Record(SDC_Mesh* mesh, SDC_DrawParams* drawParams, u_long* ot, long otSize, SDC_StackAllocator* renderMemory);


#endif /* _DC_MESH_H */