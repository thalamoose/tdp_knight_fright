load_initial_assets:

        ret
        ;
        ; 48K in size
        ;
        mmu 6,SPLASH_PAGE+0
        org SWAP_BANK_0
        incbin "build/assets/kfback.bin",$0000,$2000
        mmu 6,SPLASH_PAGE+1
        org SWAP_BANK_0
        incbin "build/assets/kfback.bin",$2000,$2000
        mmu 6,SPLASH_PAGE+2
        org SWAP_BANK_0
        incbin "build/assets/kfback.bin",$4000,$2000
        mmu 6,SPLASH_PAGE+3
        org SWAP_BANK_0
        incbin "build/assets/kfback.bin",$6000,$2000
        mmu 6,SPLASH_PAGE+4
        org SWAP_BANK_0
        incbin "build/assets/kfback.bin",$8000,$2000
        mmu 6,SPLASH_PAGE+5
        org SWAP_BANK_0
        incbin "build/assets/kfback.bin",$a000,$2000
        ;
        ; 16K in size
        
        ;
        mmu 6,SPRITES_PAGE+0
        org SWAP_BANK_0
        incbin "assets/sprites.spr",$0000,$2000
        mmu 6,SPRITES_PAGE+1
        org SWAP_BANK_0
        incbin "assets/sprites.spr",$2000,$2000

        mmu 6,TILES_PAGE+0
        org SWAP_BANK_0
        incbin "build/assets/kftiles.bin"
        ;
        ; All palettes go here
        ;
        mmu 6,PALETTE_PAGE
        org SWAP_BANK_0
; 256 colours for the layer 2 backdrop
splash_palette:
        incbin "build/assets/kfback.pal"
; 256 colours for the sprites
sprite_palette:
        incbin "build/assets/kfback.pal"
; 256 colours for the tilemap
tilemap_palette:
        incbin "build/assets/kftiles.pal"
