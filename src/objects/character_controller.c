#include "kftypes.h"
#include "utilities.h"
#include "defines.h"
#include "objects/object_manager.h"
#include "objects/player.h"
#include "objects/coin.h"
#include "objects/character_controller.h"
#include "enemies/bear.h"
#include "enemies/big_hopper.h"
#include "enemies/follower.h"
#include "enemies/spike.h"
#include "enemies/color_changer.h"
#include "hud.h"
#include "hardware.h"
#include "playarea.h"
#include "tilemap.h"
#include "level_manager.h"

//---------------------------------------------------------
object_vtable enemyVirtualTable[]=
{
	{
		CreateBear, 
	 	UpdateBear, 
		RenderBear,
	 	DestroyBear, 
	 	CollideBear,
	 	BlowupBear 
	},
	{
		CreateBigHopper, 
	 	UpdateBigHopper, 
		RenderBigHopper,
	 	DestroyBigHopper, 
	 	CollideBigHopper,
	 	BlowupBigHopper
	},
	{
		CreateColorChanger, 
		UpdateColorChanger, 
		RenderColorChanger,
		DestroyColorChanger, 
		CollideColorChanger,
		BlowupColorChanger
	},
	{
		CreateFollower, 
		UpdateFollower, 
		RenderFollower,
		DestroyFollower, 
		CollideFollower,
		BlowupFollower
	},
	{
		CreateSpike, 
		UpdateSpike, 
		RenderSpike,
		DestroySpike, 
		CollideSpike,
		BlowupSpike
	},
};

//---------------------------------------------------------
game_object* CreateEnemyObject(u8 type, const coord_s8* mapPosition, u16 param)
{
	type = 0; //*DEBUG*
	return CreateObject(&enemyVirtualTable[type], mapPosition, param);
}

//---------------------------------------------------------
anim_config playerMoves[4]=
{
    {PLAYERSPR_L+PLAYERSPR_RUN_ANIM, PLAYERDIR_TL,-FIXED_POINT_ONE, -FIXED_POINT_ONE*2},
    {PLAYERSPR_R+PLAYERSPR_RUN_ANIM, PLAYERDIR_BR, FIXED_POINT_ONE, -FIXED_POINT_ONE*5},
    {PLAYERSPR_U+PLAYERSPR_RUN_ANIM, PLAYERDIR_TR,-FIXED_POINT_ONE, -FIXED_POINT_ONE*5},
    {PLAYERSPR_D+PLAYERSPR_RUN_ANIM, PLAYERDIR_BL, FIXED_POINT_ONE, -FIXED_POINT_ONE*2}
};

//---------------------------------------------------------
void MoveCharacter(game_object* pObject, u8 spriteBase, u8 buttons)
{
    if (hud.transitionIsRunning)
        return;
    if (pObject->moveSteps)
    {
        TransformComponent(&pObject->trans);
        pObject->moveSteps--;
        if (pObject->moveSteps != 0)
        {
            s16 dy = F_TO_I(pObject->trans.pos.y+tileMap.position.y);
            if (dy > 256+32)
            {
                // We must have been in a die. Respawn now.
                ResetPlayer(pObject);
            }
            return;
        }
        SnapToPlayAreaGrid(pObject);
        pObject->trans.gravity = FIXED_POINT_HALF;
        play_cell *pCell = GetPlayAreaCell(&pObject->playGrid);
        if (pCell)
        {
            game_object* pCollider = GetObjectFromIndex(pCell->objIndex);
			if (pObject->object.vtable->Collide)
            	pObject->object.vtable->Collide(pObject, pCollider);
        }
        return;
    }

	s8 nx = pObject->playGrid.x;
    s8 ny = pObject->playGrid.y;
    u8 anim = 0;
    u8 dir = 0;
    s16 vx = 0;
    s16 vy = 0;
    const anim_config* pAnimConfig = NULL;
    if (buttons&JOYPAD_L_LEFT)
    {
        nx--;
        pAnimConfig = &playerMoves[0];
    }
    else if (buttons&JOYPAD_L_RIGHT)
    {
        nx++;
        pAnimConfig = &playerMoves[1];
    }
    else if (buttons&JOYPAD_L_UP)
    {
        ny--;
        pAnimConfig = &playerMoves[2];
    }
    else if (buttons&JOYPAD_L_DOWN)
    {
        ny++;
        pAnimConfig = &playerMoves[3];
        anim = spriteBase+PLAYERSPR_D+PLAYERSPR_RUN_ANIM;
        dir = PLAYERDIR_BL;
        vx = FIXED_POINT_ONE;
        vy = -FIXED_POINT_ONE*2;
    }
    if (pAnimConfig)
    {
        coord_s8 mapPosition={nx,ny};
        play_cell* pCell = GetPlayAreaCell(&mapPosition);
        if (pCell->type!=CELL_OBSTACLE)
        {
            pObject->playGrid.x = nx;
            pObject->playGrid.y = ny;
            SetObjectAnimRun(pObject, pAnimConfig);
        }
    }
    if (buttons&JOYPAD_R_DOWN)
    {
        levelManager.tilesRemaining = 1;
    }
}

//---------------------------------------------------------
void SetObjectAnimIdle(game_object* pObj, const anim_config* config)
{
    pObj->trans.vel.x = config->vx;
    pObj->trans.vel.y = config->vy;
    pObj->anim.frameIndex = 0;
    pObj->anim.baseIndex = config->baseIndex;
    pObj->anim.lastIndex = 0xff;
    pObj->anim.animDelay = 4;
    pObj->anim.animSpeed = 4;
    pObj->anim.totalFrames = 8;
}

//---------------------------------------------------------
void SetObjectAnimRun(game_object* pObj, const anim_config* pConfig )
{
    pObj->moveSteps = 16;
    pObj->direction = pConfig->direction;
    SetObjectAnimIdle(pObj, pConfig);
}

