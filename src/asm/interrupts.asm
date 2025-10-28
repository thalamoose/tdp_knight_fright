        SECTION code_user

        global _InitializeInterrupts
        global _vSyncCounter
_InitializeInterrupts:
        ld a,InterruptVectorTable>>8
        ld i,a
        im 1
        ei
        ret

; Interrupts must be aligned so that:
; interrupt vector routine address high byte and low byte must be the same
; interrupt vector table is 257 bytes in size, due to a random vector between
; 0..255 is presented as the low address of the vector to use.
;

        ALIGN 256

        defs $/256,0
InterruptHandler:
        push af
        push hl
        ld hl,_vSyncCounter
        inc (hl)
        jr nz,@nowrap
        inc l
        inc (hl)
        jr nz,@nowrap
        inc l
        inc (hl)
        jr nz,@nowrap
        inc l
        inc (hl)
@nowrap:
        pop hl
        pop af
        ei
        reti
_vSyncCounter:
        dw 0

        ; 
        ; This really needs to be in a different place. Low memory ideally.
        ALIGN 256
        
InterruptVectorTable:
        defs 257,(InterruptHandler>>8)

