#if !defined(__PLAYAREA_H)
#define __PLAYAREA_H
#include "objects.h"

enum
{
	CELL_TILE,
	CELL_OBSTACLE,
	CELL_ENEMY,
	CELL_COIN,
	CELL_HOLE,
	CELL_SPIKE,
};

typedef struct s_play_cell
{
	u8 type:3;
	u8 objIndex:3;
	bool isStartingPosition:1;
	bool isDark:1;
} play_cell;

typedef struct s_play_area
{
	coord_s8 position;
	coord_s8 start;
	u8 tilesToFlip;
	play_cell cells[PLAY_AREA_CELLS_HEIGHT][PLAY_AREA_CELLS_WIDTH];
} play_area;

typedef struct s_play_area_template
{
	coord_s8 size;
	coord_s8 start;
	u8 data[0];
} play_area_template;

void InitializePlayArea(const play_area_template *pTemplate);

u8 GetPlayAreaContent(s8 x, s8 y);
play_cell *GetPlayAreaCell(s8 x, s8 y);
void DrawPlayArea(s8 w, s8 h);
void RefreshPlayAreaBlock(s8 x, s8 y, s8 palette);
void SnapToPlayAreaGrid(object* pObject);


extern play_area playArea;

#endif
