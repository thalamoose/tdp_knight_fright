#include "kftypes.h"
#include "defines.h"
#include "objects/object_manager.h"
#include "enemies/enemy_controller.h"
#include "enemies/bear.h"

void CreateBear(game_object* pObject, s8 px, s8 py)
{
	(void)(pObject+px+py);
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

void CollideBear(game_object* pObject, play_cell* pCollider)
{
	(void)pObject;
	(void)pCollider;
}

void BlowupBear(game_object* pObject)
{
	(void)pObject;
}

