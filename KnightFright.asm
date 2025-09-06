
    include "include/hardware.inc"
    include "include/defines.inc"
    include "include/memorymap.inc"
    include "include/structs.inc"

;===========================================================================
; main.asm
;===========================================================================

    SLDOPT COMMENT WPMEM, LOGPOINT, ASSERTION

NEX:    equ 1   ;  1=Create nex file, 0=create sna file

    IF NEX == 0
        ;DEVICE ZXSPECTRUM128
        DEVICE ZXSPECTRUM48
        ;DEVICE NOSLOT64Kmf
    ELSE
        DEVICE ZXSPECTRUMNEXT
    ENDIF

    org 0x8000

;===========================================================================
; main routine - the code execution starts here.
; Sets up the new interrupt routine, the memory
; banks and jumps to the start loop.
;===========================================================================

main:
        di
        ld sp,STACK_TOP
        call initialize_system
        call show_loading_splash
        call load_initial_assets
        call clear_loading_splash
restart_game:
        call show_title
        call initialize_game
game_loop:
        ld a,2                      ; Red
        out (ULA_PORT),a
        call read_keyboard
        call read_controllers
        ld a,3                      ; Magenta
        out (ULA_PORT),a
        call update_user_input
        ld a,4                      ; Green
        out (ULA_PORT),a
        call update_objects
        call update_audio
        ld a,5                      ; Cyan
        out (ULA_PORT),a
        call wait_vsync
        call render
        call check_reset
        jp z,game_loop
        jp restart_game

        include "src/initialize.asm"
        include "src/utilities.asm"
        include "src/titles.asm"
        include "src/controllers.asm"
        include "src/animation.asm"
        include "src/movement.asm"
        include "src/update.asm"
        include "src/render.asm"
        include "src/display.asm"
        include "src/tilemap.asm"
        include "src/sprites.asm"
        include "src/particles.asm"
        include "src/audio.asm"
        include "src/filesystem.asm"
        include "src/print.asm"
        include "src/variables.asm"
        include "src/assets.asm"
        
        IF NEX == 0
            SAVESNA "build/KnightFright.sna", main
        ELSE
            SAVENEX OPEN "build/KnightFright.nex", main, STACK_TOP
            SAVENEX CORE 3, 1, 5
            SAVENEX CFG 7   ; Border color
            ;SAVENEX AUTO *** THIS DOESN'T WORK RIGHT! IT DOESN'T EXPORT THINGS CORRECTLY.
            ;SAVENEX AUTO
            SAVENEX BANK 5,2,0,1,3,4,6,16,17,18,19,20,21,22
            SAVENEX CLOSE
        ENDIF
