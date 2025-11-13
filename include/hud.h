#include "kftypes.h"
#include "objects.h"

typedef struct s_hud
{
	u16 activeColour[2];
	u16 inactiveColour[2];
	u16 pulseColour[2];
	u16 pulseTarget[2];
	u8 pulseColourIndex;
	u16 shakeDuration;
	u8 shakeAmplitude;
	u8 shakeDecayRate;
	coord shake;
	u8 segmentsLit;
} hud_t;

void InitializeHud(void);
void RenderHud(void);
void EnableHudProgressTile(u8 tile);
bool IncrementHudTileCount(void);
bool IncrementHudCoinCount(void);
void ResetHudTiles(void);
void BeginShake(u8 duration, u8 amplitude);

extern hud_t hud;
