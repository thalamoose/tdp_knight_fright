#if !defined(__OBJECT_H)
#define __OBJECT_H

#include "kftypes.h"
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

enum
{
	FLAG_ACTIVE = (1<<0),
	FLAG_PINGPONG = (1<<1),
	FLAG_IS_4_BIT = (1<<2),
	FLAG_DIRECTION = (1<<3),
	FLAG_TILEMAP_LOCKED = (1<<4)
};

typedef struct s_object_vtable object_vtable;

typedef struct s_transform
{
	coord pos;
	coord vel;
	s16 gravity;
} transform;

typedef struct s_sprite
{
	u8 slot[4];
	u8 pattern[4];
	u8 patternCount;
	u8 palette;
	u8 page;
	coord_s8 centerOffset;
} sprite;

typedef struct s_animation
{
	sprite sprite;
	u8 frameIndex;
	u8 baseIndex;
	u16 lastIndex;
	s8 animDelay;
	u8 animSpeed;
	u8 totalFrames;
	union
	{
		u8 value;
		struct 
		{
			u8 pingpong:1;
			u8 is4bit:1;
			u8 direction:1;
			u8 tilemapLocked:1;
		};
	} flags;
} animation;


typedef struct s_game_object
{
	struct
	{
		u8 index;
		const object_vtable* vtable;
	} object;
	
	u8 flags;
	transform trans;
	animation anim;
	u8 moveSteps;
	u8 direction;
    coord_s8 playGrid;
} game_object;

void InitializeAnimComponent(animation* pAnim, const coord_s8* centerOffset, u8 animPage, u8 spriteCount, const sprite_config* configs);
void AnimateComponent(animation* pAnim);
void TransformComponent(transform* pTrans);
void CheckPlayerToObjectCollision(game_object* pObject);
void CheckObjectToPlayerCollision(game_object* pObject);
void RenderComponent(game_object* pObject);

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

typedef struct s_anim_configs
{
    u8 baseIndex;
    u8 direction;
    s16 vx;
    s16 vy;
} anim_config;

void SetObjectAnimIdle(game_object* pObject, const anim_config* pConfig);
void SetObjectAnimRun(game_object* pObject, const anim_config* pConfig);

#endif
