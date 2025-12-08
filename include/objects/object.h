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

struct s_object_vtable;
typedef struct s_object_vtable object_vtable;

typedef struct s_object
{
	u8 index;
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
		u8 value;
	} flags;
	transform trans;
	animation anim;
    coord_s8 playGrid;
	const object_vtable* vtable;
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

#endif
