#include "kftypes.h"
#include "defines.h"
#include "utilities.h"
#include "playarea.h"
#include "globals.h"
#include "level_manager.h"
#include "sprites.h"
#include "objects.h"
#include "game_manager.h"
#include "enemies/enemy_controller.h"
#include "objects/coin.h"
#include "objects/player.h"

level_manager levelManager;

extern const play_area_template asset_PlayArea_1[];
extern const play_area_template asset_PlayArea_1_2[];
extern const play_area_template asset_PlayArea_1_3[];
extern const play_area_template asset_PlayArea_2[];
extern const play_area_template asset_PlayArea_2_2[];
extern const play_area_template asset_PlayArea_2_4[];
extern const play_area_template asset_PlayArea_3[];
extern const play_area_template asset_PlayArea_4[];
extern const play_area_template asset_PlayArea_4_2[];
extern const play_area_template asset_PlayArea_5[];
extern const play_area_template asset_PlayArea_6[];
extern const play_area_template asset_PlayArea_7[];

const level_template levelData[] =
{
    /* 0  */ {asset_PlayArea_1,  {4,3,1,2,3,0,ENEMY_BEAR|ENEMY_BIG_HOPPER}},
    /* 1  */ {asset_PlayArea_1_2,{4,3,1,4,3,0,ENEMY_BEAR|ENEMY_BIG_HOPPER}},
    /* 2  */ {asset_PlayArea_1_3,{4,3,1,4,3,0,ENEMY_BEAR|ENEMY_BIG_HOPPER}},
    /* 3  */ {asset_PlayArea_2,  {4,3,1,4,3,0,ENEMY_BEAR|ENEMY_BIG_HOPPER}},
    /* 4  */ {asset_PlayArea_2_2,{4,2,1,3,2,0,ENEMY_BEAR|ENEMY_BIG_HOPPER}},
    /* 5  */ {asset_PlayArea_4_2,{4,3,1,4,3,2,ENEMY_BEAR|ENEMY_BIG_HOPPER}},
    /* 6  */ {asset_PlayArea_2_4,{6,4,3,4,3,2,ENEMY_BEAR|ENEMY_BIG_HOPPER}},
    /* 7  */ {asset_PlayArea_3,  {6,6,4,6,2,2,ENEMY_BEAR|ENEMY_BIG_HOPPER|ENEMY_FOLLOWER}},
    /* 8  */ {asset_PlayArea_4,  {6,6,4,6,2,2,ENEMY_BEAR|ENEMY_BIG_HOPPER|ENEMY_FOLLOWER}},
    /* 9  */ {asset_PlayArea_5,  {6,6,4,6,2,4,ENEMY_BEAR|ENEMY_BIG_HOPPER|ENEMY_FOLLOWER}},
    /* 10 */ {asset_PlayArea_6,  {8,6,4,6,2,4,ENEMY_BEAR|ENEMY_BIG_HOPPER|ENEMY_FOLLOWER}},
    /* 11 */ {asset_PlayArea_7,  {8,2,4,6,2,4,ENEMY_BEAR|ENEMY_BIG_HOPPER|ENEMY_FOLLOWER|ENEMY_COLOR_CHANGER}},
	//
	// Following are selected in a random fashion
	//
	/* 12 */ {asset_PlayArea_2_4,{12,8,6,8,1,4,ENEMY_BEAR|ENEMY_BIG_HOPPER|ENEMY_FOLLOWER|ENEMY_COLOR_CHANGER}},
	/* 13 */ {asset_PlayArea_3,  {12,8,6,8,1,4,ENEMY_BEAR|ENEMY_BIG_HOPPER|ENEMY_FOLLOWER|ENEMY_COLOR_CHANGER}},
	/* 14 */ {asset_PlayArea_4,  {12,8,6,8,1,4,ENEMY_BEAR|ENEMY_BIG_HOPPER|ENEMY_FOLLOWER|ENEMY_COLOR_CHANGER}},
	/* 15 */ {asset_PlayArea_5,  {12,8,6,8,1,4,ENEMY_BEAR|ENEMY_BIG_HOPPER|ENEMY_FOLLOWER|ENEMY_COLOR_CHANGER}},
	/* 16 */ {asset_PlayArea_6,  {12,8,6,8,1,4,ENEMY_BEAR|ENEMY_BIG_HOPPER|ENEMY_FOLLOWER|ENEMY_COLOR_CHANGER}},
	/* 17 */ {asset_PlayArea_7,  {12,8,6,8,1,4,ENEMY_BEAR|ENEMY_BIG_HOPPER|ENEMY_FOLLOWER|ENEMY_COLOR_CHANGER}},
};

enum
{
	LEVEL_STATE_IDLE,
	LEVEL_STATE_INIT,
	LEVEL_STATE_BUILD,
	LEVEL_STATE_FADE_IN,
	LEVEL_STATE_RUNNING,
	LEVEL_STATE_GAME_OVER,
	LEVEL_STATE_FADE_OUT,
	LEVEL_STATE_RESET
};

//---------------------------------------------------------
const level_template* GetLevelTemplate(u8 levelIndex)
{
	if (levelIndex>=sizeof(levelData)/sizeof(level_template))
		return NULL;
	return &levelData[levelIndex];
}

//---------------------------------------------------------
bool CheckLevelComplete(void)
{
	return false;
}

