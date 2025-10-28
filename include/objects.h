#if !defined(__OBJECT_H)
#define __OBJECT_H
#include "kftypes.h"
enum fixed_point
{
	FIXED_POINT_BITS=6,
	FIXED_POINT_ONE=(1<<FIXED_POINT_BITS),
	FIXED_POINT_HALF=(FIXED_POINT_ONE/2),
};

typedef struct
{
	s16 x;
	s16 y;
} coord;

typedef struct s_object
{
	coord position;
	coord velocity;
    s16 gravity;
    u8 frameIndex;
    u8 baseIndex;
    s8 animDelay;
    u8 animSpeed;
    u8 totalFrames;
	struct 
	{
		u8 active:1;
		u8 pingpong:1;
		u8 is4bit:1;
		u8 direction:1;
	} flags;
	
} object;

void RenderObjects(void);
void UpdateObjects(void);
void AnimateObject(object* pObj);

#endif
