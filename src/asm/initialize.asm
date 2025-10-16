        include "include/hardware.inc"
        include "include/memorymap.inc"
        SECTION code_user

        global _ConfigureMemory

; Initial memory map:
; Work      $0000..$3fff
; Screen    $4000..$7fff [mapped to pages 14,15]
; Code      $8000..$bfff [mapped to pages 4,5]
; Paged     $c000..$ffff [mapped as needed]

_ConfigureMemory:
        nextreg MMU_SLOT_2,ULA_SHADOW_PAGE
        nextreg MMU_SLOT_3,ULA_SHADOW_PAGE+1
        ld bc,MEM_PAGING_CONTROL_PORT
        ret