//---------------------------------------------------------
void NewLevel(void)
{
	levelManager.levelComplete = false;

	u16 levelNum = global.level;
	if (levelNum>=12)
	{
		levelNum = 12+random8()%6;
	}
	const level_template* levelTemplate = &levelData[levelNum];
	levelManager.config = levelTemplate->config;
	InitializePlayArea(levelTemplate->template);
	levelManager.playerSpawnPos = levelTemplate->template->start;
	levelManager.enemyDropDelay = levelManager.config.spawnRate*gameManager.ticksPerSecond;
	// Create an array of enabled enemy types. This allows us to randomly pick a type later.
	u8 enemyTypes = levelManager.config.enabledEnemies;
	for (u8 i=0; i<8 && enemyTypes; i++)
	{
		levelManager.enabledEnemies[levelManager.enabledEnemiesCount++] = i;
		enemyTypes >>= 1;
	}
}

//---------------------------------------------------------
void ClearEnemies(void)
{
#if 0
	for (u8 i=0;i<MAX_ENEMIES; i++)
	{
		enemy_controller* enemy = GetEnemyFromIndex(i);
		if (enemy->flags.active)
		{
			enemy->Destroy(enemy);
		}
	}
#endif
}

//---------------------------------------------------------
void BlowupEnemies(void)
{
#if 0
	for (u8 i=0;i<MAX_ENEMIES; i++)
	{
		enemy_controller* enemy = GetEnemyFromIndex(i);
		if (enemy->flags.active)
		{
			if (enemy->Blowup) enemy->Blowup(enemy);
		}
	}
#endif
	}

//---------------------------------------------------------
void CheckForCoin(s8 x, s8 y)
{
	play_cell* pCell = GetPlayAreaCell(x, y);
	if (pCell->type==CELL_COIN)
	{
		if (global.useSuperCoins)
		{
			pCell->objIndex = AddCoin(COIN_SUPER, x, y);
		}
		else
		{
			pCell->objIndex = AddCoin(COIN_NORMAL, x, y);
		}
		pCell->type = CELL_TILE;
	}
}

//---------------------------------------------------------
bool CheckForEnemy(s8 x, s8 y)
{
	play_cell* pCell = GetPlayAreaCell(x, y);
	if (!pCell)
		return false;
	return pCell->type==CELL_ENEMY;
}

//---------------------------------------------------------
bool CheckPathBlocked(s8 x, s8 y)
{
	play_cell* pCell = GetPlayAreaCell(x, y);
	if (!pCell)
		return false;
	return pCell->type==CELL_OBSTACLE || pCell->type==CELL_HOLE;

}

//---------------------------------------------------------
void RemoveEnemy(s8 x, s8 y)
{
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
void PositionEnemy(s8 x, s8 y, u8 enemyIndex)
{
	play_cell* pCell = GetPlayAreaCell(x, y);
	if (pCell)
	{
		pCell->type = CELL_ENEMY;
		pCell->objIndex = enemyIndex;
	}
}

//---------------------------------------------------------
void PlaceRandomEnemy(s8 x, s8 y, bool drop)
{
	s8 enemyType = levelManager.enabledEnemies[random8()%levelManager.enabledEnemiesCount];
	
	object* enemy = CreateEnemy(enemyType);
	if (!enemy)
		return;

	enemy->playGrid.x = x;
	enemy->playGrid.y = y;
	// Drop him in from the top
	if (drop)
	{

	}
}

//---------------------------------------------------------
void RandomEnemyDrop(u8 maxNumEntries, u8 spawnRate)
{
	if (gameManager.isPaused)
		return;
	if (levelManager.levelComplete || levelManager.isIntroLevel || !levelManager.dropEnemies)
		return;
	if (levelManager.enemyDropDelay)
	{
		levelManager.enemyDropDelay -= 1;
		if (levelManager.enemyDropDelay)
			return;
	}
	if (levelManager.enemiesActive>=maxNumEntries)
		return;

	s8 x = (random8()%playArea.activeSize.x)-playArea.activeSize.x/2;
	s8 y = (random8()%playArea.activeSize.y)-playArea.activeSize.y/2;

	if (CheckPathBlocked(x, y) || CheckForEnemy(x, y))
		return;
	if ((x==player->object.position.x) && (y==player->object.position.y))
		return;
	PlaceRandomEnemy(x, y, true);
	levelManager.enemyDropDelay = spawnRate*gameManager.ticksPerSecond;
}

//---------------------------------------------------------
void LevelStateMachine(void)
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
			levelManager.stateCounter = 10;
			levelManager.state = LEVEL_STATE_FADE_IN;
			break;
		case LEVEL_STATE_FADE_IN:
			if (levelManager.stateCounter--)
				break;
			levelManager.state = LEVEL_STATE_RUNNING;
			break;
		case LEVEL_STATE_RUNNING:
			if (levelManager.tilesRemaining==0)
			{
				levelManager.state = LEVEL_STATE_GAME_OVER;
			}
			break;
		case LEVEL_STATE_GAME_OVER:
			BlowupEnemies();
			levelManager.stateCounter = 10;
			levelManager.state = LEVEL_STATE_FADE_OUT;
			break;
		case LEVEL_STATE_FADE_OUT:
			if (levelManager.stateCounter--)
				break;
			levelManager.state = LEVEL_STATE_RESET;
			break;
		case LEVEL_STATE_RESET:
			ClearEnemies();
			levelManager.state = LEVEL_STATE_BUILD;
		default:
			levelManager.state = LEVEL_STATE_IDLE;
			break;
	}
}

//---------------------------------------------------------
void InitializeLevelManager(void)
{
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
	levelManager.state = LEVEL_STATE_INIT;
}

//---------------------------------------------------------
void UpdateLevelManager(void)
{
	LevelStateMachine();
}

//---------------------------------------------------------


//---------------------------------------------------------
bool CheckLevelCompleted(void)
{
	return false;
}

