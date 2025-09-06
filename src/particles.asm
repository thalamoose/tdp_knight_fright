; Particle coordinates are 16 bit, fixed point 9..7 format
; Particles are rendered as square, currently xored with the
; current contents. However, in the future, we can or in the
; lower 2 bits, so it defines the colour, then we can use this
; to mask out the colour behind the banner, and clear it later
; by anding with %1111_1100.
; 

I_TO_F MACRO r1,r2
		rr r1
		rr r2
		ld r1,r2
		ld r2,0
		rr r2
		ENDM

; input register pair,
; first register gets result
F_TO_I MACRO r1,r2
		rl r2
		rl r1
		ld r2,r1
		ld r1,0
		rl r1
		ENDM

;           abcd-efgh
; aaaa-abcd.efgh-0000

; Input, r1=4.4 format
; Output r1:r2=7.9 format, sign extended
F_44_TO_79 MACRO r1,r2
		ld r2,r1
		ld a,r1
		rla
		sbc a,a
		ld r1,a
		sla r2        ; shift low, old L7 -> carry
		rl  r1        ; shift high with carry from L
		sla r2
		rl  r1
		sla r2
		rl  r1
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
.update_loop
		ld a,b
		and 7
		out (ULA_PORT),a
		bit PARTICLE_ACTIVE,(ix+PARTICLE.flags)
		jr z,.not_active
		call update_particle
.not_active
		add ix,de
		djnz .update_loop
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
		call remove_particle
		exx
		push de
		F_44_TO_79 b,d
		ld (ix+PARTICLE.VX),d
		ld (ix+PARTICLE.VX+1),b

		F_44_TO_79 c,d
		ld (ix+PARTICLE.VY),d
		ld (ix+PARTICLE.VY+1),c

		pop de
		I_TO_F h,l
		I_TO_F d,e
        ld (ix+PARTICLE.X),hl
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
		ld de,PARTICLE.size
.render_loop:
		bit PARTICLE_ACTIVE,(ix+PARTICLE.flags)
		jr z,.not_active
		call render_particle
.not_active:
		add ix,de
		djnz .render_loop
		ret


;*TODO*
; *TODO*
;	Set up another page to flip the particle sources in to.
; Use the ULA workspace at $4000, make sure to page it back when
; done.
; IX - particle, preserves all registers by using
; alternate set.
render_particle:
		exx
		ex af,af'

		; Remove previous occurance.
		call remove_particle
		; Clip if needed, we do a rough clip to the entire viewport.
		; For now, X = 0..320, so clipping region is 0..312 ,Y=0..247
		ld hl,(ix+PARTICLE.X)
		F_TO_I h,l
		push hl
		ld de,LAYER_2_HEIGHT-LAYER_2_BORDER
		and a
		sbc hl,de
		jp nc,.clipped_pop_1
		ld hl,(ix+PARTICLE.Y)
		F_TO_I h,l
		push hl
		ld de,LAYER_2_WIDTH-LAYER_2_BORDER
		and a
		sbc hl,de
		jp nc,.clipped_pop_2
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
		jr c,.clipped_pop_0
		ld a,c
		rlca
		rlca
		rlca
		and %00000111
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
		ld bc,(ix+PARTICLE.Y)
		F_TO_I b,c
		ld b,c
		;push bc
		ld bc,(ix+PARTICLE.X)
		F_TO_I b,c
		ld b,c
		push bc
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
		exx
		call restart_particle
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
		cp LAYER_2_PAGE+8
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