
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