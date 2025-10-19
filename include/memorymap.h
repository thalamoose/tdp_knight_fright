#include "kftypes.h"

#define ASM_CODE_PAGE		4
#define TILEMAP_PAGE    	10
#define ULA_SHADOW_PAGE 	14
#define LAYER_2_PAGE    	16              // 8 pages at 256x256; 10 at 256x320
#define BACKDROP_PAGE     	32              // Bank 16
#define PLAYER_ANIM_PAGE 	42 				// Bank 21
#define SPRITES_PAGE    	50				// Various things kept in this bank
#define TILES_PAGE     		50
#define PALETTE_PAGE		50
#define STACK_TOP     	(void*)0xc000
#define SWAP_BANK_0     (void*)0xc000
#define SWAP_BANK_1     (void*)0xe000
#define TEMP_SWAP_BANK  (void*)0x4000
#define CODE_BANK_0		(void*)0x8000
#define CODE_BANK_1 	(void*)0xa000

