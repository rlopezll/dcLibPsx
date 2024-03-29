#include "dcRender.h"
#include "dcPerformance.h"
#include <malloc.h>
#include <libetc.h>
#include <stdio.h>
#include <assert.h>
#include <memory.h>

#define ENABLE_DBG_CHECKERS 0

int totalPrimitives = 0;
int totalTriangles = 0;

#define FIX_TEXCOORD_U(u, rectX) (u+(rectX%64)) 
#define FIX_TEXCOORD_V(v, rectY) (v+(rectY%256))

#define SET_UV3_FIX(poly, u0,v0,u1,v1,u2,v2,rectX,rectY) setUV3(poly, FIX_TEXCOORD_U(u0, rectX), FIX_TEXCOORD_V(v0, rectY), FIX_TEXCOORD_U(u1, rectX), FIX_TEXCOORD_V(v1, rectY), FIX_TEXCOORD_U(u2, rectX), FIX_TEXCOORD_V(v2, rectY));
#define SET_UV0_FIX(sprite, u0,v0,rectX,rectY) setUV0(sprite, FIX_TEXCOORD_U(u0, rectX), FIX_TEXCOORD_V(v0, rectY));

void _dcRender_IncPrimitive(SDC_Render* render, size_t offset)
{
    u_char* base_ptr = render->primitives[render->doubleBufferIndex];
    size_t nbytes = sizeof(u_char) * render->bytesPrimitives;
    size_t curr_offset = render->nextPrimitive - base_ptr; 
    if( curr_offset + offset >= nbytes) {
        printf("Error!! Limit Primitives bytes '%d/%d'\n", curr_offset + offset, nbytes);
    }
    render->nextPrimitive += offset;
    ++totalPrimitives;
}

void dcRender_ReportPrimitivesSize(SDC_Render* render) {
    // u_char* base_ptr = render->primitives[render->doubleBufferIndex];
    // size_t nbytes = sizeof(u_char) * render->bytesPrimitives;
    // size_t curr_offset = render->nextPrimitive - base_ptr; 
    // //printf("Primitives bytes '%d/%d Triangles '%d' totalPrimitives %d\n", curr_offset, nbytes, totalPrimitives, totalTriangles);
    // totalPrimitives = 0;
    // totalTriangles = 0;
}

void dcRender_Init(SDC_Render* render, int width, int height, CVECTOR bgColor, int orderingTableCount, int bytesPrimitives, EDC_Mode mode) {
	InitGeom();

    ResetGraph( 0 );
    SetGraphDebug(0);

    render->orderingTableCount = orderingTableCount;
    render->bytesPrimitives = bytesPrimitives;
    render->doubleBufferIndex = 0;
    render->width = width;
    render->height = height;
    render->bgColor = bgColor;

    render->orderingTable[0] = (u_long*)malloc3(sizeof(u_long) * orderingTableCount);
    render->orderingTable[1] = (u_long*)malloc3(sizeof(u_long) * orderingTableCount);
    
    ClearOTagR( render->orderingTable[0], orderingTableCount );
    ClearOTagR( render->orderingTable[1], orderingTableCount );
    
    render->primitives[0] = (u_char*)malloc3(sizeof(u_char) * bytesPrimitives);
    render->primitives[1] = (u_char*)malloc3(sizeof(u_char) * bytesPrimitives);
    
    render->nextPrimitive = render->primitives[0];

    SetDefDrawEnv( &render->drawEnvironment[0],    0, 0,      width, height );
    SetDefDrawEnv( &render->drawEnvironment[1],    0, height, width, height );

    SetDefDispEnv( &render->displayEnvironment[0], 0, height, width, height );
    SetDefDispEnv( &render->displayEnvironment[1], 0, 0,      width, height );

    /* Apply offsets to display to centre the display */
    // render->displayEnvironment[0].screen.x = render->displayEnvironment[1].screen.x = SCREEN_X;
    // render->displayEnvironment[0].screen.y = render->displayEnvironment[1].screen.y = SCREEN_Y;

    setRGB0( &render->drawEnvironment[0], bgColor.r, bgColor.g, bgColor.b );
    render->drawEnvironment[0].isbg = 1;
    render->drawEnvironment[0].dtd = 1;
    render->displayEnvironment[0].isinter = 0;

    setRGB0( &render->drawEnvironment[1], bgColor.r, bgColor.g, bgColor.b );
    render->drawEnvironment[1].isbg = 1;
    render->drawEnvironment[1].dtd = 1;
    render->displayEnvironment[1].isinter = 0;

    SetDispMask(1);    
	// Set GTE offset (recommended method  of centering)
    SetGeomOffset(width>>1, height>>1);
	// Set screen depth (basically FOV control, W/2 works best)
	SetGeomScreen(width >> 1);
    SetVideoMode(mode==RENDER_MODE_NTCS?MODE_NTSC:MODE_PAL);

    //Debug font (to remove)
    FntLoad(960, 256);
    SetDumpFnt(FntOpen(16, 16, 320, 64, 0, 512));
        
    PutDispEnv( &render->displayEnvironment[render->doubleBufferIndex] );
    PutDrawEnv( &render->drawEnvironment[render->doubleBufferIndex] );

    dcRender_ResetLights(render);
}

