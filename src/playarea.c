#include "kftypes.h"
#include "defines.h"
#include "playarea.h"
#include "utilities.h"
#include "memorymap.h"
#include "tilemap.h"
#include "hardware.h"
#include "objects/components.h"
#include "objects/coin.h"
#include "level_manager.h"

play_area playArea;

//---------------------------------------------------------
void ClearPlayArea(void)
{
	memset(playArea.cells, 0, sizeof(playArea.cells));
	ClearTilemap();
}

//---------------------------------------------------------
s8 CalcIndex(const play_cell *pCell)
{
	if (pCell->type==CELL_HOLE)
		return 0;
	if (pCell->isDark)
		return 2;
	return 1;
}

//---------------------------------------------------------
void BuildPlayArea(const play_area_template *pTemplate)
{
	s8 px = -pTemplate->size.x/2;
	s8 py = -pTemplate->size.y/2;
	const u8 *pData=pTemplate->data;
	play_cell *pCell=NULL;

	levelManager.tilesRemaining = 0;
	for (u8 y=0; y<pTemplate->size.y; y++)
	{
		coord_s8 mapPosition = {px, py+y};
		
		pCell=GetPlayAreaCell(&mapPosition);
		for (u8 x=0; x<pTemplate->size.x; x++, pCell++, pData++)
		{
			u8 type = *pData;
			switch(type)
			{
				case 0:		// A hole
					pCell->type = CELL_HOLE;
					break;
				case 1:		// A tile
					pCell->type = CELL_TILE;
					levelManager.tilesRemaining++;
					break;
				case 2:		// An obstacle?
					pCell->type = CELL_OBSTACLE;
					break;
				default:
					x_printf("Unknown cell type %d\n", (u16)type);
					pCell->type = CELL_HOLE;
					break;
			}
		}
	}
}

//---------------------------------------------------------
void SnapToPlayAreaGrid(game_object* pObject)
{
    s16 x = pObject->playGrid.x - playArea.position.x;
    s16 y = pObject->playGrid.y - playArea.position.y;

    s16 sx = (x+y)*16;
    s16 sy = (y-x)*24;

    pObject->trans.pos.x = I_TO_F(sx);
    pObject->trans.pos.y = I_TO_F(sy);
}

//---------------------------------------------------------
void RefreshBlock(const coord_s8* mapPosition, s8 palette)
{
	s8 x = mapPosition->x;
	s8 y = mapPosition->y;
	s8 tx = (x+y)*2;
	s8 ty = (y-x)*3;
	tilemap_cell *pTilemap = GetTilemapCell(tx, ty);
	const coord_s8 blockmapPos = {playArea.position.x+x, playArea.position.y+y};
	const play_cell *pCell = GetPlayAreaCell(&blockmapPos);
	tilemap_paste_params params;
	params.dark = pCell->isDark;
	params.bl = CalcIndex(pCell-1);
	params.tr = CalcIndex(pCell+1);
	params.tl = CalcIndex(pCell-PLAY_AREA_CELLS_WIDTH);
	params.br = CalcIndex(pCell+PLAY_AREA_CELLS_WIDTH);
	params.palette = palette;
	if (pCell->type!=CELL_HOLE)
	{
		PasteTilemapBlock(pTilemap, &params);
	}
}

//---------------------------------------------------------
void RefreshPlayAreaCell(const coord_s8* mapPosition, u8 palette)
{
	nextreg(MMU_SLOT_6, MISC_DATA_PAGE);
	nextreg(MMU_SLOT_7, VIRTUAL_TILEMAP_PAGE);
	RefreshBlock(mapPosition, palette);
}

//---------------------------------------------------------
void DrawPlayArea(const play_area_template* template)
{
	nextreg(MMU_SLOT_6, MISC_DATA_PAGE);
	nextreg(MMU_SLOT_7, VIRTUAL_TILEMAP_PAGE);
	//
	// Must be zero for initial full play area draw
	//
	playArea.position.x = 0;
	playArea.position.y = 0;
	u8 width = template->size.x;
	u8 height = template->size.y;
	s8 sx = (s8)width/2;
	s8 sy = (s8)height/2;
	coord_s8 mapPosition = {0, -sy};
	for (u8 i=0; i<height; i++, mapPosition.y++)
	{
		mapPosition.x = -sx;
		for (u8 j=0; j<width; j++, mapPosition.x++)
		{
			RefreshBlock(&mapPosition, 0);
		}
	}
}

//---------------------------------------------------------
play_cell *GetPlayAreaCell(const coord_s8* position)
{
	s8 x = position->x+PLAY_AREA_CELLS_WIDTH/2;
	s8 y = position->y+PLAY_AREA_CELLS_HEIGHT/2;

	//x_printf("x:%d,y:%d,cell:0x%x,t:%x\n",(s16)x, (s16)y, &playArea.cells[y][x], (s16)playArea.cells[y][x].type);
	return &playArea.cells[y][x];
}
