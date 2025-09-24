initialize_player:
		ld hl,player_object
		ld bc,PLAYER
		ld a,0
		call fill_mem
		ld ix,player_object
		ld (ix+PLAYER.object.X),0
		ld (ix+PLAYER.object.Y),0
		ld (ix+PLAYER.direction),0
		ret

update_player:
		ld ix,player_object
		call update_player_movement
		call update_player_animation
		call animate_object
		ret

update_player_movement:
		ret

; Set up the correct animation frame for the base object animations
update_player_animation:
		ret
