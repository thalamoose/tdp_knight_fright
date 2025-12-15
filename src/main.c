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
    boot_nextreg(MMU_SLOT_0, CODE_PAGE);
    boot_nextreg(MMU_SLOT_1, CODE_PAGE+1);
    boot_nextreg(MMU_SLOT_2, CODE_PAGE+2);
    boot_nextreg(MMU_SLOT_3, CODE_PAGE+3);
//    memset(_BSS_head, 0, _BSS_END_tail - _BSS_head);
    game_main();
}
