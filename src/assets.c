#include "kftypes.h"
#include "assets.h"
#include "memorymap.h"

void allAssets(void)
{
#if !defined(__GNUC__)
__asm
	global _asset_BackdropPalette,_asset_PlayerPalette
	global _asset_SpritePalette,_asset_SpriteData
	global _asset_TilemapPalette, _asset_TileData
	global _asset_GameDigitsPalette, _asset_GameDigits
	global _asset_PickupPalette, _asset_PickupData
	global _asset_MapShape_01,_asset_MapShape_02
	global _asset_PlayArea_1,_asset_PlayArea_1_2,_asset_PlayArea_1_3
	global _asset_PlayArea_2,_asset_PlayArea_2_2,_asset_PlayArea_2_4
	global _asset_PlayArea_3,_asset_PlayArea_4,_asset_PlayArea_4_2
	global _asset_PlayArea_5,_asset_PlayArea_6,_asset_PlayArea_7

	include "build/assets/bear.bin.s"
	include "build/assets/bighopper.bin.s"
	include "build/assets/coin.bin.s"
	include "build/assets/colorchanger.bin.s"
	include "build/assets/follower.bin.s"
	include "build/assets/player.bin.s"
	include "build/assets/kfback.bin.s"
	include "build/assets/spike.bin.s"
	include "build/assets/coin.bin.s"
	include "build/assets/obstacles.bin.s"

	;
	; This is for miscellaneous small assets
	;
	SECTION PAGE_52
	org 0xc000
_asset_MapShape_01:
	include "build/assets/shape_01.map"
_asset_MapShape_02:
	include "build/assets/shape_02.map"

_asset_PlayArea_1:
	db 5,5
	db 0,0
	db 1, 1, 1, 1, 1
	db 1, 1, 1, 1, 1
	db 1, 1, 1, 1, 1
	db 1, 1, 1, 1, 1
	db 1, 1, 1, 1, 1

_asset_PlayArea_1_2:
	db 7,7
	db 0,0
	db 1, 1, 1, 1, 0, 0, 0
	db 1, 2, 2, 1, 0, 0 ,0
	db 1, 2, 2, 1, 0, 0 ,0
	db 1, 1, 1, 1, 1, 1 ,1
	db 0, 0, 0, 1, 2, 2 ,1
	db 0, 0, 0, 1, 2, 1, 1
	db 0, 0, 0, 1, 1, 1 ,1

_asset_PlayArea_1_3:
	db 7, 7
	db 0, 3
	db 0, 0, 0, 1, 1, 1, 1
	db 0, 0, 0, 1, 2, 2, 1
	db 0, 0, 0, 1, 2, 2 ,1
	db 1, 1, 1, 1, 1, 1 ,1
	db 1, 2, 2, 1, 0, 0, 0
	db 1, 1, 2, 1, 0, 0, 0
	db 1, 1, 1, 1, 0, 0, 0

_asset_PlayArea_2:
	db 9, 9
	db 0, 8
	db 0, 0, 0, 0, 1, 1, 1, 1, 1
	db 0, 0, 0, 0, 1, 2, 2, 2, 1
	db 0, 0, 0, 0, 1, 2, 2, 2, 1
	db 0, 0, 0, 0, 1, 2, 2, 2, 1
	db 1, 1, 1, 1, 1, 1, 1, 1, 1
	db 1, 2, 2, 2, 1, 0, 0, 0, 0
	db 1, 2, 2, 2, 1, 0, 0, 0, 0
	db 1, 2, 2, 2, 1, 0, 0, 0, 0
	db 1, 1, 1, 1, 1, 0, 0, 0, 0

_asset_PlayArea_2_2:
	db 9, 9
	db 0, 8
	db 0,0,0,0,0,0,0,1,1
	db 0,0,0,0,0,0,0,2,1
	db 0,0,0,0,0,0,0,2,1
	db 0,0,0,2,1,1,1,1,1
	db 0,0,0,1,1,2,2,1,2
	db 0,0,0,2,1,0,0,0,0
	db 0,0,0,1,1,0,0,0,0
	db 1,1,1,1,1,0,0,0,0
	db 1,1,2,1,2,0,0,0,0

_asset_PlayArea_2_3:
	db 8, 8
	db 0, 7
	db 1,1,1,1,1,1,2,1
	db 1,2,1,2,2,1,1,1
	db 1,1,0,0,0,0,0,0
	db 2,1,0,0,0,0,0,0
	db 2,1,0,0,0,0,0,0
	db 1,1,0,0,0,0,0,0
	db 1,2,0,0,0,0,0,0
	db 1,1,0,0,0,0,0,0

_asset_PlayArea_2_4:
	db 8, 8
	db 7, 0
	db 0,0,0,0,0,0,1,1
	db 0,0,0,0,0,0,1,2
	db 0,0,0,0,0,0,1,1
	db 0,0,0,0,0,0,1,1
	db 0,0,0,0,0,0,2,1
	db 0,0,0,0,0,0,2,1
	db 1,2,1,1,1,1,2,1
	db 1,1,1,2,2,1,1,1
_asset_PlayArea_3:
	db 10, 10
	db 0, 0
	db 1, 1, 1, 1, 1, 0, 0, 0, 0, 0
	db 1, 1, 1, 1, 1, 0, 0, 0, 0, 0
	db 1, 1, 1, 1, 1, 0, 0, 0, 0, 0
	db 1, 1, 1, 1, 1, 0, 0, 0, 0, 0
	db 1, 1, 1, 1, 1, 1, 1, 1, 1, 1
	db 0, 0, 0, 0, 1, 1, 1, 1, 1, 1
	db 0, 0, 0, 0, 1, 1, 1, 1, 1, 1
	db 0, 0, 0, 0, 1, 1, 1, 1, 1, 1
	db 0, 0, 0, 0, 1, 1, 1, 1, 1, 1
	db 0, 0, 0, 0, 1, 1, 1, 1, 1, 1
_asset_PlayArea_4:
	db 10, 10
	db 2, 2
	db 0, 0, 0, 0, 1, 1, 1, 1, 1, 1
	db 0, 0, 0, 0, 1, 2, 2, 2, 2, 1
	db 0, 0, 1, 1, 1, 2, 2, 2, 2, 1
	db 0, 0, 1, 2, 2, 2, 2, 2, 2, 1
	db 1, 1, 1, 2, 2, 2, 2, 2, 1, 1
	db 1, 1, 2, 2, 2, 2, 2, 1, 1, 1
	db 1, 2, 2, 2, 2, 2, 1, 1, 0, 0
	db 1, 2, 2, 2, 2, 1, 1, 1, 0, 0
	db 1, 1, 2, 2, 1, 1, 0, 0, 0, 0
	db 1, 1, 1, 1, 1, 1, 0, 0, 0, 0
_asset_PlayArea_4_2:
	db 8, 8
	db 0, 0
	db 1, 1, 1, 1, 1, 1, 1, 1
	db 1, 2, 1, 1, 1, 1, 2, 1
	db 1, 1, 1, 0, 0, 1, 1, 1
	db 1, 1, 0, 0, 0, 0, 1, 1
	db 1, 1, 0, 0, 0, 0, 1, 1
	db 1, 1, 1, 0, 0, 1, 1, 1
	db 1, 2, 1, 1, 1, 1, 2, 1
	db 1, 1, 1, 1, 1, 1, 1, 1

_asset_PlayArea_5:
	db 13, 13
	db 0, 4
	db 0, 0, 0, 0, 1, 1, 1, 1, 1, 0, 0, 0, 0
	db 0, 0, 0, 0, 1, 2, 2, 2, 1, 0, 0, 0, 0
	db 0, 0, 0, 0, 1, 2, 2, 2, 1, 0, 0, 0, 0
	db 0, 0, 0, 0, 1, 2, 2, 2, 1, 0, 0, 0, 0
	db 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1
	db 1, 2, 2, 2, 1, 0, 0, 0, 1, 2, 2, 2, 1
	db 1, 2, 2, 2, 1, 0, 0, 0, 1, 2, 2, 2, 1
	db 1, 1, 2, 2, 1, 0, 0, 0, 1, 2, 2, 1, 1
	db 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1
	db 0, 0, 0, 0, 1, 2, 2, 2, 1, 0, 0, 0, 0
	db 0, 0, 0, 0, 1, 2, 2, 2, 1, 0, 0, 0, 0
	db 0, 0, 0, 0, 1, 1, 2, 1, 1, 0, 0, 0, 0
	db 0, 0, 0, 0, 1, 1, 1, 1, 1, 0, 0, 0, 0
_asset_PlayArea_6:
	db 11, 11
	db 0, 6
	db 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1
	db 0, 0, 0, 0, 0, 0, 1, 2, 2, 1, 1
	db 0, 0, 0, 0, 1, 1, 1, 2, 2, 2, 1
	db 0, 0, 0, 0, 1, 2, 2, 2, 2, 2, 1
	db 0, 0, 1, 1, 1, 2, 2, 2, 1, 1, 1
	db 0, 0, 1, 2, 2, 2, 2, 1, 1, 0, 0
	db 1, 1, 1, 2, 2, 2, 1, 1, 1, 0, 0
	db 1, 2, 2, 2, 2, 1, 1, 0, 0, 0, 0
	db 1, 2, 2, 2, 1, 1, 1, 0, 0, 0, 0
	db 1, 1, 2, 1, 1, 0, 0, 0, 0, 0, 0
	db 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0
_asset_PlayArea_7:
	db 11, 11
	db 0, 0
	db 1, 1, 1, 1, 1, 2, 1, 1, 1, 1, 1
	db 1, 2, 1, 1, 1, 1, 1, 1, 1, 2, 1
	db 1, 1, 1, 0, 0, 0, 0, 0, 1, 1, 1
	db 1, 1, 0, 0, 0, 0, 0, 0, 0, 1, 1
	db 1, 1, 0, 0, 0, 0, 0, 0, 0, 1, 1
	db 2, 1, 0, 0, 0, 0, 0, 0, 0, 1, 2
	db 1, 1, 0, 0, 0, 0, 0, 0, 0, 1, 1
	db 1, 1, 0, 0, 0, 0, 0, 0, 0, 1, 1
	db 1, 1, 1, 0, 0, 0, 0, 0, 1, 1, 1
	db 1, 2, 1, 1, 1, 1, 1, 1, 1, 2, 1
	db 1, 1, 1, 1, 1, 2, 1, 1, 1, 1, 1

	ALIGN 256
_asset_BackdropPalette:
	incbin "build/assets/kfback.pal"
_asset_PlayerPalette:
	incbin "build/assets/player.pal"
_asset_SpritePalette:
	incbin "build/assets/sprites.pal"
_asset_CoinPalette:
	incbin "build/assets/coin.pal"
_asset_TilemapPalette:
	incbin "build/assets/tiles.pal"
_tmpe:
	// We need this space since we will be copying over the palettes
	// from one slot to another.
	defs 512-(_tmpe-_asset_TilemapPalette),0
_asset_GameDigitsPalette:
	incbin "build/assets/numbers.pal"
_asset_GameDigits:
	incbin "build/assets/numbers.bin"
	;
	;// We need the tile data to be in its own page. The tile data is exactly 8KB so it makes sense
	;// to have it within it's own page.
	;
	SECTION PAGE_53
	org 0xc000
_asset_TileData:
	incbin "build/assets/tiles.bin"
	SECTION PAGE_55
	org 0xc000
_asset_SpriteData:
	incbin "build/assets/sprites.bin"
	__endasm;
#endif
}
