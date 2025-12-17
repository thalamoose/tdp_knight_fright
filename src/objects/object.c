#include "kftypes.h"
#include "defines.h"
#include "objects/object_manager.h"
#include "utilities.h"
#include "playarea.h"
#include "memorymap.h"
#include "hardware.h"
#include "tilemap.h"
#include "hud.h"
#include "level_manager.h"
#include "sprite.h"

//---------------------------------------------------------
void TransformComponent(transform* trans)
{
	trans->pos.x += trans->vel.x;
	trans->pos.y += trans->vel.y;
	trans->vel.y += trans->gravity;
}

//---------------------------------------------------------
// Did object collide with player?
void CheckObjectToPlayerCollision(game_object* pObject)
{
	if (pObject==levelManager.player)
		return;
	if ((pObject->playGrid.x==levelManager.player->playGrid.x) &&
		(pObject->playGrid.y==levelManager.player->playGrid.y))
	{
		pObject->object.vtable->Collide(pObject, levelManager.player);
	}
}

//---------------------------------------------------------
// Did player collide with object?
void CheckPlayerToObjectCollision(game_object* pObject)
{
	play_cell* pCell = GetPlayAreaCell(&pObject->playGrid);
	if (pCell->type!=CELL_ENEMY)
		return;
	game_object* pCollider = GetObjectFromIndex(pCell->objIndex);
	if ((pObject->playGrid.x==pCollider->playGrid.x) &&
		(pObject->playGrid.y==pCollider->playGrid.y))
	{
		pObject->object.vtable->Collide(pObject, levelManager.player);
	}
}

//---------------------------------------------------------
void AnimateComponent(animation* pAnim)
{
	if (pAnim->animDelay)
	{
		pAnim->animDelay--;
		return;
	}
	pAnim->animDelay = pAnim->animSpeed;
	if (pAnim->flags.direction)
	{
		pAnim->frameIndex--;
		if (pAnim->frameIndex == 0)
			pAnim->flags.direction = false;
	}
	else
	{
		pAnim->frameIndex++;
		if (pAnim->frameIndex == pAnim->totalFrames)
		{
			if (pAnim->flags.pingpong)
			{
				pAnim->flags.direction = true;
				pAnim->frameIndex = pAnim->totalFrames - 1;
			}
			else
				pAnim->frameIndex = 0;
		}
	}
}

//---------------------------------------------------------
void RenderComponent(game_object* pObject)
{
	u16 animIndex = (pObject->anim.frameIndex+pObject->anim.baseIndex)*pObject->anim.sprite.patternCount;
    if (animIndex != pObject->anim.lastIndex)
    {
        pObject->anim.lastIndex = animIndex;
		u8 page;
        u8 *pPattern;
		u16 xferSize;
		if (pObject->flags & FLAG_IS_4_BIT)
		{
			// There's something fishy here. I'm not quite sure why I have to download 256 bytes
			// for a 4 bit sprite.
        	page = (animIndex>>5)+pObject->anim.sprite.page;
			pPattern = (u8 *)SWAP_BANK_0+(u16)(animIndex&0x1f)*16*16;
			xferSize = ((u16)pObject->anim.sprite.patternCount)*(16*16);
		}
		else
		{
        	page = (animIndex>>5)+pObject->anim.sprite.page;
 			pPattern = (u8 *)SWAP_BANK_0+(u16)(animIndex&0x1f)*16*16;
			xferSize = ((u16)pObject->anim.sprite.patternCount)*(16*16);
		}
        nextreg(SWAP_BANK_0_SLOT, page);
		//x_printf("%x:src:%x,p:%d,#:%d,f:%x\n", pObject, pPattern, (u16)pObject->anim.sprite.pattern, (u16)pObject->anim.sprite.patternCount, pObject->flags.value);

        CopySprite(pPattern, pObject->anim.sprite.pattern, xferSize);
    }
    s16 tx = pObject->trans.pos.x+tileMap.position.x&I_TO_F(0xfffe);
    s16 px = F_TO_I(tx);
    s16 x = px+hud.shake.x+TILEMAP_PIX_WIDTH/2+pObject->anim.sprite.centerOffset.x;

	s16 ty = pObject->trans.pos.y+tileMap.position.y;
    s16 py = F_TO_I(ty);
    s16 y = py+hud.shake.y+TILEMAP_PIX_HEIGHT/2+pObject->anim.sprite.centerOffset.y;

	// This weirdity is on purpose. It checks for >320, as well as <-32 in one comparison
	// due to the sign being ignored.
	if ((u16)(x-32)>LAYER_2_WIDTH || (u16)(y-32)>LAYER_2_HEIGHT)
    {
        // Hide the sprite if clipped
    	HideSprite(pObject->anim.sprite.slot);
        return;
    }

	sprite_config config=
	{
		pObject->anim.sprite.pattern,
		x,
		y,
		(pObject->anim.sprite.palette<<4)|((x>>8)&SPRITE_ATTR_2_X8),
		SPRITE_ATTR_3_VISIBLE|SPRITE_ATTR_3_ENABLE_ATTR_4,
		(y>>8)&SPRITE_ATTR_4_Y8
	};

	if (pObject->anim.flags.is4bit) config.attr4 |= SPRITE_ATTR_4_4BIT;

	SetupSprite(pObject->anim.sprite.slot, &config);
}

