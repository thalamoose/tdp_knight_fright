#include "kftypes.h"
#include "utilities.h"
#include "defines.h"
#include "objects.h"
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
		(object_create_fn*)CreateBear, 
	 	(object_update_fn*)UpdateBear, 
	 	(object_destroy_fn*)DestroyBear, 
	 	(object_blowup_fn*)BlowupBear, 
	 	(object_collide_fn*)CollideBear
	},
	{
		(object_create_fn*)CreateBigHopper, 
	 	(object_update_fn*)UpdateBigHopper, 
	 	(object_destroy_fn*)DestroyBigHopper, 
	 	(object_blowup_fn*)BlowupBigHopper, 
	 	(object_collide_fn*)CollideBigHopper
	},
	{
		(object_create_fn*)CreateColorChanger, 
		(object_update_fn*)UpdateColorChanger, 
		(object_destroy_fn*)DestroyColorChanger, 
		(object_blowup_fn*)BlowupColorChanger, 
		(object_collide_fn*)CollideColorChanger
	},
	{
		(object_create_fn*)CreateFollower, 
		(object_update_fn*)UpdateFollower, 
		(object_destroy_fn*)DestroyFollower, 
		(object_blowup_fn*)BlowupFollower, 
		(object_collide_fn*)CollideFollower
	},
	{
		(object_create_fn*)CreateSpike, 
		(object_update_fn*)UpdateSpike, 
		(object_destroy_fn*)DestroySpike, 
		(object_blowup_fn*)BlowupSpike, 
		(object_collide_fn*)CollideSpike
	},
};

//---------------------------------------------------------
object* CreateEnemy(u8 type)
{
	return CreateObject(&enemyVirtualTable[type]);
}

//---------------------------------------------------------
void InitEnemies(void)
{
}