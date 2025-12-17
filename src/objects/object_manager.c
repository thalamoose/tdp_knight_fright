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
		if (pObject->flags & FLAG_ACTIVE)
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
		if (pObject->flags & FLAG_ACTIVE)
		{
			if (pObject->object.vtable->Render)
				pObject->object.vtable->Render(pObject);
		}
	}
}

//---------------------------------------------------------
game_object* CreateObject(const object_vtable* vtable, const coord_s8* mapPosition, u16 param)
{
	u8 index = objectManager.objectIndex;
	game_object* pGameObj = &objectManager.objects[index];

	while (pGameObj->flags & FLAG_ACTIVE)
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
	pGameObj->playGrid = *mapPosition;
	SnapToPlayAreaGrid(pGameObj);
	pGameObj->flags = FLAG_ACTIVE;
	pGameObj->object.index = index;
	if (pGameObj->object.vtable->Create) pGameObj->object.vtable->Create(pGameObj, mapPosition, param);
	return pGameObj;
}

//---------------------------------------------------------
void BlowupObject(game_object* pObject)
{
	if (pObject->object.vtable->Blowup) 
		pObject->object.vtable->Blowup(pObject);
}

//---------------------------------------------------------
void DestroyObject(game_object* pObject)
{
	if (pObject->object.vtable->Destroy) 
		pObject->object.vtable->Destroy(pObject);
	pObject->flags = 0;
	// Very likely to get reused on the next object create.
	objectManager.objectIndex = pObject->object.index;
}

//---------------------------------------------------------
game_object* GetObjectFromIndex(u8 index)
{
	return &objectManager.objects[index];
}

