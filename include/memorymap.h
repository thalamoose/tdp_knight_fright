#include "kftypes.h"

#define ASM_CODE_PAGE		4
#define TILEMAP_PAGE    	10
#define ULA_SHADOW_PAGE 	14
#define LAYER_2_PAGE    	16                // 8 pages at 256x256; 10 at 256x320
#define SPLASH_PAGE     	32                // Bank 16
#define SPRITES_PAGE    	SPLASH_PAGE+10    // Bank 21
#define PALETTE_PAGE    	SPRITES_PAGE+2    // Bank 22
#define TILES_PAGE     		PALETTE_PAGE+2    // Page 24
#define PLAYER_ANIM_PAGE 	TILES_PAGE+2      // Page 26.7
#define C_CODE_PAGE			PLAYER_ANIM_PAGE+8// 28

#define unused_page     	= C_CODE_PAGE+2

#define STACK_TOP     	(void*)0xc000
#define SWAP_BANK_0     (void*)0xc000
#define SWAP_BANK_1     (void*)0xe000
#define TEMP_SWAP_BANK  (void*)0x4000
#define CODE_BANK_0		(void*)0x8000
#define CODE_BANK_1 	(void*)0xa000

