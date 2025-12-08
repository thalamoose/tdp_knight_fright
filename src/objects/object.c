#include "kftypes.h"
#include "defines.h"
#include "objects/object_manager.h"
#include "utilities.h"
#include "playarea.h"
#include "memorymap.h"
#include "hardware.h"
#include "tilemap.h"
#include "hud.h"

//---------------------------------------------------------
void TransformComponent(transform* trans)
{
	if (trans->vel.x|trans->vel.y)
	{
		trans->pos.x += trans->vel.x;
		trans->pos.y += trans->vel.y;
		trans->vel.y += trans->gravity;
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
void RenderComponent(transform* pTrans, animation* pAnim)
{
	u8 animIndex = pAnim->frameIndex+pAnim->baseIndex;
    if (animIndex != pAnim->lastIndex)
    {
        pAnim->lastIndex = animIndex;
        u8 page = (animIndex>>3)+pAnim->sprite.page;
        nextreg(MMU_SLOT_6, page);
        u8 *pPattern = (u8 *)SWAP_BANK_0+((animIndex&7)<<10);
        CopySprite(pPattern, pAnim->sprite.pattern, pAnim->sprite.patternCount);
    }
    nextreg(SPRITE_INDEX, pAnim->sprite.slot);
    s16 tx = tileMap.position.x&I_TO_F(0xfffe);
    s16 ty = tileMap.position.y;

    s16 px = F_TO_I(tx+pTrans->pos.x);
    s16 py = F_TO_I(ty+pTrans->pos.y);
     
    s16 x = px+hud.shake.x+TILEMAP_PIX_WIDTH/2+pAnim->sprite.centerOffset.x;
    s16 y = py+hud.shake.y+TILEMAP_PIX_HEIGHT/2+pAnim->sprite.centerOffset.y;

    if ((x < -32) || (x >= 320) || (y < -32) || (y >= 256))
    {
        // Hide the sprite if clipped
        nextreg(SPRITE_ATTR_3, 0);
        return;
    }
    nextreg(SPRITE_ATTR_0, x&0xff);
    nextreg(SPRITE_ATTR_1, y);
    nextreg(SPRITE_ATTR_2, (x>>8)&1);
    nextreg(SPRITE_ATTR_3, 0xc0);
    nextreg(SPRITE_ATTR_4, (y>>8)&1);

}
