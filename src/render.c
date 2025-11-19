#include "kftypes.h"
#include "hardware.h"
#include "player.h"
#include "render.h"
#include "particles.h"
#include "tilemap.h"
#include "utilities.h"
#include "sprites.h"
#include "memorymap.h"
#include "copper.h"
#include "globals.h"
#include "hud.h"

void InitializeRender(void)
{
    nextreg(LAYER_2_RAM_BANK, LAYER_2_PAGE / 2);
    port_out(LAYER_2_ACCESS_PORT, 2);
    nextreg(GLOBAL_TRANSPARENCY, 0);
    nextreg(PALETTE_CONTROL, 0);
    nextreg(PALETTE_INDEX, 0x88);   // set "bright+black" ULA colour
    nextreg(PALETTE_VALUE_8, 0xe3); // set BRIGHT+BLACK to transparent
    nextreg(PALETTE_INDEX, 0x18);   // set BRIGHT BLACK to transparent
    nextreg(PALETTE_VALUE_8, 0);
    // nextreg(SPRITE_AND_LAYERS,0x23|(2<<2));             	// enable sprites, SUL, sprites over border
    nextreg(SPRITE_AND_LAYERS, 0x23 | (1 << 2)); // enable sprites, LSU, sprites over border
    nextreg(ULA_CONTROL, 0);
    nextreg(DISPLAY_CONTROL_1, 0xc0); // Enable layer 2, ULA to shadow bank (bank 7, page 14,15)

    nextreg(LAYER_2_CONTROL, 0x10); // Enable 320 mode for layer 2
                                    // Set clip to 4,4,314,252 to give us an 8 pixel border for clipping. We don't bother
                                    // clipping the actual shape as we're guaranteed any particle effects are below 8x8 in
                                    // size.
    nextreg(CLIP_WINDOW_CONTROL, 0x0f);
    nextreg(LAYER_2_CLIP_WINDOW, (4) >> 1);         // X1 (Y1 in 320 mode)
    nextreg(LAYER_2_CLIP_WINDOW, ((319 - 4) >> 1)); // X2 (Y2 in 320 mode)
    nextreg(LAYER_2_CLIP_WINDOW, (4));              // Y1 (X1 in 320 mode)
    nextreg(LAYER_2_CLIP_WINDOW, (255 - 4) & 0xff); // Y2 (X2 in 320 mode)

    nextreg(SPRITE_CLIP_WINDOW, (4) >> 1);         // X1
    nextreg(SPRITE_CLIP_WINDOW, ((319 - 4) >> 1)); // X2
    nextreg(SPRITE_CLIP_WINDOW, (4));              // Y1
    nextreg(SPRITE_CLIP_WINDOW, (255 - 4) & 0xff); // Y2
}

void Render(void)
{
    RenderCopper(); // Must be done first, just after vsync
    UpdateTilemap();
    RenderPlayer();
    RenderSprites();
    RenderHud();
    port_out(ULA_PORT, ULA_COLOUR_BLACK);
    RenderParticles(&particles[0]);
    port_out(ULA_PORT, ULA_COLOUR_WHITE);
}