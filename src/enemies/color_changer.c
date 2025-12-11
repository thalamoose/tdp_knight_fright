#include "kftypes.h"
#include "defines.h"
#include "objects/object_manager.h"
#include "enemies/enemy_controller.h"
#include "enemies/color_changer.h"

void CreateColorChanger(game_object* pObject, s8 px, s8 py)
{
	(void)(pObject+px+py);
}

bool UpdateColorChanger(game_object* pObject)
{
	(void)pObject;
	return true;
}

void RenderColorChanger(game_object* pObject)
{
	(void)pObject;
}

void DestroyColorChanger(game_object* pObject)
{
	(void)pObject;
}

void BlowupColorChanger(game_object* pObject)
{
	(void)pObject;
}

void CollideColorChanger(game_object* pObject, play_cell* pCollider)
{
	(void)pObject;
	(void)pCollider;
}
