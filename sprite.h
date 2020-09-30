#ifndef _SPRITE_H_
#define _SPRITE_H_

#include <string.h>

#define ACCEL_FORCE 0.25
#define SLOWDOWN_RATIO 0.03
#define COLLISION_SLOWDOWN 0.2
#define PUNCH_FORCE 2
#define PUNCH_HORIZONTAL 0x01
#define PUNCH_VERTICAL 0x02

#define IMAGE_WIDTH 50
#define IMAGE_HEIGHT 117

typedef struct _actions
{
    int up : 1;
    int down : 1;
    int left : 1;
    int right : 1;
} ACTIONS;

typedef struct _sprite
{
    float x;
    float y;
    float width;
    float height;
    float vx;
    float vy;
    ACTIONS actions;
} SPRITE;

void UpdateSprite(SPRITE *sprite);
void LooseControl(SPRITE *sprite);
void PunchSprite(SPRITE *sprite, int axis, int dir);

#endif