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
		if (pObject->object.flags.active)
		{
			bool isStillRunning = pObject->object.vtable->Update(&pObject->object);
			if (!isStillRunning)
			{
				DestroyObject(&pObject->object);
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
		if (pObject->object.flags.active)
		{
			pObject->object.vtable->Render(&pObject->object);
		}
	}
}

//---------------------------------------------------------
object* CreateObject(const object_vtable* vtable, s8 px, s8 py)
{
	u8 index = objectManager.objectIndex;
	game_object* pGameObj = &objectManager.objects[index];

	while (pGameObj->object.flags.active)
	{
		pGameObj++;
		index++;
		if (index>=MAX_OBJECTS)
		{
			index = 0;
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
 	object* pObj = &pGameObj->object;
	pObj->vtable = vtable;
	pObj->index = index;
	pObj->playGrid.x = px;
	pObj->playGrid.y = py;
	SnapToPlayAreaGrid(pObj);
	pObj->flags.active = true;
	if (pObj->vtable->Create) pObj->vtable->Create(pObj, px, py);
	return pObj;
}

//---------------------------------------------------------
void DestroyObject(object* pObject)
{
	if (pObject->vtable->Destroy) pObject->vtable->Destroy(pObject);
	pObject->flags.active = false;
	// Very likely to get reused on the next object create.
	objectManager.objectIndex = pObject->index;
}

//---------------------------------------------------------
object* GetObjectFromIndex(u8 index)
{
	return &objectManager.objects[index].object;
}

