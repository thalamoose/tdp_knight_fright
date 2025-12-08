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
		(object_create_fn*)CreateBear, 
	 	(object_update_fn*)UpdateBear, 
		(object_render_fn*)RenderBear,
	 	(object_destroy_fn*)DestroyBear, 
	 	(object_collide_fn*)CollideBear,
	 	(object_blowup_fn*)BlowupBear 
	},
	{
		(object_create_fn*)CreateBigHopper, 
	 	(object_update_fn*)UpdateBigHopper, 
		(object_render_fn*)RenderBigHopper,
	 	(object_destroy_fn*)DestroyBigHopper, 
	 	(object_collide_fn*)CollideBigHopper,
	 	(object_blowup_fn*)BlowupBigHopper
	},
	{
		(object_create_fn*)CreateColorChanger, 
		(object_update_fn*)UpdateColorChanger, 
		(object_render_fn*)RenderColorChanger,
		(object_destroy_fn*)DestroyColorChanger, 
		(object_collide_fn*)CollideColorChanger,
		(object_blowup_fn*)BlowupColorChanger
	},
	{
		(object_create_fn*)CreateFollower, 
		(object_update_fn*)UpdateFollower, 
		(object_render_fn*)RenderFollower,
		(object_destroy_fn*)DestroyFollower, 
		(object_collide_fn*)CollideFollower,
		(object_blowup_fn*)BlowupFollower
	},
	{
		(object_create_fn*)CreateSpike, 
		(object_update_fn*)UpdateSpike, 
		(object_render_fn*)RenderSpike,
		(object_destroy_fn*)DestroySpike, 
		(object_collide_fn*)CollideSpike,
		(object_blowup_fn*)BlowupSpike
	},
};

//---------------------------------------------------------
object* CreateEnemy(u8 type, s8 x, s8 y)
{
	return CreateObject(&enemyVirtualTable[type], x, y);
}

//---------------------------------------------------------
void InitEnemies(void)
{
}