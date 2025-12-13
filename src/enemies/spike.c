#include "kftypes.h"
#include "defines.h"
#include "objects/object_manager.h"
#include "enemies/spike.h"

void CreateSpike(game_object* pObject, s8 px, s8 py)
{
	(void)(pObject+px+py);
}

bool UpdateSpike(game_object* pObject)
{
	(void)pObject;
	return true;
}

void RenderSpike(game_object* pObject)
{
	(void)pObject;
}

void DestroySpike(game_object* pObject)
{
	(void)pObject;
}

void BlowupSpike(game_object* pObject)
{
	(void)pObject;
}

void CollideSpike(game_object* pObject, const game_object* pCollider)
{
	(void)pObject;
	(void)pCollider;
}
