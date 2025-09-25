initialize_npcs:
		ld hl,animated_objects
		ld bc,OBJECT*MAX_OBJECTS
		ld a,0
		call fill_mem

		; Just set up the first sprite for now.
        ld ix,animated_objects
        ld (ix+OBJECT.X),128
        ld (ix+OBJECT.Y),96
        ld (ix+OBJECT.frame_index),0
        ld (ix+OBJECT.base_index),0
        ld (ix+OBJECT.total_frames),1
        ld (ix+OBJECT.flags),(1<<ANIM_ACTIVE)
        ld (ix+OBJECT.anim_speed),5
        ld (ix+OBJECT.anim_delay),0
        ld de,OBJECT
        add ix,de

        ld (ix+OBJECT.X),160
        ld (ix+OBJECT.Y),120
        ld (ix+OBJECT.frame_index),0
        ld (ix+OBJECT.base_index),1
        ld (ix+OBJECT.total_frames),3
        ld (ix+OBJECT.flags),(1<<ANIM_ACTIVE)
        ld (ix+OBJECT.anim_speed),5
        ld (ix+OBJECT.anim_delay),0
        add ix,de

        ld (ix+OBJECT.X),120
        ld (ix+OBJECT.Y),160
        ld (ix+OBJECT.frame_index),0
        ld (ix+OBJECT.base_index),0
        ld (ix+OBJECT.total_frames),8
        ld (ix+OBJECT.flags),(1<<ANIM_ACTIVE)|(1<<ANIM_PINGPONG)
        ld (ix+OBJECT.anim_speed),10
        ld (ix+OBJECT.anim_delay),0
        add ix,de

		ret

update_npcs:
		ret