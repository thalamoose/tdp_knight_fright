#include "objects.h"

#define SCORE_BCD_DIGITS 8
typedef struct s_hud
{
	bool gameIsRunning;
	bool transitionIsRunning;
	u8 transDuration;
	coord transPosition;
	coord transVelocity;
	s16 transGravity;
	u8 pulseColourIndex;
	u16 shakeDuration;
	u8 shakeAmplitude;
	u8 shakeDecayRate;
	// We'll store this bcd, because why not?
	// 2 digits per byte.
	u8 tilesBCD[SCORE_BCD_DIGITS / 2];
	u8 coinsBCD[SCORE_BCD_DIGITS / 2];
	u8 tilesDigitsShown[SCORE_BCD_DIGITS / 2];
	u8 coinsDigitsShown[SCORE_BCD_DIGITS / 2];
	coord shake;
	u8 segmentsLit;
	u8 unflippedTiles;
	u8 totalTiles;
	u16 activeColour[2];
	u16 inactiveColour[2];
	u16 pulseColour[2];
	u16 pulseTarget[2];
} hud_t;

void InitializeHud(void);
void ResetHud(void);
void StartTransition(u8 duration, s16 x, s16 y, s16 vx, s16 vy, s16 gravity);
void RenderHud(void);
void EnableHudProgressTile(u8 tile);
bool IncrementHudTileCount(void);
bool IncrementHudCoinCount(void);
void ResetHudTiles(void);
void BeginShake(u8 duration, u8 amplitude);
void TileFlipped(void);

extern hud_t hud;
