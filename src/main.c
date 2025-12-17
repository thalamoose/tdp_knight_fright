#include "kftypes.h"
#include "defines.h"
#include "globals.h"
#include "utilities.h"
#include "hardware.h"
#include "audio.h"
#include "assets.h"
#include "render.h"
#include "memorymap.h"
#include "copper.h"
#include "hud.h"
#include "game_manager.h"

extern u8 _PAGE_16_head[];
extern u8 _PAGE_16_tail[];
extern u8 _DATA_END_tail[];

extern void game_main(void);
extern void boot_disable_interrupts(void);
//---------------------------------------------------------
void main(void)
{
    nextreg(CLOCK_SEL, 0x03);
    nextreg(PERIPHERAL_3_SETTING, 0x70);
    // Remap 16K to ULA shadow. This must be done before pretty mych
	nextreg(MMU_SLOT_0, ULA_SHADOW_PAGE);
	nextreg(MMU_SLOT_1, ULA_SHADOW_PAGE+1);
    memset(_PAGE_16_head, 0, _PAGE_16_tail-_PAGE_16_head);
    x_printf("VARS: 0x%x.\n", _PAGE_16_tail);
    x_printf("CODE: 0x%x\n", _DATA_END_tail);

    game_main();
}
