#include "kftypes.h"
#if !defined(__TILEMAP_H)
#define __TILEMAP_H

typedef struct s_tile_map
{
	s8 x;
	s8 y;
	s8 play_x;
	s8 play_y;
} tile_map;

typedef struct s_tile
{
	u8 w;
	u8 h;
	u8 data[0];
} tile_template;

extern void InitializeTilemap(void);
extern void UpdateTilemap(void);
#endif

