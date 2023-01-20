#include "dcFont.h"
#include <types.h>
#include <stdio.h>
#include <stdarg.h>

typedef struct {
    SDC_TIM_IMAGE tim;
    u_short firstChar;
    u_short charsPerLine;
    u_short charWidth;
    u_short charHeight;
    u_short lineWidth;

	RECT	crect;		/* CLUT rectangle on frame buffer */
	RECT	prect;		/* texture image rectangle on frame buffer */

} SFont;

SFont gFont;
u_short gFontOtz = 0;

void dcFont_Load(unsigned long *texture) {
    // gFont.firstChar = 32;
    // gFont.charsPerLine = 16;
    // gFont.charWidth = 8;
    // gFont.charHeight = 8;
    // gFont.lineWidth = gFont.charsPerLine*gFont.charWidth;
    // dcRender_LoadTexture(&gFont.tim, texture);
    dcFont_LoadEx(texture, 16, 8, 8);
}

void dcFont_LoadEx(unsigned long *texture, int charPerLine, int charWidth, int charHeight) {
    gFont.firstChar = 32;
    gFont.charsPerLine = charPerLine;
    gFont.charWidth = charWidth;
    gFont.charHeight = charHeight;
    gFont.lineWidth = gFont.charsPerLine*gFont.charWidth;
    dcRender_LoadTexture(&gFont.tim, texture);
}

void dcFont_UseSystemFont() {
    gFont.firstChar = 32;
    gFont.charsPerLine = 16;
    gFont.charWidth = 8;
    gFont.charHeight = 8;
    gFont.lineWidth = gFont.charsPerLine*gFont.charWidth;

    RECT crect = {960,384,0,0};
    gFont.crect = crect;
    RECT prect = {960,256,0,0};
    gFont.prect = prect;

    gFont.tim.mode = 0; //4bits
    gFont.tim.crect = gFont.crect;
    gFont.tim.caddr = NULL;
    gFont.tim.prect = gFont.prect;
    gFont.tim.paddr = NULL;
}

void dcFont_Print(SDC_Render* render, int x, int y, CVECTOR *color, const char *text) 
{
    dcFont_PrintZ(render, x, y, 0, color, text);
}

void dcFont_PrintZ(SDC_Render* render, int x, int y, int z, CVECTOR *color, const char *text)
{
    int idx = 0;
    DVECTOR uv;
    while(text[idx] != 0) {
        int idxFont = text[idx] - gFont.firstChar;
        uv.vx = (idxFont * gFont.charWidth) % (gFont.lineWidth);
        uv.vy = (idxFont/gFont.charsPerLine) * gFont.charHeight;
        dcRender_DrawSpriteRectZ(render, &gFont.tim, x, y, gFont.charWidth, gFont.charHeight, &uv, color, z);
        ++idx;
        x += gFont.charWidth;
    }
}

void dcFont_GetCharacterSize(DVECTOR* outSize)
{
    outSize->vx = gFont.charWidth;
    outSize->vy = gFont.charHeight;
}
