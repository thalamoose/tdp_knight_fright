#include "kftypes.h"
#include "defines.h"
#include "objects/object_manager.h"
#include "utilities.h"
#include "playarea.h"

object_manager objectManager;

//---------------------------------------------------------
void InitializeObjects(void)
{
	memset(&objectManager, 0, sizeof(objectManager));
}

//---------------------------------------------------------
void ResetObjects(void)
{
	InitializeObjects();
}

//---------------------------------------------------------
void UpdateObjects(void)
{
	game_object* pObject = objectManager.objects;
	for (u8 i=0; i<MAX_OBJECTS; i++, pObject++)
	{
		if (pObject->flags.active)
		{
			bool isStillRunning = pObject->object.vtable->Update(pObject);
			if (!isStillRunning)
			{
				DestroyObject(pObject);
			}
		}
	}
}

//---------------------------------------------------------
void RenderObjects(void)
{
	game_object* pObject = objectManager.objects;
	for (u8 i=0; i<MAX_OBJECTS; i++, pObject++)
	{
		if (pObject->flags.active)
		{
			pObject->object.vtable->Render(pObject);
		}
	}
}

//---------------------------------------------------------
game_object* CreateObject(const object_vtable* vtable, s8 px, s8 py)
{
	u8 index = objectManager.objectIndex;
	game_object* pGameObj = &objectManager.objects[index];

	while (pGameObj->flags.active)
	{
		pGameObj++;
		index = (index+1)%MAX_OBJECTS;
		if (index==0)
		{
			pGameObj = &objectManager.objects[0];
		}
		if (index==objectManager.objectIndex)
		{
			return NULL;
		}
	}
	// Set this up to point to the next one, it's very likely to be free.
	objectManager.objectIndex = (index+1)%MAX_OBJECTS;

 	// pObj->object and pObj are the same address, this is done for type safety.
	// Effectively, game_object is the same as object, but just with some extra
	// storage space at the end.
	pGameObj->object.vtable = vtable;
	pGameObj->object.index = index;
	pGameObj->playGrid.x = px;
	pGameObj->playGrid.y = py;
	SnapToPlayAreaGrid(pGameObj);
	pGameObj->flags.active = true;
	if (pGameObj->object.vtable->Create) pGameObj->object.vtable->Create(pGameObj, px, py);
	return pGameObj;
}

//---------------------------------------------------------
void DestroyObject(game_object* pObject)
{
	if (pObject->object.vtable->Destroy) pObject->object.vtable->Destroy(pObject);
	pObject->flags.active = false;
	// Very likely to get reused on the next object create.
	objectManager.objectIndex = pObject->object.index;
}

//---------------------------------------------------------
game_object* GetObjectFromIndex(u8 index)
{
	return &objectManager.objects[index];
}

