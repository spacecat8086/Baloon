#include "sprite.h"

void UpdateSprite(SPRITE *sprite)
{
    sprite->vx += ACCEL_FORCE * (sprite->actions.left - sprite->actions.right);
    sprite->vx *= (1 - SLOWDOWN_RATIO);

    sprite->vy += ACCEL_FORCE * (sprite->actions.up - sprite->actions.down);
    sprite->vy *= (1 - SLOWDOWN_RATIO);

    sprite->x += sprite->vx;
    sprite->y += sprite->vy;
}

void LooseControl(SPRITE *sprite)
{
    // Disable sprite controls by setting them to 0
    memset((void *)&sprite->actions, 0, sizeof (ACTIONS));
}

// Apply momentum to the sprite vertically or horizontally
void PunchSprite(SPRITE *sprite, int axis, int dir)
{
    if (axis == PUNCH_HORIZONTAL)
    {
        sprite->vx -= PUNCH_FORCE * dir;
    }
    else
    {
        sprite->vy -= PUNCH_FORCE * dir;
    }
}