#ifndef _DC_FONT_H
#define _DC_FONT_H

#include "dcRender.h"

void dcFont_Load(unsigned long *texture);
void dcFont_LoadEx(unsigned long *texture, int charPerLine, int charWidth, int charHeight);
void dcFont_UseSystemFont();
void dcFont_Print(SDC_Render* render, int x, int y, CVECTOR *color, const char *text);
void dcFont_PrintZ(SDC_Render* render, int x, int y, int z, CVECTOR *color, const char *text);
void dcFont_GetCharacterSize(DVECTOR* outSize);

#endif