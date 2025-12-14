#if !defined(__OBJECT_H)
#define __OBJECT_H

#include "kftypes.h"
#include "components.h"
/*
	All coordinates are internally held as signed 10.6 fixed point.
	Coordinate origin is (0,0), this matches the center of the
	screen.
	The dynamic range is -512 to 511
*/

enum object_type
{
	OBJECT_PLAYER = 0,
	OBJECT_BEAR,
	OBJECT_BIG_HOPPER,
	OBJECT_COLOR_CHANGER,
	OBJECT_FOLLOWER,
	OBJECT_SPIKE,
	OBJECT_OBSTACLE,
} ;

typedef struct s_play_cell play_cell;

typedef void(object_create_fn)(game_object* obj, const coord_s8* mapPosition, u16 param);
typedef bool(object_update_fn)(game_object* obj);
typedef void(object_render_fn)(game_object* obj);
typedef void(object_destroy_fn)(game_object* obj);
typedef void(object_collide_fn)(game_object* obj, const game_object* collider);
typedef void(object_blowup_fn)(game_object* obj);

typedef struct s_object_vtable
{
	object_create_fn* Create;
	object_update_fn* Update;
	object_render_fn* Render;
	object_destroy_fn* Destroy;
	object_collide_fn* Collide;
	object_blowup_fn* Blowup;
} object_vtable;

typedef struct s_game_object
{
	struct
	{
		u8 index;
		const object_vtable* vtable;
	} object;
	
	union
	{
		u8 value;
		struct 
		{
			bool active:1;
			bool pingpong:1;
			bool is4bit:1;
			bool direction:1;
			bool tilemapLocked:1;
		};
	} flags;
	transform trans;
	animation anim;
	u8 moveSteps;
	u8 direction;
    coord_s8 playGrid;
} game_object;

#endif
