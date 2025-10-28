#include "objects.h"
#include "kftypes.h"
#include "objects.h"
#include "defines.h"
#include "utilities.h"
#include "globals.h"

void InitializeNpcs(void)
{
        memset(global.objects,0,sizeof(global.objects));

        global.objects[0].position.x = 128;
        global.objects[0].position.y = 96;
        global.objects[0].baseIndex = 8;
        global.objects[0].totalFrames = 1;
        global.objects[0].flags.active = true;
        global.objects[0].animSpeed = 5;

        global.objects[1].position.x = 160;
        global.objects[1].position.y = 120;
        global.objects[1].baseIndex = 16;
        global.objects[1].totalFrames = 8;
        global.objects[1].flags.active = true;
        global.objects[1].animSpeed = 5;

        global.objects[2].position.x = 120;
        global.objects[2].position.y = 160;
        global.objects[1].baseIndex = 24;
        global.objects[2].totalFrames = 8;
        global.objects[2].flags.active = true;
        global.objects[2].flags.pingpong = true;
        global.objects[2].animSpeed = 10;
}

void UpdateNpcs(void)
{
    object* npc = global.objects;

    for( u8 i=0; i<MAX_OBJECTS; i++)
    {
        AnimateObject(npc);
    }
}
