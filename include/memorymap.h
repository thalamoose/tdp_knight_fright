#include "kftypes.h"

#define ASM_CODE_PAGE		4
#define TILEMAP_CHARS_PAGE	10
#define TILEMAP_PAGE		11
#define ULA_SHADOW_PAGE 	14
#define LAYER_2_PAGE    	16              // 8 pages at 256x256; 10 at 256x320
#define BACKDROP_PAGE     	32              // Bank 16
#define PLAYER_ANIM_PAGE 	42 				// Bank 21
#define PALETTE_PAGE		50				// Various things kept in this bank
#define TILES_PAGE     		51
#define VIRTUAL_TILEMAP_PAGE 52
#define SPRITES_PAGE    	53
#define STACK_TOP     	(void*)0xc000
#define SWAP_BANK_0     (u8*)0xc000
#define SWAP_BANK_1     (u8*)0xe000
#define TEMP_SWAP_BANK  (u8*)0x4000
#define CODE_BANK_0		(u8*)0x8000
#define CODE_BANK_1 	(u8*)0xa000

