#include "kftypes.h"
#include "defines.h"
#include "objects/object_manager.h"
#include "enemies/enemy_controller.h"
#include "enemies/character.h"

void CreateCharacter(game_object* pObject, const coord_s8* mapPosition, u16 param)
{
	UNUSED(pObject);
	UNUSED(mapPosition);
	UNUSED(param);
}

bool UpdateCharacter(game_object* pObject)
{
	(void)pObject;
	return true;

}

void DestroyCharacter(game_object* pObject)
{
	(void)pObject;

}