void dcRender_ResetLights(SDC_Render* render)
{
    memset(&render->lightColorMatrix, 0, sizeof(MATRIX));
    memset(&render->lightDirMatrix, 0, sizeof(MATRIX));

    SetLightMatrix(&render->lightDirMatrix);
    SetColorMatrix(&render->lightColorMatrix);
}

void dcRender_SetLight(SDC_Render* render, u_short lightIdx, const SVECTOR* lightDirection, const SVECTOR* lightColor)
{
    render->lightColorMatrix.m[0][lightIdx] = lightColor->vx;
    render->lightColorMatrix.m[1][lightIdx] = lightColor->vy;
    render->lightColorMatrix.m[2][lightIdx] = lightColor->vz;

    render->lightDirMatrix.m[lightIdx][0] = lightDirection->vx;
    render->lightDirMatrix.m[lightIdx][1] = lightDirection->vy;
    render->lightDirMatrix.m[lightIdx][2] = lightDirection->vz;

    SetLightMatrix(&render->lightDirMatrix);
    SetColorMatrix(&render->lightColorMatrix);
}

void dcRender_SetAmbientColor(SDC_Render* render, CVECTOR* ambientColor)
{
    render->ambientColor = *ambientColor;
    SetBackColor(render->ambientColor.r, render->ambientColor.g, render->ambientColor.b);
}

int dcRender_SwapBuffers(SDC_Render* render) {
    //dcPerformance_BeginCounter();
    DrawSync( 0 );
    //dcPerformance_EndCounterPrintf();

    int syncValue = VSync( 0 );
    SetDispMask( 1 );
    
    render->doubleBufferIndex = !render->doubleBufferIndex;
    
    ClearImage(&render->drawEnvironment[render->doubleBufferIndex].clip, render->bgColor.r, render->bgColor.g, render->bgColor.b);

    PutDrawEnv( &render->drawEnvironment[render->doubleBufferIndex] );
    PutDispEnv( &render->displayEnvironment[render->doubleBufferIndex] );
    
    DrawOTag( render->orderingTable[!render->doubleBufferIndex]+(render->orderingTableCount-1) );
    FntFlush(-1); // Font Debug
    
    ClearOTagR( render->orderingTable[render->doubleBufferIndex], render->orderingTableCount );
    render->nextPrimitive = render->primitives[render->doubleBufferIndex];    

    return syncValue; 
}

void dcRender_LoadTexture(SDC_TIM_IMAGE* sdcTim, u_long* texture) {
    TIM_IMAGE tim;
    OpenTIM(texture);                            // Open the tim binary data, feed it the address of the data in memory
    ReadTIM(&tim);                                // This read the header of the TIM data and sets the corresponding members of the SDC_TIM_IMAGE structure

    LoadImage( tim.prect, tim.paddr );        // Transfer the data from memory to VRAM at position prect.x, prect.y
    if( tim.mode & 0x8 ) {                     // check 4th bit       // If 4th bit == 1, TIM has a CLUT
        LoadImage( tim.crect, tim.caddr );    // Load it to VRAM at position crect.x, crect.y
    }
    sdcTim->mode = tim.mode;
    sdcTim->caddr = tim.caddr;
    sdcTim->paddr = tim.paddr;
    sdcTim->prect = *tim.prect;
    sdcTim->crect = *tim.crect;
    DrawSync(0);                                // Wait for drawing to end
}

void dcRender_DrawSpriteRect(SDC_Render* render, const SDC_TIM_IMAGE *tim, short x, short y, short w, short h, const DVECTOR *uv, const CVECTOR *color) 
{
    dcRender_DrawSpriteRectZ(render, tim, x, y, w, h, uv, color, 0);
}

