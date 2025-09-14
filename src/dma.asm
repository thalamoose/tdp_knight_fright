DMA_RESET                      equ $c3
DMA_RESET_PORT_A_TIMING        equ $c7
DMA_RESET_PORT_B_TIMING        equ $cb
DMA_LOAD                       equ $cf ; %11001111
DMA_CONTINUE                   equ $d3
DMA_DISABLE_INTERUPTS          equ $af
DMA_ENABLE_INTERUPTS           equ $ab
DMA_RESET_DISABLE_INTERUPTS    equ $a3
DMA_ENABLE_AFTER_RETI          equ $b7
DMA_READ_STATUS_BYTE           equ $bf
DMA_REINIT_STATUS_BYTE         equ $8b
DMA_START_READ_SEQUENCE        equ $a7
DMA_FORCE_READY                equ $b3
DMA_DISABLE                    equ $83
DMA_ENABLE                     equ $87
DMA_WRITE_REGISTER_COMMAND     equ $bb
DMA_BURST                      equ %11001101
DMA_CONTINUOUS                 equ %10101101
ZXN_DMA_PORT                   equ $6b
SPRITE_STATUS_SLOT_SELECT      equ $303B
SPRITE_IMAGE_PORT              equ $5b
SPRITE_INFO_PORT               equ $57
initialize_dma:
		ret

update_dma:
		ret



; HL - Source
; DE - Destination
; BC - Size
memcpy_dma:
		ld a,6
		out (ULA_PORT),a
		ld (memcpy_src),hl
		ld (memcpy_dst),de
		ld (memcpy_len),bc
		ld hl,memcpy_dma_xfer
		ld b,memcpy_dma_len
		ld c,ZXN_DMA_PORT
		ld a,5
		out (ULA_PORT),a
		otir
		ld a,0
		out (ULA_PORT),a
		ret
memcpy_dma_xfer:
 		db DMA_DISABLE
        db %01111101                ; R0-Transfer mode, A -> B, write adress 
                                      	; + block length
memcpy_src 	
		dw 0                        ; R0-Port A, Start address 
                                     	; (source address)
memcpy_len	
		dw 0                        ; R0-Block length (length in bytes)
        db %01010100                ; R1-write A time byte, increment, to 
                                      	; memory, bitmask
        db %00000010                ; 2t
        db %01010000                ; R2-write B time byte, increment, to 
                                      	; memory, bitmask
        db %00000010                ; R2-Cycle length port B
        db DMA_CONTINUOUS           ; R4-Continuous mode (use this for block 
                                      	; transfer), write dest adress
memcpy_dst	
		dw 0                       	; R4-Dest address (destination address)
        db %10000010                ; R5-Restart on end of block, RDY active 
                                      	; LOW
        db DMA_LOAD                 ; R6-Load
        db DMA_ENABLE               ; R6-Enable DMA
        
memcpy_dma_len equ $-memcpy_dma_xfer

; A - Sprite slot
; HL - Source data
; E - Slots to transfer (16x16)
sprcpy_dma:
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
		ret

sprcpy_dma_xfer:
		db DMA_DISABLE
        db %01111101                            ; R0-Transfer mode, A -> B, write adress + block length
sprcpy_src
	  	dw 0                                    ; R0-Port A, Start address                              (source address)
sprcpy_len
	  	dw 0                                    ; R0-Block length                                       (length in bytes)
        db %01010100                            ; R1-read A time byte, increment, to memory, bitmask
        db %00000010                            ; R1-Cycle length port A
        db %01101000                            ; R2-write B time byte, increment, to memory, bitmask
        db %00000010                            ; R2-Cycle length port B
        db DMA_CONTINUOUS                       ; R4-Continuous mode  (use this for block tansfer), write dest adress
        dw SPRITE_PATTERN_PORT                  ; R4-Dest address                                       (destination port)
        db %10000010                            ; R5-Restart on end of block, RDY active LOW
        db DMA_LOAD                             ; R6-Load
        db DMA_ENABLE                           ; R6-Enable DMA
sprcpy_dma_len equ $-sprcpy_dma_xfer

dma_code:
 		db DMA_DISABLE
        db %01111101                ; R0-Transfer mode, A -> B, write adress 
                                      	; + block length
dma_source 	
		dw 0                        ; R0-Port A, Start address 
                                     	; (source address)
dma_length	
		dw 0                        ; R0-Block length (length in bytes)
        db %01010100                ; R1-write A time byte, increment, to 
                                   	; memory, bitmask
        db %00000010                ; 2t
        db %01010000                ; R2-write B time byte, increment, to 
                                      	; memory, bitmask
        db %00000010                ; R2-Cycle length port B
        db DMA_CONTINUOUS           ; R4-Continuous mode (use this for block 
                                      	; transfer), write dest adress
dma_dest
	 	dw 0                       	; R4-Dest address (destination address)
        db %10000010                ; R5-Restart on end of block, RDY active 
                                      	; LOW
        db DMA_LOAD                 ; R6-Load
        db DMA_ENABLE               ; R6-Enable DMA
        
dma_code_length equ $-dma_code