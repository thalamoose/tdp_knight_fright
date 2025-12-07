#include "kftypes.h"
#include "defines.h"
#include "playarea.h"
#include "game_manager.h"
#include "globals.h"
#include "sprites.h"
#include "utilities.h"
#include "hud.h"
#include "copper.h"
#include "tilemap.h"
#include "hardware.h"
#include "audio.h"
#include "particles.h"
#include "objects.h"
#include "objects/coin.h"
#include "objects/player.h"
#include "enemies/enemy_controller.h"
#include "level_manager.h"

game_manager gameManager;

//---------------------------------------------------------
void InitializeGameManager(void)
{
    gameManager.ticksPerSecond = GetTicksPerSecond();
    InitializeSprites();
    InitializeTilemap();
    InitializeHud();
    InitializeParticles();
    InitializeObjects();
    InitializeCopper();
    InitializeLevelManager();
    x_printf("Game is running\n");
}

//---------------------------------------------------------
void ResetGameManager(void)
{
    ResetSprites();
    ResetTilemap();
    ResetHud();
    ResetParticles();
    ResetObjects();
    ResetLevelManager();
    NewLevel();
    InitializePlayer();
    x_printf("Game is running\n");
}

//---------------------------------------------------------
void UpdateGameManager(void)
{
    UpdateLevelManager();
    UpdateObjects();
    UpdateTilemap();
	gameManager.particlesActive = UpdateParticles(&particles[0]);
    UpdateAudio();
}

//---------------------------------------------------------
void RenderGameManager(void)
{
    RenderCopper(); // Must be done first, just after vsync
    DebugTiming(ULA_COLOUR_MAGENTA);
    RenderTilemap();
    RenderObjects();
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
