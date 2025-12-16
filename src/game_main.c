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

globals global;

extern void InitializeInterrupts(void);
extern void ClearScreen(void);

//---------------------------------------------------------
void InitializeSystem(void)
{
    nextreg(CLOCK_SEL, 0x03);
    nextreg(PERIPHERAL_3_SETTING, 0x70);
    // Remap 16K to ULA shadow.
    InitializeInterrupts();
    ClearScreen();
    InitializeRender();
    x_printf("Booting Knight Fright...\n");
    x_printf("Built: %s, %s\n", __TIME__, __DATE__);
}

//---------------------------------------------------------
//---------------------------------------------------------
u8 c=0;
int game_main(void)
{
    InitializeSystem();
    InitializeGameManager();
    while (true)
    {
        ResetGameManager();
        // Game mode is derived from these two flags.
        // Begin game   - intro transitions     - gameIsRunning = true,  transitionIsRunning = true
        // Play game    - main body of game     - gameIsRunning = true,  transitionIsRunning = false
        // Game complete- exit game transition  - gameIsRunning = false, transitionIsRunninf = true
        while (gameManager.isRunning || hud.transitionIsRunning)
        {
            UpdateGameManager();
            DebugTiming(ULA_COLOUR_BLACK);
            c++;
            if (c>50)
            {
                c=0;
                x_printf("\n");
            }
            WaitVSync();
            DebugTiming(ULA_COLOUR_CYAN);
            RenderGameManager();
            DebugTiming(ULA_COLOUR_YELLOW);
        }
    }
}
