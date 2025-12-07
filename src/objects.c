#include "kftypes.h"
#include "defines.h"
#include "objects.h"
#include "utilities.h"

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
	for (u8 i=0; i<MAX_OBJECTS; i++)
	{
		if (pObject->object.flags.active)
		{
			bool continueRunning = pObject->object.vtable->Update(&pObject->object);
			if (continueRunning)
				continue;
			DestroyObject(&pObject->object);
		}
		pObject++;
	}
}

//---------------------------------------------------------
void RenderObjects(void)
{
	game_object* pObject = objectManager.objects;
	for (u8 i=0; i<MAX_OBJECTS; i++)
	{
		if (pObject->object.flags.active)
		{
			pObject->object.vtable->Render(&pObject->object);
		}
		pObject++;
	}
}

//---------------------------------------------------------
object* CreateObject(const object_vtable* vtable)
{
	(void)vtable;
	return NULL;
}

//---------------------------------------------------------
void DestroyObject(object* pObject)
{
	if (pObject->vtable->Destroy) pObject->vtable->Destroy(pObject);
	pObject->flags.active = false;
}

//---------------------------------------------------------
void AnimateObject(object *pObj)
{
	if (!pObj->flags.active)
		return;
	pObj->frameIndex++;
	if (pObj->animDelay)
	{
		pObj->animDelay--;
		return;
	}
	pObj->animDelay = pObj->animSpeed;
	if (pObj->flags.direction)
	{
		pObj->frameIndex--;
		if (pObj->frameIndex == 0)
			pObj->flags.direction = false;
	}
	else
	{
		pObj->frameIndex++;
		if (pObj->frameIndex == pObj->totalFrames)
		{
			if (pObj->flags.pingpong)
			{
				pObj->flags.direction = true;
				pObj->frameIndex = pObj->totalFrames - 1;
			}
			else
				pObj->frameIndex = 0;
		}
	}
}

//---------------------------------------------------------
object* GetObjectFromIndex(u8 index)
{
	return &objectManager.objects[index].object;
}

