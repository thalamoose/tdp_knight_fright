#include "kftypes.h"
#include "defines.h"
#include "hardware.h"
#include "memorymap.h"
#include "assets.h"
#include "utilities.h"
#include "playarea.h"
#include "sprite.h"
#include "objects/object.h"
#include "objects/object_manager.h"
#include "objects/components.h"
#include "objects/coin.h"
#include "hud.h"
#include "game_manager.h"

//---------------------------------------------------------
void CreateCoin(game_object* pCoin, s8 x, s8 y)
{
	u8 type=(gameManager.useSuperCoins)?COIN_SUPER:COIN_NORMAL;
	pCoin->flags.active = true;
	pCoin->flags.tilemapLocked = true;
	pCoin->flags.is4bit = true;
	pCoin->playGrid.x = x;
	pCoin->playGrid.y = y;
	pCoin->anim.sprite.pattern = AllocSpritePattern();
	pCoin->anim.sprite.patternCount = 1;
	pCoin->anim.sprite.palette = COIN_PALETTE+type;
	pCoin->anim.sprite.page = PICKUPS_PAGE;
	pCoin->anim.sprite.slot = AllocSpriteSlot();
	pCoin->anim.lastIndex = 0xff;
	pCoin->anim.baseIndex = type*9;
	pCoin->anim.totalFrames = 9;
	pCoin->anim.frameIndex = pCoin->object.index%8;
	pCoin->anim.animSpeed = 2+(pCoin->object.index&1);
	pCoin->anim.animDelay = pCoin->object.index%4+1;
}

//---------------------------------------------------------
bool UpdateCoin(game_object* pCoin)
{
	AnimateComponent(&pCoin->anim);
	TransformComponent(&pCoin->trans);
	CheckObjectToPlayerCollision(pCoin);
	return true;
}

//---------------------------------------------------------
void DestroyCoin(game_object* pCoin)
{
	(void)pCoin;
}

//---------------------------------------------------------
void BlowupCoin(game_object* pCoin)
{
	(void)pCoin;
}

//---------------------------------------------------------
void CollideCoin(game_object* pCoin, const game_object* pCollider)
{

	(void)pCoin;
	(void)pCollider;
}

//---------------------------------------------------------
const object_vtable coinVirtualTable =
{
	CreateCoin, 
	UpdateCoin, 
	RenderComponent,
	DestroyCoin, 
	CollideCoin,
	BlowupCoin, 
};

//---------------------------------------------------------
game_object* CreateCoinObject(s8 x, s8 y)
{
	return CreateObject(&coinVirtualTable, x, y);
}

//---------------------------------------------------------
void RemoveCoin(u8 coinIndex)
{
	game_object* pCoin = GetObjectFromIndex(coinIndex);
	DestroyObject(pCoin);
}