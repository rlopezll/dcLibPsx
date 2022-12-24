
#include <stddef.h>
#include <stdio.h>
#include "dcRender.h"
#include "dcMemory.h"
#include "dcUtilsMath.h"

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

    size_t nBytes = sizeof(SDC_Mesh3D) + sizeof(SDC_VertexColor) * numVertices + sizeof(u_short) * numIndices;
    nBytes += 8; // for alignment

    // Allocate the required memory using a stack allocator as helper
    SDC_StackAllocator sa;
    dcStackAllocator_Init(&sa, nBytes);

    SDC_Mesh3D* mesh = (SDC_Mesh3D*)dcStackAllocator_Alloc(&sa, sizeof(SDC_Mesh3D), 1 );
    mesh->polygonVertexType = POLIGON_VERTEX_COLOR_GSHADED;
    mesh->vertexs = dcStackAllocator_Alloc(&sa, sizeof(SDC_VertexColor) * numVertices, 4 );
    mesh->indices = dcStackAllocator_Alloc(&sa, sizeof(u_short) * numIndices, 2 );
    mesh->numIndices = numIndices;

    // generate the mesh data
    SDC_VertexColor* verts = (SDC_VertexColor*)mesh->vertexs;

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

        printf("angle: %d sin: %d cos: %d )\n", lgAngle, sinLg, cosLg);
        // north pole vert
        verts[i + 1].position.vx = DC_MUL(DC_MUL(sinLg, radius), sinLt);
        verts[i + 1].position.vy = y;
        verts[i + 1].position.vz = DC_MUL(DC_MUL(cosLg, radius), sinLt);

        verts[i + 1].color.r = (255 * lgAngle) / DC_ONEREV;
        verts[i + 1].color.g = 255 * ((cosLt + DC_ONE) >> 1) / DC_ONE;
        verts[i + 1].color.b = 255 * ((DC_ONE - cosLt) >> 1) / DC_ONE;

        short vx = verts[i + 1].position.vx;
        short vy = verts[i + 1].position.vy;
        short vz = verts[i + 1].position.vz;
        
        //printf("(%d.%04d, %d.%04d, %d.%04d)\n", vx >> 12, (vx & 4095) * 10000 / 4096, vy >> 12, (vy & 4095) * 10000 / 4096, vz >> 12, (vz & 4095) * 10000 / 4096 );
        printf("vtx: ( %d %d %d )\n", vx, vy, vz);

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

            short vx = verts[vtxIdx].position.vx;
            short vy = verts[vtxIdx].position.vy;
            short vz = verts[vtxIdx].position.vz;
            printf("vtx: ( %d %d %d )\n", vx, vy, vz);

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


    return mesh;

};
