ASM_CODE_PAGE 	= 4
C_CODE_PAGE		= 6
TILEMAP_PAGE    = 10
ULA_SHADOW_PAGE = 14
LAYER_2_PAGE    = 16                ; 8 pages at 256x256; 10 at 256x320
SPLASH_PAGE     = 32                    ; Bank 16
SPRITES_PAGE    = SPLASH_PAGE+10        ; Bank 21
PALETTE_PAGE    = SPRITES_PAGE+2        ; Bank 22
TILES_PAGE     	= PALETTE_PAGE+1        ; Page 23
PLAYER_ANIM_PAGE = TILES_PAGE+2        ; Page 24.5

unused_page     = PLAYER_ANIM_PAGE+8

STACK_TOP       = $c000
SWAP_BANK_0     = $c000
SWAP_BANK_1     = $e000
TEMP_SWAP_BANK  = $4000
CODE_BANK_0		= $8000
CODE_BANK_1 	= $a000

