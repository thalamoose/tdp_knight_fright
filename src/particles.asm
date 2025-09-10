; Particle coordinates are 16 bit, fixed point 10..6 format
; Particles are rendered as square, currently xored with the
; current contents. However, in the future, we can or in the
; lower 2 bits, so it defines the colour, then we can use this
; to mask out the colour behind the banner, and clear it later
; by anding with %1111_1100.
; 

;
; Defining this to 1 causes it to XOR to the background
; Defining it to 0 causes it to be written directly to the background, and zero'd when cleared.
	DEFINE SLOW_PARTICLE 1
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
		ld de,PARTICLE.size
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
		ld de,PARTICLE.size
		ld b,MAX_PARTICLES
		ld h,0
.update_loop
		ld a,b
		and 7
		out (ULA_PORT),a
		bit PARTICLE_ACTIVE,(ix+PARTICLE.flags)
		jr z,.not_active
		call update_particle
		inc h
.not_active
		add ix,de
		djnz .update_loop
		ld a,h
		ld (particles_active),a
		ret

update_particle:
		exx
        ; X += VX
        ld hl,(ix+PARTICLE.X)
        ld de,(ix+PARTICLE.VX)
        add hl,de
        ld (ix+PARTICLE.X),hl

        ; Y += VY
        ld hl,(ix+PARTICLE.Y)
        ld de,(ix+PARTICLE.VY)
        add hl,de
        ld (ix+PARTICLE.Y),hl

        ; VY += GRAVITY (downward)
        add de,GRAVITY
        ld (ix+PARTICLE.VY),de
        ; Decrement life
		dec (ix+PARTICLE.life)
		jr z,.dead
		exx
		ret
.dead
		ld (ix+PARTICLE.life),0
		exx
		ret

; in: HL = start X, DE = start Y
; B = delta x (signed) 4.4 format
; C = delta y (signed) 4.4 format
; destroys: AF, BC
add_particle:
		exx
		ld a,(particle_index)
		ld e,a
		ld d,PARTICLE.size
		mul d,e
		ld ix,particle_objects
		add ix,de
		ld b,MAX_PARTICLES
		ld de,PARTICLE.size
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
		ld hl,(ix+PARTICLE.prev_address)
		ld e,(ix+PARTICLE.prev_colour)
		ld c,(ix+PARTICLE.width)
	if SLOW_PARTICLE == 0
		ld e,0
	endif
		call xor_particle
		ld (ix+PARTICLE.prev_page),0
.no_restore:
		ret


render_particles:
		ld ix,particle_objects
		ld b,MAX_PARTICLES
		ld c,0
		ld de,PARTICLE.size
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

		ld (ULA_PORT),a
		ret


; IX - particle, preserves all registers by using
; alternate set.
render_particle:
		exx
		ex af,af'
		ld a,6
		out (ULA_PORT),a
		; Remove previous occurance.
		call remove_particle
		ld a,6
		out (ULA_PORT),a
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
		; divide x by 32, this gives bank number.
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
		add de,de										; shift it up 2 bits, to move 9:6 to 11:8
		add de,de										; and extract from there.
		add de,de										; and extract from there.
		ld a,d
		and %00001111
		add a,LAYER_2_PAGE
		ld de,bc
		ld b,a											; B now holds 8K page to use
		ld a,e
		
		and %00011111
		add SWAP_BANK_0>>8

		ld h,a
		; Figure out the offset within the bank
		ld a,b												; Bank
		ld e,(ix+PARTICLE.colour)							; Colour
		ld c,(ix+PARTICLE.width)							; Size
		ld (ix+PARTICLE.prev_page),a						; Set up for restore
		ld (ix+PARTICLE.prev_address),hl
		call xor_particle
		ld a,0
		out (ULA_PORT),a
		ex af,af'
		exx
		ret
.clipped:
		ld a,0
		out (ULA_PORT),a
		ex af,af'
		ld (ix+PARTICLE.flags),0
		exx
		ret
.debug_info:
		push ix
		ld hl,(ix+PARTICLE.prev_address)
		push hl
		ld a,(ix+PARTICLE.prev_page)
		push af
		ld de,(ix+PARTICLE.Y)
		ld b,FIXED_POINT_BITS
		bsra de,b
		ld d,e
		push de
		ld de,(ix+PARTICLE.X)
		bsra de,b
		ld d,e
		push de
		call print_str
		db "x=%c, y=%c, page=%c, addr=%x\r\n",0
		pop ix
		ret

; 6x6 is ~1159 cycles, ~ 82uS (  9 per ms)
; 5x5 is ~ 862 cycles, ~ 61uS ( 12 per ms)
; 4x4 is  ~613 cycles, ~ 44uS ( 19 per ms)
; 3x3 is  ~412 cycles, ~ 29uS ( 32 per ms)
; 2x2 is  ~259 cycles, ~ 18uS ( 90 per ms)
; 1x1 is  ~154 cycles, ~ 11uS (200 per ms)
; hl - screen address
; e - colour
; c - width
; a - mmu page

DO_PARTICLE_PIXEL macro
	; SLOW PATH is 22 cycles
	; FAST PATH is 11 cycles
	if SLOW_PARTICLE != 0
		ld a,(hl)					; 7
		xor e						; 4
	endif
		ld (hl),a					; 7
		inc l						; 4
	endm
xor_particle:
		ld b,a
		ld a,(particle_slot)
		cp b
		jr z,.same_slot
		ld a,b
		ld (particle_slot),a
		nextreg MMU_SLOT_6,a
		inc a
		nextreg MMU_SLOT_7,a
.same_slot
		ld a,c
		and 3
		add 2
		out (ULA_PORT),a
		ld a,7
		sub c
		add a,a						; FOR 2 INSTRUCTIONS!!!!
	if SLOW_PARTICLE != 0
		add a,a						; FOR 4 INSTRUCTIONS!!!!
	endif
		ld (.y_loop+1),a			; I FEEL SO DIRTY! SELF MODIFYING CODE.
		ld b,c
		ld c,l

.y_loop:
		jr .zero_pixel
.seven_pixel:
		DO_PARTICLE_PIXEL
.six_pixel:
		DO_PARTICLE_PIXEL
.five_pixel:
		DO_PARTICLE_PIXEL
.four_pixel:
		DO_PARTICLE_PIXEL
.three_pixel:
		DO_PARTICLE_PIXEL
.two_pixel:
		DO_PARTICLE_PIXEL
.one_pixel:
		DO_PARTICLE_PIXEL
.zero_pixel:
		ld l,c
		inc h
		djnz .y_loop
		ld a,0
		out (ULA_PORT),a
		ret


