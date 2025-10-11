initialize_interrupts:
        ld a,interrupt_vector_table>>8
        ld i,a
        im 2
        ei
        ret

; Interrupts must be aligned so that:
; interrupt vector routine address high byte and low byte must be the same
; interrupt vector table is 257 bytes in size, due to a random vector between
; 0..255 is presented as the low address of the vector to use.
;

        ALIGN 256
        block ($>>8)
interrupt_handler:
        push af,hl
        ld hl,vsync_counter
        inc (hl)
        jr nz,.nowrap
        inc l
        inc (hl)
        jr nz,.nowrap
        inc l
        inc (hl)
        jr nz,.nowrap
        inc l
        inc (hl)
.nowrap
        pop hl,af
        ei
        reti
        ALIGN 256
interrupt_vector_table:
        block 257,(interrupt_handler>>8)

