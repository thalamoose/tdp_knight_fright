#include "objects/object_manager.h"

enum
{
	CELL_HOLE,
	CELL_TILE,
	CELL_OBSTACLE,
	CELL_ENEMY,
	CELL_COIN,
	CELL_SPIKE,
};

typedef struct s_play_cell
{
	u8 type:3;
	u8 objIndex:5;
	bool isDark;
} play_cell;

typedef struct s_play_area
{
	coord_s8 position;
	coord_s8 start;
	coord_s8 activeSize;
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

play_cell *GetPlayAreaCell(s8 x, s8 y);
void DrawPlayArea(s8 w, s8 h);
void RefreshPlayAreaBlock(s8 x, s8 y, s8 palette);
void SnapToPlayAreaGrid(object* pObject);

extern play_area playArea;
