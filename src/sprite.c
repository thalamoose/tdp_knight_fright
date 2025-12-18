#include "kftypes.h"
#include "hardware.h"
#include "defines.h"
#include "memorymap.h"
#include "utilities.h"
#include "sprite.h"
#include "objects/object_manager.h"
#include "globals.h"

u8 spriteSlot = 0;
u8 spritePattern = 0;

#if !defined(__GNUC__)
#define USE_ASSEMBLY 0
#endif

//---------------------------------------------------------
void InitializeSprites(void)
{
	for (u8 i = 0; i<MAX_SPRITE_SLOTS; i++)
	{
		nextreg(SPRITE_INDEX, i);
		nextreg(SPRITE_ATTR_3, 0);
	}
	spriteSlot = 0;
	spritePattern = 0;
}

//---------------------------------------------------------
void ResetSprites(void)
{
	InitializeSprites();
}

//---------------------------------------------------------
void HideSprite(u8 slot)
{
	nextreg(SPRITE_INDEX, slot);
	nextreg(SPRITE_ATTR_3, 0);
}

//---------------------------------------------------------
void SetupSprite(u8 slot, const sprite_config* config)
{
#if USE_ASSEMBLY
	(void)slot;
	(void)config;
__asm
include "include/hardware.inc"
	ld hl,2
	add hl,sp
	ld a,(hl)										; Read Sprite slot
	inc hl
	nextreg SPRITE_INDEX, a
	ld e,(hl)
	inc hl
	ld d,(hl)
	ex de,hl
	ld b,(hl)										; Pattern
	inc hl
	ld a,(hl)
	nextreg SPRITE_ATTR_0, a
	inc hl
	ld a,(hl)
	nextreg SPRITE_ATTR_1, a
	inc hl
	ld a,(hl)
	nextreg SPRITE_ATTR_2, a
	inc hl
	ld a,(hl)
	inc hl
	or b
	nextreg SPRITE_ATTR_3, a
	ld a,(de)
	nextreg SPRITE_ATTR_4, a
__endasm;
#else
	nextreg(SPRITE_INDEX, slot);
	nextreg(SPRITE_ATTR_0, config->attr0);			 		// X7 X6 X5 X4 X3 X2 X1 X0
	nextreg(SPRITE_ATTR_1, config->attr1);			 		// Y7 Y6 Y5 Y4 Y3 Y2 Y1 Y0
	nextreg(SPRITE_ATTR_2, config->attr2);			 		// P3 P2 P1 P0 XM YM R  X8
	nextreg(SPRITE_ATTR_3, config->attr3|config->pattern); 	// V  E  N5 N4 N3 N2 N1 N0
	nextreg(SPRITE_ATTR_4, config->attr4);			 		// H  N6 T  X  X  Y  Y  Y8
#endif
}

//---------------------------------------------------------
void UpdateSprites(void)
{
}

//---------------------------------------------------------
u8 AllocSpriteSlot(void)
{
	u8 slot = spriteSlot%(MAX_SPRITE_SLOTS/2);
	spriteSlot++;
	return slot;
}

//---------------------------------------------------------
u8 AllocSpritePattern(void)
{
	u8 pattern = spritePattern;
	spritePattern++;
	if (spritePattern>=MAX_SPRITE_PATTERNS-4)
	{
		spritePattern = 0;
	}
	return pattern;
}