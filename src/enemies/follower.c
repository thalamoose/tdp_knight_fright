#include "kftypes.h"
#include "defines.h"
#include "objects/object_manager.h"
#include "enemies/follower.h"

void CreateFollower(game_object* pObject, const coord_s8* mapPosition, u16 param)
{
	UNUSED(pObject);
	UNUSED(mapPosition);
	UNUSED(param);
}

bool UpdateFollower(game_object* pObject)
{
	(void)pObject;
	return true;
}

void RenderFollower(game_object* pObject)
{
	(void)pObject;
}

void DestroyFollower(game_object* pObject)
{
	(void)pObject;
}

void BlowupFollower(game_object* pObject)
{
	(void)pObject;
}

void CollideFollower(game_object* pObject, const game_object* pCollider)
{
	(void)pObject;
	(void)pCollider;
}