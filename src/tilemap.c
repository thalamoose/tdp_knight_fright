#include "kftypes.h"
#include "defines.h"
#include "memorymap.h"
#include "hardware.h"
#include "utilities.h"
#include "tilemap.h"
#include "assets.h"
#include "playarea.h"
#include "objects/player.h"
#include "hud.h"
#include "input.h"
#include "level_manager.h"

tile_map tileMap;
//---------------------------------------------------------
void InitializeTilemap(void)
{
	// normally, colour 0 is transparent but the test image has it as colour 1
	// ULA has already been mapped to it's alternate pages at 14,15. Tilemap can
	// use all of pages 10,11.

	nextreg(TRANS_TILEMAP_INDEX, 0);
	nextreg(TILEMAP_CONTROL, 0x80);
	nextreg(TILEMAP_CHAR_ADDRESS, 0); // Start character data right after tilemap
	nextreg(TILEMAP_BASE_ADDRESS, 0x2000 >> 8);

	//
	// Copy tilemap character data to tilemap area
	//
	nextreg(SWAP_BANK_PAGE_0, TILES_PAGE);
	nextreg(SWAP_BANK_PAGE_1, TILEMAP_CHARS_PAGE);
	// Copy tile bitmap data
	memcpy_dma((void*)SWAP_BANK_1, (void*)SWAP_BANK_0, 0x2000);
	//
	// This is copying characters 1...255 to 0...254 (test purposes). We need character 0 to be blank.
	//
	memcpy_dma((u8*)SWAP_BANK_1+32, (u8*)SWAP_BANK_0, 0x1fe0);
	// Make the first character 0
	memset((u8*)SWAP_BANK_1, 0, 32);

	nextreg(SWAP_BANK_PAGE_0, MISC_DATA_PAGE);
	// Copy slot 0 palette to slot 1, this will be used for pulsing
	// colours on a block switch.
	memcpy_dma(asset_TilemapPalette+16, asset_TilemapPalette, 16*sizeof(u16));
	CopyPalette(asset_TilemapPalette, PALETTE_TILE_PRIMARY);

	nextreg(SWAP_BANK_PAGE_1, VIRTUAL_TILEMAP_PAGE);
	ClearTilemap();

	// Tilemap templates are in the same bank as the palette
	playArea.position.x = 0;
	playArea.position.y = 0;
	tileMap.position.x = 0;
	tileMap.position.y = 0;
	tileMap.lastTilemapPos.x = -1;
	tileMap.lastTilemapPos.y = -1;
	SetTilemapMoveTarget();
	// Just clip the entire tilemap. It'll get reset after the first update.
	nextreg(TILEMAP_CLIP_WINDOW, 255);
	nextreg(TILEMAP_CLIP_WINDOW, 255);
	nextreg(TILEMAP_CLIP_WINDOW, 255);
	nextreg(TILEMAP_CLIP_WINDOW, 255);
}

//---------------------------------------------------------
void ResetTilemap(void)
{
	InitializeTilemap();

}

//---------------------------------------------------------
void ClearTilemap(void)
{
	u8 *pTileTable = (u8*)SWAP_BANK_1;
	pTileTable[0] = 0x00;
	pTileTable[1] = 0x01;
	// Since I know this copies forward, I can copy the first 2 bytes
	// to the rest with a DMA memcpy. It's a lot faster.
	u16 szTilemap = VIRTUAL_TILEMAP_WIDTH*VIRTUAL_TILEMAP_HEIGHT*sizeof(tilemap_cell);
	memcpy_dma(pTileTable+2, pTileTable, szTilemap-sizeof(tilemap_cell));
}

//---------------------------------------------------------
tilemap_cell *GetTilemapCell(s8 x, s8 y)
{
	x += VIRTUAL_TILEMAP_WIDTH/2;
	y += VIRTUAL_TILEMAP_HEIGHT/2;
	if ((x<0) || (x>=VIRTUAL_TILEMAP_WIDTH))
		return NULL;
	if ((y<0) || (y>=VIRTUAL_TILEMAP_HEIGHT))
		return NULL;
	return (tilemap_cell*)SWAP_BANK_1+(y*VIRTUAL_TILEMAP_WIDTH)+x;
}

//---------------------------------------------------------
void SetTilemapMoveTarget(void)
{
	tileMap.lastPlayGrid.x=levelManager.player->playGrid.x;
	tileMap.lastPlayGrid.y=levelManager.player->playGrid.y;

	s16 px=playArea.position.x-levelManager.player->playGrid.x;
	s16 py=playArea.position.y-levelManager.player->playGrid.y;

	s16 sx=(px+py)*16-22;
	s16 sy=(py-px)*24+16;

	tileMap.moveTarget.x=I_TO_F(sx);
	tileMap.moveTarget.y=I_TO_F(sy);
}

//---------------------------------------------------------
void UpdateTilemap(void)
{
	u8 buttons=ReadController();
	if (buttons & JOYPAD_R_DOWN)
	{
		tileMap.position.y += FIXED_POINT_ONE*2;
	}
	if (buttons & JOYPAD_R_UP)
	{
		tileMap.position.y -= FIXED_POINT_ONE*2;
	}
	if (buttons & JOYPAD_R_RIGHT)
	{
		tileMap.position.x += FIXED_POINT_ONE*2;
	}
	if (buttons & JOYPAD_R_LEFT)
	{
		tileMap.position.x -= FIXED_POINT_ONE*2;
	}

	s8 dgx=levelManager.player->playGrid.x-tileMap.lastPlayGrid.x;
	s8 dgy=levelManager.player->playGrid.y-tileMap.lastPlayGrid.y;
	if (dgx<-1 || dgx>1 || dgy<-1 || dgy>1)
	{
		SetTilemapMoveTarget();
	}

	tileMap.velocity.x=(tileMap.moveTarget.x-tileMap.position.x)/16;
	tileMap.velocity.y=(tileMap.moveTarget.y-tileMap.position.y)/16;
	tileMap.position.x += tileMap.velocity.x;
	tileMap.position.y += tileMap.velocity.y;
}

