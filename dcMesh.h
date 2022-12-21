/* VERTEX MESHES */

#ifndef _DC_MESH_H
#define _DC_MESH_H

#include <libgte.h>

/* VERTEX FORMATS */

enum EDC_VertexFormat
{
    POS,
    POS_COL,
    POS_COL_NORM
    POS_TEX,
    POS_TEX_NORM,
    POS_COL_TEX,
    POS_COL_TEXT_NORM
};

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

//void dcMesh_RecordToOT(SDC_Mesh* mesh, );


#endif /* _DC_MESH_H */