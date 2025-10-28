        include "include/hardware.inc"
        include "include/defines.inc"
        include "include/memorymap.inc"
; Particle coordinates are 16 bit, fixed point 10..6 format
; Particles are rendered as square, currently xored with the
; current contents. However, in the future, we can or in the
; lower 2 bits, so it defines the colour, then we can use this
; to mask out the colour behind the banner, and clear it later
; by anding with %1111_1100.
; 
        SECTION code_user
;
FIXED_POINT_BITS equ 6

FIXED_POINT_ONE equ (1<<FIXED_POINT_BITS)
FIXED_POINT_HALF equ (FIXED_POINT_ONE/2)
FIXED_POINT_SPEED_BITS equ 4

DEFVARS 0
{
        PARTICLE_VX             ds.w 1                    ; These have to be in a specific order to make the particle update fast.
        PARTICLE_X              ds.w 1
        PARTICLE_VY             ds.w 1
        PARTICLE_Y              ds.w 1
        PARTICLE_life           ds.b 1
        PARTICLE_prev_address   ds.w 1
        PARTICLE_prev_page      ds.b 1
        PARTICLE_prev_colour    ds.b 1
        PARTICLE_colour         ds.b 1
        PARTICLE_width          ds.b 1
        PARTICLE_flags          ds.b 1
        PARTICLE_sizeof         ds.b 0
}

PARTICLE_ACTIVE equ 0
F_4_4_TO_10_6 MACRO r1,r2
        ld r2,r1
        ld a,r1
        rla
        sbc a,a
        ld r1,a
        sla r2
        rl  r1
        ENDM

        global _InitializeParticles
        global _RenderParticles
        global _UpdateParticles
        global _DebugAddParticle
        extern _get_random
_InitializeParticles:
        ;;;;
        push ix
        xor a
        ld (_particle_index),a
        ld b,MAX_PARTICLES
        ld ix,_particle_objects
        ld de,PARTICLE_sizeof
@init_loop:
        ld (ix+PARTICLE_flags),0                ; Mark it inactive
        ld (ix+PARTICLE_prev_page),0            ; Mark it so it doesn't need restoring.
        add ix,de
        djnz @init_loop
        pop ix
        ret

_DebugAddParticle:
        ld hl,160-16
        call _get_random
        and 0x07
        add l
        ld l,a
        ld de,64
        call _get_random
        and 0x07
        add e
        ld e,a

        call _get_random
        sra a
        sra a
        inc a
        ld b,a
        call _get_random
        sra a
        sra a
        inc a
        ld c,a
        call add_particle
        ret
_UpdateParticles:
        push ix
        ld ix,_particle_objects
        ld de,PARTICLE_sizeof
        ld b,MAX_PARTICLES
        ld l,0
@update_loop:
        bit PARTICLE_ACTIVE,(ix+PARTICLE_flags)
        jr z,@not_active
        exx
        call update_particle
        exx
        inc l
@not_active:
        add ix,de
        djnz @update_loop
        pop ix
        ret

; IX is assumed to be on a 64 byte boundary.
; 220 cycles per update. vs 257 on old method
update_particle:
        ; X += VX
        ld de,ix
        ld hl,de
        ld c,(hl)                ; Load VX
        inc l
        ld b,(hl)
        inc l
        ld e,(hl)                ; Load X
        inc l
        ld d,(hl)
        ex de,hl                ; X=X+VX
        add hl,bc
        ex de,hl
        ld (hl),d                ; Store X
        dec l
        ld (hl),e
        inc l
        inc l

        ; Y += VY
        ld a,l                    ; Save offset to VY
        ld c,(hl)                ; Load VY
        inc l
        ld b,(hl)
        inc l
        ld e,(hl)                ; Load Y
        inc l
        ld d,(hl)
        ex de,hl                ; Y=Y+VY
        add hl,de
        ex de,hl
                        
        ld (hl),d                ; Store Y
        dec l
        ld (hl),e
        inc l
        inc l

        add bc,GRAVITY
        ld l,a                    ; Store back in VY
        ld (hl),c
        inc l
        ld (hl),b
        inc l
        inc l
        inc l
        ; Decrement life
        dec (hl)
        ret nz                    ; If not dead, return
        ld (ix+PARTICLE_life),0
        ret

; in: HL = start X, DE = start Y
; B = delta x (signed) 4.4 format
; C = delta y (signed) 4.4 format
; destroys: AF, BC
add_particle:
        exx
        ld a,(_particle_index)
        ld e,a
        ld d,PARTICLE_sizeof
        mul d,e
        ld ix,_particle_objects
        add ix,de
        ld b,MAX_PARTICLES
        ld de,PARTICLE_sizeof
