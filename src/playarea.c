#include "kftypes.h"
#include "globals.h"
#include "playarea.h"
#include "utilities.h"
#include "memorymap.h"
#include "tilemap.h"
#include "hardware.h"


//---------------------------------------------------------
void ClearPlayArea(void)
{
	memset(global.playArea.cells, 0, sizeof(global.playArea.cells));
}

//---------------------------------------------------------
s8 CalcIndex(const play_cell* pCell)
{
	if (pCell->type==0)
		return 0;
	if (pCell->dark)
		return 2;
	return 1;
}

//---------------------------------------------------------
void BuildPlayArea(const play_area_template* pTemplate)
{
	s8 px = -pTemplate->width/2;
	s8 py = -pTemplate->height/2;

	const u8* pData = pTemplate->data;
	for (u8 y=0; y<pTemplate->height; y++)
	{
		play_cell* pCell = GetPlayAreaCell(px, py+y);
		for (u8 x=0; x<pTemplate->width; x++)
		{
			pCell->type = *pData++;
			pCell++;
		}
	}
}

//---------------------------------------------------------
void RefreshPlayAreaBlock(s8 x, s8 y)
{
	nextreg(MMU_SLOT_6, TILEMAP_PAGE+1);
	s16 tx = (x+y)*2;
	s16 ty = (y-x)*3;
	tilemap_cell* pTilemap = GetTilemapCell(tx, ty);
	const play_cell* pCell = GetPlayAreaCell(global.playArea.position.x+x, global.playArea.position.y+y);
	if (pTilemap)
	{
		if (pCell->type)
		{
			s8 dark = pCell->dark;
			s8 bl = CalcIndex(pCell-1);
			s8 tr = CalcIndex(pCell+1);
			s8 tl = CalcIndex(pCell-PLAY_AREA_CELLS_WIDTH);
			s8 br = CalcIndex(pCell+PLAY_AREA_CELLS_WIDTH);
			PasteTilemapBlock(pTilemap, dark, tl, tr, bl, br);
		}
	}
}

//---------------------------------------------------------
void DrawPlayArea(void)
{
	nextreg(MMU_SLOT_6, TILEMAP_PAGE+1);

	for (s16 y=-4; y<5; y++)
	{
		//x_printf("cell:0x%x\n", pCell);
		for(s16 x=-4; x<5; x++)
		{
			RefreshPlayAreaBlock(x, y);
		}
	}
}

//---------------------------------------------------------
void InitializePlayArea(const play_area_template* pTemplate)
{
	global.playArea.position.x = 0;
	global.playArea.position.y = 0;
	ClearPlayArea();
	nextreg(MMU_SLOT_6, PALETTE_PAGE);
	BuildPlayArea(pTemplate);
	DrawPlayArea();
}

//---------------------------------------------------------
play_cell* GetPlayAreaCell(s8 x, s8 y)
{
	x = x+PLAY_AREA_CELLS_WIDTH/2;
	y = y+PLAY_AREA_CELLS_HEIGHT/2;

	return &global.playArea.cells[y][x];
}
