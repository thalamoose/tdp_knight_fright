#if !defined(__PICKUPS_H)
#define __PICKUPS_H
#include "objects.h"

typedef struct s_pickup
{
	object object;
	sprite sprite;
	coord_s8 playmapCell;
	u8 type;
} pickup_t;

typedef struct s_pickups
{
	u8 itemIndex;
	pickup_t item[MAX_PICKUPS];
} pickups_t;

void InitializePickups(void);
void ResetPickups(void);

pickup_t* AddPickup(u8 type, s8 x, s8 y);
void RemovePickup(pickup_t* pPickup);
void UpdatePickups(void);
void RenderPickups(void);

#endif