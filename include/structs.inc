
; Bit definitions for animation flags
ANIM_ACTIVE     equ 0
ANIM_DIRECTION  equ 1
ANIM_PINGPONG   equ 2
ANIM_4_BIT      equ 3

                STRUCT OBJECT
X               WORD
Y               WORD
VX              WORD
VY              WORD
frame_index     BYTE
base_index      BYTE
total_frames    BYTE
flags           BYTE
anim_speed      BYTE
anim_delay      BYTE
                ENDS

PLAYER_STATE_IDLE       equ 0
PLAYER_STATE_MOVE_TR    equ 1
PLAYER_STATE_MOVE_TL    equ 2
PLAYER_STATE_MOVE_BR    equ 3
PLAYER_STATE_MOVE_BL    equ 4
PLAYER_STATE_DIE        equ 5
PLAYER_STATE_JUMP       equ 6
PLAYER_STATE_DEAD       equ 7
PLAYER_STATE_BONUS      equ 8


                STRUCT PLAYER
object          OBJECT
lives_remaining BYTE
score           BLOCK 4
invincible      BYTE
direction       BYTE            ; Direction facing: 0 - up; 1 left; 2 down; 3 right
state           BYTE
move_steps      BYTE            ; If non zero, player is in the process of moving. When this hits 0, position gets forced.
playgrid_x      BYTE            ; Position within the play-grid. This is an index to the playgrid array. This contains contents of each display cell group.
playgrid_y      BYTE
                ENDS

PARTICLE_ACTIVE equ 0
                STRUCT PARTICLE
VX              WORD                    ; These have to be in a specific order to make the particle update fast.
X               WORD
VY              WORD
Y               WORD
life            BYTE
prev_address    WORD
prev_page       BYTE
prev_colour     BYTE
colour          BYTE
width           BYTE
flags           BYTE
                ENDS

                STRUCT PLAYMAP_ENTRY
contents        BYTE
                ENDS
