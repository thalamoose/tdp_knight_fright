#include "kftypes.h"
#include "hardware.h"
#include "memorymap.h"
#include "utilities.h"
#include "assets.h"
#include "hud.h"

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
}

void RenderHud(void)
{
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
