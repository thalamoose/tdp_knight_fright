#include "kftypes.h"
#include "defines.h"
#include "hardware.h"
#include "memorymap.h"
#include "assets.h"
#include "utilities.h"
#include "playarea.h"
#include "objects/object.h"
#include "objects/object_manager.h"
#include "objects/components.h"
#include "objects/coin.h"
#include "hud.h"

#define COIN_PALETTE 8

//---------------------------------------------------------
void CreateCoin(game_object* pCoin, s8 x, s8 y)
{
	(void)(pCoin+x+y);
}

bool UpdateCoin(game_object* pCoin)
{
	(void)pCoin;
	return true;
}
//---------------------------------------------------------
void RenderCoin(game_object* pCoin)
{
	nextreg(MMU_SLOT_6, PICKUPS_PAGE);
	if (pCoin->flags.active)
	{
		u8 animIndex = pCoin->anim.baseIndex+pCoin->anim.frameIndex;
		if (animIndex!=pCoin->anim.lastIndex)
		{
			//CopySprite(SWAP_BANK_0+animIndex*(16*16/2), pCoin->sprite.pattern, 1);
			pCoin->anim.lastIndex = animIndex;
		}
		//nextreg(SPRITE_INDEX, pCoin->sprite.slot);
		s16 tx = tileMap.position.x & I_TO_F(0xfffe);
		s16 ty = tileMap.position.y;

		s16 px = F_TO_I(tx+pCoin->trans.pos.x);
		s16 py = F_TO_I(ty+pCoin->trans.pos.y);
		
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

void DestroyCoin(game_object* pCoin)
{
	(void)pCoin;
}

void BlowupCoin(game_object* pCoin)
{
	(void)pCoin;
}

void CollideCoin(game_object* pCoin, play_cell* pCollider)
{
	(void)pCoin;
	(void)pCollider;
}

const object_vtable coinVirtualTable =
{
	CreateCoin, 
	UpdateCoin, 
	RenderCoin,
	DestroyCoin, 
	CollideCoin,
	BlowupCoin, 
};

//---------------------------------------------------------
u8 AddCoin(u8 type, s8 x, s8 y)
{
	(void)type;
	game_object* pCoin = CreateObject(&coinVirtualTable, x, y);
	//x_printf( "px:%d,py:%d,x:%d,y:%d\n", x, y, F_TO_I(pCoin->position.x), F_TO_I(pCoin->position.y));
	pCoin->flags.active = true;
	pCoin->flags.tilemapLocked = true;
	//pCoin->type = type;
	//pCoin->sprite.pattern = AllocSpritePattern();
	//pCoin->sprite.palette = COIN_PALETTE;
	//pCoin->sprite.slot = AllocSpriteSlot();
	pCoin->anim.lastIndex = 0xff;
	pCoin->anim.baseIndex = 0;
	pCoin->anim.totalFrames = 9;
	pCoin->anim.frameIndex = pCoin->object.index%8;
	pCoin->anim.animSpeed = 2+(pCoin->object.index&1);
	pCoin->anim.animDelay = pCoin->object.index%4+1;
	return pCoin->object.index;
}

//---------------------------------------------------------
void RemoveCoin(u8 coinIndex)
{
	game_object* pCoin = GetObjectFromIndex(coinIndex);
	DestroyObject(pCoin);
}