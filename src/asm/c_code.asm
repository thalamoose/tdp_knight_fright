;
; Interface to C code.
;
        mmu 4,C_CODE_PAGE+0
        mmu 5,C_CODE_PAGE+1
        org CODE_BANK_0
		include "build/c_code.map"
        incbin "build/c_code.bin"
