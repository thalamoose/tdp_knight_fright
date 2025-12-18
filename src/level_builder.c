#include "kftypes.h"
#include "sprite.h"
#include "defines.h"
#include "hardware.h"
#include "memorymap.h"
#include "utilities.h"
#include "globals.h"
#include "game_manager.h"
#include "objects/object.h"
#include "objects/object_manager.h"
#include "objects/coin.h"
#include "objects/player.h"
#include "objects/obstacle.h"
#include "objects/character_controller.h"
#include "playarea.h"
#include "level_manager.h"
#include "hud.h"
#include "level_builder.h"

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
	// 		  Tiledata		numCoins,numHoles,numObstacles,enemies,spawnrate,spikes,enabledEnemies
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

//---------------------------------------------------------
const level_template* GetLevelTemplate(u8 levelIndex)
{
	if (levelIndex>=sizeof(levelData)/sizeof(level_template))
		return NULL;
	return &levelData[levelIndex];
}

//---------------------------------------------------------
u8 AddCoin(const coord_s8* mapPosition)
{
	game_object* pCoin = CreateCoinObject(mapPosition);
	return pCoin->object.index;
	//x_printf( "px:%d,py:%d,x:%d,y:%d\n", x, y, F_TO_I(pCoin->position.x), F_TO_I(pCoin->position.y));
}

//---------------------------------------------------------
void PlaceHole(play_cell* pCell)
{
	if (pCell->type==CELL_TILE)
	{
		pCell->type=CELL_HOLE;
		levelManager.config.numHoles--;
		levelManager.tilesRemaining--;
	}
}

//---------------------------------------------------------
void PlaceSpike(const coord_s8* mapPosition)
{
	x_printf( "SPIKE:%d,%d\n", (s16)mapPosition->x, (s16)mapPosition->y);
	UNUSED(mapPosition);
	levelManager.config.numHoles--;
	levelManager.tilesRemaining--;
}

//---------------------------------------------------------
void PlaceObstacle(const coord_s8* mapPosition)
{
	u8 shapeIndex = random8()%3;

	game_object* pObstacle = CreateObstacleObject(mapPosition, shapeIndex);
	x_printf( "OBSTACLE:%d,%d,t:%c 0x%x\n", (s16)mapPosition->x, (s16)mapPosition->y, shapeIndex, pObstacle);
	levelManager.config.numHoles--;
	levelManager.tilesRemaining--;
}

//---------------------------------------------------------
void PlaceRandomEnemy(const coord_s8* mapPosition, bool drop)
{
	s8 enemyType = levelManager.enabledEnemies[random8()%levelManager.enabledEnemiesCount];
	
	game_object* pEnemy = CreateEnemyObject(enemyType, mapPosition, drop);
	x_printf( "ENEMY:%d,%d, 0x%x\n", (s16)mapPosition->x, (s16)mapPosition->y, pEnemy);
}
//---------------------------------------------------------
void CheckForCoin(play_cell* pCell, coord_s8* mapPosition)
{
	if (pCell->type==CELL_COIN)
	{
		pCell->objIndex = AddCoin(mapPosition);
		pCell->type = CELL_TILE;
	}
}

//---------------------------------------------------------
bool CheckForEnemy(const play_cell* pCell)
{
	return pCell->type==CELL_ENEMY;
}

//---------------------------------------------------------
u8 CheckPathBlocked(const play_cell* pCell)
{
	return (pCell->type==CELL_OBSTACLE || pCell->type==CELL_HOLE)?1:0;

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

	const coord_s8 mapPos = {x, y};
	const play_cell* pCell = GetPlayAreaCell(&mapPos);
	if (CheckPathBlocked(pCell) || CheckForEnemy(pCell))
		return;
	if ((x==levelManager.player->trans.pos.x) && (y==levelManager.player->trans.pos.y))
		return;
	coord_s8 position = {x, y};
	PlaceRandomEnemy(&position, true);
	levelManager.enemyDropDelay = spawnRate*gameManager.ticksPerSecond;
}

