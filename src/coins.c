#include "kftypes.h"
#include "defines.h"
#include "hardware.h"
#include "memorymap.h"
#include "assets.h"
#include "utilities.h"
#include "playarea.h"
#include "coins.h"
#include "hud.h"

#define PICKUP_PALETTE 8
//---------------------------------------------------------
// The number of pickups is so low, there's no point doing
// a list. Also, we should be programming for worst case
// anyway.

coin_manager coins;

//---------------------------------------------------------
void ResetCoins(void)
{
	memset(&coins, 0, sizeof(coins));
	nextreg(MMU_SLOT_6, PALETTE_PAGE);
	CopyPalettePartial(asset_PickupPalette, PALETTE_SPRITE_PRIMARY, PICKUP_PALETTE*16, 16);
}

//---------------------------------------------------------
void InitializeCoins(void)
{
	ResetCoins();
}

//---------------------------------------------------------
void UpdateCoins(void)
{
	coin* pCoin = coins.coin;
	for (u8 i=0; i<MAX_COINS; i++)
	{
		if (pCoin->object.flags.active)
		{
			pCoin->object.animDelay--;
			if (pCoin->object.animDelay<0)
			{
				pCoin->object.animDelay = pCoin->object.animSpeed;
				pCoin->object.frameIndex++;
				if (pCoin->object.frameIndex>=pCoin->object.totalFrames)
				{
					pCoin->object.frameIndex = 0;
				}
			}
		}
		pCoin++;
	}
}

//---------------------------------------------------------
void RenderCoins(void)
{
	coin* pCoin = coins.coin;

	nextreg(MMU_SLOT_6, PICKUPS_PAGE);
	for (u8 i=0; i<MAX_COINS; i++)
	{
		if (pCoin->object.flags.active)
		{
			u8 animIndex = pCoin->object.baseIndex+pCoin->object.frameIndex;
			if (animIndex!=pCoin->object.lastIndex)
			{
				CopySprite(SWAP_BANK_0+animIndex*(16*16/2), pCoin->sprite.pattern, 1);
				pCoin->object.lastIndex = animIndex;
			}
			nextreg(SPRITE_INDEX, pCoin->sprite.slot);
			s16 tx = tileMap.position.x & I_TO_F(0xfffe);
			s16 ty = tileMap.position.y;

			s16 px = F_TO_I(tx+pCoin->object.position.x);
			s16 py = F_TO_I(ty+pCoin->object.position.y);
			
			s16 x = px+hud.shake.x+TILEMAP_PIX_WIDTH/2+10;
			s16 y = py+hud.shake.y+TILEMAP_PIX_HEIGHT/2-3;

			if ((x<-32)||(x>=320)||(y<-32)||(y>=256))
			{
				// Hide the sprite if clipped
				nextreg(SPRITE_ATTR_3, 0);
			}
			else
			{
				nextreg(SPRITE_ATTR_0, x);
				nextreg(SPRITE_ATTR_1, y);
				nextreg(SPRITE_ATTR_2, ((x>>8)&SPRITE_ATTR_2_X8)|(pCoin->sprite.palette<<4));
				nextreg(SPRITE_ATTR_3, SPRITE_ATTR_3_VISIBLE|SPRITE_ATTR_3_ENABLE_4|pCoin->sprite.pattern);
				nextreg(SPRITE_ATTR_4, SPRITE_ATTR_4_4BIT|((y>>8)&SPRITE_ATTR_4_Y8));
			}
		}
		pCoin++;
	}
}

//---------------------------------------------------------
u8 AddCoin(u8 type, s8 x, s8 y)
{
	u8 index = coins.itemIndex;
	coin* pCoin = (coin*)&coins.coin[coins.itemIndex];
	while (pCoin->object.flags.active)
	{
		index = (index+1)%MAX_COINS;
		pCoin = &coins.coin[index];
		if (index==coins.itemIndex)
		{
			x_printf("Ran out of coins\n");
			return 0;
		}
	}
	coins.itemIndex = (index+1)%MAX_COINS;
	pCoin->object.playGrid.x = x;
	pCoin->object.playGrid.y = y;
	SnapToPlayAreaGrid(&pCoin->object);
	//x_printf( "px:%d,py:%d,x:%d,y:%d\n", x, y, F_TO_I(pCoin->object.position.x), F_TO_I(pCoin->object.position.y));
	pCoin->object.flags.active = true;
	pCoin->object.flags.tilemapLocked = true;
	pCoin->type = type;
	pCoin->sprite.pattern = PLAYER_PICKUP_PATTERN+index;
	pCoin->sprite.palette = PICKUP_PALETTE;
	pCoin->sprite.slot = PLAYER_PICKUP_SLOT+index;
	pCoin->object.lastIndex = 0xff;
	pCoin->object.baseIndex = 0;
	pCoin->object.totalFrames = 9;
	pCoin->object.frameIndex = index%8;
	pCoin->object.animSpeed = 2+(index&1);
	pCoin->object.animDelay = index%4;
	return index;
}

//---------------------------------------------------------
void RemoveCoin(u8 coinIndex)
{
	coins.coin[coinIndex].object.flags.active = false;
}