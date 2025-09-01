initialize_sprites:
        ld a,0
        ld b,MAX_SPRITES
.clear_next
        nextreg SPRITE_INDEX,a
        nextreg SPRITE_ATTR_3,0
        inc a
        djnz .clear_next
        ret

