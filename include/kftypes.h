#if !defined(_KFTYPES_H)
#define _KFTYPES_H
#include <stdbool.h>

typedef unsigned char u8;
typedef unsigned short u16;
typedef signed char s8;
typedef signed short s16;

#define NULL ((void *)0)

enum fixed_point
{
	FIXED_POINT_BITS = 6,
	FIXED_POINT_ONE = (1 << FIXED_POINT_BITS),
	FIXED_POINT_HALF = (FIXED_POINT_ONE / 2),
};

#define I_TO_F(x) ((x) << FIXED_POINT_BITS)
#define F_TO_I(x) ((x) >> FIXED_POINT_BITS)

typedef struct s_coord
{
	s16 x;
	s16 y;
} coord;

typedef struct s_coord_s8
{
	s8 x;
	s8 y;
} coord_s8;

#endif
