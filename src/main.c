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

globals global;

extern void ConfigureMemory(void);
extern void InitializeInterrupts(void);
extern void ClearScreen(void);
extern void PrintStr(const char* fmt,...);

extern void InitializeParticles(void);

extern void DebugAddParticle(void);

void InitializeSystem(void)
{
    nextreg(CLOCK_SEL,0x03);
    nextreg(PERIPHERAL_3_CONTROL,0x70);
    ConfigureMemory();
    InitializeInterrupts();
    ClearScreen();
    InitializeRender();
    PrintStr("Booting Knight Fright...\r\n");
    PrintStr(__DATE__,__TIME__,"\r\n");
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

int main(void)
{
    InitializeSystem();
    LoadInitialAssets();
    while( true )
    {
        InitializeGame();
        while( CheckReset()==false )
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
        }
    }
}

bool CheckReset(void)
{
    return false;
}
