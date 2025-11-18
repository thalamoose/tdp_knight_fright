#include "kftypes.h"
#include "memorymap.h"
#include "hardware.h"
#include "utilities.h"
#include "defines.h"
#include "tilemap.h"
#include "assets.h"
#include "globals.h"
#include "playarea.h"
#include "hud.h"
#include "input.h"

//---------------------------------------------------------
void InitializeTilemap(void)
{
	// normally, colour 0 is transparent but the test image has it as colour 1
	// ULA has already been mapped to it's alternate pages at 14,15. Tilemap can
	// use all of pages 10,11.

	nextreg(TRANS_TILEMAP_INDEX,0);
	nextreg(TILEMAP_CONTROL,0x80);
	nextreg(TILEMAP_CHAR_ADDRESS,0);     // Start character data right after tilemap
	nextreg(TILEMAP_BASE_ADDRESS,0x2000>>8);
	
	//
	// Copy tilemap character data to tilemap area
	//
	nextreg(MMU_SLOT_6,TILES_PAGE);
	nextreg(MMU_SLOT_7,TILEMAP_PAGE);
	// Copy tile bitmap data
	memcpy_dma(SWAP_BANK_1, SWAP_BANK_0, 0x2000);
	//
	// This is copying characters 1...255 to 0...254 (test purposes). We need character 0 to be blank.
	//
	memcpy_dma(SWAP_BANK_1+32,SWAP_BANK_0,0x1fe0);
	// Make the first character 0
	memset(SWAP_BANK_1,0,32);

	nextreg(MMU_SLOT_6, PALETTE_PAGE);
	// Copy slot 0 palette to slot 1, this will be used for pulsing
	// colours on a block switch.
	memcpy_dma(asset_TilemapPalette+16, asset_TilemapPalette, 16*sizeof(u16));
	CopyPalette(asset_TilemapPalette, PALETTE_TILE_PRIMARY);

	nextreg(MMU_SLOT_6, TILEMAP_PAGE+1);
	ClearTilemap();

	// Tilemap templates are in the same bank as the palette
	playArea.position.x = 0;
	playArea.position.y = 0;
	global.tileMap.x = -24;
	global.tileMap.y = 32;
	// Just clip the entire tilemap. It'll get reset after the first update.
	nextreg(TILEMAP_CLIP_WINDOW, 255);
	nextreg(TILEMAP_CLIP_WINDOW, 255);
	nextreg(TILEMAP_CLIP_WINDOW, 255);
	nextreg(TILEMAP_CLIP_WINDOW, 255);
}

void ResetTilemap(void)
{
	InitializeTilemap();
}

//---------------------------------------------------------
void ClearTilemap(void)
{
	u8* pTileTable = SWAP_BANK_0;
	pTileTable[0]=0x00;
	pTileTable[1]=0x01;
	// Since I know this copies forward, I can copy the first 2 bytes
	// to the rest with a DMA memcpy. It's a lot faster.
	memcpy_dma(pTileTable+2, pTileTable, TILEMAP_CHAR_WIDTH*TILEMAP_CHAR_HEIGHT*sizeof(tilemap_cell)-2);
}

//---------------------------------------------------------
tilemap_cell* GetTilemapCell(s8 x, s8 y)
{
	x += TILEMAP_CHAR_WIDTH/2;
	y += TILEMAP_CHAR_HEIGHT/2;
	if ((x<0) || (x>=TILEMAP_CHAR_WIDTH))
		return NULL;
	if ((y<0) || (y>=TILEMAP_CHAR_HEIGHT))
		return NULL;
	return (tilemap_cell*)SWAP_BANK_0+(y*TILEMAP_CHAR_WIDTH+x);

}

