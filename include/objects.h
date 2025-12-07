#if !defined(__OBJECT_H)
#define __OBJECT_H

/*
	All coordinates are internally held as signed 10.6 fixed point.
	Coordinate origin is (0,0), this matches the center of the
	screen.
	The dynamic range is -512 to 511
*/

struct s_object;
typedef struct s_object object;

typedef void(object_create_fn)(object* obj);
typedef bool(object_update_fn)(object* obj);
typedef void(object_render_fn)(object* obj);
typedef void(object_destroy_fn)(object* obj);
typedef void(object_blowup_fn)(object* obj);
typedef void(object_collide_fn)(object* obj, object* collider);

typedef struct 
{
	object_create_fn* Create;
	object_update_fn* Update;
	object_render_fn* Render;
	object_destroy_fn* Destroy;
	object_collide_fn* Collide;
	object_blowup_fn* Blowup;
} object_vtable;

typedef struct s_object
{
	coord position;
	coord velocity;
	s16 gravity;
	u8 objectIndex;
	u8 frameIndex;
	u8 baseIndex;
	u8 lastIndex;
	s8 animDelay;
	u8 animSpeed;
	u8 totalFrames;
    coord_s8 playGrid;
	union
	{
		struct 
		{
			bool active:1;
			bool pingpong:1;
			bool is4bit:1;
			bool direction:1;
			bool tilemapLocked:1;
		};
		u8 flagValue;
	} flags;
	object_vtable* vtable;
} object;

enum object_type
{
	OBJECT_PLAYER = 0,
	OBJECT_BEAR,
	OBJECT_BIG_HOPPER,
	OBJECT_COLOR_CHANGER,
	OBJECT_FOLLOWER,
	OBJECT_SPIKE,
} ;

typedef struct
{
	object object;
	u8 typeSpecific[16];
} game_object;

void InitObjects(void);
object* CreateObject(const object_vtable* vtable);
void UpdateObject(void);
void DestroyObject(object* obj);
object* GetObjectFromIndex(u8 objectIndex);

typedef struct 
{
	u8 objectIndex;
	game_object objects[MAX_OBJECTS];
} object_manager;

void InitializeObjects(void);
void ResetObjects(void);
void UpdateObjects(void);
void RenderObjects(void);
void AnimateObject(object *pObj);

extern object_manager objectManager;

#endif
