		ld a,ASM_CODE_PAGE
		ld hl,entry_point
; A - Page
; HL - address
call_banked_func = $

		nextreg MMU_SLOT_4,a
		inc a
		nextreg MMU_SLOT_5,a
		jp (hl)
entry_point = $