//---------------------------------------------------------
void BlitTilemap(tilemap_cell* src, tilemap_cell* dst)
{
	DebugTiming(ULA_COLOUR_MAGENTA);
	for (u8 i=0; i<TILEMAP_CHAR_HEIGHT; i++)
	{
		memcpy_dma(dst, src, TILEMAP_CHAR_WIDTH*sizeof(tilemap_cell));
		src += VIRTUAL_TILEMAP_WIDTH;
		dst += TILEMAP_CHAR_WIDTH;
	}
	DebugTiming(ULA_COLOUR_BLACK);
}

//---------------------------------------------------------
void RenderTilemap(void)
{
	s16 x=F_TO_I(tileMap.position.x)+hud.shake.x+2;
	s16 y=F_TO_I(tileMap.position.y)+hud.shake.y;
	if (x==tileMap.lastTilemapPos.x && y==tileMap.lastTilemapPos.y)
	{
		return;
	}
	tileMap.lastTilemapPos.x = x;
	tileMap.lastTilemapPos.y = y;
	s16 lClip=x+(TILEMAP_PIX_WIDTH-VIRTUAL_TILEMAP_PIX_WIDTH)/2;
	s16 rClip=lClip+VIRTUAL_TILEMAP_PIX_WIDTH-1;
	s16 tClip=y+(TILEMAP_PIX_HEIGHT-VIRTUAL_TILEMAP_PIX_HEIGHT)/2;
	s16 bClip=tClip+VIRTUAL_TILEMAP_PIX_HEIGHT-1;
	lClip = MAX(lClip, TILEMAP_SAFE_AREA);
	rClip = MIN(rClip, TILEMAP_PIX_WIDTH-TILEMAP_SAFE_AREA-1);
	tClip = MAX(tClip,TILEMAP_SAFE_AREA);
	bClip = MIN(bClip, TILEMAP_PIX_HEIGHT-TILEMAP_SAFE_AREA-1);
	s16 sx = -x;
	s16 sy = -y;
	nextreg(TILEMAP_OFFSET_X_H, 0);
	nextreg(TILEMAP_OFFSET_X_L, sx&7);
	nextreg(TILEMAP_OFFSET_Y, sy&7);
	nextreg(TILEMAP_CLIP_WINDOW, lClip>>1);
	nextreg(TILEMAP_CLIP_WINDOW, rClip>>1);
	nextreg(TILEMAP_CLIP_WINDOW, tClip);
	nextreg(TILEMAP_CLIP_WINDOW, bClip);
	nextreg(SWAP_BANK_PAGE_0, TILEMAP_PAGE);
	nextreg(SWAP_BANK_PAGE_1, VIRTUAL_TILEMAP_PAGE);
	s16 tmapx=(sx>>3)+4;
	s16 tmapy=(sy>>3)+8;
	tilemap_cell* pCell = ((tilemap_cell*)SWAP_BANK_1)+tmapx+(tmapy*VIRTUAL_TILEMAP_WIDTH);
	BlitTilemap(pCell, (tilemap_cell*)SWAP_BANK_0);
}

//---------------------------------------------------------
void PasteTilemapEdge(tilemap_cell *pTile, u8 baseBlock, u8 attr, u8 l, u8 r)
{
	u8 blk=baseBlock+l*4;

	pTile->value=blk++;
	pTile->attr=attr;
	pTile++;
	pTile->value=blk;
	pTile->attr=attr;
	pTile++;
	blk=baseBlock+r*4+2;
	// TR
	pTile->value=blk++;
	pTile->attr=attr;
	pTile++;
	pTile->value=blk;
	pTile->attr=attr;
}

//---------------------------------------------------------
static void PasteTilemapMiddle(tilemap_cell *pTile, u8 baseBlock, u8 attr)
{
	for (u8 i=0; i<4; i++)
	{
		pTile->value=baseBlock++;
		pTile->attr=attr;
		pTile++;
	}
}

//---------------------------------------------------------
void PasteTilemapBlock(tilemap_cell *pTile, const paste_tilemap_params* params)
{
	// The +1 is because block 0 is the blank block. We're leaving that for now. But we'll
	// sort it out when the tilemap is properly laid out.
	tileMap.lastTilemapPos.x = -1;		// Invalidate tilemap so it'll get updated
	u8 attr=(params->palette << 4) | 1;
	u8 darkOffset=params->dark*12+1;
	PasteTilemapEdge(pTile, darkOffset, attr, params->tl, params->tr);
	PasteTilemapMiddle(pTile+VIRTUAL_TILEMAP_WIDTH, darkOffset+24, attr);
	PasteTilemapMiddle(pTile+VIRTUAL_TILEMAP_WIDTH*2, darkOffset+48, attr);
	PasteTilemapEdge(pTile+VIRTUAL_TILEMAP_WIDTH*3, darkOffset+72, 1, params->bl, params->br);
}
