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

extern u8 _BSS_head[];
extern u8 _BSS_END_tail[];
extern u8 _DATA_END_tail[];

extern void game_main(void);
extern void boot_nextreg(u8 reg, u8 val);
extern void boot_disable_interrupts(void);
//---------------------------------------------------------
void main(void)
{
    boot_disable_interrupts();
    boot_nextreg(CLOCK_SEL, 0x03);
    boot_nextreg(PERIPHERAL_3_SETTING, 0x70);
    // Remap 16K to ULA shadow.
	boot_nextreg(MMU_SLOT_2, ULA_SHADOW_PAGE);
	boot_nextreg(MMU_SLOT_3, ULA_SHADOW_PAGE+1);
    memset(_BSS_head, 0, _BSS_END_tail - _BSS_head);
    x_printf("VARS: 0x%x.\n", _BSS_END_tail);
    x_printf("CODE: 0x%x\n", _DATA_END_tail);

    game_main();
}
