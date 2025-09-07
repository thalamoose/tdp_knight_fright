; Particle coordinates are 16 bit, fixed point 9..7 format
; Particles are rendered as square, currently xored with the
; current contents. However, in the future, we can or in the
; lower 2 bits, so it defines the colour, then we can use this
; to mask out the colour behind the banner, and clear it later
; by anding with %1111_1100.
; 

;           abcd-efgh
; aaaa-abcd.efgh-0000

; Input, r1=4.4 format
; Output r1:r2=7.9 format, sign extended
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
        ld hl,(ix+PARTICLE.VY)
        ld de,GRAVITY
        add hl,de
        ld (ix+PARTICLE.VY),hl
        ; Decrement life
		dec (ix+PARTICLE.life)
		jr z,.dead
		exx
		ret
.dead
		exx
restart_particle:
		push bc,de,hl,ix,af
		call remove_particle
		ld (ix+PARTICLE.flags),0
		ld (ix+PARTICLE.prev_page),0
		call debug_add_particle
		pop af,ix,hl,de,bc
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
		call xor_particle
		ld (ix+PARTICLE.prev_page),0
.no_restore:
		ret


render_particles:
		ld ix,particle_objects
		ld b,MAX_PARTICLES
		ld c,0
		ld de,PARTICLE.size
.render_loop:
		bit PARTICLE_ACTIVE,(ix+PARTICLE.flags)
		jr z,.not_active
		call render_particle
		inc c
.not_active:
		add ix,de
		djnz .render_loop

		ld a,7
		out (ULA_PORT),a
		push bc
		call print_str
		db "A:%d \n\r",0
		ld a,0
		ld (ULA_PORT),a
		ret


; IX - particle, preserves all registers by using
; alternate set.
render_particle:
		exx
		ex af,af'

		; Remove previous occurance.
		call remove_particle
		; Clip if needed, we do a rough clip to the entire viewport.
		; For now, X = 0..320, so clipping region is 0..312 ,Y=0..247
		ld de,(ix+PARTICLE.X)
		ld b,FIXED_POINT_BITS
		bsra de,b
		ld a,d
		and a
		jr z,.not_above
		nop
.not_above:
		push de
		ld hl,PARTICLE_LAYER_WIDTH-PARTICLE_SAFE_AREA-1
		and a
		sbc hl,de

		jp c,.clipped_pop_1
		ld de,(ix+PARTICLE.Y)
		bsra de,b
		push de
		ld hl,PARTICLE_LAYER_HEIGHT-PARTICLE_SAFE_AREA-1
		and a
		sbc hl,de
		jp c,.clipped_pop_2
		ld a,(ix+PARTICLE.colour)
		ld (ix+PARTICLE.prev_colour),a
		;call get_random
		;ld (ix+PARTICLE.colour),a		; **DEBUG**
		; Now calculate screen position
		; Page in the correct bank. Each bank is 8KB, but we page it in to
		; SWAP_BANK_0 and SWAP_BANK_1; this allows us to not have to worry
		; about crossing a bank boundary when rendering.
		pop hl		; X coordinate on screen (Flipped due to 320 mode, this would be Y in 256 mode)
		pop bc		; Y coordinate on screen (Flipped due to 320 mode, this would be X in 256 mode)

		ld a,h
		or b
		rlc a
		jp c,.clipped_pop_0
		ld de,bc
		add de,de
		add de,de
		add de,de
		ld a,d
		and %00001111
		add a,LAYER_2_PAGE
		ld b,a				; Page number, save for below
		; To figure out address within 8K page, take X coordinate and it with 7, then multiply by 256
		ld a,c
		and %00011111
		add a,SWAP_BANK_0>>8
		; L already contains the offset within the line
		ld h,a
		ld a,b
		ld e,(ix+PARTICLE.colour)
		ld c,(ix+PARTICLE.width)
		ld (ix+PARTICLE.prev_page),a
		ld (ix+PARTICLE.prev_address),hl
		call xor_particle
		ex af,af'
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

.clipped_pop_2:
		pop hl
.clipped_pop_1:
		pop hl
.clipped_pop_0:
		ex af,af'
		ld (ix+PARTICLE.flags),0
		exx
		ret

; 6x6 is ~1500 cycles, ~110uS (  9 per ms)
; 5x5 is ~1100 cycles, ~ 80uS ( 12 per ms)
; 4x4 is  ~720 cycles, ~ 52uS ( 19 per ms)
; 3x3 is  ~434 cycles, ~ 31uS ( 32 per ms)
; 2x2 is  ~148 cycles, ~ 11uS ( 90 per ms)
; 1x1 is   ~70 cycles, ~  5uS (200 per ms)
; hl - screen address
; e - colour
; c - width
; a - mmu page
xor_particle:
		cp LAYER_2_PAGE
		jr c,xor_particle
		cp LAYER_2_PAGE+10
		jr nc,xor_particle
		nextreg MMU_SLOT_6,a
		inc a
		nextreg MMU_SLOT_7,a
		ld d,c
.y_loop:
		ld b,c
.x_loop:
		ld a,(hl)
		xor e
		ld (hl),a
		inc hl
		djnz .x_loop
		sub hl,bc
		inc h				; On to the next line, it's easy, as it's always 256 bytes
		dec d
		jr nz,.y_loop
		ret