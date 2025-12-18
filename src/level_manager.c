#include "kftypes.h"
#include "defines.h"
#include "hardware.h"
#include "memorymap.h"
#include "utilities.h"
#include "playarea.h"
#include "globals.h"
#include "level_manager.h"
#include "level_builder.h"
#include "game_manager.h"
#include "objects/object.h"
#include "objects/components.h"
#include "objects/object_manager.h"
//#include "objects/coin.h"
#include "objects/player.h"
#include "enemies/enemy_controller.h"
#include "hud.h"

level_manager levelManager;

enum
{
	LEVEL_STATE_IDLE,
	LEVEL_STATE_INIT,
	LEVEL_STATE_BUILD,
	LEVEL_STATE_FADE_IN,
	LEVEL_STATE_RUNNING,
	LEVEL_STATE_NEXT_LIFE,
	LEVEL_STATE_GAME_OVER,
	LEVEL_STATE_FADE_OUT,
	LEVEL_STATE_NEXT_LEVEL,
};

//---------------------------------------------------------
bool CheckLevelComplete(void)
{
	return false;
}

//---------------------------------------------------------
void NewLevel(void)
{
	levelManager.levelComplete = false;

	u16 levelNum = gameManager.level;
	if (levelNum>=12)
	{
		levelNum = 12+random8()%6;
	}
	nextreg(SWAP_BANK_0_SLOT, MISC_DATA_PAGE);
	nextreg(SWAP_BANK_1_SLOT, VIRTUAL_TILEMAP_PAGE);
	const level_template* levelTemplate = GetLevelTemplate(levelNum);
	levelManager.config = levelTemplate->config;
	ClearPlayArea();
	BuildPlayArea(levelTemplate->template);
	AddObstacles(levelTemplate->template);
	DrawPlayArea(levelTemplate->template);
	playArea.start = levelTemplate->template->start;
	playArea.position = playArea.start;
	levelManager.playerSpawnPos = levelTemplate->template->start;
	levelManager.enemyDropDelay = levelManager.config.spawnRate*gameManager.ticksPerSecond;
	levelManager.currentMaxEnemies = levelManager.config.maxNumEnemies;
}

//---------------------------------------------------------
void ClearEnemies(void)
{
	game_object* objList = objectManager.objects;
	for (u8 i=0;i<MAX_ENEMIES; i++, objList++)
	{
		game_object* enemy = objList;
		if (enemy->flags & FLAG_ACTIVE)
		{
			play_cell* pCell = GetPlayAreaCell(&enemy->playGrid);
			if (pCell->type==CELL_ENEMY)
				DestroyObject(enemy);
		}
	}
}

//---------------------------------------------------------
void BlowupEnemies(void)
{
	game_object* objList = objectManager.objects;
	for (u8 i=0;i<MAX_ENEMIES; i++, objList++)
	{
		game_object* enemy = objList;
		if (enemy->flags & FLAG_ACTIVE)
		{
			play_cell* pCell = GetPlayAreaCell(&enemy->playGrid);
			if (pCell->type==CELL_ENEMY)
				BlowupObject(enemy);
		}
	}
	}

//---------------------------------------------------------
void RemoveEnemy(s8 x, s8 y)
{
	(void)(x+y);
	#if 0
	play_cell* pCell = GetPlayAreaCell(x, y);
	if (!pCell)
		return;
	if (pCell->type==CELL_ENEMY)
	{
		enemy_controller* enemy = GetEnemyFromIndex(pCell->objIndex);
		if (enemy && enemy->flags.active)
		{
			if (enemy->Destroy) enemy->Destroy(enemy);
			enemy->flags.active = false;
		}
		pCell->type = CELL_TILE;
	}
		#endif
}

//---------------------------------------------------------
void PositionEnemy(const coord_s8* mapPosition, u8 enemyIndex)
{
	play_cell* pCell = GetPlayAreaCell(mapPosition);
	if (pCell)
	{
		pCell->type = CELL_ENEMY;
		pCell->objIndex = enemyIndex;
	}
}

//---------------------------------------------------------
bool TransitionComplete(void)
{
	return true;
}

//---------------------------------------------------------
void InitializeLevelManager(void)
{
	levelManager.state = LEVEL_STATE_INIT;
	ResetLevelManager();
}

//---------------------------------------------------------
void ResetLevelManager(void)
{
	ClearEnemies();
	levelManager.dropEnemies = false;
	levelManager.levelComplete = false;
	global.bonusTracker = 0;

	levelManager.randomDropCounter = levelManager.currentSpawnRate;
	levelManager.player = CreatePlayerObject();
}

//---------------------------------------------------------
void UpdateLevelManager(void)
{
	switch (levelManager.state)
	{
		case LEVEL_STATE_IDLE:
			break;
		case LEVEL_STATE_INIT:
			levelManager.state = LEVEL_STATE_BUILD;
			break;
		case LEVEL_STATE_BUILD:
			NewLevel();
			levelManager.state = LEVEL_STATE_FADE_IN;
			StartTransition(85, I_TO_F(-240), I_TO_F(-191), I_TO_F(51) / 18, I_TO_F(-3), I_TO_F(1) / 8);
			break;
		case LEVEL_STATE_FADE_IN:
			if (hud.transitionIsRunning)
				return;
			levelManager.state = LEVEL_STATE_RUNNING;
			break;
		case LEVEL_STATE_RUNNING:
			if (levelManager.tilesRemaining==0)
			{
				levelManager.state = LEVEL_STATE_GAME_OVER;
			}
			break;
		case LEVEL_STATE_NEXT_LIFE:
			gameManager.livesRemaining--;
			x_printf("Lives remaining:%d\n", gameManager.livesRemaining);
			if (gameManager.livesRemaining)
				levelManager.state = LEVEL_STATE_RUNNING;
			else
				levelManager.state = LEVEL_STATE_GAME_OVER;
			break;
		case LEVEL_STATE_GAME_OVER:
			BlowupEnemies();
			levelManager.state = LEVEL_STATE_FADE_OUT;
			StartTransition(50, 0, 0, I_TO_F(5), I_TO_F(-1), I_TO_F(1) / 4);
			break;
		case LEVEL_STATE_FADE_OUT:
			if (hud.transitionIsRunning)
				return;
			levelManager.state = LEVEL_STATE_NEXT_LEVEL;
			break;
		case LEVEL_STATE_NEXT_LEVEL:
			ClearEnemies();
			if (levelManager.playerIsDead)
			{
				levelManager.state = LEVEL_STATE_NEXT_LIFE;
				return;
			}
			gameManager.level++;
			levelManager.state = LEVEL_STATE_BUILD;
			break;
		default:
			levelManager.state = LEVEL_STATE_IDLE;
			break;
	}
}

//---------------------------------------------------------
bool CheckLevelCompleted(void)
{
	return false;
}

