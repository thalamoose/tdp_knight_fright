#include "kftypes.h"
#include "memorymap.h"
#include "hardware.h"
#include "utilities.h"
#include "defines.h"
#include "tilemap.h"
#include "assets.h"
#include "globals.h"

tile_template _asset_MapShape_02;
tile_template _asset_MapShape_01;

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
	nextreg(MMU_SLOT_7, TILEMAP_PAGE+1);
	u8* pTileTable = (u8*)SWAP_BANK_1;
	pTileTable[0]=0x00;
	pTileTable[1]=0x01;
	// Since I know this copies forward, I can copy the first 2 bytes
	// to the rest with a DMA memcpy. It's a lot faster.
	memcpy_dma(pTileTable+2, pTileTable, 40*32*2-2);

	// Tilemap templates are in the same bank as the palette
	CopyTileBlock(asset_MapShape_02, SWAP_BANK_1, 1);
	global.playArea.x = PLAY_AREA_CELLS_WIDTH/2;
	global.playArea.y = PLAY_AREA_CELLS_HEIGHT/2;
	global.tileMap.x = 12;
	global.tileMap.y = 0;
}

void UpdateTilemap(void)
{
		//tilemap.x++;
		//tilemap.y++;
		nextreg(TILEMAP_OFFSET_X_H,(-global.tileMap.x>>8) & 0x3);
		nextreg(TILEMAP_OFFSET_X_L,-global.tileMap.x);
		nextreg(TILEMAP_OFFSET_Y,global.tileMap.y);
}
