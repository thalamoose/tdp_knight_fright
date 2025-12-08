#if !defined(__OBJECT_MANAGER_H)
#define __OBJECT_MANAGER_H

#include "object.h"
/*
	All coordinates are internally held as signed 10.6 fixed point.
	Coordinate origin is (0,0), this matches the center of the
	screen.
	The dynamic range is -512 to 511
*/

typedef void(object_create_fn)(object* obj, s8 px, s8 py);
typedef bool(object_update_fn)(object* obj);
typedef void(object_render_fn)(object* obj);
typedef void(object_destroy_fn)(object* obj);
typedef void(object_collide_fn)(object* obj, object* collider);
typedef void(object_blowup_fn)(object* obj);

typedef struct s_object_vtable
{
	object_create_fn* Create;
	object_update_fn* Update;
	object_render_fn* Render;
	object_destroy_fn* Destroy;
	object_collide_fn* Collide;
	object_blowup_fn* Blowup;
} object_vtable;

typedef struct
{
	object object;
	u8 typeSpecific[16];
} game_object;

typedef struct 
{
	u8 objectIndex;
	game_object objects[MAX_OBJECTS];
} object_manager;

void InitializeObjects(void);
void ResetObjects(void);
void UpdateObjects(void);
void RenderObjects(void);

object* CreateObject(const object_vtable* vtable, s8 px, s8 py);
void DestroyObject(object* obj);
object* GetObjectFromIndex(u8 objectIndex);

extern object_manager objectManager;

#endif
