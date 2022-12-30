
#include <stddef.h>
#include <stdio.h>
#include "dcRender.h"
#include "dcMemory.h"
#include "dcMath.h"
#include "dcCamera.h"

SDC_Mesh3D* dcMisc_generateSphereMesh(long radius, unsigned latDivs, unsigned longDivs)
{
    longDivs = DC_MAX(longDivs, 3);
    latDivs = DC_MAX(latDivs, 2);
    unsigned numVertices = 2 + latDivs * longDivs;

    /*
        / | \  north pole: [longDivs] triangles
        |\|\|  strips: [longDivs] quads or 2x[longDivs] triangles
        \ | /  south pole: [longDivs] triangles
        
        2 poles of [longDivs] triangles each and
        [latDivs] - 1 triangle strips of 2x[longDivs] triangles each
    */

    unsigned numTriangles = 2 * longDivs + (latDivs - 1) * longDivs * 2;
    unsigned numIndices = numTriangles * 3;

    size_t nBytes = sizeof(SDC_Mesh3D) + sizeof(SDC_VertexColorNormal) * numVertices + sizeof(u_short) * numIndices;
    nBytes += 8; // for alignment

    // Allocate the required memory using a stack allocator as helper
    SDC_StackAllocator sa;
    dcStackAllocator_Init(&sa, nBytes);

    SDC_Mesh3D* mesh = (SDC_Mesh3D*)dcStackAllocator_Alloc(&sa, sizeof(SDC_Mesh3D), 1 );
    mesh->polygonVertexType = POLIGON_VERTEX_COLOR_NORMAL;
    mesh->vertexs = dcStackAllocator_Alloc(&sa, sizeof(SDC_VertexColorNormal) * numVertices, 4 );
    mesh->indices = dcStackAllocator_Alloc(&sa, sizeof(u_short) * numIndices, 2 );
    mesh->numIndices = numIndices;
    mesh->numVertices = numVertices;

    // generate the mesh data
    SDC_VertexColorNormal* verts = (SDC_VertexColorNormal*)mesh->vertexs;

    // north pole
    verts[0].position.vx = 0;
    verts[0].position.vy = -radius;
    verts[0].position.vz = 0;
    verts[0].color.r = 0x00;
    verts[0].color.g = 0xFF;
    verts[0].color.b = 0x00;

    // south pole
    verts[numVertices - 1].position.vx = 0;
    verts[numVertices - 1].position.vy = radius;
    verts[numVertices - 1].position.vz = 0;
    verts[numVertices - 1].color.r = 0x00;
    verts[numVertices - 1].color.g = 0x00;
    verts[numVertices - 1].color.b = 0xFF;

    long ltAngle = DC_HALFREV / (latDivs + 1);
    long cosLt = ccos(ltAngle);
    long sinLt = csin(ltAngle);
    long y = -DC_MUL(cosLt, radius);

    for(int i = 0; i < longDivs; ++i )
    {
        long lgAngle = i * DC_ONEREV / longDivs;
        long sinLg = csin(lgAngle);
        long cosLg = ccos(lgAngle);

        // north pole vert
        verts[i + 1].position.vx = DC_MUL(DC_MUL(sinLg, radius), sinLt);
        verts[i + 1].position.vy = y;
        verts[i + 1].position.vz = DC_MUL(DC_MUL(cosLg, radius), sinLt);

        verts[i + 1].color.r = (255 * lgAngle) / DC_ONEREV;
        verts[i + 1].color.g = 255 * ((cosLt + DC_ONE) >> 1) / DC_ONE;
        verts[i + 1].color.b = 255 * ((DC_ONE - cosLt) >> 1) / DC_ONE;

        // clockwise indices
        mesh->indices[i*3] = 0;
        mesh->indices[i*3 + 1] = i + 1;
        mesh->indices[i*3 + 2] = (i + 2) > longDivs ? 1 : i + 2;
    }

    for(int i = 1; i < latDivs; ++i)
    {
        ltAngle = (DC_HALFREV * (i + 1)) / (latDivs + 1);
        cosLt = ccos(ltAngle);
        sinLt = csin(ltAngle);
        y = -DC_MUL(cosLt, radius);

        for(int j = 0; j < longDivs; ++j )
        {
            long lgAngle = j * DC_ONEREV / longDivs;
            long sinLg = csin(lgAngle);
            long cosLg = ccos(lgAngle);

            // vtx
            unsigned short vtxIdx = 1 + i * longDivs + j;
            unsigned short nextVtxIdx = 1 + i * longDivs + (j + 1) % longDivs;
            verts[vtxIdx].position.vx = DC_MUL(DC_MUL(sinLg, radius), sinLt);
            verts[vtxIdx].position.vy = y;
            verts[vtxIdx].position.vz = DC_MUL(DC_MUL(cosLg, radius), sinLt);

            verts[vtxIdx].color.r = (255 * lgAngle) / DC_ONEREV;;
            verts[vtxIdx].color.g = 255 * ((cosLt + DC_ONE) >> 1) / DC_ONE;
            verts[vtxIdx].color.b = 255 * ((DC_ONE - cosLt) >> 1) / DC_ONE;

            unsigned short upperVtxIdx = 1 + (i - 1) * longDivs + j;
            unsigned short nextUpperVtxIdx = 1 + (i - 1) * longDivs + (j + 1) % longDivs;
            
            unsigned short indicesIndex = longDivs * 3 + (i - 1) * longDivs * 6 + j * 6;
            
            // quad triangle 1
            mesh->indices[indicesIndex] = vtxIdx;
            mesh->indices[indicesIndex + 1] = nextUpperVtxIdx;
            mesh->indices[indicesIndex + 2] = upperVtxIdx;

            // quad triangle 2
            mesh->indices[indicesIndex + 3] = vtxIdx;
            mesh->indices[indicesIndex + 4] = nextVtxIdx;
            mesh->indices[indicesIndex + 5] = nextUpperVtxIdx;
        }
    }

    // now finish the south cap
    unsigned short vtxIdx = numVertices - longDivs - 1;
    unsigned short indicesIdx = numIndices - 3 * longDivs;
    for(int i = 0; i < longDivs; ++i )
    {
        // clockwise indices
        mesh->indices[indicesIdx + i*3] = numVertices - 1; 
        mesh->indices[indicesIdx + i*3 + 1] = vtxIdx + (i + 1) % longDivs;
        mesh->indices[indicesIdx + i*3 + 2] = vtxIdx + i;
    }

    // compute normals
    for( int vi = 0; vi < numVertices; ++vi )
    {
        SDC_VertexColorNormal* vert = &verts[vi];
        VECTOR pos = {
            .vx = -vert->position.vx,
            .vy = -vert->position.vy,
            .vz = -vert->position.vz
        };
        VectorNormalS(&pos, &vert->normal);
        vert->normal.vx = -vert->normal.vx;
        vert->normal.vy = -vert->normal.vy;
        vert->normal.vz = -vert->normal.vz;
        printf("v: %d %d %d N: %d %d %d\n", pos.vx, pos.vy, pos.vz, vert->normal.vx, vert->normal.vy, vert->normal.vz );

    }

    return mesh;
};

