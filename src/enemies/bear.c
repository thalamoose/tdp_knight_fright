#include "kftypes.h"
#include "defines.h"
#include "objects/object_manager.h"
#include "enemies/enemy_controller.h"
#include "enemies/bear.h"

void CreateBear(game_object* pObject, const coord_s8* mapPosition, u16 param)
{
	UNUSED(pObject);
	UNUSED(mapPosition);
	UNUSED(param);
}

bool UpdateBear(game_object* pObject)
{
	(void)pObject;
	return true;
}

void DestroyBear(game_object* pObject)
{
	(void)pObject;
}

void RenderBear(game_object* pObject)
{
	(void)pObject;
}

void CollideBear(game_object* pObject, const game_object* pCollider)
{
	(void)pObject;
	(void)pCollider;
}

void BlowupBear(game_object* pObject)
{
	(void)pObject;
}

