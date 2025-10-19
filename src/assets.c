#include "kftypes.h"
#include "utilities.h"
#include "assets.h"
#include "memorymap.h"
#include "hardware.h"

void CopyBackgroundBitmap(void);
void UploadSprites(void);

void LoadInitialAssets(void)
{
	CopyBackgroundBitmap();
	UploadSprites();
}

void CopyBackgroundBitmap(void)
{
	u8 srcPage = BACKDROP_PAGE;
	u8 dstPage = LAYER_2_PAGE;
	u8 totalPages = 10;
	for (int i=0; i<totalPages;i++)
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

void UploadSprites(void)
{
	nextreg(MMU_SLOT_6,SPRITES_PAGE);
	nextreg(MMU_SLOT_7,SPRITES_PAGE+1);
	CopySprite(asset_SpriteData, 16, 32);
}

void allAssets(void)
{
__asm
	global _asset_BackdropPalette,_asset_PlayerPalette
	global _asset_SpritePalette,_asset_SpriteData
	global _asset_TilemapPalette, _asset_TileData
	global _asset_MapShape_01,_asset_MapShape_02

	include "build/assets/kfplayer.bin.s"
	include "build/assets/kfback.bin.s"

	;
	; This is for miscellaneous small assets
	;
	SECTION BANK_25
	org 0xc000
_asset_MapShape_01:
	incbin "build/assets/shape_01.map"
_asset_MapShape_02:
	incbin "build/assets/shape_02.map"
_asset_TileData:
	incbin "build/assets/kftiles.bin"
_asset_SpriteData:
	incbin "build/assets/kfsprites.bin"
_asset_BackdropPalette:
	incbin "build/assets/kfback.pal"
_asset_PlayerPalette:
	incbin "build/assets/kfplayer.pal"
_asset_SpritePalette:
	incbin "build/assets/kfsprites.pal"
_asset_TilemapPalette:
	incbin "build/assets/kftiles.pal"
__endasm;
}




