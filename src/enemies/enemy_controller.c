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
typedef struct 
{
	union 
	{
		enemy_controller controller;
		bear bear;
		big_hopper hopper;
		follower follower;
		spike spike;
		color_changer color;
	} enemies[MAX_ENEMIES];
} enemy_manager;

enemy_manager enemyManager;

struct 
{
	enemy_create_fn* Create;
	enemy_update_fn* Update;
	enemy_destroy_fn* Destroy;
	enemy_blowup_fn* Blowup;
	enemy_collide_fn* Collide;
} enemyVirtualTable[]=
{
	{
		(enemy_create_fn*)CreateBear, 
	 	(enemy_update_fn*)UpdateBear, 
	 	(enemy_destroy_fn*)DestroyBear, 
	 	(enemy_blowup_fn*)BlowupBear, 
	 	(enemy_collide_fn*)CollideBear
	},
	{
		(enemy_create_fn*)CreateBigHopper, 
	 	(enemy_update_fn*)UpdateBigHopper, 
	 	(enemy_destroy_fn*)DestroyBigHopper, 
	 	(enemy_blowup_fn*)BlowupBigHopper, 
	 	(enemy_collide_fn*)CollideBigHopper
	},
	{
		(enemy_create_fn*)CreateColorChanger, 
		(enemy_update_fn*)UpdateColorChanger, 
		(enemy_destroy_fn*)DestroyColorChanger, 
		(enemy_blowup_fn*)BlowupColorChanger, 
		(enemy_collide_fn*)CollideColorChanger
	},
	{
		(enemy_create_fn*)CreateFollower, 
		(enemy_update_fn*)UpdateFollower, 
		(enemy_destroy_fn*)DestroyFollower, 
		(enemy_blowup_fn*)BlowupFollower, 
		(enemy_collide_fn*)CollideFollower
	},
	{
		(enemy_create_fn*)CreateSpike, 
		(enemy_update_fn*)UpdateSpike, 
		(enemy_destroy_fn*)DestroySpike, 
		(enemy_blowup_fn*)BlowupSpike, 
		(enemy_collide_fn*)CollideSpike
	},
};

//---------------------------------------------------------
enemy_controller* CreateEnemy(u8 type)
{
	for (u8 i=0;i<MAX_ENEMIES;i++)
	{
		if (enemyManager.enemies[i].controller.flags.active==false)
		{
			enemy_controller* enemy = &enemyManager.enemies[i].controller;

			enemy->type = type;
			enemy->flags.active = true;
			enemy->Create = enemyVirtualTable[type].Create;
			enemy->Update = enemyVirtualTable[type].Update;
			enemy->Destroy = enemyVirtualTable[type].Destroy;
			enemy->Blowup = enemyVirtualTable[type].Blowup;
			enemy->Create(enemy);
			return enemy;
		}
	}
	return NULL;
}

//---------------------------------------------------------
void InitEnemies(void)
{
	memset(&enemyManager.enemies, 0, sizeof(enemyManager.enemies));
}

//---------------------------------------------------------
void UpdateEnemies(void)
{
	for (u8 i=0; i<MAX_ENEMIES; i++)
	{
		enemy_controller* enemy = &enemyManager.enemies[i].controller;
		if (enemy->flags.active==false)
		{
			continue;
		}
		bool result = true;
		enemy->Update(enemy);
		if (result==false)
		{
			enemy->Destroy(enemy);
			enemy->flags.active = false;
		}
	}
}

//---------------------------------------------------------
enemy_controller* GetEnemyFromIndex(u8 enemyIndex)
{
	return &enemyManager.enemies[enemyIndex].controller;
}
