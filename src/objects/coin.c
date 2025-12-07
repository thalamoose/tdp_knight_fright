#include "kftypes.h"
#include "defines.h"
#include "hardware.h"
#include "memorymap.h"
#include "assets.h"
#include "utilities.h"
#include "playarea.h"
#include "objects.h"
#include "sprites.h"
#include "objects/coin.h"
#include "hud.h"

#define COIN_PALETTE 8

//---------------------------------------------------------
void CreateCoin(coin* pCoin)
{
	(void)pCoin;	
}

void UpdateCoin(coin* pCoin)
{
	(void)pCoin;
}
//---------------------------------------------------------
void RenderCoin(coin* pCoin)
{
	nextreg(MMU_SLOT_6, PICKUPS_PAGE);
	if (pCoin->object.flags.active)
	{
		u8 animIndex = pCoin->object.baseIndex+pCoin->object.frameIndex;
		if (animIndex!=pCoin->object.lastIndex)
		{
			//CopySprite(SWAP_BANK_0+animIndex*(16*16/2), pCoin->sprite.pattern, 1);
			pCoin->object.lastIndex = animIndex;
		}
		//nextreg(SPRITE_INDEX, pCoin->sprite.slot);
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
			//nextreg(SPRITE_ATTR_2, ((x>>8)&SPRITE_ATTR_2_X8)|(pCoin->sprite.palette<<4));
			//nextreg(SPRITE_ATTR_3, SPRITE_ATTR_3_VISIBLE|SPRITE_ATTR_3_ENABLE_4|pCoin->sprite.pattern);
			nextreg(SPRITE_ATTR_4, SPRITE_ATTR_4_4BIT|((y>>8)&SPRITE_ATTR_4_Y8));
		}
	}
}

void DestroyCoin(coin* pCoin)
{
	(void)pCoin;
}

void BlowupCoin(coin* pCoin)
{
	(void)pCoin;
}

void CollideCoin(coin* pCoin)
{
	(void)pCoin;
}

const object_vtable coinVtable =
{
	(object_create_fn*)CreateCoin, 
	(object_update_fn*)UpdateCoin, 
	(object_destroy_fn*)DestroyCoin, 
	(object_blowup_fn*)BlowupCoin, 
	(object_collide_fn*)CollideCoin
};

//---------------------------------------------------------
u8 AddCoin(u8 type, s8 x, s8 y)
{
	coin* pCoin = (coin*)CreateObject(&coinVtable);
	pCoin->object.playGrid.x = x;
	pCoin->object.playGrid.y = y;
	SnapToPlayAreaGrid(&pCoin->object);
	//x_printf( "px:%d,py:%d,x:%d,y:%d\n", x, y, F_TO_I(pCoin->object.position.x), F_TO_I(pCoin->object.position.y));
	pCoin->object.flags.active = true;
	pCoin->object.flags.tilemapLocked = true;
	pCoin->type = type;
	//pCoin->sprite.pattern = AllocSpritePattern();
	//pCoin->sprite.palette = COIN_PALETTE;
	//pCoin->sprite.slot = AllocSpriteSlot();
	pCoin->object.lastIndex = 0xff;
	pCoin->object.baseIndex = 0;
	pCoin->object.totalFrames = 9;
	pCoin->object.frameIndex = pCoin->object.objectIndex%8;
	pCoin->object.animSpeed = 2+(pCoin->object.objectIndex&1);
	pCoin->object.animDelay = pCoin->object.objectIndex%4;
	return pCoin->object.objectIndex;
}

//---------------------------------------------------------
void RemoveCoin(u8 coinIndex)
{
	coin* pCoin = (coin*)GetObjectFromIndex(coinIndex);
	DestroyObject(&pCoin->object);
}