#include "kftypes.h"
#include "globals.h"
#include "tilemap.h"
#include "objects.h"
#include "utilities.h"
#include "hardware.h"
#include "audio.h"
#include "input.h"
#include "sprites.h"
#include "player.h"
#include "npcs.h"
#include "particles.h"
#include "assets.h"
#include "render.h"
#include "memorymap.h"

globals global;

extern void ConfigureMemory(void);
extern void InitializeInterrupts(void);
extern void ClearScreen(void);
extern void PrintStr(const char* fmt,...);

extern void InitializeParticles(void);

extern void DebugAddParticle(void);

extern u8 _BSS_head[];
extern u8 _BSS_END_tail[];

void InitializeSystem(void)
{
    nextreg(CLOCK_SEL,0x03);
    nextreg(PERIPHERAL_3_CONTROL,0x70);
    // Remap 16K to ULA shadow.
    nextreg(MMU_SLOT_2, ULA_SHADOW_PAGE);
    nextreg(MMU_SLOT_3, ULA_SHADOW_PAGE+1);
    memset(_BSS_head,0,_BSS_END_tail-_BSS_head);
    InitializeInterrupts();
    ClearScreen();
    InitializeRender();
    x_printf("Booting Knight Fright...\n");
    x_printf("Built: %s, %s\n", __TIME__, __DATE__);
}

void InitializeGame(void)
{
    InitializeSprites();
    InitializeTilemap();
    InitializeParticles();
    InitializePlayer();
    InitializeNpcs();
    global.debugMaxParticles = 64;
}

short counter=0;
char c=0;

int main(void)
{
    InitializeSystem();
    LoadInitialAssets();
    while(true)
    {
        InitializeGame();
        while (CheckReset()==false)
        {
            UpdateObjects();
            UpdateAudio();
            while( global.particlesActive<global.debugMaxParticles )
            {
                DebugAddParticle();
                global.particlesActive++;
            }
            port_out(ULA_PORT, ULA_COLOUR_BLACK);
            WaitVSync();
            port_out(ULA_PORT, ULA_COLOUR_CYAN);
            Render();
            port_out(ULA_PORT, ULA_COLOUR_YELLOW);
            c++;
            if (c>10)
            {
                c=0;
                nextreg(ULA_CONTROL, ULA_COLOUR_WHITE);
                x_printf("Count %d\n",counter);
                nextreg(ULA_CONTROL, ULA_COLOUR_BLACK);
                counter++;
            }
        }
    }
}

bool CheckReset(void)
{
    return false;
}
