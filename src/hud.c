#include "kftypes.h"
#include "hardware.h"
#include "memorymap.h"
#include "utilities.h"
#include "assets.h"
#include "hud.h"
#include "game_manager.h"

hud_t hud;

void CopyBackgroundBitmap(u8 srcPage);
void UpdateHudCount(s16 x, s16 y, u8* bcdDigits, u8* bcdShown);

#define MAX_HUD_SEGMENTS 9
#define TILE_COUNT_X 8
#define TILE_COUNT_Y 4
#define COIN_COUNT_X 23
#define COIN_COUNT_Y 4
//---------------------------------------------------------
void InitializeHud(void)
{
	memset(&hud, 0, sizeof(hud));
	ResetHud();
}

void ResetHud(void)
{
	CopyBackgroundBitmap(BACKDROP_PAGE);
	nextreg(MMU_SLOT_6, PALETTE_PAGE);
	memset(&hud.coinsDigitsShown, 0xff, sizeof(hud.coinsDigitsShown));
	memset(&hud.tilesDigitsShown, 0xff, sizeof(hud.tilesDigitsShown));
	ResetHudTiles();
	hud.activeColour[0] = asset_BackdropPalette[0xe0];
	hud.activeColour[1] = asset_BackdropPalette[0xe1];
	hud.inactiveColour[0] = asset_BackdropPalette[0xe2];
	hud.inactiveColour[1] = asset_BackdropPalette[0xe3];
 
	UpdateHudCount(TILE_COUNT_X, TILE_COUNT_Y, hud.tilesBCD, hud.tilesDigitsShown);
	UpdateHudCount(COIN_COUNT_X, COIN_COUNT_Y, hud.coinsBCD, hud.coinsDigitsShown);
	StartTransition(85, I_TO_F(-240), I_TO_F(-191), I_TO_F(51) / 18, I_TO_F(-3), I_TO_F(1) / 8);
}
//---------------------------------------------------------
void CopyBackgroundBitmap(u8 srcPage)
{
	u8 dstPage = LAYER_2_PAGE;
	u8 totalPages = 10;
	for (int i = 0; i < totalPages; i++)
	{
		nextreg(MMU_SLOT_6, srcPage);
		nextreg(MMU_SLOT_7, dstPage);
		memcpy_dma(SWAP_BANK_1, SWAP_BANK_0, 8192);
		srcPage++;
		dstPage++;
	}
	nextreg(MMU_SLOT_6, PALETTE_PAGE);
	CopyPalette(asset_BackdropPalette, PALETTE_LAYER_2_PRIMARY);
	CopyPalettePartial(asset_GameDigitsPalette, PALETTE_LAYER_2_PRIMARY, 0xd0, 16);
}

//---------------------------------------------------------
void BeginShake(u8 duration, u8 amplitude)
{
	hud.shakeAmplitude = amplitude;
	hud.shakeDuration = duration;
	hud.shakeDecayRate = (duration * 4) / amplitude;
}

coord testSpline[4] =
	{
		{I_TO_F(50), I_TO_F(50)},
		{I_TO_F(100), I_TO_F(100)},
		{I_TO_F(150), I_TO_F(0)},
		{I_TO_F(200), I_TO_F(100)}};

//---------------------------------------------------------
void UpdateTransition(void)
{
	if (hud.transitionIsRunning == false)
	{
		return;
	}
	hud.transDuration--;

	hud.transPosition.x += hud.transVelocity.x;
	hud.transPosition.y += hud.transVelocity.y;
	hud.transVelocity.y += hud.transGravity;

	hud.shake.x = F_TO_I(hud.transPosition.x);
	hud.shake.y = F_TO_I(hud.transPosition.y);

	if (hud.transDuration == 0)
	{
		hud.transitionIsRunning = false;
		hud.shake.x = 0;
		hud.shake.y = 0;
		if (gameManager.isRunning)
		{
			x_printf("End transition. Start game.\n");
		}
		else
		{
			x_printf("End transition. Game over.\n");
		}
	}
}

//---------------------------------------------------------
void UpdateShake(void)
{
	if (hud.shakeDuration == 0)
	{
		return;
	}
	hud.shakeDuration--;
	if ((hud.shakeDuration % hud.shakeDecayRate) == 0)
	{
		hud.shakeAmplitude--;
	}
	if (hud.shakeDuration == 0 || hud.shakeAmplitude == 0)
	{
		hud.shake.x = 0;
		hud.shake.y = 0;
		return;
	}
	hud.shake.x = random8() % (hud.shakeAmplitude * 2) - hud.shakeAmplitude;
	hud.shake.y = random8() % (hud.shakeAmplitude * 2) - hud.shakeAmplitude;
}

