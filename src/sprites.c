#include "hardware.h"
#include "sprites.h"
#include "utilities.h"

void InitializeSprites(void)
{
	for( u8 i=0; i<MAX_SPRITES; i++)
	{
		nextreg(SPRITE_INDEX,i);
		nextreg(SPRITE_ATTR_3,0);
	}
}
void SetupSprite(u8 slot, u8 pattern, u8 attr0, u8 attr1, u8 attr2, u8 attr3, u8 attr4)
{
	nextreg(SPRITE_INDEX, slot);
    nextreg(SPRITE_ATTR_0,attr0);                     	// X7 X6 X5 X4 X3 X2 X1 X0
    nextreg(SPRITE_ATTR_1,attr1);                       // Y7 Y6 Y5 Y4 Y3 Y2 Y1 Y0
    nextreg(SPRITE_ATTR_2,attr2);                       // P3 P2 P1 P0 XM YM R  X8
    nextreg(SPRITE_ATTR_3,attr3|pattern);              	// V  E  N5 N4 N3 N2 N1 N0
    nextreg(SPRITE_ATTR_4,attr4);                       // H  N6 T  X  X  Y  Y  Y8
}

void UpdateSprites(void)
{

}