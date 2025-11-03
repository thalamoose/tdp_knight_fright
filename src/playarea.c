#include "kftypes.h"
#include "globals.h"
#include "playarea.h"
#include "utilities.h"


void ClearPlayArea(void)
{
	memset(global.playArea.cells,0,sizeof(global.playArea.cells));
}

void InitializePlayArea(play_area_template* pTemplate)
{
	ClearPlayArea();
	u8* pData = pTemplate->data;

	global.playArea.position.x = PLAY_AREA_CELLS_WIDTH/2;
	global.playArea.position.y = PLAY_AREA_CELLS_HEIGHT/2;
	s16 gx = global.playArea.position.x-pTemplate->width/2;
	s16 gy = global.playArea.position.y-pTemplate->height/2+2;

	x_printf("gx:%d, gy:%d\n", gx, gy);
	play_cell* pCell = &global.playArea.cells[gy][gx];
	for (u8 y=0; y<pTemplate->height; y++)
	{
		for( u8 x=0; x<pTemplate->width; x++)
		{
			pCell->blockContent = *pData;
			pData++;
			pCell++;
		}
		pCell += PLAY_AREA_CELLS_WIDTH-pTemplate->width;
	}
}

u8 GetPlayAreaContent(u8 x, u8 y)
{
	return global.playArea.cells[y][x].blockContent;
}
