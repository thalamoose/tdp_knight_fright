#include "kftypes.h"
#include "hardware.h"
#include "memorymap.h"
#include "utilities.h"
#include "tilemap.h"

typedef struct
{
	s8 x;
	s8 y;
	s8 play_x;
	s8 play_y;
} tile_map;

tile_map tilemap;

extern u16 tilemap_palette[];

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

	copy_palette(tilemap_palette,3);
	nextreg(MMU_SLOT_6,TILEMAP_PAGE);
	nextreg(MMU_SLOT_7,TILEMAP_PAGE+1);
	u8* pTileTable = (u8*)SWAP_BANK_0+0x2000;
	for( u8 y=0;y<32;y++ )
	{
		for( u8 x=0;x<32;x++ )
		{
			*pTileTable++=0;
			*pTileTable++=0x01;
		}
	}
/*

        ; Fill tilemap area
        ld hl,SWAP_BANK_0+$2000
        ld e,%00000001      ; Tile attributes
        ld c,32
tile_next_line:
        ld b,40
tile_next_char:
        ld (hl),a
        ld (hl),0 ;; TEST CODEf
        inc hl
        ld (hl),e
        inc hl
        inc a
        cp 96
        jr nz,tile_no_wrap
        xor a
tile_no_wrap:
        djnz tile_next_char
        dec c
        jr nz,tile_next_line

        ld hl,test_tilemap_table
        ld de,SWAP_BANK_0+$2000
        ld a,1
        call copy_tile_block
        ; default position until map is properly set up.
        ld hl,play_area_center_x
        ld (hl),PLAY_AREA_CELLS_WIDTH/2
        inc hl
        ld (hl),PLAY_AREA_CELLS_HEIGHT/2
        ld hl,tilemap_x
        ld (hl),12
        inc hl
        ld (hl),0
        ret
*/
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