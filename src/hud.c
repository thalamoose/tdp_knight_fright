#include "kftypes.h"
#include "hardware.h"
#include "memorymap.h"
#include "utilities.h"
#include "assets.h"
#include "hud.h"
#include "lerp.h"

hud_t hud;

void InitializeHud(void)
{
	nextreg(MMU_SLOT_6, PALETTE_PAGE);

	memset(&hud,0,sizeof(hud));
	hud.activeColour[0] = asset_BackdropPalette[0xe0];
	hud.activeColour[1] = asset_BackdropPalette[0xe1];
	hud.inactiveColour[0] = asset_BackdropPalette[0xe2];
	hud.inactiveColour[1] = asset_BackdropPalette[0xe3];
	ResetHudTiles();
	hud.gameIsRunning = true;
	StartTransition(85, I_TO_F(-240), I_TO_F(-191), I_TO_F(51)/18, I_TO_F(-3), I_TO_F(1)/8);
}

void BeginShake(u8 duration, u8 amplitude)
{
	hud.shakeAmplitude = amplitude;
	hud.shakeDuration = duration;
	hud.shakeDecayRate = (duration*4)/amplitude;
}

coord testSpline[4] =
{
	{I_TO_F(50),I_TO_F(50)},
	{I_TO_F(100),I_TO_F(100)},
	{I_TO_F(150),I_TO_F(0)},
	{I_TO_F(200),I_TO_F(100)}
};

void UpdateTransition(void)
{
	if (hud.transitionIsRunning==false)
	{
		return;
	}
	hud.transDuration--;

	hud.transPosition.x += hud.transVelocity.x;
	hud.transPosition.y += hud.transVelocity.y;
	hud.transVelocity.y += hud.transGravity;

	hud.shake.x = F_TO_I(hud.transPosition.x);
	hud.shake.y = F_TO_I(hud.transPosition.y);

	//x_printf("dur:%d, shake: %d,%d\n", hud.transDuration, hud.shake.x, hud.shake.y);
	if (hud.transDuration==0)
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

void UpdateShake(void)
{

	if (hud.shakeDuration==0)
	{
		return;
	}
	hud.shakeDuration--;
	if ((hud.shakeDuration % hud.shakeDecayRate)==0)
	{
		hud.shakeAmplitude--;
	}
	if (hud.shakeDuration==0 || hud.shakeAmplitude==0)
	{
		hud.shake.x = 0;
		hud.shake.y = 0;
		return;
	}
	hud.shake.x = random8()%(hud.shakeAmplitude*2)-hud.shakeAmplitude;
	hud.shake.y = random8()%(hud.shakeAmplitude*2)-hud.shakeAmplitude;
}

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

void RenderHud(void)
{
	UpdateTransition();
	UpdateShake();
	if (hud.pulseColour[0]!=hud.pulseTarget[0])
	{
		hud.pulseColour[0] = BlendColour(hud.pulseColour[0], hud.pulseTarget[0]);
		SetColour(PALETTE_LAYER_2_PRIMARY, hud.pulseColourIndex, hud.pulseColour[0]);
	}
	if (hud.pulseColour[1]!=hud.pulseTarget[1])
	{
		hud.pulseColour[1] = BlendColour(hud.pulseColour[1], hud.pulseTarget[1]);
		SetColour(PALETTE_LAYER_2_PRIMARY, hud.pulseColourIndex+1, hud.pulseColour[1]);
	}
}

#define MAX_HUD_SEGMENTS 9

bool IncrementHudTileCount(void)
{
	u8 segment = hud.segmentsLit;
	hud.segmentsLit++;
	if (hud.segmentsLit>=MAX_HUD_SEGMENTS)
	{
		ResetHudTiles();
		BeginShake(20, 3);
		return true;
	}
	hud.pulseColourIndex = 0xe0+(segment*2);
	hud.pulseTarget[0] = hud.activeColour[0];
	hud.pulseTarget[1] = hud.activeColour[1];
	hud.pulseColour[0] = 0x0fc;		// Bright yellow, in next format.
	hud.pulseColour[1] = 0x1ff;
	SetColour(PALETTE_LAYER_2_PRIMARY, hud.pulseColourIndex, hud.pulseColour[0]);
	SetColour(PALETTE_LAYER_2_PRIMARY, hud.pulseColourIndex+1, hud.pulseColour[1]);
	return false;
}

void ResetHudTiles(void)
{
	u8 colour=0xe0;
	while (colour!=0xf0)
	{
		SetColour(PALETTE_LAYER_2_PRIMARY, colour++, hud.inactiveColour[0]);
		SetColour(PALETTE_LAYER_2_PRIMARY, colour++, hud.inactiveColour[1]);
	}
	hud.segmentsLit = 0;
}
