        include "include/hardware.inc"
        include "include/memorymap.inc"
        
        SECTION code_user
        SECTION code_compiler
        SECTION data_user
        SECTION data_compiler

        SECTION bss_user
        org $6000
        SECTION bss_compiler        
        global _ConfigureMemory

; Initial memory map:
; Work      $0000..$3fff
; Screen    $4000..$7fff [mapped to pages 14,15]
; Code      $8000..$bfff [mapped to pages 4,5]
; Paged     $c000..$ffff [mapped as needed]

        SECTION code_user
_ConfigureMemory:
        nextreg MMU_SLOT_2,ULA_SHADOW_PAGE
        nextreg MMU_SLOT_3,ULA_SHADOW_PAGE+1
        ld bc,MEM_PAGING_CONTROL_PORT
        ret

