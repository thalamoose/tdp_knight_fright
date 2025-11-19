#include "kftypes.h"
#include "globals.h"
#include "playarea.h"
#include "utilities.h"
#include "memorymap.h"
#include "tilemap.h"
#include "hardware.h"

play_area playArea;

//---------------------------------------------------------
void ClearPlayArea(void)
{
	memset(playArea.cells, 0, sizeof(playArea.cells));
}

//---------------------------------------------------------
s8 CalcIndex(const play_cell *pCell)
{
	if (pCell->type == 0)
		return 0;
	if (pCell->dark)
		return 2;
	return 1;
}

//---------------------------------------------------------
void BuildPlayArea(const play_area_template *pTemplate)
{
	s8 px = -pTemplate->size.x / 2;
	s8 py = -pTemplate->size.y / 2;
	const u8 *pData = pTemplate->data;
	play_cell *pCell = NULL;
	playArea.tilesToFlip = 0;
	for (s8 y = 0; y < pTemplate->size.y; y++)
	{
		pCell = GetPlayAreaCell(px, py + y);
		for (s8 x = 0; x < pTemplate->size.x; x++)
		{
			pCell->type = *pData++;
			// If the cell is not empty, then it must be flippable.
			// This should be expanded to handle the case where the
			// tile block is fatal.
			if (pCell->type)
				playArea.tilesToFlip++;
			pCell++;
		}
	}
}

//---------------------------------------------------------
void RefreshPlayAreaBlock(s8 x, s8 y, s8 palette)
{
	nextreg(MMU_SLOT_6, TILEMAP_PAGE + 1);
	s16 tx = (x + y) * 2;
	s16 ty = (y - x) * 3;
	tilemap_cell *pTilemap = GetTilemapCell(tx, ty);
	const play_cell *pCell = GetPlayAreaCell(playArea.position.x + x, playArea.position.y + y);
	s8 dark = pCell->dark;
	s8 bl = CalcIndex(pCell - 1);
	s8 tr = CalcIndex(pCell + 1);
	s8 tl = CalcIndex(pCell - PLAY_AREA_CELLS_WIDTH);
	s8 br = CalcIndex(pCell + PLAY_AREA_CELLS_WIDTH);
	if (pTilemap)
	{
		if (pCell->type)
		{
			PasteTilemapBlock(pTilemap, dark, tl, tr, bl, br, palette);
		}
	}
}

//---------------------------------------------------------
void DrawPlayArea(s8 w, s8 h)
{
	nextreg(MMU_SLOT_6, TILEMAP_PAGE + 1);

	w = (w + 1) / 2;
	h = (h + 1) / 2;
	for (s16 y = -h; y < h; y++)
	{
		for (s16 x = -w; x < w; x++)
		{
			RefreshPlayAreaBlock(x, y, 0);
		}
	}
}

//---------------------------------------------------------
void InitializePlayArea(const play_area_template *pTemplate)
{
	playArea.position.x = 0;
	playArea.position.y = 0;
	playArea.start.x = pTemplate->start.x;
	playArea.start.y = pTemplate->start.y;
	ClearPlayArea();
	nextreg(MMU_SLOT_6, PALETTE_PAGE);
	BuildPlayArea(pTemplate);
	DrawPlayArea(pTemplate->size.x, pTemplate->size.y);
}

//---------------------------------------------------------
play_cell *GetPlayAreaCell(s8 x, s8 y)
{
	x = x + PLAY_AREA_CELLS_WIDTH / 2;
	y = y + PLAY_AREA_CELLS_HEIGHT / 2;

	// x_printf("x:%d,y:%d, cell:0x%x,pa:%x\n",(s16)x, (s16)y,&playArea.cells[y][x],playArea.cells);
	return &playArea.cells[y][x];
}