@find_slot:
        bit PARTICLE_ACTIVE,(ix+PARTICLE_flags)
        jr z,@found
        add ix,de
        inc a
        cp MAX_PARTICLES
        jr nz,@no_wrap
        xor a
        ld ix,_particle_objects
@no_wrap:
        djnz @find_slot
        nop
@found:
        call remove_particle
        exx
        push de

        F_4_4_TO_10_6 b,d
        ld (ix+PARTICLE_VX),d
        ld (ix+PARTICLE_VX+1),b

        F_4_4_TO_10_6 c,d
        ld (ix+PARTICLE_VY),d
        ld (ix+PARTICLE_VY+1),c

        ld de,hl
        ld b,FIXED_POINT_BITS
        bsla de,b
        ld (ix+PARTICLE_X),de
        pop de
        bsla de,b
        ld (ix+PARTICLE_Y),de
        call _get_random
        ld (ix+PARTICLE_colour),a

        call _get_random
        and %00000011
        inc a
        ld (ix+PARTICLE_width),a
        ; Give it some initial velocity
        call _get_random
        and %01111111
        add 100
        ld (ix+PARTICLE_life),a    ; frames of life - 5 seconds for testing
        ld a,b
        set PARTICLE_ACTIVE,(ix+PARTICLE_flags)
        ld a,(_particle_index)
        inc a
        and MAX_PARTICLES-1
        ld (_particle_index),a
        ret

remove_particle:
        ld a,(ix+PARTICLE_prev_page)
        and a
        jr z,@no_restore
        ld de,(ix+PARTICLE_prev_address)
        ld b,(ix+PARTICLE_prev_colour)
        ld c,(ix+PARTICLE_width)
        call xor_particle
        ld (ix+PARTICLE_prev_page),0
@no_restore:
        ret


_RenderParticles:
        push ix
        ld ix,_particle_objects
        ld b,MAX_PARTICLES
        ld c,0
        ld de,PARTICLE_sizeof
        xor a
        ld (_particle_slot),a
@render_loop:
        bit PARTICLE_ACTIVE,(ix+PARTICLE_flags)
        jr z,@not_active
        call render_particle
        inc c
@not_active:
        add ix,de
        djnz @render_loop
        pop ix
        ret


; IX - particle, preserves all registers by using
; alternate set.
render_particle:
        exx
        ex af,af'
        ; Remove previous occurance.
        call remove_particle
        ld a,(ix+PARTICLE_colour)
        ld (ix+PARTICLE_prev_colour),a
        ; Clip if needed, we do a rough clip to the entire viewport.
        ; For now, X = 0..320, so clipping region is 0..312 ,Y=0..247
        ld de,(ix+PARTICLE_Y)
        ld b,FIXED_POINT_BITS
        bsra de,b
        ld a,e
        cp (PARTICLE_LAYER_WIDTH-PARTICLE_SAFE_AREA-1)&$ff        ; 7
        jp c,@clipped                                            ; 12
        ld hl,de
        ld de,(ix+PARTICLE_X)
        bsra de,b
        ; here, HL = X coordinate
        ;       DE = Y coordinate

        ;call _get_random
        ;ld (ix+PARTICLE_colour),a        ; **DEBUG**
        ; Now calculate screen position
        ; Page in the correct bank. Each bank is 8KB, but we page it in to
        ; SWAP_BANK_0 and SWAP_BANK_1; this allows us to not have to worry
        ; about crossing a bank boundary when rendering.
        ld a,d
        sub (PARTICLE_LAYER_WIDTH-PARTICLE_SAFE_AREA-1)>>8
        jr c,@not_clipped
        ld a,e        
        cp (PARTICLE_LAYER_WIDTH-PARTICLE_SAFE_AREA-1)&$ff
        jr nc,@clipped
        ; Here, DE holds X coordinate, which has been clipped.