//---------------------------------------------------------
void StartTransition(u8 duration, s16 x, s16 y, s16 vx, s16 vy, s16 gravity)
{
	hud.transitionIsRunning = true;
	hud.transDuration = duration;
	hud.transPosition.x = x;
	hud.transPosition.y = y;
	hud.transVelocity.x = vx;
	hud.transVelocity.y = vy;
	hud.transGravity = gravity;

	hud.shakeDuration = 0;
	hud.shake.x = F_TO_I(x);
	hud.shake.y = F_TO_I(y);
}

//---------------------------------------------------------
void RenderHud(void)
{
	UpdateTransition();
	UpdateShake();
	if (hud.pulseColour[0] != hud.pulseTarget[0])
	{
		hud.pulseColour[0] = BlendColour(hud.pulseColour[0], hud.pulseTarget[0]);
		SetColour(PALETTE_LAYER_2_PRIMARY, hud.pulseColourIndex, hud.pulseColour[0]);
	}
	if (hud.pulseColour[1] != hud.pulseTarget[1])
	{
		hud.pulseColour[1] = BlendColour(hud.pulseColour[1], hud.pulseTarget[1]);
		SetColour(PALETTE_LAYER_2_PRIMARY, hud.pulseColourIndex + 1, hud.pulseColour[1]);
	}
}

//---------------------------------------------------------
void DrawHudDigit(u8 *bitmap, u8 value)
{
	u8 *digitData = &asset_GameDigits[value*10*4]; // 10 pixels high, 8 pixels wide (packed)

	for (u8 y = 0; y < 10; y++)
	{
		u8 *pLine = bitmap;
		
		for (u8 x = 0; x < 4; x++)
		{
			u8 digitLine = *digitData++;
			*pLine = (digitLine >> 4) | 0xd0;
			pLine += 256;
			*pLine = (digitLine & 0x0f) | 0xd0;
			pLine += 256;
		}
		bitmap++;
	}
}

//---------------------------------------------------------
void UpdateHudCount(s16 x, s16 y, u8* bcdDigits, u8* bcdShown)
{
	// Need access to the font.
	nextreg(MMU_SLOT_6, PALETTE_PAGE);
	for (s8 i=3; i>=0; i--)
	{
		if (bcdDigits[i]!=bcdShown[i])
		{
			s16 dx = x;
			bcdShown[i] = bcdDigits[i];
			u16 page= LAYER_2_PAGE+(dx>>2);
			nextreg(MMU_SLOT_7, page);
			u8 *charBase = (u8 *)SWAP_BANK_1 + ((dx & 3)<<11) + y*10;
			DrawHudDigit(charBase, bcdDigits[i]>>4);
			dx++;
			charBase = (u8 *)SWAP_BANK_1 + ((dx & 3)<<11) + y*10;
			page= LAYER_2_PAGE+((dx)>>2);
			nextreg(MMU_SLOT_7, page);
			DrawHudDigit(charBase, bcdDigits[i] & 0x0f);
		}
		x += 2;
	}
}

//---------------------------------------------------------
bool IncrementHudTileCount(void)
{
	u8 segment = hud.segmentsLit;
	hud.segmentsLit++;
	bcd_add(hud.tilesBCD, 1);
	UpdateHudCount(TILE_COUNT_X, TILE_COUNT_Y, hud.tilesBCD, hud.tilesDigitsShown);
	UpdateHudCount(COIN_COUNT_X, COIN_COUNT_Y, hud.coinsBCD, hud.coinsDigitsShown);
	if (hud.segmentsLit >= MAX_HUD_SEGMENTS)
	{
		ResetHudTiles();
		BeginShake(20, 3);
		return true;
	}
	hud.pulseColourIndex = 0xe0 + (segment * 2);
	hud.pulseTarget[0] = hud.activeColour[0];
	hud.pulseTarget[1] = hud.activeColour[1];
	hud.pulseColour[0] = 0x0fc; // Bright yellow, in next format.
	hud.pulseColour[1] = 0x1ff;
	SetColour(PALETTE_LAYER_2_PRIMARY, hud.pulseColourIndex, hud.pulseColour[0]);
	SetColour(PALETTE_LAYER_2_PRIMARY, hud.pulseColourIndex + 1, hud.pulseColour[1]);
	return false;
}

//---------------------------------------------------------
void ResetHudTiles(void)
{
	u8 colour = 0xe0;
	while (colour != 0xf0)
	{
		SetColour(PALETTE_LAYER_2_PRIMARY, colour++, hud.inactiveColour[0]);
		SetColour(PALETTE_LAYER_2_PRIMARY, colour++, hud.inactiveColour[1]);
	}
	hud.segmentsLit = 0;
}
