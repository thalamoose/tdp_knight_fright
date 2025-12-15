#define ASM_CODE_PAGE		4
#define TILEMAP_CHARS_PAGE	10
#define TILEMAP_PAGE		11
#define ULA_SHADOW_PAGE 	14
#define CODE_PAGE			16
#define LAYER_2_PAGE    	26              // 8 pages at 256x256; 10 at 256x320
#define BACKDROP_PAGE     	42              // Bank 16
#define MISC_DATA_PAGE		52				// Various things kept in this bank
#define TILES_PAGE     		53
#define VIRTUAL_TILEMAP_PAGE 54
#define SPRITES_PAGE    	55
#define PICKUPS_PAGE		56
#define OBSTACLES_PAGE		58
#define BEAR_ANIM_PAGE 		74
#define BIGHOPPER_ANIM_PAGE 82
#define COLORCHANGER_ANIM_PAGE 90
#define FOLLOWER_ANIM_PAGE 	98
#define PLAYER_ANIM_PAGE 	106
#define SPIKE_ANIM_PAGE 	114
#define STACK_TOP     	(void*)0xc000
#define SWAP_BANK_0     (u8*)0xc000
#define SWAP_BANK_1     (u8*)0xe000

