#include "kftypes.h"
#include "utilities.h"
#include "assets.h"
#include "memorymap.h"
#include "hardware.h"

void LoadInitialAssets(void)
{
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
	global _asset_PlayArea_03

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

#define ___ 0		; Blank
#define BLK 1		; Normal block
#define SPK 3		; Spikes
#define TRE 4		; Cacti or tree, not sure
#define TMB 5		; Tomb
#define GRV 6		; Gravestone

_asset_PlayArea_03:
	db 10,10
	db BLK,BLK,BLK,BLK,BLK,BLK,___,___,___,___
	db BLK,BLK,TRE,___,BLK,BLK,___,___,___,___
	db BLK,BLK,BLK,BLK,BLK,___,BLK,BLK,___,___
	db SPK,BLK,BLK,BLK,SPK,BLK,BLK,BLK,___,___
	db BLK,BLK,BLK,___,BLK,BLK,TMB,SPK,BLK,BLK
	db BLK,BLK,BLK,SPK,BLK,BLK,TMB,BLK,___,BLK
	db ___,___,BLK,BLK,BLK,BLK,BLK,BLK,BLK,BLK
	db ___,___,BLK,BLK,GRV,BLK,BLK,TRE,BLK,BLK
	db ___,___,___,___,BLK,BLK,BLK,GRV,BLK,BLK
	db ___,___,___,BLK,BLK,BLK,BLK,BLK,BLK,___


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