#define CELL_OFFSET(x,y) ((x)+((y)*PLAY_AREA_CELLS_WIDTH))
static const s16 offsetTable[]=
{
	CELL_OFFSET(-1,-1),
	CELL_OFFSET(-1, 1),
	CELL_OFFSET(0, 1),
	CELL_OFFSET(0,-1),
	CELL_OFFSET(1, 0),
	CELL_OFFSET(1, 1),
	CELL_OFFSET(1,-1),
};
//---------------------------------------------------------
bool CanPlaceObstacleOrHole(play_cell* pCell)
{
	u8 borderingObstacles = 0;
	const u16* pOffset = offsetTable;
	
	for(u8 i=0; i<7; i++, pOffset++)
	{
		borderingObstacles += CheckPathBlocked(pCell+*pOffset);
	}
	return (borderingObstacles<=1)?true:false;
}

//---------------------------------------------------------
void AddObstacles(const play_area_template* template)
{
		// Create an array of enabled enemy types. This allows us to randomly pick a type later.
	u8 enemyTypes = levelManager.config.enabledEnemies;
	for (u8 i=0; i<8 && enemyTypes; i++)
	{
		levelManager.enabledEnemies[levelManager.enabledEnemiesCount++] = i;
		enemyTypes >>= 1;
	}
	u8 sx = template->size.x;
	u8 sy = template->size.y;
	s8 spawnx = template->start.x;
	s8 spawny = template->start.y;
	s8 cy = -sy/2;
	for (u8 y = 0; y<sy; cy++, y++)
	{
		s8 cx = -sx/2;

		const coord_s8 mapPos = {cx, cy};
		play_cell* pCell = GetPlayAreaCell(&mapPos);
		for (u8 x = 0; x<sx; x++, cx++, pCell++)
		{
			if ((x!=0) && (y!=0) && (pCell->type!=CELL_HOLE) && (pCell->type!=CELL_OBSTACLE) && !((cx==spawnx) && (cy==spawny)))
			{
				u8 upperRange = sx-1+sy-(x+y);
				upperRange += levelManager.config.numCoins+levelManager.config.numHoles+levelManager.config.numObstacles;
				upperRange += levelManager.currentMaxEnemies+levelManager.config.numSpikes;
				u8 rand = random8()%upperRange;
				coord_s8 mapPosition={cx,cy};
				if (rand>0 && rand<=levelManager.config.numCoins)
				{
					AddCoin(&mapPosition);
					levelManager.config.numCoins--;
				}
				else if ((rand>levelManager.config.numHoles+levelManager.config.numCoins+levelManager.config.numObstacles+levelManager.config.maxNumEnemies) && 
						 (rand<=levelManager.config.numHoles+levelManager.config.numCoins+levelManager.config.numObstacles+levelManager.config.maxNumEnemies+levelManager.config.numSpikes))
				{
					PlaceSpike(&mapPosition);
					levelManager.config.numSpikes--;
				}
				else
				{
					if (CanPlaceObstacleOrHole(pCell))
					{
						if ((rand>levelManager.config.numCoins) && (rand<=levelManager.config.numHoles+levelManager.config.numCoins))
						{
							PlaceHole(pCell);
						}
						else if ((rand>levelManager.config.numHoles+levelManager.config.numCoins) && 
								 (rand <= levelManager.config.numHoles+levelManager.config.numCoins+levelManager.config.numObstacles))
						{
							PlaceObstacle(&mapPosition);
							levelManager.config.numObstacles--;
						}
						else if ((rand>levelManager.config.numHoles+levelManager.config.numCoins) &&
								 (rand<=levelManager.config.numHoles+levelManager.config.numCoins+levelManager.config.numObstacles+levelManager.config.maxNumEnemies))
						{
							PlaceRandomEnemy(&mapPosition, true);
							levelManager.config.maxNumEnemies--;
						}
					}
				}
			}
		}
	}
}
