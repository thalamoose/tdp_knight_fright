#include "kftypes.h"
#include "defines.h"
#include "list.h"
#include "playarea.h"
#include "game.h"
#include "globals.h"
#include "sprites.h"
#include "utilities.h"
#include "hud.h"
#include "npcs.h"
#include "copper.h"
#include "tilemap.h"
#include "hardware.h"
#include "audio.h"
#include "sprites.h"
#include "npcs.h"
#include "pickups.h"

extern play_area_template asset_PlayArea_01[];
extern play_area_template asset_PlayArea_02[];
extern play_area_template asset_PlayArea_03[];
extern play_area_template asset_PlayArea_04[];
play_area_template *levelData[] =
{
    asset_PlayArea_01,
    asset_PlayArea_02,
    asset_PlayArea_03,
    asset_PlayArea_04,
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
    InitializePickups();
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
    ResetPickups();
    InitializePlayArea(levelData[global.playAreaIndex]);
    InitializePlayer();
    InitializeNpcs();
    x_printf("Game is running\n");
}

//---------------------------------------------------------
void UpdateGame(void)
{
    UpdateTilemap();
	UpdateNpcs();
	UpdatePlayer();
    UpdatePickups();
	UpdateSprites();
	global.particlesActive = UpdateParticles(&particles[0]);
    UpdateAudio();
}

//---------------------------------------------------------
void RenderGame(void)
{
    RenderCopper(); // Must be done first, just after vsync
    DebugTiming(ULA_COLOUR_MAGENTA);
    RenderTilemap();
    RenderPlayer();
    RenderPickups();
    RenderSprites();
    DebugTiming(ULA_COLOUR_BLUE);
    RenderHud();
    DebugTiming(ULA_COLOUR_GREEN);
    RenderParticles(&particles[0]);
    DebugTiming(ULA_COLOUR_WHITE);
}

void EndGame(void)
{
    global.playAreaIndex += 1;
    if (levelData[global.playAreaIndex]==NULL)
    {
        global.playAreaIndex = 0;
    }
}