void dcRender_DrawSpriteRectZ(SDC_Render* render, const SDC_TIM_IMAGE *tim, short x, short y, short w, short h, const DVECTOR *uv, const CVECTOR *color, int otz)
{
    SPRT *sprt = (SPRT*)render->nextPrimitive;
    u_long* ot = render->orderingTable[render->doubleBufferIndex];

    setSprt(sprt);
    setXY0(sprt, x, y);
    setWH(sprt, w, h);
    setRGB0(sprt, color->r, color->g, color->b);
    SET_UV0_FIX(sprt, uv->vx, uv->vy, tim->prect.x, tim->prect.y);
    setClut(sprt, tim->crect.x, tim->crect.y);

    addPrim(ot[otz], sprt);

    _dcRender_IncPrimitive(render, sizeof(SPRT));

    DR_TPAGE *tpri = (DR_TPAGE*)render->nextPrimitive;
    u_short tpage = getTPage(tim->mode, 0, tim->prect.x, tim->prect.y);
    setDrawTPage(tpri, 0, 0, tpage);
    addPrim(ot[otz], tpri);
    _dcRender_IncPrimitive(render, sizeof(DR_TPAGE));
}

void dcRender_DrawMesh(SDC_Render* render,  SDC_Mesh3D* mesh, MATRIX* transform, SDC_DrawParams* drawParams) 
{
    assert(render && mesh && transform);
    u_long *orderingTable = render->orderingTable[render->doubleBufferIndex];
    int orderingTableCount = render->orderingTableCount;
    long p, otz, flg;
    int nclip;

    SetRotMatrix(transform);
    SetTransMatrix(transform);

    const u_short bLighting = drawParams && drawParams->bLighting;
    CVECTOR* color = drawParams && drawParams->bUseConstantColor ? &drawParams->constantColor : NULL;
    
    CVECTOR c0, c1, c2;  
    CVECTOR curr_color = {255, 255, 255};
    if(color) 
        curr_color = *color;

    u_short tpage = (drawParams && drawParams->tim) ? getTPage(drawParams->tim->mode, 0, drawParams->tim->prect.x, drawParams->tim->prect.y) : 0; /*texture page*/
    u_short clut = (drawParams && drawParams->tim) ? getClut(drawParams->tim->crect.x, drawParams->tim->crect.y) : 0; /*texture CLUT*/

    for (int i = 0; i < mesh->numIndices; i += 3) {               
        u_short index0 = mesh->indices[i];
        u_short index1 = mesh->indices[i+1];
        u_short index2 = mesh->indices[i+2];
#if ENABLE_DBG_CHECKERS       
        assert(index0 < mesh->numVertices);
        assert(index1 < mesh->numVertices);
        assert(index2 < mesh->numVertices);
        totalTriangles += 3;  
#endif
        void *poly = render->nextPrimitive;

        switch(mesh->polygonVertexType)
        {
            case POLIGON_VERTEX:
            {
                POLY_F3* polyF3 = (POLY_F3*)poly;
                setPolyF3(polyF3);
                SDC_Vertex *vertexs = (SDC_Vertex *)mesh->vertexs;
                nclip = RotAverageNclip3(&vertexs[index0].position, &vertexs[index1].position, &vertexs[index2].position,
                    (long *)&polyF3->x0, (long *)&polyF3->x1, (long *)&polyF3->x2, &p, &otz, &flg);
                setRGB0(polyF3, curr_color.r, curr_color.g, curr_color.b);

                if (nclip <= 0) continue;
                if ((otz <= 0) || (otz >= orderingTableCount)) continue;

				addPrim(orderingTable[otz], polyF3);
                _dcRender_IncPrimitive(render, sizeof(POLY_F3));

            }
            break;
            case POLIGON_VERTEX_COLOR:
            {
                POLY_G3* polyG3 = (POLY_G3*)poly;
                SDC_VertexColor *vertexs = (SDC_VertexColor *)mesh->vertexs;

                setPolyG3(polyG3);
                if(color) {
                    setRGB0(polyG3, color->r, color->g, color->b);
                    setRGB1(polyG3, color->r, color->g, color->b);
                    setRGB2(polyG3, color->r, color->g, color->b);
                }
                else {
                    setRGB0(polyG3, vertexs[index0].color.r, vertexs[index0].color.g, vertexs[index0].color.b);
                    setRGB1(polyG3, vertexs[index1].color.r, vertexs[index1].color.g, vertexs[index1].color.b);
                    setRGB2(polyG3, vertexs[index2].color.r, vertexs[index2].color.g, vertexs[index2].color.b);
                }

                nclip = RotAverageNclip3(&vertexs[index0].position, &vertexs[index1].position, &vertexs[index2].position,
                                        (long *)&polyG3->x0, (long *)&polyG3->x1, (long *)&polyG3->x2, &p, &otz, &flg);
                if (nclip <= 0) continue;
                if ((otz <= 0) || (otz >= orderingTableCount)) continue;

				addPrim(&orderingTable[otz], polyG3);
                _dcRender_IncPrimitive(render, sizeof(POLY_G3));
            }
            break;
            case POLIGON_VERTEX_COLOR_NORMAL:
            {
                POLY_G3* polyG3 = (POLY_G3*)poly;
                SDC_VertexColorNormal *vertexs = (SDC_VertexColorNormal *)mesh->vertexs;

                setPolyG3(polyG3);

                if(bLighting)
                {
                    nclip = RotAverageNclipColorCol3
                    (
                        &vertexs[index0].position, &vertexs[index1].position, &vertexs[index2].position, // positions
                        &vertexs[index0].normal, &vertexs[index1].normal, &vertexs[index2].normal, // normals
                        color ? color : &vertexs[index0].color, // input color
                        (long *)&polyG3->x0, (long *)&polyG3->x1, (long *)&polyG3->x2, // out 2d positions
                        &c0, &c1, &c2, // out colors
                        &otz, &flg
                    );
                    
                    /**
                     * TODO: we could pass white as input color to the above function and then multiply the output colors 
                     * by each vertex color to get different colors per vertex.
                    */

                    setRGB0(polyG3, c0.r, c0.g, c0.b);
                    setRGB1(polyG3, c1.r, c1.g, c1.b);
                    setRGB2(polyG3, c2.r, c2.g, c2.b);
                }
                else
                {
                    nclip = RotAverageNclip3(&vertexs[index0].position, &vertexs[index1].position, &vertexs[index2].position,
                                            (long *)&polyG3->x0, (long *)&polyG3->x1, (long *)&polyG3->x2, &p, &otz, &flg);

                    if(color) {
                        setRGB0(polyG3, color->r, color->g, color->b);
                        setRGB1(polyG3, color->r, color->g, color->b);
                        setRGB2(polyG3, color->r, color->g, color->b);
                    }
                    else {
                        setRGB0(polyG3, vertexs[index0].color.r, vertexs[index0].color.g, vertexs[index0].color.b);
                        setRGB1(polyG3, vertexs[index1].color.r, vertexs[index1].color.g, vertexs[index1].color.b);
                        setRGB2(polyG3, vertexs[index2].color.r, vertexs[index2].color.g, vertexs[index2].color.b);
                    }
                }

                if (nclip <= 0) continue;
                if ((otz <= 0) || (otz >= orderingTableCount)) continue;

				addPrim(&orderingTable[otz], polyG3);
                _dcRender_IncPrimitive(render, sizeof(POLY_G3));

            } break;
            case POLIGON_VERTEX_NORMAL:
            {
                POLY_G3* polyG3 = (POLY_G3*)poly;
                SDC_VertexNormal *vertexs = (SDC_VertexNormal *)mesh->vertexs;

                setPolyG3(polyG3);

                if(bLighting)
                {
                    nclip = RotAverageNclipColorCol3
                    (
                        &vertexs[index0].position, &vertexs[index1].position, &vertexs[index2].position, // positions
                        &vertexs[index0].normal, &vertexs[index1].normal, &vertexs[index2].normal, // normals
                        &curr_color, // input color
                        (long *)&polyG3->x0, (long *)&polyG3->x1, (long *)&polyG3->x2, // out 2d positions
                        &c0, &c1, &c2, // out colors
                        &otz, &flg
                    );
                    
                    /**
                     * TODO: we could pass white as input color to the above function and then multiply the output colors 
                     * by each vertex color to get different colors per vertex.
                    */

                    setRGB0(polyG3, c0.r, c0.g, c0.b);
                    setRGB1(polyG3, c1.r, c1.g, c1.b);
                    setRGB2(polyG3, c2.r, c2.g, c2.b);
                }
                else
                {
                    nclip = RotAverageNclip3(&vertexs[index0].position, &vertexs[index1].position, &vertexs[index2].position,
                                            (long *)&polyG3->x0, (long *)&polyG3->x1, (long *)&polyG3->x2, &p, &otz, &flg);

                    setRGB0(polyG3, curr_color.r, curr_color.g, curr_color.b);
                    setRGB1(polyG3, curr_color.r, curr_color.g, curr_color.b);
                    setRGB2(polyG3, curr_color.r, curr_color.g, curr_color.b);
                }

                if (nclip <= 0) continue;
                if ((otz <= 0) || (otz >= orderingTableCount)) continue;

				addPrim(&orderingTable[otz], polyG3);
                _dcRender_IncPrimitive(render, sizeof(POLY_G3));

            } break;
            case POLIGON_VERTEX_TEXTURED:
            {
                POLY_FT3* polyFT3 = (POLY_FT3*)poly;
                SDC_VertexTextured *vertexs = (SDC_VertexTextured *)mesh->vertexs;

                setPolyFT3(polyFT3);
                setRGB0(polyFT3, curr_color.r, curr_color.g, curr_color.b);
                if(drawParams && drawParams->tim) {
                    SET_UV3_FIX(polyFT3, vertexs[index0].u, vertexs[index0].v, vertexs[index1].u, vertexs[index1].v, vertexs[index2].u, vertexs[index2].v, drawParams->tim->prect.x, drawParams->tim->prect.y);
                    polyFT3->tpage = tpage; /*texture page*/
                    polyFT3->clut = clut;   /*texture CLUT*/
                } else{
                    setUV3(polyFT3, vertexs[index0].u , vertexs[index0].v, vertexs[index1].u , vertexs[index1].v, vertexs[index2].u , vertexs[index2].v);
                }

                nclip = RotAverageNclip3(&vertexs[index0].position, &vertexs[index1].position, &vertexs[index2].position,
                                        (long *)&polyFT3->x0, (long *)&polyFT3->x1, (long *)&polyFT3->x2, &p, &otz, &flg);
                if (nclip <= 0) continue;
                if ((otz <= 0) || (otz >= orderingTableCount)) continue;

				addPrim(&orderingTable[otz], polyFT3);

                _dcRender_IncPrimitive(render, sizeof(POLY_FT3));
            }
            break;
            case POLIGON_VERTEX_TEXTURED_COLOR:
            {
                POLY_GT3* polyGT3 = (POLY_GT3*)poly;
                SDC_VertexColorTextured *vertexs = (SDC_VertexColorTextured *)mesh->vertexs;
                setPolyGT3(polyGT3);

                setRGB0(polyGT3, curr_color.r, curr_color.g, curr_color.b);
                setRGB1(polyGT3, curr_color.r, curr_color.g, curr_color.b);
                setRGB2(polyGT3, curr_color.r, curr_color.g, curr_color.b);

                if(drawParams && drawParams->tim) {
                    SET_UV3_FIX(polyGT3, vertexs[index0].u, vertexs[index0].v, vertexs[index1].u, vertexs[index1].v, vertexs[index2].u, vertexs[index2].v, drawParams->tim->prect.x, drawParams->tim->prect.y);
                    polyGT3->tpage = tpage; /*texture page*/
                    polyGT3->clut = clut; /*texture CLUT*/
                }
                else {
                    setUV3(polyGT3, vertexs[index0].u , vertexs[index0].v, vertexs[index1].u , vertexs[index1].v, vertexs[index2].u , vertexs[index2].v);
                }

                nclip = RotAverageNclip3(&vertexs[index0].position, &vertexs[index1].position, &vertexs[index2].position,
                                        (long *)&polyGT3->x0, (long *)&polyGT3->x1, (long *)&polyGT3->x2, &p, &otz, &flg);
                if (nclip <= 0) continue;
                if ((otz <= 0) || (otz >= orderingTableCount)) continue;

				addPrim(&orderingTable[otz], polyGT3);
                _dcRender_IncPrimitive(render, sizeof(POLY_GT3));
            }
            break;
            case POLIGON_VERTEX_TEXTURED_NORMAL:
            {
                POLY_GT3* polyGT3 = (POLY_GT3*)poly;
                SDC_VertexTexturedNormal *vertexs = (SDC_VertexTexturedNormal *)mesh->vertexs;
                setPolyGT3(polyGT3);

                nclip = RotAverageNclip3(&vertexs[index0].position, &vertexs[index1].position, &vertexs[index2].position,
                                        (long *)&polyGT3->x0, (long *)&polyGT3->x1, (long *)&polyGT3->x2, &p, &otz, &flg);
                if (nclip <= 0) continue;
                if ((otz <= 0) || (otz >= orderingTableCount)) continue;

                if(bLighting)
                {
                    NormalColorCol3(
                        &vertexs[index0].normal, &vertexs[index1].normal, &vertexs[index2].normal, // input normal
                        &curr_color, // input base color
                        &c0, &c1, &c2 // output colors
                    );
                    
                    setRGB0(polyGT3, c0.r, c0.g, c0.b);
                    setRGB1(polyGT3, c1.r, c1.g, c1.b);
                    setRGB2(polyGT3, c2.r, c2.g, c2.b);
                }
                else
                {
                    setRGB0(polyGT3, curr_color.r, curr_color.g, curr_color.b);
                    setRGB1(polyGT3, curr_color.r, curr_color.g, curr_color.b);
                    setRGB2(polyGT3, curr_color.r, curr_color.g, curr_color.b);
                }

                if(drawParams && drawParams->tim) {
                    SET_UV3_FIX(polyGT3, vertexs[index0].u, vertexs[index0].v, vertexs[index1].u, vertexs[index1].v, vertexs[index2].u, vertexs[index2].v, drawParams->tim->prect.x, drawParams->tim->prect.y);
                    polyGT3->tpage = tpage; /*texture page*/
                    polyGT3->clut = clut;   /*texture CLUT*/
                }
                else {
                    setUV3(polyGT3, vertexs[index0].u , vertexs[index0].v, vertexs[index1].u , vertexs[index1].v, vertexs[index2].u , vertexs[index2].v);
                }

				addPrim(&orderingTable[otz], polyGT3);
                _dcRender_IncPrimitive(render, sizeof(POLY_GT3));

            }
            break;
        }
    }
}

