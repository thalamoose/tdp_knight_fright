//---------------------------------------------------------
// Memory Map:
// 0x0000:0x1fff - swap bank 0
// 0x2000:0x3fff - swap bank 1
// 0x4000:0x5b00 - ULA layer (from 2nd ULA layer)
// 0x6000:0x7fff - BSS
// 0x8000:0xfbff - Code space
// 0xfc00:0xfcff - Stack
// 0xfd00:0xfe01 - Interrupt vector table (to 0xfefe)
// 0xfe02:0xfefd - Unused
// 0xfefe:0xff01 - Interrupt dispatch
// 0xff01:0xffff - Unused
//---------------------------------------------------------

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
#define SWAP_BANK_0     0x4000
#define SWAP_BANK_1     0x6000

#define SWAP_BANK_0_SLOT MMU_SLOT_2
#define SWAP_BANK_1_SLOT MMU_SLOT_3
