initialize_system:
again:

        nextreg CLOCK_SEL,$03                 ; 28Mhz
        nextreg PERIPHERAL_3_CONTROL,%01110000
        call configure_memory
        call initialize_interrupts
        call clear_screen
        call print_str
        db "Booting KnightFright...\r\n"
        db __TIME__," ",__DATE__,"\r\n",0
        call initialize_display
        call initialize_audio
        call initialize_filesystem
        ret
initialize_game:
        call initialize_sprites
        call initialize_tilemap
        call initialize_particles
        call initialize_player
        call initialize_baddies

        ret

; Initial memory map:
; Work      $0000..$3fff
; Screen    $4000..$7fff [mapped to pages 14,15]
; Code      $8000..$bfff [mapped to pages 4,5]
; Paged     $c000..$ffff [mapped as needed]

configure_memory:
        nextreg MMU_SLOT_2,ULA_SHADOW_PAGE
        nextreg MMU_SLOT_3,ULA_SHADOW_PAGE+1
        ld bc,MEM_PAGING_CONTROL_PORT
        ret
