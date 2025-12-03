#include "kftypes.h"
#include "defines.h"
#include "hardware.h"
#include "memorymap.h"
#include "assets.h"
#include "utilities.h"
#include "playarea.h"
#include "pickups.h"
#include "hud.h"

#define PICKUP_PALETTE 8
//---------------------------------------------------------
// The number of pickups is so low, there's no point doing
// a list. Also, we should be programming for worst case
// anyway.

pickups_t pickups;

//---------------------------------------------------------
void InitializePickups(void)
{
	ResetPickups();
}

//---------------------------------------------------------
void ResetPickups(void)
{
	memset(&pickups, 0, sizeof(pickups));
	nextreg(MMU_SLOT_6, PALETTE_PAGE);
	CopyPalettePartial(asset_PickupPalette, PALETTE_SPRITE_PRIMARY, PICKUP_PALETTE*16, 16);
}

//---------------------------------------------------------
void UpdatePickups(void)
{
	pickup_t* pPickup = pickups.item;
	for (u8 i=0; i<MAX_PICKUPS; i++)
	{
		if (pPickup->object.flags.active)
		{
			pPickup->object.animDelay--;
			if (pPickup->object.animDelay<0)
			{
				pPickup->object.animDelay = pPickup->object.animSpeed;
				pPickup->object.frameIndex++;
				if (pPickup->object.frameIndex>=pPickup->object.totalFrames)
				{
					pPickup->object.frameIndex = 0;
				}
			}
		}
		pPickup++;
	}
}

//---------------------------------------------------------
void RenderPickups(void)
{
	pickup_t* pPickup = pickups.item;

	nextreg(MMU_SLOT_6, PICKUPS_PAGE);
	for (u8 i=0; i<MAX_PICKUPS; i++)
	{
		if (pPickup->object.flags.active)
		{
			u8 animIndex = pPickup->object.baseIndex+pPickup->object.frameIndex;
			if (animIndex!=pPickup->object.lastIndex)
			{
				CopySprite(SWAP_BANK_0+animIndex*(16*16/2), pPickup->sprite.pattern, 1);
				pPickup->object.lastIndex = animIndex;
			}
			nextreg(SPRITE_INDEX, pPickup->sprite.slot);
			s16 tx = tileMap.position.x & I_TO_F(0xfffe);
			s16 ty = tileMap.position.y;

			s16 px = F_TO_I(tx+pPickup->object.position.x);
			s16 py = F_TO_I(ty+pPickup->object.position.y);
			
			s16 x = px+hud.shake.x+TILEMAP_PIX_WIDTH/2+10;
			s16 y = py+hud.shake.y+TILEMAP_PIX_HEIGHT/2-6;

			if ((x<-32)||(x>=320)||(y<-32)||(y>=256))
			{
				// Hide the sprite if clipped
				nextreg(SPRITE_ATTR_3, 0);
			}
			else
			{
				nextreg(SPRITE_ATTR_0, x);
				nextreg(SPRITE_ATTR_1, y);
				nextreg(SPRITE_ATTR_2, ((x>>8)&SPRITE_ATTR_2_X8)|(pPickup->sprite.palette<<4));
				nextreg(SPRITE_ATTR_3, SPRITE_ATTR_3_VISIBLE|SPRITE_ATTR_3_ENABLE_4|pPickup->sprite.pattern);
				nextreg(SPRITE_ATTR_4, SPRITE_ATTR_4_4BIT|((y>>8)&SPRITE_ATTR_4_Y8));
			}
		}
		pPickup++;
	}
}

//---------------------------------------------------------
pickup_t* AddPickup(u8 type, s8 x, s8 y)
{
	u8 index = pickups.itemIndex;
	pickup_t* pPickup = (pickup_t*)&pickups.item[pickups.itemIndex];
	while (pPickup->object.flags.active)
	{
		index = (index+1)%MAX_PICKUPS;
		pPickup = &pickups.item[index];
		if (index==pickups.itemIndex)
			return NULL;
	}
	pickups.itemIndex = (index+1)%MAX_PICKUPS;
	//x=y=0;
	pPickup->object.playGrid.x = x;
	pPickup->object.playGrid.y = y;
	SnapToPlayAreaGrid(&pPickup->object);
	x_printf( "px:%d,py:%d,x:%d,y:%d\n", x, y, F_TO_I(pPickup->object.position.x), F_TO_I(pPickup->object.position.y));
	pPickup->object.flags.active = true;
	pPickup->object.flags.tilemapLocked = true;
	pPickup->type = type;
	pPickup->sprite.pattern = PLAYER_PICKUP_PATTERN+index;
	pPickup->sprite.palette = PICKUP_PALETTE;
	pPickup->sprite.slot = PLAYER_PICKUP_SLOT+index;
	pPickup->object.lastIndex = 0xff;
	pPickup->object.baseIndex = 0;
	pPickup->object.totalFrames = 9;
	pPickup->object.animSpeed = 4;
	pPickup->object.animDelay = 4;
	return pPickup;
}

//---------------------------------------------------------
void RemovePickup(pickup_t* pPickup)
{
	pPickup->object.flags.active = false;
}