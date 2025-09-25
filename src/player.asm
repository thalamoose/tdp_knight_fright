initialize_player:
		ld hl,player_object
		ld bc,PLAYER
		ld a,0
		call fill_mem
		ld ix,player_object
		ld (ix+PLAYER.object.X),320/2
		ld (ix+PLAYER.object.Y),256/2
		ld (ix+PLAYER.direction),0
		ld (ix+PLAYER.object.frame_index),0
		ld (ix+PLAYER.object.base_index),32			; Max of 256 frames of animation for the player
		ld (ix+PLAYER.object.anim_delay),4
		ld (ix+PLAYER.object.anim_speed),4
		ld (ix+PLAYER.object.total_frames),8
		;
		; Set up sprites 64..67, so that only the minimum needs to be set up below. 
		;
		; This is going to be a 4x4 sprite. The center point is in the top left corner
		; of the lower right sprite. These are linked sprites.
		nextreg SPRITE_INDEX,PLAYER_SPRITE_SLOT
		nextreg SPRITE_ATTR_0,0										; X7 X6 X5 X4 X3 X2 X1 X0
		nextreg SPRITE_ATTR_1,0										; Y7 Y6 Y5 Y4 Y3 Y2 Y1 Y0
		nextreg SPRITE_ATTR_2,%00000000								; P3 P2 P1 P0 XM YM R  X8
		nextreg SPRITE_ATTR_3,%11000000|PLAYER_SPRITE_PATTERN  		; V  E  N5 N4 N3 N2 N1 N0
		nextreg SPRITE_ATTR_4,%00000000  							; H  N6 T  X  X  Y  Y  Y8

		nextreg SPRITE_INDEX,PLAYER_SPRITE_SLOT+1
		nextreg SPRITE_ATTR_0,16									; X7 X6 X5 X4 X3 X2 X1 X0
		nextreg SPRITE_ATTR_1,0										; Y7 Y6 Y5 Y4 Y3 Y2 Y1 Y0
		nextreg SPRITE_ATTR_2,%00000000								; P3 P2 P1 P0 XM YM R  X8
		nextreg SPRITE_ATTR_3,%11000000|PLAYER_SPRITE_PATTERN+1		; V  E  N5 N4 N3 N2 N1 N0
		nextreg SPRITE_ATTR_4,%01100000  							; H  N6 T  X  X  Y  Y  Y8

		nextreg SPRITE_INDEX,PLAYER_SPRITE_SLOT+2
		nextreg SPRITE_ATTR_0,0										; X7 X6 X5 X4 X3 X2 X1 X0
		nextreg SPRITE_ATTR_1,16									; Y7 Y6 Y5 Y4 Y3 Y2 Y1 Y0
		nextreg SPRITE_ATTR_2,%00000000								; P3 P2 P1 P0 XM YM R  X8
		nextreg SPRITE_ATTR_3,%11000000|PLAYER_SPRITE_PATTERN+2		; V  E  N5 N4 N3 N2 N1 N0
		nextreg SPRITE_ATTR_4,%01100000  							; H  N6 T  X  X  Y  Y  Y8

		nextreg SPRITE_INDEX,PLAYER_SPRITE_SLOT+3
		nextreg SPRITE_ATTR_0,16									; X7 X6 X5 X4 X3 X2 X1 X0
		nextreg SPRITE_ATTR_1,16									; Y7 Y6 Y5 Y4 Y3 Y2 Y1 Y0
		nextreg SPRITE_ATTR_2,%00000000								; P3 P2 P1 P0 XM YM R  X8
		nextreg SPRITE_ATTR_3,%11000000|PLAYER_SPRITE_PATTERN+3		; V  E  N5 N4 N3 N2 N1 N0
		nextreg SPRITE_ATTR_4,%01100000  							; H  N6 T  X  X  Y  Y  Y8
		ld hl,player_palette
        ld a,%010                        							; Sprite palette 0
        call copy_palette

		nextreg MMU_SLOT_6,PLAYER_ANIM_PAGE
		ld a,(SWAP_BANK_0)
		nextreg TRANS_SPRITE_INDEX,a
		ret

update_player:
		ld ix,player_object
		call update_player_movement
		call update_player_animation
		ret

update_player_movement:
        ld a,(joystick_buttons)
        ld bc,(ix+PLAYER.object.X)
        bit JOYPAD_L_LEFT,a
        jr z,.no_dec_x
        dec bc
.no_dec_x
        bit JOYPAD_L_RIGHT,a
        jr z,.no_inc_x
        inc bc
.no_inc_x
        ld (ix+PLAYER.object.X),bc
        ld bc,(ix+PLAYER.object.Y)
        bit JOYPAD_L_UP,a
        jr z,.no_dec_y
        dec bc
.no_dec_y
        bit JOYPAD_L_DOWN,a
        jr z,.no_inc_y
        inc bc
.no_inc_y
        ld (ix+PLAYER.object.Y),bc

        ld de,OBJECT
        add ix,de

        ld a,(ix+PLAYER.object.X)
        add 1
        ld (ix+PLAYER.object.X),a
        ld a,(ix+PLAYER.object.X+1)
        adc 0
.no_wrap        
        ld (ix+PLAYER.object.X+1),a
        cp $01
        jr nz,.no_reset_x
        ld a,(ix+PLAYER.object.X)
        cp $40
        jr nz,.no_reset_x
        ret nz
        ld (ix+PLAYER.object.X),0
        ld (ix+PLAYER.object.X+1),0
.no_reset_x
        add ix,de
        inc (ix+PLAYER.object.Y)
        ret

; Set up the correct animation frame for the base object animations
update_player_animation:
		ld ix,player_object
		dec (ix+PLAYER.object.anim_delay)
		ret p
		ld a,(ix+PLAYER.object.anim_speed)
		ld (ix+PLAYER.object.anim_delay),a
		ld a,(ix+PLAYER.object.frame_index)
		inc a
		cp (ix+PLAYER.object.total_frames)
		jr c,.no_wrap
		xor a
.no_wrap:
		ld (ix+PLAYER.object.frame_index),a		
		ret

; Uploads correct frame to the player sprite memory slot. Player is always sprite pattern 0..3
render_player:
		ld ix,player_object
		ld a,(ix+PLAYER.object.base_index)
		add (ix+PLAYER.object.frame_index)
		; We have 8KB per page, this is 8 frames of 32x32 sprites.
		ld h,a
		srl a
		srl a
		srl a
		add PLAYER_ANIM_PAGE
		nextreg MMU_SLOT_6,a
		ld a,h
		and 7
		add a
		add a
		or (SWAP_BANK_0>>8)
		ld h,a
		ld l,0
		ld a,PLAYER_SPRITE_PATTERN								; Always sprite pattern 0
		ld e,4													; 4 16x16 sprites to transfer
		call sprcpy_dma
		; Now update position of player sprite. Note that sprite 64 is relative to 65,66,67.
		nextreg SPRITE_INDEX,PLAYER_SPRITE_SLOT
		ld a,(ix+PLAYER.object.X)
		nextreg SPRITE_ATTR_0,a
		ld a,(ix+PLAYER.object.Y)
		nextreg SPRITE_ATTR_1,a
		ld a,(ix+PLAYER.object.X+1)
		and %00000001
		nextreg SPRITE_ATTR_2,a

		ret