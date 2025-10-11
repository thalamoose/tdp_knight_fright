CLOCK_SEL               equ $07
PERIPHERAL_3_CONTROL    equ $08
LAYER_2_ACCESS          equ $123b
LAYER_2_RAM_BANK        equ $12
LAYER_2_SHADOW_BANK     equ $13
GLOBAL_TRANSPARENCY     equ $14
SPRITE_AND_LAYERS       equ $15
LAYER_2_OFFSET_X        equ $16
LAYER_2_OFFSET_Y        equ $17
LAYER_2_CLIP_WINDOW     equ $18
SPRITE_CLIP_WINDOW      equ $19
ULA_CLIP_WINDOW         equ $1a
TILEMAP_CLIP_WINDOW     equ $1b
CLIP_WINDOW_CONTROL     equ $1c
TILEMAP_OFFSET_X_H      equ $2f
TILEMAP_OFFSET_X_L      equ $30
TILEMAP_OFFSET_Y        equ $31
ULA_OFFSET_X            equ $32
ULA_OFFSET_Y            equ $33
SPRITE_INDEX            equ $34
SPRITE_ATTR_0           equ $35
SPRITE_ATTR_1           equ $36
SPRITE_ATTR_2           equ $37
SPRITE_ATTR_3           equ $38
SPRITE_ATTR_4           equ $39
PALETTE_INDEX           equ $40
PALETTE_VALUE_8         equ $41
PALETTE_CONTROL         equ $43
PALETTE_VALUE_9            equ $44
TRANS_FALLBACK          equ $4a
TRANS_SPRITE_INDEX      equ $4b
TRANS_TILEMAP_INDEX     equ $4c
MMU_SLOT_0              equ $50     ; $0000-$1fff
MMU_SLOT_1              equ $51     ; $2000-$3fff
MMU_SLOT_2              equ $52     ; $4000-$5fff
MMU_SLOT_3              equ $53     ; $6000-$7fff
MMU_SLOT_4              equ $54     ; $8000-$9fff
MMU_SLOT_5              equ $55     ; $a000-$bfff
MMU_SLOT_6              equ $56     ; $c000-$dfff
MMU_SLOT_7              equ $57     ; $e000-$ffff
COPPER_DATA             equ $60
COPPER_CONTROL_L        equ $61
COPPER_CONTROL_H        equ $62
ULA_CONTROL             equ $68
DISPLAY_CONTROL_1       equ $69
TILEMAP_CONTROL         equ $6b
DEFAULT_TILEMAP_ATTR    equ $6c
TILEMAP_BASE_ADDRESS    equ $6e
TILEMAP_CHAR_ADDRESS    equ $6f
LAYER_2_CONTROL            equ $70

ULA_PORT                equ $fe
KEMPSTON_JOYSTICK_PORT  equ $1f
KEMPSTON_MOUSE_BTN_PORT equ $fadf
KEMPSTON_MOUSE_X_PORT   equ $fbdf
KEMPSTON_MOUSE_Y_PORT   equ $ffdf

SPRITE_SLOT_PORT        equ $303b
SPRITE_ATTRIB_PORT      equ $57
SPRITE_PATTERN_PORT     equ $5b
MEM_PAGING_CONTROL_PORT    equ $7ffd


JOYPAD_R_UP             equ 7
JOYPAD_R_LEFT           equ 6
JOYPAD_R_RIGHT          equ 5
JOYPAD_R_DOWN           equ 4
JOYPAD_L_UP             equ 3
JOYPAD_L_DOWN           equ 2
JOYPAD_L_LEFT           equ 1
JOYPAD_L_RIGHT          equ 0

MAX_SPRITES             equ 128

DMA_RESET                      equ $c3
DMA_RESET_PORT_A_TIMING        equ $c7
DMA_RESET_PORT_B_TIMING        equ $cb
DMA_LOAD                       equ $cf ; %11001111
DMA_CONTINUE                   equ $d3
DMA_DISABLE_INTERUPTS          equ $af
DMA_ENABLE_INTERUPTS           equ $ab
DMA_RESET_DISABLE_INTERUPTS    equ $a3
DMA_ENABLE_AFTER_RETI          equ $b7
DMA_READ_STATUS_BYTE           equ $bf
DMA_REINIT_STATUS_BYTE         equ $8b
DMA_START_READ_SEQUENCE        equ $a7
DMA_FORCE_READY                equ $b3
DMA_DISABLE                    equ $83
DMA_ENABLE                     equ $87
DMA_WRITE_REGISTER_COMMAND     equ $bb
DMA_BURST                      equ %11001101
DMA_CONTINUOUS                 equ %10101101
ZXN_DMA_PORT                   equ $6b
SPRITE_STATUS_SLOT_SELECT      equ $303B
SPRITE_IMAGE_PORT              equ $5b
SPRITE_INFO_PORT               equ $57
