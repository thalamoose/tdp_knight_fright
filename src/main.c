#include "kftypes.h"
#include "globals.h"
#include "utilities.h"
#include "hardware.h"
#include "audio.h"
#include "assets.h"
#include "render.h"
#include "memorymap.h"
#include "copper.h"
#include "hud.h"
#include "game.h"

globals global;

extern void InitializeInterrupts(void);
extern void ClearScreen(void);

extern u8 _BSS_head[];
extern u8 _BSS_END_tail[];
extern u8 _DATA_END_tail[];

//---------------------------------------------------------
void InitializeSystem(void)
{
    nextreg(CLOCK_SEL, 0x03);
    nextreg(PERIPHERAL_3_CONTROL, 0x70);
    // Remap 16K to ULA shadow.
    nextreg(MMU_SLOT_2, ULA_SHADOW_PAGE);
    nextreg(MMU_SLOT_3, ULA_SHADOW_PAGE + 1);
    memset(_BSS_head, 0, _BSS_END_tail - _BSS_head);
    InitializeInterrupts();
    ClearScreen();
    InitializeRender();
    x_printf("Booting Knight Fright...\n");
    x_printf("Built: %s, %s\n", __TIME__, __DATE__);
    x_printf("VARS: 0x%x.\n", _BSS_END_tail);
    x_printf("CODE: 0x%x\n", _DATA_END_tail);
    x_printf("Globals: 0x%x bytes.\n", sizeof(globals));
}

//---------------------------------------------------------
//---------------------------------------------------------
u8 c=0;
int main(void)
{
    InitializeSystem();
    LoadInitialAssets();
    InitializeGame();
    while (true)
    {
        ResetGame();
        // Game mode is derived from these two flags.
        // Begin game   - intro transitions     - gameIsRunning = true,  transitionIsRunning = true
        // Play game    - main body of game     - gameIsRunning = true,  transitionIsRunning = false
        // Game complete- exit game transition  - gameIsRunning = false, transitionIsRunninf = true
        while (hud.gameIsRunning || hud.transitionIsRunning)
        {
            UpdateGame();
            port_out(ULA_PORT, ULA_COLOUR_BLACK);
            WaitVSync();
            port_out(ULA_PORT, ULA_COLOUR_CYAN);
            RenderGame();
            port_out(ULA_PORT, ULA_COLOUR_YELLOW);
            c++;
            if (c>50)
            {
                c=0;
                x_printf("\n");
            }
        }
        EndGame();
    }
}
