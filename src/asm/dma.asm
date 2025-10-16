
        include "include/hardware.inc"
        SECTION code_user
initialize_dma:
        ret

update_dma:
        ret

        GLOBAL _memcpy
        GLOBAL _CopySprite

; HL - Source
; DE - Destination
; BC - Size
_memcpy:
        ld ix,0
        add ix,sp
        ld a,6
        out (ULA_PORT),a
        ld de,(ix+2)
        ld (memcpy_src),de
        ld de,(ix+4)
        ld (memcpy_dst),de
        ld de,(ix+6)
        ld (memcpy_len),de
        ld hl,memcpy_dma_xfer
        ld b,memcpy_dma_len
        ld c,ZXN_DMA_PORT
        ld a,5
        out (ULA_PORT),a
        otir
        ld a,0
        out (ULA_PORT),a
        ret

; A - Sprite slot
; HL - Source data
; E - Slots to transfer (16x16)
_CopySprite:
        push ix
        ld ix,2
        add ix,sp
        ld hl,(ix+2)
        ld a,(ix+4)
        ld e,(ix+5)

        ld bc,SPRITE_SLOT_PORT
        out (c),a
        ld a,6
        out (ULA_PORT),a
        ld a,e
        ld (sprcpy_len+1),a
        xor a
        ld (sprcpy_len),a
        ld (sprcpy_src),hl
        ld hl,sprcpy_dma_xfer
        ld b,sprcpy_dma_len
        ld c,ZXN_DMA_PORT
        ld a,5
        out (ULA_PORT),a
        otir
        ld a,0
        out (ULA_PORT),a
        pop ix
        ret

        SECTION data_user
sprcpy_dma_xfer:
        db DMA_DISABLE
        db %01111101                    ; R0-Transfer mode, A -> B, write adress + block length
sprcpy_src:
        dw 0                            ; R0-Port A, Start address                              (source address)
sprcpy_len:
        dw 0                            ; R0-Block length                                       (length in bytes)
        db %01010100                    ; R1-read A time byte, increment, to memory, bitmask
        db %00000010                    ; R1-Cycle length port A
        db %01101000                    ; R2-write B time byte, to port, bitmask
        db %00000010                    ; R2-Cycle length port B
        db DMA_CONTINUOUS               ; R4-Continuous mode  (use this for block tansfer), write dest adress
        dw SPRITE_PATTERN_PORT          ; R4-Dest address                                       (destination port)
        db %10000010                    ; R5-Restart on end of block, RDY active LOW
        db DMA_LOAD                     ; R6-Load
        db DMA_ENABLE                   ; R6-Enable DMA
sprcpy_dma_len: equ $-sprcpy_dma_xfer

memcpy_dma_xfer:
        db DMA_DISABLE
        db %01111101                    ; R0-Transfer mode, A -> B, write adress 
                                        ; + block length
memcpy_src:
        dw 0                            ; R0-Port A, Start address 
                                        ; (source address)
memcpy_len:
        dw 0                            ; R0-Block length (length in bytes)
        db %01010100                    ; R1-write A time byte, increment, to 
                                        ; memory, bitmask
        db %00000010                    ; 2t
        db %01010000                    ; R2-write B time byte, increment, to 
                                        ; memory, bitmask
        db %00000010                    ; R2-Cycle length port B
        db DMA_CONTINUOUS               ; R4-Continuous mode (use this for block 
                                        ; transfer), write dest adress
memcpy_dst:
        dw 0                            ; R4-Dest address (destination address)
        db %10000010                    ; R5-Restart on end of block, RDY active 
                                        ; LOW
        db DMA_LOAD                     ; R6-Load
        db DMA_ENABLE                   ; R6-Enable DMA
        
memcpy_dma_len equ $-memcpy_dma_xfer
