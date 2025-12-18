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
#include "objects/coin.h"
#include "hud.h"
#include "game_manager.h"

//---------------------------------------------------------
void CreateCoin(game_object* pCoin, const coord_s8* mapPosition, u16 param)
{
	UNUSED(param);
	u8 type=(gameManager.useSuperCoins)?COIN_SUPER:COIN_NORMAL;
	pCoin->flags = FLAG_ACTIVE|FLAG_TILEMAP_LOCKED|FLAG_IS_4_BIT;
	pCoin->playGrid = *mapPosition;
	pCoin->anim.sprite.pattern[0] = AllocSpritePattern();
	pCoin->anim.sprite.patternCount = 1;
	pCoin->anim.sprite.palette = COIN_PALETTE+type;
	pCoin->anim.sprite.page = PICKUPS_PAGE;
	pCoin->anim.sprite.slot[0] = AllocSpriteSlot();
	pCoin->anim.sprite.centerOffset.x = 10;
	pCoin->anim.sprite.centerOffset.y = -6;

	pCoin->anim.lastIndex = 0xff;
	pCoin->anim.baseIndex = type*9;
	pCoin->anim.totalFrames = 9;
	pCoin->anim.frameIndex = pCoin->object.index%8;
	pCoin->anim.animSpeed = 2+(pCoin->object.index&1);
	pCoin->anim.animDelay = pCoin->object.index%4+1;
	nextreg(SWAP_BANK_0_SLOT, MISC_DATA_PAGE);
	CopyPalettePartial(asset_CoinPalette, PALETTE_SPRITE_PRIMARY, pCoin->anim.sprite.palette*16, 16);

	play_cell* pCell = GetPlayAreaCell(mapPosition);
	pCell->type = CELL_COIN;
	pCell->objIndex = pCoin->object.index;
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
	HideSprite(pCoin->anim.sprite.slot[0]);
}

//---------------------------------------------------------
void BlowupCoin(game_object* pCoin)
{
	DestroyObject(pCoin);
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
game_object* CreateCoinObject(const coord_s8* mapPosition)
{
	return CreateObject(&coinVirtualTable, mapPosition, 0);
}

//---------------------------------------------------------
void RemoveCoin(u8 coinIndex)
{
	game_object* pCoin = GetObjectFromIndex(coinIndex);
	DestroyObject(pCoin);
}