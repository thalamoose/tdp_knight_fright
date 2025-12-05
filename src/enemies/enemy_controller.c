#include "kftypes.h"
#include "utilities.h"
#include "enemies/enemy_controller.h"
#include "enemies/bear.h"
#include "enemies/big_hopper.h"
#include "enemies/follower.h"
#include "enemies/spike.h"
#include "enemies/color_changer.h"

#define MAX_ENEMIES 8

//---------------------------------------------------------
union 
{
	enemy_controller controller;
	bear bear;
	big_hopper hopper;
	follower follower;
	spike spike;
	color_changer color;
} enemies[MAX_ENEMIES];

//---------------------------------------------------------
enemy_controller* CreateEnemy(u8 type)
{
	for (u8 i=0;i<MAX_ENEMIES;i++)
	{
		if (enemies[i].controller.flags.active==false)
		{
			enemy_controller* enemy = &enemies[i].controller;

			enemy->type = type;
			enemy->flags.active = true;
			switch(enemy->type)
			{
				case ENEMY_TYPE_BEAR: InitBear((bear*)enemy); break;
				case ENEMY_TYPE_BIG_HOPPER: InitBigHopper((big_hopper*)enemy); break;
				case ENEMY_TYPE_FOLLOWER: InitFollower((follower*)enemy); break;
				case ENEMY_TYPE_SPIKE:		InitSpike((spike*)enemy); break;
				case ENEMY_TYPE_COLOR_CHANGER: InitColorChanger((color_changer*)enemy); break;
			}
			return enemy;
		}
	}
	return NULL;
}

//---------------------------------------------------------
void InitEnemies(void)
{
	memset(&enemies, 0, sizeof(enemies));
}

//---------------------------------------------------------
void UpdateEnemies(void)
{
	for (u8 i=0; i<MAX_ENEMIES; i++)
	{
		enemy_controller* enemy = &enemies[i].controller;
		if (enemy->flags.active==false)
		{
			continue;
		}
		bool result = false;
		switch(enemy->type)
		{
			case ENEMY_TYPE_BEAR: result = UpdateBear((bear*)enemy); break;
			case ENEMY_TYPE_BIG_HOPPER: result = UpdateBigHopper((big_hopper*)enemy); break;
			case ENEMY_TYPE_FOLLOWER: result = UpdateFollower((follower*)enemy); break;
			case ENEMY_TYPE_SPIKE: result = UpdateSpike((spike*)enemy); break;
			case ENEMY_TYPE_COLOR_CHANGER: result = UpdateColorChanger((color_changer*)enemy); break;
		}
		if (result==false)
		{
			DestroyEnemy(enemy);
		}
	}
}

//---------------------------------------------------------
void DestroyEnemy(enemy_controller* enemy)
{
	switch(enemy->type)
	{
		case ENEMY_TYPE_BEAR: DestroyBear((bear*)enemy); break;
		case ENEMY_TYPE_BIG_HOPPER: DestroyBigHopper((big_hopper*)enemy); break;
		case ENEMY_TYPE_FOLLOWER: DestroyFollower((follower*)enemy); break;
		case ENEMY_TYPE_SPIKE: DestroySpike((spike*)enemy); break;
		case ENEMY_TYPE_COLOR_CHANGER: DestroyColorChanger((color_changer*)enemy); break;
	}
	enemy->flags.active = false;
}