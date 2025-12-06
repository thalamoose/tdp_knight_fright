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
#include "hardware.h"
#include "audio.h"
#include "sprites.h"
#include "npcs.h"
#include "coins.h"
#include "enemies/enemy_controller.h"
#include "level_manager.h"

//---------------------------------------------------------
void InitializeGame(void)
{
    InitializeSprites();
    InitializeTilemap();
    InitializeHud();
    InitializeParticles();
    InitializeObjects();
    InitializeCoins();
    InitializeCopper();
    InitializeLevelManager();
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
    ResetCoins();
    ResetLevelManager();
    NewLevel();
    InitializePlayer();
    InitializeNpcs();
    x_printf("Game is running\n");
}

//---------------------------------------------------------
void UpdateGame(void)
{
    UpdateLevelManager();
    UpdateTilemap();
	UpdateNpcs();
	UpdatePlayer();
    UpdateCoins();
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
    RenderCoins();
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
    if (GetLevelTemplate(global.playAreaIndex)==NULL)
    {
        global.playAreaIndex = 0;
    }
}
