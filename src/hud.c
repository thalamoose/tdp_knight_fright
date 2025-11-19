#include "kftypes.h"
#include "hardware.h"
#include "memorymap.h"
#include "utilities.h"
#include "assets.h"
#include "hud.h"
#include "lerp.h"

hud_t hud;

void CopyBackgroundBitmap(u8 srcPage);

//---------------------------------------------------------
void InitializeHud(void)
{
	nextreg(MMU_SLOT_6, PALETTE_PAGE);
	memset(&hud, 0, sizeof(hud));
	memset(&hud.coinsDigitsShown, 0xff, sizeof(hud.coinsDigitsShown));
	memset(&hud.tilesDigitsShown, 0xff, sizeof(hud.tilesDigitsShown));
	hud.activeColour[0] = asset_BackdropPalette[0xe0];
	hud.activeColour[1] = asset_BackdropPalette[0xe1];
	hud.inactiveColour[0] = asset_BackdropPalette[0xe2];
	hud.inactiveColour[1] = asset_BackdropPalette[0xe3];
	CopyBackgroundBitmap(BACKDROP_PAGE);
	ResetHudTiles();
	hud.gameIsRunning = true;
	StartTransition(85, I_TO_F(-240), I_TO_F(-191), I_TO_F(51) / 18, I_TO_F(-3), I_TO_F(1) / 8);
}

void ResetHud(void)
{
	InitializeHud();
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
	nextreg(MMU_SLOT_7, PALETTE_PAGE + 1);
	CopyPalette(asset_BackdropPalette, PALETTE_LAYER_2_PRIMARY);
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

	// x_printf("dur:%d, shake: %d,%d\n", hud.transDuration, hud.shake.x, hud.shake.y);
	if (hud.transDuration == 0)
	{
		hud.transitionIsRunning = false;
		hud.shake.x = 0;
		hud.shake.y = 0;
		if (hud.gameIsRunning)
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
	u8 *digitData = &asset_GameDigits[value * 9];

	for (u8 y = 0; y < 9; y++)
	{
		u8 *pLine = bitmap;
		u8 digitLine = *digitData++;
		for (u8 x = 0; x < 7; x++)
		{
			*pLine = (digitLine & 128) ? 254 : 0;
			digitLine = digitLine << 1;
			pLine += 256;
		}
		bitmap++;
	}
}

//---------------------------------------------------------
void UpdateHudCount(s16 x, s16 y, u8 bcdDigits[], u8 bcdShown[])
{
	u8 i = 0;
	nextreg(MMU_SLOT_6, LAYER_2_PAGE + 2);
	nextreg(MMU_SLOT_7, LAYER_2_PAGE + 3);
	u8 *charBase = (u8 *)SWAP_BANK_0 + (x & 63) * 256 + y;

	while (i < 4)
	{
		u8 digit = bcdDigits[i] >> 4;
		u8 shown = bcdShown[i] >> 4;

		if (digit != shown)
		{
			DrawHudDigit(charBase, digit);
		}
		digit = bcdDigits[i] & 0x0f;
		shown = bcdShown[i] & 0x0f;
		charBase += 256 * 7;
		if (digit != shown)
		{
			DrawHudDigit(charBase, digit);
		}
		bcdShown[i] = bcdDigits[i];
		i++;
		charBase -= 256 * 7 * 2;
	}
}

#define MAX_HUD_SEGMENTS 9

//---------------------------------------------------------
bool IncrementHudTileCount(void)
{
	u8 segment = hud.segmentsLit;
	hud.segmentsLit++;
	bcd_add(hud.tilesBCD, 1);
	UpdateHudCount(42, 40, hud.tilesBCD, hud.tilesDigitsShown);
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
