#include "kftypes.h"
#include "defines.h"
#include "objects/object_manager.h"
#include "objects/character_controller.h"
#include "enemies/big_hopper.h"

void CreateBigHopper(game_object* pObject, const coord_s8* mapPosition, u16 param)
{
	UNUSED(pObject);
	UNUSED(mapPosition);
	UNUSED(param);
}

bool UpdateBigHopper(game_object* pObject)
{
	UNUSED(pObject);
	return true;
}

void RenderBigHopper(game_object* pObject)
{
	UNUSED(pObject);
}

void DestroyBigHopper(game_object* pObject)
{
	UNUSED(pObject);
}

void BlowupBigHopper(game_object* pObject)
{
	UNUSED(pObject);
}

void CollideBigHopper(game_object* pObject, const game_object* pCollider)
{
	UNUSED(pObject);
	UNUSED(pCollider);
}