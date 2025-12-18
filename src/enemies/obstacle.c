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
#include "objects/obstacle.h"
#include "hud.h"
#include "game_manager.h"

const sprite_config obstacleSpriteConfig[4]=
{
    {0,  0,  0, 0, 0x40, 0x00},
    {0, 16,  0, 0, 0xc0, 0x60},
    {0,  0, 16, 0, 0xc0, 0x60},
    {0, 16, 16, 0, 0xc0, 0x60}
};

//---------------------------------------------------------
void CreateObstacle(game_object* pObstacle, const coord_s8* mapPosition, u16 param)
{
	UNUSED(param);
	pObstacle->flags = FLAG_ACTIVE|FLAG_TILEMAP_LOCKED;
	pObstacle->playGrid = *mapPosition;
	pObstacle->anim.sprite.patternCount = 4;
	pObstacle->anim.sprite.palette = OBSTACLE_PALETTE;
	pObstacle->anim.sprite.page = OBSTACLES_PAGE;
	pObstacle->anim.sprite.centerOffset.x = 0;
	pObstacle->anim.sprite.centerOffset.y = -18;

	for (u8 i=0; i<pObstacle->anim.sprite.patternCount; i++)
	{
		sprite_config spriteConfig = obstacleSpriteConfig[i];
		pObstacle->anim.sprite.slot[i] = AllocSpriteSlot();
		pObstacle->anim.sprite.pattern[i] = AllocSpritePattern();
		spriteConfig.pattern = pObstacle->anim.sprite.pattern[i];
		spriteConfig.attr2 |= pObstacle->anim.sprite.palette<<4;
        SetupSprite(pObstacle->anim.sprite.slot[i],  &spriteConfig);
	}

	pObstacle->anim.lastIndex = 0xff;
	pObstacle->anim.baseIndex = param;
	pObstacle->anim.totalFrames = 1;
	pObstacle->anim.frameIndex = 0;
	pObstacle->anim.animSpeed = 0;
	pObstacle->anim.animDelay = 0;

	nextreg(SWAP_BANK_0_SLOT, MISC_DATA_PAGE);
	CopyPalettePartial(asset_ObstaclePalette, PALETTE_SPRITE_PRIMARY, pObstacle->anim.sprite.palette*16, 32);

	play_cell* pCell = GetPlayAreaCell(mapPosition);
	pCell->type = CELL_OBSTACLE;
	pCell->objIndex = pObstacle->object.index;
}

//---------------------------------------------------------
bool UpdateObstacle(game_object* pObject)
{
	TransformComponent(&pObject->trans);
	AnimateComponent(&pObject->anim);
	return true;
}

//---------------------------------------------------------
void RenderObstacle(game_object* pObject)
{
	RenderComponent(pObject);
}

//---------------------------------------------------------
void DestroyObstacle(game_object* pObject)
{
	(void)pObject;
}

//---------------------------------------------------------
const object_vtable obstacleVirtualTable =
{
	CreateObstacle, 
	UpdateObstacle, 
	RenderObstacle,
	DestroyObstacle, 
	NULL,
	NULL, 
};


//---------------------------------------------------------
game_object* CreateObstacleObject(const coord_s8* mapPosition, u16 param)
{
	return CreateObject(&obstacleVirtualTable, mapPosition, param);
}
