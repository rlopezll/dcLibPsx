#ifndef _DC_SPRITE_
#define _DC_SPRITE_

#include <stdlib.h>
#include <libgte.h>
#include <libgpu.h>
#include <libgs.h>
#include "dcRender.h"

typedef struct {
    short x, y, w, h;
} SDC_SpriteFrame;

typedef struct {
    SDC_SpriteFrame* frames;
    TIM_IMAGE* timImage;
    u_char speed;
    u_char nframes;
    char dummy0, dummy1;
} SDC_SpriteAnimation;

typedef struct {
    int currCounter;
    int currAnimFrame;
    SDC_SpriteAnimation* currAnimation;
} SDC_Sprite;

void dcSprite_LoadAnimationTex(SDC_SpriteAnimation* animation, u_long *image_data);
void dcSprite_SetAnimation(SDC_Sprite* sprite, SDC_SpriteAnimation* animation);
void dcSprite_Update(SDC_Sprite* sprite);
void dcSprite_Render(SDC_Render* render, SDC_Sprite* sprite, u_short x, u_short y, CVECTOR* Color);

#endif