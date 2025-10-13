;
; Interface to C code.
;
        mmu 6,C_CODE_PAGE+0
        mmu 6,C_CODE_PAGE+1
        org CODE_BANK_0
		include "build/c_code.map"
        incbin "build/c_code.bin"
