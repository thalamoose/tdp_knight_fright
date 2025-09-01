animate_objects:
        ld ix,animated_objects
        ld b,MAX_OBJECTS
.anim_loop
        ld d,(ix+OBJECT.flags)
        ld e,(ix+OBJECT.frame_index)
        bit ANIM_ACTIVE,d                               ; Not animating, skip
        jr z,.not_active
        ld a,(ix+OBJECT.anim_delay)
        and a
        jr z,.reload_anim
        dec (ix+OBJECT.anim_delay)
        jr .frame_update_done
.reload_anim
        ld a,(ix+OBJECT.anim_speed)                     ; Reload animation speed
        ld (ix+OBJECT.anim_delay),a
        bit ANIM_DIRECTION,d                            ; Are we counting down?
        jr nz,.going_down
        inc e                                           ; We're counting up, so increment frame number
        ld a,(ix+OBJECT.total_frames)                     ; Check if it's maxed out
        and a                                           ; Validity check
        jr z,.invalid_frame_count
        cp e                                            ; End of animation?
        jr nz,.frame_update_done
        bit ANIM_PINGPONG,d                             ; Are we ping-ponging?
        jr z,.reset_frame
        set ANIM_DIRECTION,d                            ; Now counting down
        ld e,(ix+OBJECT.total_frames)
        dec e                                           ; Back to previous frame
.frame_update_done
        ld (ix+OBJECT.flags),d
        ld (ix+OBJECT.frame_index),e
.not_active
        ld de,OBJECT.size
        add ix,de
        djnz .anim_loop
        ret
.going_down
        dec e
        jr nz,.frame_update_done
        res ANIM_DIRECTION,d
.reset_frame
        ld e,0
        jr .frame_update_done
.invalid_frame_count
        call print_str
        db "Invalid frame count",0
.abort  
        halt
        jr .abort
