#if !defined(__TILEMAP_H)
#define __TILEMAP_H

typedef struct s_tile_map
{
	coord position;
	coord velocity;
	coord moveTarget;
	coord lastPlayGrid;
	coord lastTilemapPos;
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

typedef struct s_paste_params
{
	s8 dark;
	s8 tl;
	s8 tr;
	s8 bl;
	s8 br;
	s8 palette;
} tilemap_paste_params;

void InitializeTilemap(void);
void ResetTilemap(void);
void UpdateTilemap(void);
void RenderTilemap(void);
void ClearTilemap(void);
void PasteTilemapBlock(tilemap_cell *pTile, const tilemap_paste_params* params);
void SetTilemapMoveTarget(void);
tilemap_cell *GetTilemapCell(s8 x, s8 y);

extern tile_map tileMap;

#endif
