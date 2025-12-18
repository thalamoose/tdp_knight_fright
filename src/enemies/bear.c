#include "kftypes.h"
#include "defines.h"
#include "objects/object_manager.h"
#include "enemies/enemy_controller.h"
#include "enemies/bear.h"
#include "memorymap.h"
#include "assets.h"
#include "sprite.h"
#include "utilities.h"
#include "hardware.h"

const sprite_config bearSpriteConfig[4]=
{
    {0,  0,  0, 0, 0x40, 0x00},
    {0, 16,  0, 0, 0xc0, 0x60},
    {0,  0, 16, 0, 0xc0, 0x60},
    {0, 16, 16, 0, 0xc0, 0x60}
};

anim_config bearIdleConfig = {PLAYERSPR_IDLE_ANIM, 0, 0};

void CreateBear(game_object* pBear, const coord_s8* mapPosition, u16 param)
{
	UNUSED(param);

	pBear->anim.sprite.page = BEAR_ANIM_PAGE;
    pBear->anim.sprite.patternCount = 4;
    pBear->anim.sprite.centerOffset.x = 0;
    pBear->anim.sprite.centerOffset.y = -18;

	SetObjectAnimIdle(pBear, &bearIdleConfig);
	for (u8 i=0; i<pBear->anim.sprite.patternCount; i++)
	{
		sprite_config spriteConfig = bearSpriteConfig[i];
		pBear->anim.sprite.slot[i] = AllocSpriteSlot();
		pBear->anim.sprite.pattern[i] = AllocSpritePattern();
		spriteConfig.pattern = pBear->anim.sprite.pattern[i];
		spriteConfig.attr2 |= pBear->anim.sprite.palette<<4;
        SetupSprite(pBear->anim.sprite.slot[i],  &spriteConfig);
	}
    pBear->playGrid = *mapPosition;
}

void MoveBear(game_object* pBear)
{
	UNUSED(pBear);
}

bool UpdateBear(game_object* pBear)
{
    MoveBear(pBear);
    AnimateComponent(&pBear->anim);
	return true;
}

void DestroyBear(game_object* pObject)
{
	(void)pObject;
}

void RenderBear(game_object* pObject)
{
	RenderComponent(pObject);
}

void CollideBear(game_object* pObject, const game_object* pCollider)
{
	(void)pObject;
	(void)pCollider;
}

void BlowupBear(game_object* pObject)
{
	(void)pObject;
}

