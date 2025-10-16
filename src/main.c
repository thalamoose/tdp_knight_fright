#include <sys/types.h>
#include <stdbool.h>
#include "globals.h"
#include "tilemap.h"

#define CLOCK_SEL 0x03
#define PERIPHERAL_3_CONTROL 0x08
globals global;

extern void ConfigureMemory(void);
extern void InitializeInterrupts(void);
extern void ClearScreen(void);
extern void PrintStr(const char* fmt,...);

extern void ShowLoadingSplash(void);

extern void LoadInitialAssets(void);
extern void ClearLoadingSplash(void);
extern void ShowTitle(void);

extern void InitializeSprites(void);
extern void InitializeParticles(void);
extern void InitializePlayer(void);
extern void InitializeNpcs(void);

extern void ReadKeyboard(void);
extern void ReadControllers(void);
extern void UpdateUserInput(void);
extern void UpdateObjects(void);
extern void UpdateAudio(void);
extern void DebugAddParticle(void);
extern bool CheckReset(void);
extern void ReadControllers(void);
extern void nextreg(char register,char value);

void InitializeSystem(void)
{
    nextreg(CLOCK_SEL,0x03);
    nextreg(PERIPHERAL_3_CONTROL,0x70);
    ConfigureMemory();
    // *NEEDS FIXED* SEGMENT ALIGNMENT IS WRONG! InitializeInterrupts();
    ClearScreen();
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
}

int main(void)
{
    InitializeSystem();
    ShowLoadingSplash();
    LoadInitialAssets();
    ClearLoadingSplash();
    while( true )
    {
        ShowTitle();
        InitializeGame();
        while( CheckReset()==false )
        {
            ReadKeyboard();
            ReadControllers();
            UpdateUserInput();
            UpdateObjects();
            UpdateAudio();
            while( global.particlesActive<global.debugMaxParticles )
            {
                DebugAddParticle();
            }
        }
    }
}

void* player_palette;