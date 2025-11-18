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

typedef struct s_tilemap_cell
{
	u8 value;
	u8 attr;
} tilemap_cell;

void InitializeTilemap(void);
void ResetTilemap(void);
void UpdateTilemap(void);
void ClearTilemap(void);
void PasteTilemapBlock(tilemap_cell* pTile, s8 dark, s8 tl, s8 tr, s8 bl, s8 br, s8 palette);
tilemap_cell* GetTilemapCell(s8 x, s8 y);

#endif

