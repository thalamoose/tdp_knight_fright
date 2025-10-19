#include "objects.h"
#include "kftypes.h"
#include "npcs.h"
#include "defines.h"
#include "utilities.h"

object npcs[MAX_OBJECTS];
void InitializeNpcs(void)
{
        memset(npcs,0,sizeof(npcs));

        npcs[0].position.x = 128;
        npcs[0].position.y = 96;
        npcs[0].totalFrames = 1;
        npcs[0].flags.active = true;
        npcs[0].animSpeed = 5;

        npcs[1].position.x = 160;
        npcs[1].position.y = 120;
        npcs[1].baseIndex = 1;
        npcs[1].totalFrames = 3;
        npcs[1].flags.active = true;
        npcs[1].animSpeed = 5;

        npcs[2].position.x = 120;
        npcs[2].position.y = 160;
        npcs[2].totalFrames = 8;
        npcs[2].flags.active = true;
        npcs[2].flags.pingpong = true;
        npcs[2].animSpeed = 10;
}

void UpdateNpcs(void)
{

}
