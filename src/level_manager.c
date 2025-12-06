#include "kftypes.h"
#include "defines.h"
#include "utilities.h"
#include "enemies/enemy_controller.h"
#include "playarea.h"
#include "globals.h"
#include "level_manager.h"
#include "coins.h"

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
	*(u8*)&levelManager.enemyTypeArray = levelTemplate->config.enabledEnemies;
}

//---------------------------------------------------------
void ClearEnemies(void)
{
	for (u8 i=0;i<MAX_ENEMIES; i++)
	{
		enemy_controller* enemy = GetEnemyFromIndex(i);
		if (enemy->flags.active)
		{
			enemy->Destroy(enemy);
		}
	}
}

//---------------------------------------------------------
void BlowupEnemies(void)
{
	for (u8 i=0;i<MAX_ENEMIES; i++)
	{
		enemy_controller* enemy = GetEnemyFromIndex(i);
		if (enemy->flags.active)
		{
			enemy->Blowup(enemy);
		}
	}
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
	if (pCell)
	{
		return pCell->type==CELL_ENEMY;
	}
	return false;
}

//---------------------------------------------------------
void RemoveEnemy(s8 x, s8 y)
{
	play_cell* pCell = GetPlayAreaCell(x, y);
	if (pCell && pCell->type==CELL_ENEMY)
	{
		enemy_controller* enemy = GetEnemyFromIndex(pCell->objIndex);
		if (enemy && enemy->flags.active)
		{
			enemy->Destroy(enemy);
		}
		pCell->type = CELL_TILE;
	}
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

