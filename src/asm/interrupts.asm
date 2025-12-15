        SECTION code_user

        global _InitializeInterrupts
        global _vSyncCounter

_InitializeInterrupts:
        di
        ; Set up the vector table. Fixed at 0xfc00 to 0xfd01
        ; Then copy the actual dispatch to 0xfdfd
        ld hl,0xbe00
        ld a,h
        ld de,hl
        ld bc,256
        ld (hl),0xbf
        inc de
        ldir

        ld hl,interrupt_trampoline
        ld de,0xbfbf
        ld bc, 3
        ldir
        ld i,a
        im 2
        ei
        ret
interrupt_trampoline:
        jp InterruptHandler

; Interrupts must be aligned so that:
; interrupt vector routine address high byte and low byte must be the same
; interrupt vector table is 257 bytes in size, due to a random vector between
; 0..255 is presented as the low address of the vector to use.
;
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
        dw 0,0

