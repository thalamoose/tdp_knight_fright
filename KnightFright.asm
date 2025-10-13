
    include "include/hardware.inc"
    include "include/defines.inc"
    include "include/memorymap.inc"
    include "include/structs.inc"

;===========================================================================
; main.asm
;===========================================================================

    SLDOPT COMMENT WPMEM, LOGPOINT, ASSERTION

    DEVICE ZXSPECTRUMNEXT

    org CODE_BANK_0

;===========================================================================
; main routine - the code execution starts here.
; Sets up the new interrupt routine, the memory
; banks and jumps to the start loop.
;===========================================================================
        ; This file must be first. The bank code call must be at the same
        ; address in the assembly code, as well as the C code.
        ; e.g. 0x8004.
main:
        include src/bootstrap.asm

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
            ld a,(debug_max_particles)
            ld d,a
            ld a,(particles_active)
.more
            cp d
            jr nc,.enough
;
; *DEBUG CODE*
;
            push af,de
            call debug_add_particle
            pop de,af
            inc a
            jr .more
.enough
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
        include "src/player.asm"
        include "src/npcs.asm"
        include "src/display.asm"
        include "src/tilemap.asm"
        include "src/sprites.asm"
        include "src/particles.asm"
        include "src/audio.asm"
        include "src/filesystem.asm"
        include "src/print.asm"
        include "src/dma.asm"
        include "src/interrupts.asm"
        include "src/variables.asm"
        include "src/assets.asm"
        include "src/c_code.asm"
        
        SAVENEX OPEN "build/KnightFright.nex", main, STACK_TOP
        SAVENEX CORE 3, 1, 5
        SAVENEX CFG 7   ; Border color
        ;SAVENEX AUTO *** THIS DOESN'T WORK RIGHT! IT DOESN'T EXPORT THINGS CORRECTLY.
        SAVENEX BANK ASM_CODE_PAGE/2
        SAVENEX BANK SPLASH_PAGE/2,SPLASH_PAGE/2+1,SPLASH_PAGE/2+2,SPLASH_PAGE/2+3,SPLASH_PAGE/2+4
        SAVENEX BANK SPRITES_PAGE/2,PALETTE_PAGE/2,TILES_PAGE/2
        SAVENEX BANK PLAYER_ANIM_PAGE/2,PLAYER_ANIM_PAGE/2+1,PLAYER_ANIM_PAGE/2+2,PLAYER_ANIM_PAGE/2+3
        SAVENEX BANK C_CODE_PAGE/2
         SAVENEX CLOSE
