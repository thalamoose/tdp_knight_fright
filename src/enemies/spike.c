#include "kftypes.h"
#include "defines.h"
#include "objects/object_manager.h"
#include "enemies/spike.h"

void CreateSpike(game_object* pObject, const coord_s8* mapPosition, u16 param)
{
	UNUSED(pObject);
	UNUSED(mapPosition);
	UNUSED(param);
}

bool UpdateSpike(game_object* pObject)
{
	UNUSED(pObject);
	return true;
}

void RenderSpike(game_object* pObject)
{
	UNUSED(pObject);
}

void DestroySpike(game_object* pObject)
{
	UNUSED(pObject);
}

void BlowupSpike(game_object* pObject)
{
	UNUSED(pObject);
}

void CollideSpike(game_object* pObject, const game_object* pCollider)
{
	UNUSED(pObject);
	UNUSED(pCollider);
}