s16 lastL;
s16 lastT;
s16 lastR;
s16 lastB;
u8 c2;
//---------------------------------------------------------
void UpdateTilemap(void)
{
	s16 x = global.tileMap.x+hud.shake.x;
	s16 y = global.tileMap.y+hud.shake.y;
	s16 lClip = x;
	s16 rClip = lClip+319;
	s16 tClip = y;
	s16 bClip = tClip+255;
	if (lClip<0) lClip = 0;
	if (rClip>319) rClip = 319;
	if (tClip<0) tClip = 0;
	if (bClip>255) bClip=255;
#if false
	if (lClip!=lastL || tClip!=lastT || rClip!=lastR || bClip!=lastB)
	{
		lastL = lClip;
		lastT = tClip;
		lastR = rClip;
		lastB = bClip;
		x_printf("x:%d,y:%d,l:%d,r:%d,t:%d,b:%d\n", x, y, lClip, rClip, tClip, bClip);
	}
#endif
	x = -x;
	y = -y;
	if (x<0) x=320+x;
	if (y<0) y=256+y;
	nextreg(TILEMAP_OFFSET_X_H,(x>>8) & 0x1);
	nextreg(TILEMAP_OFFSET_X_L, x);
	nextreg(TILEMAP_OFFSET_Y, y);
	nextreg(TILEMAP_CLIP_WINDOW, lClip>>1);
	nextreg(TILEMAP_CLIP_WINDOW, rClip>>1);
	nextreg(TILEMAP_CLIP_WINDOW, tClip);
	nextreg(TILEMAP_CLIP_WINDOW, bClip);
#if 0
	c2++;
	if (c2>1)
	{
		u8 buttons = ReadController();
		if (buttons & (1<<JOYPAD_R_LEFT))
		{
			global.tileMap.x--;
			if (global.tileMap.x<=-320) global.tileMap.x = 320;
		}
		if (buttons & (1<<JOYPAD_R_RIGHT))
		{
			global.tileMap.x++;
			if (global.tileMap.x>=320) global.tileMap.x = -320;
		}
		if (buttons & (1<<JOYPAD_R_UP))
		{
			global.tileMap.y--;
			if (global.tileMap.y<=-256) global.tileMap.y = 256;
		}
		if (buttons & (1<<JOYPAD_R_DOWN))
		{
			global.tileMap.y++;
			if (global.tileMap.y>=256) global.tileMap.y = -256;
		}

		c2=0;
	}
#endif
}

//---------------------------------------------------------
void PasteTilemapEdge(tilemap_cell* pTile, u8 baseBlock, u8 attr, u8 l, u8 r)
{
	u8 blk = baseBlock+l*4;

	pTile->value = blk++;
	pTile->attr = attr;
	pTile++;
	pTile->value = blk;
	pTile->attr = attr;
	pTile++;
	blk = baseBlock+r*4+2;
	// TR
	pTile->value = blk++;
	pTile->attr = attr;
	pTile++;
	pTile->value = blk;
	pTile->attr = attr;
}

//---------------------------------------------------------
static void PasteTilemapMiddle(tilemap_cell* pTile, u8 baseBlock, u8 attr)
{
	for(u8 i=0; i<4; i++)
	{
		pTile->value = baseBlock++;
		pTile->attr = attr;
		pTile++;
	}
}

//---------------------------------------------------------
void PasteTilemapBlock(tilemap_cell* pTile, s8 dark, s8 tl, s8 tr, s8 bl, s8 br, s8 palette)
{
	// The +1 is because block 0 is the blank block. We're leaving that for now. But we'll
	// sort it out when the tilemap is properly laid out.
	u8 attr = (palette<<4)|1;
	u8 darkOffset = dark*12+1;
	PasteTilemapEdge(pTile, darkOffset, attr, tl, tr);
	PasteTilemapMiddle(pTile+TILEMAP_CHAR_WIDTH, darkOffset+24, attr);
	PasteTilemapMiddle(pTile+TILEMAP_CHAR_WIDTH*2, darkOffset+48, attr);
	PasteTilemapEdge(pTile+TILEMAP_CHAR_WIDTH*3, darkOffset+72, 1, bl, br);
}