long dcMisc_noise(long x, long y, long z)
{   
    u_long ux = x;
    u_long uy = y;
    u_long uz = z;

    ux ^= ux << 13;
	ux ^= ux >> 17;
	ux ^= ux << 5;
    
    uy ^= uy << 11;
	uy ^= uy >> 19;
	uy ^= uy << 7;

    uz ^= uz << 17;
	uz ^= uz >> 13;
	uz ^= uz << 9;
    
    ux ^= uy ^ uz;
    
    ux *= 0x846ca68bU;
    ux ^= ux >> 16;
    
    return ((long)ux) >> 20; // keep just decimal bits and sign
};

long dcMisc_fbm(long x, long y, long z)
{
    long r = dcMisc_noise(x, y, z) >> 1; // freq = 1, amplitude = 0.5
    r += dcMisc_noise( x << 1, y << 1, z << 1 ) >> 2; // freq = 2, amplitude = 0.25
    r += dcMisc_noise( x << 2, y << 2, z << 2 ) >> 3; // freq = 4, amplitude = 0.125
    r += dcMisc_noise( x << 3, y << 3, z << 3 ) >> 4; // freq = 8, amplitude = 0.0625
    
    return r;
};


void dcMisc_DrawAxis(SDC_Render* render, SDC_Camera* camera)
{
    const short halfSize = ONE;

    //SVECTOR zero = {0};

    CVECTOR cx = {255, 0, 0};
    SVECTOR x0 = {-halfSize, 0, 0};
    SVECTOR x1 = {halfSize, 0, 0};

    CVECTOR cy = {0, 255, 0};
    SVECTOR y0 = { 0, -halfSize, 0};
    SVECTOR y1 = { 0, halfSize, 0};

    CVECTOR cz = {0, 0, 255};
    SVECTOR z0 = { 0, 0, -halfSize};
    SVECTOR z1 = { 0, 0, halfSize};

    MATRIX MVP = {0};
    SVECTOR rot = {0};

    RotMatrix(&rot, &MVP);
    dcCamera_ApplyCameraTransform(camera, &MVP, &MVP);

    dcRender_DrawLine(render, &x0, &x1, &MVP, &cx, 32);
    dcRender_DrawLine(render, &y0, &y1, &MVP, &cy, 32);
    dcRender_DrawLine(render, &z0, &z1, &MVP, &cz, 32);
}

