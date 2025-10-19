#include "kftypes.h"
#include "memorymap.h"
#include "hardware.h"
#include "utilities.h"
#include "defines.h"
#include "tilemap.h"
#include "assets.h"

tile_map tilemap;

tile_template _asset_MapShape_02;

void CopyTileBlock(tile_template* tile, u8* tiletable, u8 attr )
{
	u8* src = tile->data;
	u8 step = (40-tile->w)*2;
	for( u8 h=0; h<tile->h; h++ )
	{
		u8* dst = tiletable;
		for( u8 w=0; w<tile->w; w++ )
		{
			*dst++ = *src++;
			*dst++ = attr;
		}
		tiletable += step;
	}
}

void InitializeTilemap(void)
{
	// normally, colour 0 is transparent but the test image has it as colour 1
	// ULA has already been mapped to it's alternate pages at 14,15. Tilemap can
	// use all of pages 10,11.

	nextreg(TRANS_TILEMAP_INDEX,0);
	nextreg(TILEMAP_CONTROL,0x80);
	nextreg(TILEMAP_CHAR_ADDRESS,0x0000>>8);     // Start character data right after tilemap
	nextreg(TILEMAP_BASE_ADDRESS,0x2000>>8);
	
	nextreg(TILEMAP_CLIP_WINDOW,8>>1);
	nextreg(TILEMAP_CLIP_WINDOW,(320-8)>>1);
	nextreg(TILEMAP_CLIP_WINDOW,8);
	nextreg(TILEMAP_CLIP_WINDOW,(u8)(256-8));
	//
	// Copy tilemap character data to tilemap area
	//
	u8* baseOfTileMemory = (u8*)TEMP_SWAP_BANK;
	nextreg(MMU_SLOT_2,TILEMAP_PAGE);
	nextreg(MMU_SLOT_3,TILEMAP_PAGE+1);
	nextreg(MMU_SLOT_6,TILES_PAGE);
	nextreg(MMU_SLOT_7,TILES_PAGE+1);
	// Copy tile
	memcpy_dma(baseOfTileMemory, asset_TileData, 0x2000);
	//
	// TEMPORARY FOR TESTING UNTIL TILEMAP PROPERLY SORTED
	//
	memcpy_dma(baseOfTileMemory+32,SWAP_BANK_0,0x1fe0);
	memset(baseOfTileMemory,0,32);
	//
	// Copy tile palettes
	//
	nextreg(MMU_SLOT_6,PALETTE_PAGE);
	nextreg(MMU_SLOT_7,PALETTE_PAGE+1);

	CopyPalette(asset_TilemapPalette,PALETTE_TILE_PRIMARY);
	nextreg(MMU_SLOT_6,TILES_PAGE);
	nextreg(MMU_SLOT_7,TILES_PAGE+1);
	u8* pTileTable = baseOfTileMemory;
	for( u8 y=0; y<32; y++ )
	{
		for( u8 x=0;x<40;x++ )
		{
			*pTileTable++=0;
			*pTileTable++=0x01;
		}
	}
	CopyTileBlock(asset_MapShape_02, baseOfTileMemory+0x2000, 1);
	tilemap.play_x = PLAY_AREA_CELLS_WIDTH/2;
	tilemap.play_y = PLAY_AREA_CELLS_HEIGHT/2;
	tilemap.x = 12;
	tilemap.y = 0;
	nextreg(MMU_SLOT_2, ULA_SHADOW_PAGE);
	nextreg(MMU_SLOT_2, ULA_SHADOW_PAGE+1);
}

void UpdateTilemap(void)
{
		tilemap.x++;
		tilemap.y++;
		nextreg(TILEMAP_OFFSET_X_H,0);
		nextreg(TILEMAP_OFFSET_X_L,-tilemap.x);
		nextreg(TILEMAP_OFFSET_Y,tilemap.y);
}
