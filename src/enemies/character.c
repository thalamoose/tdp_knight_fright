#include "kftypes.h"
#include "defines.h"
#include "objects/object_manager.h"
#include "enemies/enemy_controller.h"
#include "enemies/character.h"

void CreateCharacter(game_object* pObject, s8 px, s8 py)
{
	(void)(pObject+px+py);
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
