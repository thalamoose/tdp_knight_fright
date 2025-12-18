#include "kftypes.h"
#include "defines.h"
#include "objects/object_manager.h"
#include "utilities.h"
#include "playarea.h"
#include "memorymap.h"
#include "hardware.h"
#include "tilemap.h"
#include "hud.h"
#include "level_manager.h"
#include "sprite.h"

//---------------------------------------------------------
void TransformComponent(transform* trans)
{
	trans->pos.x += trans->vel.x;
	trans->pos.y += trans->vel.y;
	trans->vel.y += trans->gravity;
}

//---------------------------------------------------------
// Did object collide with player?
void CheckObjectToPlayerCollision(game_object* pObject)
{
	if (pObject==levelManager.player)
		return;
	if ((pObject->playGrid.x==levelManager.player->playGrid.x) &&
		(pObject->playGrid.y==levelManager.player->playGrid.y))
	{
		pObject->object.vtable->Collide(pObject, levelManager.player);
	}
}

//---------------------------------------------------------
// Did player collide with object?
void CheckPlayerToObjectCollision(game_object* pObject)
{
	play_cell* pCell = GetPlayAreaCell(&pObject->playGrid);
	if (pCell->type!=CELL_ENEMY)
		return;
	game_object* pCollider = GetObjectFromIndex(pCell->objIndex);
	if ((pObject->playGrid.x==pCollider->playGrid.x) &&
		(pObject->playGrid.y==pCollider->playGrid.y))
	{
		pObject->object.vtable->Collide(pObject, levelManager.player);
	}
}

