#include "kftypes.h"
#include "defines.h"
#include "objects/object_manager.h"
#include "enemies/enemy_controller.h"
#include "enemies/color_changer.h"

void CreateColorChanger(game_object* pObject, const coord_s8* mapPosition, u16 param)
{
	UNUSED(pObject);
	UNUSED(mapPosition);
	UNUSED(param);
}

bool UpdateColorChanger(game_object* pObject)
{
	UNUSED(pObject);
	return true;
}

void RenderColorChanger(game_object* pObject)
{
	UNUSED(pObject);
}

void DestroyColorChanger(game_object* pObject)
{
	UNUSED(pObject);
}

void BlowupColorChanger(game_object* pObject)
{
	UNUSED(pObject);
}

void CollideColorChanger(game_object* pObject, const game_object* pCollider)
{
	UNUSED(pObject);
	UNUSED(pCollider);
}
