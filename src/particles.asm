; Particle coordinates are 16 bit, fixed point 10..6 format
; Particles are rendered as square, currently xored with the
; current contents. However, in the future, we can or in the
; lower 2 bits, so it defines the colour, then we can use this
; to mask out the colour behind the banner, and clear it later
; by anding with %1111_1100.
; 

;
FIXED_POINT_BITS equ 6
FIXED_POINT_SPEED_BITS equ 4
F_4.4_TO_10.6 MACRO r1,r2
		ld r2,r1
		ld a,r1
		rla
		sbc a,a
		ld r1,a
		sla r2        ; shift low, old L7 -> carry
		rl  r1        ; shift high with carry from L
		ENDM
initialize_particles:
		;;;;
		xor a
		ld (particle_index),a
		ld b,MAX_PARTICLES
		ld ix,particle_objects
		ld de,PARTICLE
.init_loop:
		ld (ix+PARTICLE.flags),0				; Mark it inactive
		ld (ix+PARTICLE.prev_page),0			; Mark it so it doesn't need restoring.
		add ix,de
		djnz .init_loop
		ret

debug_add_particle:
        ld hl,160-16
        call get_random
        and 0x07
        add l
        ld l,a
        ld de,64
        call get_random
        and 0x07
        add e
        ld e,a

        call get_random
        sra a
        sra a
        inc a
        ld b,a
        call get_random
        sra a
        sra a
        inc a
        ld c,a
        call add_particle
        ret
update_particles:
		ld ix,particle_objects
		ld de,PARTICLE
		ld b,MAX_PARTICLES
		ld h,0
.update_loop
		bit PARTICLE_ACTIVE,(ix+PARTICLE.flags)
		jr z,.not_active
		exx
		call update_particle
		exx
		inc h
.not_active
		add ix,de
		djnz .update_loop
		ld a,h
		ld (particles_active),a
		ret

; IX is assumed to be on a 64 byte boundary.
; 220 cycles per update. vs 257 on old method
update_particle:
        ; X += VX
		ld de,ix
		ld hl,de
		ld c,(hl)				; Load VX
		inc l
		ld b,(hl)
		inc l
		ld e,(hl)				; Load X
		inc l
		ld d,(hl)
        add de,bc				; X=X+VX
		ld (hl),d				; Store X
		dec l
		ld (hl),e
		inc l
		inc l

        ; Y += VY
		ld a,l					; Save offset to VY
		ld c,(hl)				; Load VY
		inc l
		ld b,(hl)
		inc l
		ld e,(hl)				; Load Y
		inc l
		ld d,(hl)
        add de,bc				; Y=Y+VY
		ld (hl),d				; Store Y
		dec l
		ld (hl),e
		inc l
		inc l

		add bc,GRAVITY
		ld l,a					; Store back in VY
		ld (hl),c
		inc l
		ld (hl),b
		inc l
		inc l
		inc l
        ; Decrement life
		dec (hl)
		ret nz					; If not dead, return
		ld (ix+PARTICLE.life),0
		ret

; in: HL = start X, DE = start Y
; B = delta x (signed) 4.4 format
; C = delta y (signed) 4.4 format
; destroys: AF, BC
add_particle:
		exx
		ld a,(particle_index)
		ld e,a
		ld d,PARTICLE
		mul d,e
		ld ix,particle_objects
		add ix,de
		ld b,MAX_PARTICLES
		ld de,PARTICLE
.find_slot
		bit PARTICLE_ACTIVE,(ix+PARTICLE.flags)
		jr z,.found
		add ix,de
		inc a
		cp MAX_PARTICLES
		jr nz,.no_wrap
		xor a
		ld ix,particle_objects
.no_wrap
		djnz .find_slot
		nop
.found
		call remove_particle
		exx
		push de

		F_4.4_TO_10.6 b,d
		ld (ix+PARTICLE.VX),d
		ld (ix+PARTICLE.VX+1),b

		F_4.4_TO_10.6 c,d
		ld (ix+PARTICLE.VY),d
		ld (ix+PARTICLE.VY+1),c

		ld de,hl
		ld b,FIXED_POINT_BITS
		bsla de,b
		ld (ix+PARTICLE.X),de
		pop de
		bsla de,b
        ld (ix+PARTICLE.Y),de
		call get_random
		ld (ix+PARTICLE.colour),a

		call get_random
		and %00000011
		inc a
		ld (ix+PARTICLE.width),a
        ; Give it some initial velocity
		call get_random
		and %01111111
		add 100
        ld (ix+PARTICLE.life),a    ; frames of life - 5 seconds for testing
		ld a,b
		set PARTICLE_ACTIVE,(ix+PARTICLE.flags)
		ld a,(particle_index)
		inc a
		and MAX_PARTICLES-1
		ld (particle_index),a
		ret

remove_particle:
		ld a,(ix+PARTICLE.prev_page)
		and a
		jr z,.no_restore
		ld de,(ix+PARTICLE.prev_address)
		ld b,(ix+PARTICLE.prev_colour)
		ld c,(ix+PARTICLE.width)
		call xor_particle
		ld (ix+PARTICLE.prev_page),0
