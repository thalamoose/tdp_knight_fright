#if !defined(__COMPONENTS_H)
#define __COMPONENTS_H
#include "kftypes.h"

typedef struct s_transform
{
	coord pos;
	coord vel;
	s16 gravity;
} transform;

typedef struct s_sprite
{
	u8 slot;
	u8 pattern;
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
	u8 lastIndex;
	s8 animDelay;
	u8 animSpeed;
	u8 totalFrames;
	union
	{
		struct 
		{
			bool pingpong:1;
			bool is4bit:1;
			bool direction:1;
			bool tilemapLocked:1;
		};
		u8 value;
	} flags;
} animation;

void AnimateComponent(animation* pAnim);
void TransformComponent(transform* pTrans);
void RenderComponent(transform* pTrans, animation* pAnim);

#endif
