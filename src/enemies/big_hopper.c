#include "kftypes.h"
#include "defines.h"
#include "objects/object_manager.h"
#include "enemies/enemy_controller.h"
#include "enemies/big_hopper.h"

void CreateBigHopper(game_object* pObject, s8 px, s8 py)
{
	(void)(pObject+px+py);
}

bool UpdateBigHopper(game_object* pObject)
{
	(void)pObject;
	return true;
}

void RenderBigHopper(game_object* pObject)
{
	(void)pObject;
}

void DestroyBigHopper(game_object* pObject)
{
	(void)pObject;
}
void BlowupBigHopper(game_object* pObject)
{
	(void)pObject;
}

void CollideBigHopper(game_object* pObject, play_cell* pCollider)
{
	(void)pObject;
	(void)pCollider;
}