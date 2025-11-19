#if !defined(__OBJECT_H)
#define __OBJECT_H
#include "kftypes.h"

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
		u8 active : 1;
		u8 pingpong : 1;
		u8 is4bit : 1;
		u8 direction : 1;
	} flags;

} object;

extern object objects[];

void InitializeObjects(void);
void ResetObjects(void);
void RenderObjects(void);
void UpdateObjects(void);
void AnimateObject(object *pObj);

#endif
