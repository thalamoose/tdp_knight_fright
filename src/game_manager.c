#include "kftypes.h"
#include "defines.h"
#include "playarea.h"
#include "game_manager.h"
#include "globals.h"
#include "utilities.h"
#include "hud.h"
#include "copper.h"
#include "tilemap.h"
#include "hardware.h"
#include "audio.h"
#include "sprite.h"
#include "particles.h"
#include "objects/components.h"
#include "objects/object_manager.h"
#include "objects/coin.h"
#include "objects/player.h"
#include "enemies/enemy_controller.h"
#include "level_manager.h"

game_manager gameManager;

//---------------------------------------------------------
void InitializeGameManager(void)
{
    gameManager.ticksPerSecond = GetTicksPerSecond();
    InitializeTilemap();
    InitializeHud();
    InitializeParticles();
    InitializeObjects();
    InitializeCopper();
    InitializeLevelManager();
    gameManager.livesRemaining = 4;
    x_printf("Game is running\n");
}

//---------------------------------------------------------
void ResetGameManager(void)
{
    ResetTilemap();
    ResetSprites();
    ResetHud();
    ResetParticles();
    ResetObjects();
    ResetLevelManager();
    NewLevel();
    InitializePlayer();
    gameManager.isRunning = true;
    gameManager.isPaused = false;
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