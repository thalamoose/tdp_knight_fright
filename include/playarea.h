#if !defined(__PLAY_AREA_H)
#define __PLAY_AREA_H
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
	coord_u8 size;
	coord_s8 start;
	u8 data[0];
} play_area_template;

void deprecatedInitializePlayArea(const play_area_template *pTemplate);

play_cell *GetPlayAreaCell(const coord_s8* mapPosition);
void ClearPlayArea(void);
void DrawPlayArea(const play_area_template* template);
void BuildPlayArea(const play_area_template* template);
void RefreshPlayAreaCell(const coord_s8* mapPosition, u8 palette);
void SnapToPlayAreaGrid(game_object* pObject);

extern play_area playArea;
#endif
