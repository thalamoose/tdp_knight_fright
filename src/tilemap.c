#include "kftypes.h"
#include "memorymap.h"
#include "hardware.h"
#include "utilities.h"
#include "tilemap.h"
#include "defines.h"

typedef struct s_tile_map
{
	s8 x;
	s8 y;
	s8 play_x;
	s8 play_y;
} tile_map;

tile_map tilemap;

extern u16 tilemap_palette[];

typedef struct s_tile
{
	u8 w;
	u8 h;
	u8 data[0];
} tile_template;

tile_template* test_tilemap_table;

void CopyTileBlock(tile_template* tile, u8* tiletable, u8 attr )
{
	u8* src = tile->data;
	for( u8 h=0; h<tile->h; h++ )
	{
		u8* dst = tiletable+h*40*2;
		for( u8 w=0; w<tile->w; w++ )
		{
			*dst++ = *src++;
			*dst++ = attr;
		}
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
	nextreg(MMU_SLOT_6,TILES_PAGE);
	nextreg(MMU_SLOT_7,TILEMAP_PAGE);
	memcpy(SWAP_BANK_1,SWAP_BANK_0,0x2000);
	//
	// TEMPORARY FOR TESTING UNTIL TILEMAP PROPERLY SORTED
	//
	memcpy(SWAP_BANK_1+32,SWAP_BANK_0,0x1fe0);
	memset(SWAP_BANK_1,0,32);
	//
	// Copy tile palettes
	//
	nextreg(MMU_SLOT_6,PALETTE_PAGE);

	CopyPalette(tilemap_palette,3);
	nextreg(MMU_SLOT_6,TILEMAP_PAGE);
	nextreg(MMU_SLOT_7,TILEMAP_PAGE+1);
	u8* pTileTable = (u8*)SWAP_BANK_0+0x2000;
	for( u8 y=0;y<32;y++ )
	{
		for( u8 x=0;x<40;x++ )
		{
			*pTileTable++=0;
			*pTileTable++=0x01;
		}
	}
	CopyTileBlock(test_tilemap_table,(u8*)(SWAP_BANK_0)+0x2000,1);
	tilemap.play_x = PLAY_AREA_CELLS_WIDTH/2;
	tilemap.play_y = PLAY_AREA_CELLS_HEIGHT/2;
	tilemap.x = 12;
	tilemap.y = 0;
}

void UpdateTilemap(void)
{
		tilemap.x++;
		tilemap.y++;
		nextreg(TILEMAP_OFFSET_X_H,0);
		nextreg(TILEMAP_OFFSET_X_L,-tilemap.x);
		nextreg(TILEMAP_OFFSET_Y,tilemap.y);
}

u16 tilemap_palette[32];