.no_restore:
		ret


render_particles:
		ld ix,particle_objects
		ld b,MAX_PARTICLES
		ld c,0
		ld de,PARTICLE
		xor a
		ld (particle_slot),a
.render_loop:
		bit PARTICLE_ACTIVE,(ix+PARTICLE.flags)
		jr z,.not_active
		call render_particle
		inc c
.not_active:
		add ix,de
		djnz .render_loop

		ret


; IX - particle, preserves all registers by using
; alternate set.
render_particle:
		exx
		ex af,af'
		; Remove previous occurance.
		call remove_particle
		ld a,(ix+PARTICLE.colour)
		ld (ix+PARTICLE.prev_colour),a
		; Clip if needed, we do a rough clip to the entire viewport.
		; For now, X = 0..320, so clipping region is 0..312 ,Y=0..247
		ld de,(ix+PARTICLE.Y)
		ld b,FIXED_POINT_BITS
		bsra de,b
		ld a,e
		cp (PARTICLE_LAYER_WIDTH-PARTICLE_SAFE_AREA-1)&$ff		; 7
		jp c,.clipped											; 12
		ld hl,de
		ld de,(ix+PARTICLE.X)
		bsra de,b
		; here, HL = X coordinate
		;       DE = Y coordinate

		;call get_random
		;ld (ix+PARTICLE.colour),a		; **DEBUG**
		; Now calculate screen position
		; Page in the correct bank. Each bank is 8KB, but we page it in to
		; SWAP_BANK_0 and SWAP_BANK_1; this allows us to not have to worry
		; about crossing a bank boundary when rendering.
		ld a,d
		sub (PARTICLE_LAYER_WIDTH-PARTICLE_SAFE_AREA-1)>>8
		jr c,.not_clipped
		ld a,e		
		cp (PARTICLE_LAYER_WIDTH-PARTICLE_SAFE_AREA-1)&$ff
		jr nc,.clipped
		; Here, DE holds X coordinate, which has been clipped.
.not_clipped:
		; PIXELADDR = (x*256)+y (240x320 mode)

		; Either x or y negative, then they're clipped.
		ld a,h
		or d
		rlc a
		jr c,.clipped
		;
		; DE - x coordinate
		; HL - y coordinate
		; Figure out which 8K page we should be accessing
		; We need to get bits 17:14 of the address, we extract this from bits 9:6 of the x-coordinate
		;    y comes from bits ...._...._yyyy_yyyy 7..0 of hl mask 0000_0000_1111_1111
		;    x comes from bits 12..0 of de mask 0001_1111_1111_1111
		; page comes from bits  9..6 of de mask 0000_00xx_xx00_0000
		; Final address 110x_xxxx_yyyy_yyyy
		;				...4_3210_7654_3210
		;				0000_xxxx
		; Page:			...._8765
		; 
		; Page:
		;				0000_deee_llll_llll
		;     			...._0765
		; Final address 110e_eeee_llll_llll
		;				...4_3210_7654_3210
		;	
		; Figure out which page
		ld bc,de
		ex de,hl
		ld a,d
		add hl,hl										; shift it up 2 bits, to move 9:6 to 11:8
		add hl,hl										; and extract from there.
		add hl,hl										; and extract from there.
		ex de,hl
		ld a,d
		and %00001111
		add a,LAYER_2_PAGE
		ld de,bc
		ld b,a											; B now holds 8K page to use
		ld a,e
		
		and %00011111
		add SWAP_BANK_0>>8

		ld d,a
		ld e,l
		; Figure out the offset within the bank
		ld a,b												; Bank
		ld b,(ix+PARTICLE.colour)							; Colour
		ld c,(ix+PARTICLE.width)							; Size
		ld (ix+PARTICLE.prev_page),a						; Set up for restore
		ld (ix+PARTICLE.prev_address),de
		call xor_particle
		ex af,af'
		exx
		ret
.clipped:
		ex af,af'
		ld (ix+PARTICLE.flags),0
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
		ld a,(de)					; 7
		xor b						; 4
		ld (de),a					; 7
		IF hpixels > 1
			inc e					; 4
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
		ld a,(particle_slot)
		cp l
		jr z,.same_slot
		ld a,l
		ld (particle_slot),a
		nextreg MMU_SLOT_6,a
		inc a
		nextreg MMU_SLOT_7,a
.same_slot
		ld a,c
		dec a
		and 7
		add a
		ld hl,.index_table
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
.index_table:
		dw .one_pixel,.two_pixel,.three_pixel,.four_pixel,.five_pixel,.six_pixel,.seven_pixel,.eight_pixel

.eight_pixel:
		DO_PIXELS 8,8
.seven_pixel:
		DO_PIXELS 7,7
.six_pixel:
		DO_PIXELS 6,6
.five_pixel:
		DO_PIXELS 5,5
.four_pixel:
		DO_PIXELS 4,4
.three_pixel:
		DO_PIXELS 3,3
.two_pixel:
		DO_PIXELS 2,2
.one_pixel:
		DO_PIXELS 1,1
.zero_pixel:
		ret

