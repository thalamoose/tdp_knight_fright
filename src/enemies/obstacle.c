#include "kftypes.h"
#include "defines.h"
#include "objects/object_manager.h"
#include "enemies/enemy_controller.h"
#include "enemies/color_changer.h"

void CreateObstacle(game_object* pObject, const coord_s8* mapPosition, u16 param)
{
	UNUSED(pObject);
	UNUSED(mapPosition);
	UNUSED(param);
}

void DestroyObstacle(game_object* pObject)
{
	(void)pObject;
}

//---------------------------------------------------------
const object_vtable obstacleVirtualTable =
{
	CreateObstacle, 
	NULL, 
	RenderComponent,
	DestroyObstacle, 
	NULL,
	NULL, 
};

//---------------------------------------------------------
game_object* CreateObstacleObject(const coord_s8* mapPosition, u16 param)
{
	return CreateObject(&obstacleVirtualTable, mapPosition, param);
}
