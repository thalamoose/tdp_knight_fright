#include "kftypes.h"
#include "defines.h"
#include "playarea.h"
#include "game.h"
#include "globals.h"
#include "sprites.h"
#include "utilities.h"
#include "hud.h"
#include "npcs.h"
#include "copper.h"
#include "tilemap.h"

extern play_area_template asset_PlayArea_01[];
extern play_area_template asset_PlayArea_02[];
extern play_area_template asset_PlayArea_03[];
play_area_template *levelData[] =
{
    asset_PlayArea_01,
    asset_PlayArea_02,
    asset_PlayArea_03,
    NULL,
};

//---------------------------------------------------------
void InitializeGame(void)
{
    InitializeSprites();
    InitializeTilemap();
    InitializeHud();
    InitializeParticles();
    InitializeObjects();
    InitializeCopper();
    x_printf("Game is running\n");
}

//---------------------------------------------------------
void ResetGame(void)
{
    ResetSprites();
    ResetTilemap();
    ResetHud();
    ResetParticles();
    ResetObjects();
    InitializePlayArea(levelData[global.playAreaIndex]);
    InitializePlayer();
    InitializeNpcs();
    x_printf("Game is running\n");
}

void EndGame(void)
{
    global.playAreaIndex += 1;
    if (levelData[global.playAreaIndex] == NULL)
    {
        global.playAreaIndex = 0;
    }
}
