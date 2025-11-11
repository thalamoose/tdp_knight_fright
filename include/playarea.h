#if !defined(__PLAYAREA_H)
#define __PLAYAREA_H
#include "kftypes.h"
#include "objects.h"

typedef struct s_play_cell
{
	unsigned char type:2;
	unsigned char dark:1;
} play_cell;

typedef struct s_play_area
{
	coord position;
	play_cell cells[PLAY_AREA_CELLS_HEIGHT][PLAY_AREA_CELLS_WIDTH];
} play_area;

typedef struct s_play_area_template
{
	u8 width;
	u8 height;
	u8 data[0];
} play_area_template;

void InitializePlayArea(const play_area_template* pTemplate);

u8 GetPlayAreaContent(s8 x, s8 y);
play_cell* GetPlayAreaCell(s8 x, s8 y);
void DrawPlayArea(void);
void RefreshPlayAreaBlock(s8 x, s8 y, s8 palette);


#endif
