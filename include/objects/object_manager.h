#if !defined(__OBJECT_MANAGER_H)
#define __OBJECT_MANAGER_H

#include "object.h"
/*
	All coordinates are internally held as signed 10.6 fixed point.
	Coordinate origin is (0,0), this matches the center of the
	screen.
	The dynamic range is -512 to 511
*/


typedef struct 
{
	u8 objectIndex;
	game_object objects[MAX_OBJECTS];
} object_manager;

void InitializeObjects(void);
void ResetObjects(void);
void UpdateObjects(void);
void RenderObjects(void);

game_object* CreateObject(const object_vtable* vtable, s8 px, s8 py);
void BlowupObject(game_object* pObject);
void DestroyObject(game_object* obj);
game_object* GetObjectFromIndex(u8 objectIndex);

extern object_manager objectManager;

#endif
