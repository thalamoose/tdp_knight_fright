#if !defined(__OBJECT_H)
#define __OBJECT_H
#include "kftypes.h"

/*
	All coordinates are internally held as signed 10.6 fixed point.
	Coordinate origin is (0,0), this matches the center of the
	screen.
	The dynamic range is -512 to 511
*/

typedef struct s_object
{
	coord position;
	coord velocity;
	s16 gravity;
	u8 frameIndex;
	u8 baseIndex;
	u8 lastIndex;
	s8 animDelay;
	u8 animSpeed;
	u8 totalFrames;
	struct
	{
		bool active;
		bool pingpong;
		bool is4bit;
		bool direction;
	} flags;

} object;

extern object objects[];

void InitializeObjects(void);
void ResetObjects(void);
void RenderObjects(void);
void AnimateObject(object *pObj);

#endif
