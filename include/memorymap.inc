TILEMAP_PAGE    equ 10
ULA_SHADOW_PAGE equ 14
LAYER_2_PAGE    equ 16                ; 8 pages at 256x256; 10 at 256x320
SPLASH_PAGE     equ 32                    ; Bank 16
SPRITES_PAGE    equ SPLASH_PAGE+10        ; Bank 21
PALETTE_PAGE    equ SPRITES_PAGE+2        ; Bank 22
TILES_PAGE         equ PALETTE_PAGE+1        ; Page 23
PLAYER_ANIM_PAGE equ TILES_PAGE+2        ; Page 24.5

unused_page        equ PLAYER_ANIM_PAGE+8

STACK_TOP       equ $c000
SWAP_BANK_0     equ $c000
SWAP_BANK_1     equ $e000
TEMP_SWAP_BANK     equ $4000
