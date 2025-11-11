#include "kftypes.h"
#include "utilities.h"
#include "assets.h"
#include "memorymap.h"
#include "hardware.h"

void CopyBackgroundBitmap(void);

void LoadInitialAssets(void)
{
	CopyBackgroundBitmap();
}

void CopyBackgroundBitmap(void)
{
	u8 srcPage = BACKDROP_PAGE;
	u8 dstPage = LAYER_2_PAGE;
	u8 totalPages = 10;
	for (int i=0; i<totalPages; i++)
	{
		nextreg(MMU_SLOT_6,srcPage);
		nextreg(MMU_SLOT_7,dstPage);
		memcpy_dma(SWAP_BANK_1,SWAP_BANK_0,8192);
		srcPage++;
		dstPage++;
	}
	nextreg(MMU_SLOT_6, PALETTE_PAGE);
	nextreg(MMU_SLOT_7, PALETTE_PAGE+1);
	CopyPalette(asset_BackdropPalette,PALETTE_LAYER_2_PRIMARY);
}

void allAssets(void)
{
#if !defined(__GNUC__)
__asm
	global _asset_BackdropPalette,_asset_PlayerPalette
	global _asset_SpritePalette,_asset_SpriteData
	global _asset_TilemapPalette, _asset_TileData
	global _asset_MapShape_01,_asset_MapShape_02
	global _asset_PlayArea_01,_asset_PlayArea_02

	include "build/assets/kfplayer.bin.s"
	include "build/assets/kfback.bin.s"

	;
	; This is for miscellaneous small assets
	;
	SECTION BANK_25_L
	org 0xc000
_asset_MapShape_01:
	include "build/assets/shape_01.map"
_asset_MapShape_02:
	include "build/assets/shape_02.map"
_asset_PlayArea_01:
	db 8,6
	db 0,0,0,1,1,1,2,0
	db 0,0,0,1,1,1,1,1
	db 0,0,0,1,1,1,1,1
	db 0,0,2,1,1,1,0,1
	db 0,0,1,1,1,1,1,2
	db 0,0,0,1,0,0,0,0

	;; 0,1,2,3,4,5,6,7,8,9
	db 0,0,1,1,0	; 0
	db 0,1,1,1,0	; 1
	db 0,1,1,1,1	; 2
	db 1,1,1,0,1	; 3
	db 0,1,1,1,1	; 4
	db 0,2,1,1,0	; 5
	db 0,0,1,1,0	; 6
	db 0,1,1,0,0	; 7
	db 0,0,1,1,0	; 8
	db 0,0,1,0,0	; 9

_asset_PlayArea_02:
	db 7,7
	db 1, 1, 1, 1, 0, 0, 0
	db 1, 2, 2, 1, 0, 0 ,0
	db 1, 2, 2, 1, 0, 0 ,0
	db 1, 1, 1, 1, 1, 1 ,1
	db 0, 0, 0, 1, 2, 2 ,1
	db 0, 0, 0, 1, 2, 1, 1
	db 0, 0, 0, 1, 1, 1 ,1

_asset_BackdropPalette:
	incbin "build/assets/kfback.pal"
_asset_PlayerPalette:
	incbin "build/assets/kfplayer.pal"
_asset_SpritePalette:
	incbin "build/assets/kfsprites.pal"
_asset_TilemapPalette:
	incbin "build/assets/kftiles.pal"

	;
	;// We need the tile data to be in its own page. The tile data is exactly 8KB so it makes sense
	;// to have it within it's own page.
	;
	SECTION BANK_25_H
	org 0xc000
_asset_TileData:
	incbin "build/assets/kftiles.bin"
	SECTION BANK_26_L
_asset_SpriteData:
	incbin "build/assets/kfsprites.bin"



	__endasm;
#endif
}
