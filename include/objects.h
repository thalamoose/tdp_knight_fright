#if !defined(__OBJECT_H)
#define __OBJECT_H
#include "kftypes.h"
enum fixed_point
{
	FIXED_POINT_BITS=6,
	FIXED_POINT_ONE=1<<FIXED_POINT_BITS,
	FIXED_POINT_HALF=FIXED_POINT_ONE/2,
};

typedef struct s_object
{
	struct
	{
		s16 x;
		s16 y;
	} position;
	struct
	{
		s16 x;
		s16 y;
	} velocity;
    s16 gravity;
    u8 frameIndex;
    u8 baseIndex;
    u8 animDelay;
    u8 animSpeed;
    u8 totalFrames;
	u8 direction;
} object;

void UpdateObjects(void);


#endif
