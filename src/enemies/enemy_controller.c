#include "kftypes.h"
#include "utilities.h"
#include "defines.h"
#include "objects/object_manager.h"
#include "objects/player.h"
#include "objects/coin.h"
#include "enemies/enemy_controller.h"
#include "enemies/bear.h"
#include "enemies/big_hopper.h"
#include "enemies/follower.h"
#include "enemies/spike.h"
#include "enemies/color_changer.h"

//---------------------------------------------------------
object_vtable enemyVirtualTable[]=
{
	{
		CreateBear, 
	 	UpdateBear, 
		RenderBear,
	 	DestroyBear, 
	 	CollideBear,
	 	BlowupBear 
	},
	{
		CreateBigHopper, 
	 	UpdateBigHopper, 
		RenderBigHopper,
	 	DestroyBigHopper, 
	 	CollideBigHopper,
	 	BlowupBigHopper
	},
	{
		CreateColorChanger, 
		UpdateColorChanger, 
		RenderColorChanger,
		DestroyColorChanger, 
		CollideColorChanger,
		BlowupColorChanger
	},
	{
		CreateFollower, 
		UpdateFollower, 
		RenderFollower,
		DestroyFollower, 
		CollideFollower,
		BlowupFollower
	},
	{
		CreateSpike, 
		UpdateSpike, 
		RenderSpike,
		DestroySpike, 
		CollideSpike,
		BlowupSpike
	},
};

//---------------------------------------------------------
game_object* CreateEnemy(u8 type, s8 x, s8 y)
{
	return CreateObject(&enemyVirtualTable[type], x, y);
}

//---------------------------------------------------------
void InitEnemies(void)
{
}