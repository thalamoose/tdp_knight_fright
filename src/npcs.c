#include "objects.h"
#include "kftypes.h"
#include "objects.h"
#include "defines.h"
#include "utilities.h"
#include "globals.h"

void InitializeNpcs(void)
{
        objects[0].position.x = 128;
        objects[0].position.y = 96;
        objects[0].baseIndex = 8;
        objects[0].totalFrames = 1;
        objects[0].flags.active = true;
        objects[0].animSpeed = 5;

        objects[1].position.x = 160;
        objects[1].position.y = 120;
        objects[1].baseIndex = 16;
        objects[1].totalFrames = 8;
        objects[1].flags.active = true;
        objects[1].animSpeed = 5;

        objects[2].position.x = 120;
        objects[2].position.y = 160;
        objects[1].baseIndex = 24;
        objects[2].totalFrames = 8;
        objects[2].flags.active = true;
        objects[2].flags.pingpong = true;
        objects[2].animSpeed = 10;
}

void UpdateNpcs(void)
{
    object* npc = objects;

    for( u8 i=0; i<MAX_OBJECTS; i++)
    {
        AnimateObject(npc);
    }
}
