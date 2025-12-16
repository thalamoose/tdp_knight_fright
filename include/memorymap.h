#define ASM_CODE_PAGE		4
#define TILEMAP_CHARS_PAGE	10
#define TILEMAP_PAGE		11
#define ULA_SHADOW_PAGE 	14
#define LAYER_2_PAGE    	16              // 8 pages at 256x256; 10 at 256x320
#define BACKDROP_PAGE     	32              // Bank 16
#define MISC_DATA_PAGE		42				// Various things kept in this bank
#define TILES_PAGE     		43
#define VIRTUAL_TILEMAP_PAGE 44
#define SPRITES_PAGE    	45
#define PICKUPS_PAGE		46
#define OBSTACLES_PAGE		48
#define BEAR_ANIM_PAGE 		64
#define BIGHOPPER_ANIM_PAGE 72
#define COLORCHANGER_ANIM_PAGE 80
#define FOLLOWER_ANIM_PAGE 	88
#define PLAYER_ANIM_PAGE 	96
#define SPIKE_ANIM_PAGE 	104
#define STACK_TOP     	(void*)0x0
#define SWAP_BANK_0     (u8*)0x0000
#define SWAP_BANK_1     (u8*)0x2000
#define SWAP_BANK_PAGE_0 MMU_SLOT_0
#define SWAP_BANK_PAGE_1 MMU_SLOT_1

