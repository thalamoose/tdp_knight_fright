#define ASM_CODE_PAGE		4
#define TILEMAP_CHARS_PAGE	10
#define TILEMAP_PAGE		11
#define ULA_SHADOW_PAGE 	14
#define LAYER_2_PAGE    	16              // 8 pages at 256x256; 10 at 256x320
#define BACKDROP_PAGE     	32              // Bank 16
#define PALETTE_PAGE		42				// Various things kept in this bank
#define TILES_PAGE     		43
#define VIRTUAL_TILEMAP_PAGE 44
#define SPRITES_PAGE    	45
#define PICKUPS_PAGE		46
#define BEAR_ANIM_PAGE 		64
#define BIGHOPPER_ANIM_PAGE 72
#define COLORCHANGER_ANIM_PAGE 80
#define FOLLOWER_ANIM_PAGE 	88
#define PLAYER_ANIM_PAGE 	96
#define SPIKE_ANIM_PAGE 	104
#define STACK_TOP     	(void*)0xc000
#define SWAP_BANK_0     (u8*)0xc000
#define SWAP_BANK_1     (u8*)0xe000
#define TEMP_SWAP_BANK  (u8*)0x4000
#define CODE_BANK_0		(u8*)0x8000
#define CODE_BANK_1 	(u8*)0xa000

