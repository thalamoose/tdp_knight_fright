
; Bit definitions for animation flags
ANIM_ACTIVE     equ 0
ANIM_DIRECTION  equ 1
ANIM_PINGPONG   equ 2
ANIM_4_BIT      equ 3

                STRUCT OBJECT
X               WORD
Y               WORD
frame_index     BYTE
base_index      BYTE
total_frames    BYTE
flags           BYTE
anim_speed      BYTE
anim_delay      BYTE
size            BYTE
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
size            BYTE
                ENDS
