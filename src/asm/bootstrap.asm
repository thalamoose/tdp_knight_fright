		ld hl,entry_point
		jp call_asm_func
; HL - address
call_c_func = $
		nextreg MMU_SLOT_4,C_CODE_PAGE
		nextreg MMU_SLOT_5,C_CODE_PAGE+1
		ld de,ret_c_func
		push de
		jp (hl)
		
ret_c_func = $
		nextreg MMU_SLOT_4,ASM_CODE_PAGE
		nextreg MMU_SLOT_5,ASM_CODE_PAGE+1
		ret
call_asm_func = $
		nextreg MMU_SLOT_4,ASM_CODE_PAGE
		nextreg MMU_SLOT_5,ASM_CODE_PAGE+1
		ld de,ret_asm_func
		push de
		jp (hl)
		
ret_asm_func = $
		nextreg MMU_SLOT_4,C_CODE_PAGE
		nextreg MMU_SLOT_5,C_CODE_PAGE+1
		ret
entry_point = $