void dcRender_DrawLine(SDC_Render* render, SVECTOR* v0, SVECTOR* v1, MATRIX* transform, CVECTOR* color, u_short segments )
{
    
    assert(render && v0 && v1 && transform && segments > 0);
    u_long *orderingTable = render->orderingTable[render->doubleBufferIndex];
    int orderingTableCount = render->orderingTableCount;
    long p, flg;

    SetRotMatrix(transform);
    SetTransMatrix(transform);
    
    CVECTOR curr_color = {255, 255, 255};

    if(color) 
    {
        curr_color = *color;
    }

    short xDelta = (v1->vx - v0->vx) / segments;
    short yDelta = (v1->vy - v0->vy) / segments;
    short zDelta = (v1->vz - v0->vz) / segments;

    SVECTOR p0 = *v0;

    for(u_short i = 0; i < segments; ++ i)
    {
        SVECTOR p1 = {p0.vx + xDelta, p0.vy + yDelta, p0.vz + zDelta};

        void *poly = render->nextPrimitive;
        LINE_F2* lineF2 = (LINE_F2*)poly;
        SetLineF2(lineF2);
        long otz = RotTransPers(&p0, (long*)&lineF2->x0, &p, &flg);
        long otz1 = RotTransPers(&p1, (long*)&lineF2->x1, &p, &flg);

        otz = (otz + otz1) >> 1;

        p0 = p1;
        if ((otz < 0) || (otz >= orderingTableCount)) continue;

        setRGB0(lineF2, curr_color.r, curr_color.g, curr_color.b);
        addPrim(orderingTable[otz], lineF2);
        _dcRender_IncPrimitive(render, sizeof(LINE_F2));
    }
}

u_long dcRender_VertexSize( EDC_PolygonVertexType vertexType )
{
    switch(vertexType)
    {
        case POLIGON_VERTEX: return sizeof(SDC_Vertex);
        case POLIGON_VERTEX_COLOR: return sizeof(SDC_VertexColor);
        case POLIGON_VERTEX_NORMAL: return sizeof(SDC_VertexNormal);
        case POLIGON_VERTEX_COLOR_NORMAL: return sizeof(SDC_VertexColorNormal);
        case POLIGON_VERTEX_TEXTURED: return sizeof(SDC_VertexTextured);
        case POLIGON_VERTEX_TEXTURED_COLOR: return sizeof(SDC_VertexColorTextured);
        case POLIGON_VERTEX_TEXTURED_NORMAL: return sizeof(SDC_VertexTexturedNormal);
    };
    
    assert(0);
    return 0;
}

//#pragma GCC pop_options
