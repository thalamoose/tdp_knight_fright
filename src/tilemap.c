#include "kftypes.h"
#include "memorymap.h"
#include "hardware.h"
#include "utilities.h"
#include "defines.h"
#include "tilemap.h"
#include "assets.h"
#include "globals.h"
#include "playarea.h"

tile_template _asset_MapShape_02;
tile_template _asset_MapShape_01;

//---------------------------------------------------------
u8* CreateBlockMap( u8 dark, u8 tl, u8 tr, u8 bl, u8 br)
{
	// Just make it static, we only ever need one at a time.
	// This can be optimized later to fill in the target map directly.
	static u8 blockMap[16];
	u8 darkOffset = dark*12;
	// TL
	blockMap[0] = darkOffset+tl*4;
	blockMap[1] = darkOffset+tl*4+1;
	// TR
	blockMap[2] = darkOffset+tr*4+2;
	blockMap[3] = darkOffset+tr*4+3;
	// Center
	blockMap[4] = darkOffset+24;
	blockMap[5] = darkOffset+25;
	blockMap[6] = darkOffset+26;
	blockMap[7] = darkOffset+27;
	blockMap[8] = darkOffset+24+24;
	blockMap[9] = darkOffset+25+24;
	blockMap[10]= darkOffset+26+24;
	blockMap[11]= darkOffset+27+24;
	// BL
	blockMap[12] = darkOffset+72+bl*4;
	blockMap[13] = darkOffset+73+bl*4;
	// BR
	blockMap[14] = darkOffset+72+br*4+2;
	blockMap[15] = darkOffset+73+br*4+2;
	return blockMap;
}

//---------------------------------------------------------
void PasteTilemapBlock(tilemap_cell* pTile, s8 dark, s8 tl, s8 tr, s8 bl, s8 br)
{
	// The +1 is because block 0 is the blank block. We're leaving that for now. But we'll
	// sort it out when the tilemap is properly laid out.
	u8 darkOffset = dark*12+1;
	// TL
	pTile[0].value = darkOffset+tl*4+0;
	pTile[1].value = darkOffset+tl*4+1;
	// TR
	pTile[2].value = darkOffset+tr*4+2;
	pTile[3].value = darkOffset+tr*4+3;
	// Center
	pTile[40].value = darkOffset+24;
	pTile[41].value = darkOffset+25;
	pTile[42].value = darkOffset+26;
	pTile[43].value = darkOffset+27;
	pTile[80].value = darkOffset+48;
	pTile[81].value = darkOffset+49;
	pTile[82].value = darkOffset+50;
	pTile[83].value = darkOffset+51;
	// BL
	pTile[120].value = darkOffset+72+bl*4;
	pTile[121].value = darkOffset+73+bl*4;
	// BR
	pTile[122].value = darkOffset+74+br*4;
	pTile[123].value = darkOffset+75+br*4;
}

//---------------------------------------------------------
void ClearTilemapBlock(tilemap_cell* pTile)
{
	u8 fillvalue=0;
	// TL
	pTile[0].value = fillvalue;
	pTile[1].value = fillvalue;
	// TR
	pTile[2].value = fillvalue;
	pTile[3].value = fillvalue;
	// Center
	pTile[40].value = fillvalue;
	pTile[41].value = fillvalue;
	pTile[42].value = fillvalue;
	pTile[43].value = fillvalue;
	pTile[80].value = fillvalue;
	pTile[81].value = fillvalue;
	pTile[82].value= fillvalue;
	pTile[83].value= fillvalue;
	// BL
	pTile[120].value = fillvalue;
	pTile[121].value = fillvalue;
	// BR
	pTile[122].value = fillvalue;
	pTile[123].value = fillvalue;
}

//---------------------------------------------------------
void CopyTileBlock(tile_template* tile, u8* tiletable, u8 attr )
{
	u8* src = tile->data;
	u8 step = (40-tile->w)*2;
	u8* dst = tiletable;
	for( u8 h=0; h<tile->h; h++ )
	{
		for( u8 w=0; w<tile->w; w++ )
		{
			*dst++ = *src++;
			*dst++ = attr;
		}
		dst += step;
	}
}

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
	
	nextreg(TILEMAP_CLIP_WINDOW,8>>1);
	nextreg(TILEMAP_CLIP_WINDOW,(320-8)>>1);
	nextreg(TILEMAP_CLIP_WINDOW,8);
	nextreg(TILEMAP_CLIP_WINDOW,(u8)(256-8));
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
	//
	// Copy tile palettes
	//
	nextreg(MMU_SLOT_6, PALETTE_PAGE);
	CopyPalette(asset_TilemapPalette, PALETTE_TILE_PRIMARY);
	//
	// Initialize tile data
	//
	nextreg(MMU_SLOT_6, TILEMAP_PAGE+1);
	ClearTilemap();

	// Tilemap templates are in the same bank as the palette
	//CopyTileBlock(asset_MapShape_02, SWAP_BANK_1, 1);
	global.playArea.position.x = 0;
	global.playArea.position.y = 0;
	global.tileMap.x = -24;
	global.tileMap.y = 32;
}

//---------------------------------------------------------
void ClearTilemap(void)
{
	u8* pTileTable = SWAP_BANK_0;
	pTileTable[0]=0x00;
	pTileTable[1]=0x01;
	// Since I know this copies forward, I can copy the first 2 bytes
	// to the rest with a DMA memcpy. It's a lot faster.
	memcpy_dma(pTileTable+2, pTileTable, 40*32*2-2);

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

u8 c=0;
u8 c2=3;
u8 t=0;

//---------------------------------------------------------
void UpdateTilemap(void)
{
	nextreg(MMU_SLOT_6, TILEMAP_PAGE+1);
#if 0
	c2++;
	if (c2>5)
	{
		play_cell* cell = GetPlayAreaCell(0, 0);
		cell->blockContent++;
		cell->blockContent = 5;
		if (cell->blockContent>7)
		{
			cell->blockContent = 0;
		}
		t++;
		// (-1,0) - OK
		// (1,0) - NG
		// (0,-1) - OK
		// (0,1) - NG
		cell = GetPlayAreaCell(-1, 0);
		cell->blockContent=(t & 16)!=0;
		cell = GetPlayAreaCell(0, -1);
		cell->blockContent=(t & 8)!=0;
		cell = GetPlayAreaCell(1, 0);
		cell->blockContent=(t & 4)!=0;
		cell = GetPlayAreaCell(0, 1);
		cell->blockContent=(t & 2)!=0;
		ClearTilemap();
		DrawPlayArea(NULL);
		c2 = 0;
	}
#endif

	nextreg(TILEMAP_OFFSET_X_H,(-global.tileMap.x>>8) & 0x1);
	nextreg(TILEMAP_OFFSET_X_L,-global.tileMap.x);
	nextreg(TILEMAP_OFFSET_Y,-global.tileMap.y);
}
