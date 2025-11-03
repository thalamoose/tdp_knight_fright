#if !defined(__PLAYAREA_H)
#define __PLAYAREA_H
#include "kftypes.h"
#include "objects.h"

typedef struct s_play_cell
{
	u8 blockContent;
} play_cell;

typedef struct s_play_area
{
	coord position;
	play_cell cells[PLAY_AREA_CELLS_WIDTH][PLAY_AREA_CELLS_HEIGHT];
} play_area;

typedef struct s_play_area_template
{
	u8 width;
	u8 height;
	u8 data[0];
} play_area_template;

void InitializePlayArea(play_area_template* pTemplate);

u8 GetPlayAreaContent(u8 x, u8 y);
#endif