@not_clipped:
        ; PIXELADDR = (x*256)+y (240x320 mode)

        ; Either x or y negative, then they're clipped.
        ld a,h
        or d
        rlc a
        jr c,@clipped
        ;
        ; DE - x coordinate
        ; HL - y coordinate
        ; Figure out which 8K page we should be accessing
        ; We need to get bits 17:14 of the address, we extract this from bits 9:6 of the x-coordinate
        ;    y comes from bits ...._...._yyyy_yyyy 7..0 of hl mask 0000_0000_1111_1111
        ;    x comes from bits 12..0 of de mask 0001_1111_1111_1111
        ; page comes from bits  9..6 of de mask 0000_00xx_xx00_0000
        ; Final address 110x_xxxx_yyyy_yyyy
        ;                ...4_3210_7654_3210
        ;                0000_xxxx
        ; Page:            ...._8765
        ; 
        ; Page:
        ;                0000_deee_llll_llll
        ;                 ...._0765
        ; Final address 110e_eeee_llll_llll
        ;                ...4_3210_7654_3210
        ;    
        ; Figure out which page
        ld bc,de
        ex de,hl
        ld a,d
        add hl,hl                                        ; shift it up 2 bits, to move 9:6 to 11:8
        add hl,hl                                        ; and extract from there.
        add hl,hl                                        ; and extract from there.
        ex de,hl
        ld a,d
        and %00001111
        add a,LAYER_2_PAGE
        ld de,bc
        ld b,a                                            ; B now holds 8K page to use
        ld a,e
        
        and %00011111
        add SWAP_BANK_0>>8

        ld d,a
        ld e,l
        ; Figure out the offset within the bank
        ld a,b                                                ; Bank
        ld b,(ix+PARTICLE_colour)                            ; Colour
        ld c,(ix+PARTICLE_width)                            ; Size
        ld (ix+PARTICLE_prev_page),a                        ; Set up for restore
        ld (ix+PARTICLE_prev_address),de
        call xor_particle
        ex af,af'
        exx
        ret
@clipped:
        ex af,af'
        ld (ix+PARTICLE_flags),0
        exx
        ret


; 6x6 is ~ 818 cycles, ~ 82uS (  9 per ms)
; 5x5 is ~ 572 cycles, ~ 61uS ( 12 per ms)
; 4x4 is  ~370 cycles, ~ 44uS ( 19 per ms)
; 3x3 is  ~212 cycles, ~ 29uS ( 32 per ms)
; 2x2 is  ~ 98 cycles, ~ 18uS ( 90 per ms)
; 1x1 is  ~ 28 cycles, ~ 11uS (200 per ms)

; Scary macro because there's no loops, only recursion
; Row is 22*hpixels cycles
DO_ROW MACRO hpixels
        ld a,(de)                    ; 7
        xor b                        ; 4
        ld (de),a                    ; 7
        IF hpixels > 1
            inc e                    ; 4
        ENDIF
        IF hpixels > 1
            DO_ROW hpixels-1
        ENDIF
    ENDM

; (22*hpixels+8)*hpixels
DO_PIXELS MACRO hpixels,vpixels
    DO_ROW hpixels
    IF vpixels > 1
        ld e,c
        inc d
    ENDIF
    IF vpixels > 1
        DO_PIXELS hpixels,vpixels-1
    ENDIF
    ret
    ENDM
; INPUT:
; a - mmu slot
; c - Size (1..8) - 0 doesn't make sense and will result in 8.
; b - Colour
; de - Address
; 
; h,l only available
xor_particle:
        ld l,a
        ld a,(_particle_slot)
        cp l
        jr z,@same_slot
        ld a,l
        ld (_particle_slot),a
        nextreg MMU_SLOT_6,a
        inc a
        nextreg MMU_SLOT_7,a
@same_slot:
        ld a,c
        dec a
        and 7
        add a
        ld hl,@index_table
        or l
        ld l,a
        ld c,(hl)
        inc l
        ld h,(hl)
        ld l,c
        ld c,e
        ; on entry to the macro expanded routine:
        ; DE - Screen address
        ;  B - Colour
        ;  C - Copy of E
        jp (hl)
        ALIGN 16
@index_table:
        dw @one_pixel,@two_pixel,@three_pixel,@four_pixel,@five_pixel,@six_pixel,@seven_pixel,@eight_pixel

@eight_pixel:
        DO_PIXELS 8,8
@seven_pixel:
        DO_PIXELS 7,7
@six_pixel:
        DO_PIXELS 6,6
@five_pixel:
        DO_PIXELS 5,5
@four_pixel:
        DO_PIXELS 4,4
@three_pixel:
        DO_PIXELS 3,3
@two_pixel:
        DO_PIXELS 2,2
@one_pixel:
        DO_PIXELS 1,1
@zero_pixel:
        ret

        SECTION data_user
_particle_slot:
        dw 0
_particle_index:
        dw 0

particles_active:
        db 0
        SECTION bss_user
_particle_objects:
        ds PARTICLE_sizeof*MAX_PARTICLES
