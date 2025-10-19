#include "kftypes.h"
#include "defines.h"
#include "hardware.h"
#include "memorymap.h"
#include "utilities.h"
#include "objects.h"
#include "player.h"
#include "sprites.h"
#include "input.h"
#include "globals.h"

player_object player;

extern u8 asset_PlayerPalette[];

//---------------------------------------------------------
void SetPlayerAnimIdle(u8 baseIndex, s16 vx, s16 vy)
{
    player.object.velocity.x = vx;
    player.object.velocity.y = vy;
    player.object.baseIndex = baseIndex;
    player.object.totalFrames = 8;
    player.object.frameIndex = 0;
    player.object.animDelay = 4;
    player.object.animSpeed = 4;
}

//---------------------------------------------------------
void SetPlayerAnim(u8 baseIndex, u8 direction, s16 vx, s16 vy)
{
    player.moveSteps = 16;
    player.direction = direction;
    SetPlayerAnimIdle(baseIndex, vx, vy);
}

//---------------------------------------------------------
u8 GetPlaygridContent(u8 x, u8 y)
{
    u8* tileEntry = (u8*)(SWAP_BANK_0)+(y*40+x)*2;
    nextreg(MMU_SLOT_6,TILEMAP_PAGE);
    nextreg(MMU_SLOT_7,TILEMAP_PAGE+1);
    return *tileEntry;
}

//---------------------------------------------------------
void SnapToGrid(void)
{
    player.object.position.x = ((global.playArea.x+player.playgrid.x)*24+LAYER_2_WIDTH/2+12)<<FIXED_POINT_BITS;
    player.object.position.y = ((global.playArea.y+player.playgrid.y)*24+LAYER_2_HEIGHT/2)<<FIXED_POINT_BITS;
}

//---------------------------------------------------------
void InitializePlayer(void)
{
    player.object.position.x = (320/2)<<FIXED_POINT_BITS;
    player.object.position.y = (256/2)<<FIXED_POINT_BITS;
    player.object.direction = 0;
    player.playgrid.x = PLAY_AREA_CELLS_WIDTH/2;
    player.playgrid.y = PLAY_AREA_CELLS_HEIGHT/2;
    player.object.frameIndex = 0;
    player.object.baseIndex = 32;
    player.object.animDelay = 4;
    player.object.animSpeed = 4;
    player.object.totalFrames = 8;
    player.object.gravity = FIXED_POINT_HALF;
//
// Set up sprites 64..67, so that only the minimum needs to be set up below. 
//
    SetupSprite(PLAYER_SPRITE_SLOT, PLAYER_SPRITE_PATTERN, 0, 0, 0, 0, 0);
    SetupSprite(PLAYER_SPRITE_SLOT+1, PLAYER_SPRITE_PATTERN+1, 16, 0, 0, 0, 0x60);
    SetupSprite(PLAYER_SPRITE_SLOT+2, PLAYER_SPRITE_PATTERN+2, 0, 16, 0, 0, 0);
    SetupSprite(PLAYER_SPRITE_SLOT+3, PLAYER_SPRITE_PATTERN+3, 16,16, 0, 0, 0);

    CopyPalette(asset_PlayerPalette,2);
    nextreg(MMU_SLOT_6,PLAYER_ANIM_PAGE);
    nextreg(TRANS_SPRITE_INDEX,*(u8*)SWAP_BANK_0);
    SnapToGrid();
}

//---------------------------------------------------------
void MovePlayer(void)
{
    if (player.moveSteps)
    {
        player.object.position.x += player.object.velocity.x;
        player.object.position.y += player.object.velocity.y;
        player.object.velocity.y += player.object.gravity;
        player.moveSteps--;
        if (player.moveSteps==0)
        {
            SnapToGrid();
            u8 content = GetPlaygridContent(player.playgrid.x, player.playgrid.y);
            if (content==0)
            {
                // dead now.
            }
            else
            {
                SetPlayerAnimIdle(player.direction*8+PLAYERSPR_IDLE_ANIM,0,0);
            }
        }
    }
    if (player.moveSteps==0)
    {
        u8 buttons = ReadController();
        if (buttons & (1<<JOYPAD_L_LEFT))
        {
            player.playgrid.x--;
            player.playgrid.y++;
            SetPlayerAnim(PLAYERSPR_L+PLAYERSPR_RUN_ANIM,PLAYERDIR_BL,-FIXED_POINT_ONE,-FIXED_POINT_ONE*2);
            return;
        }
        if (buttons & (1<<JOYPAD_L_RIGHT))
        {
            player.playgrid.x++;
            player.playgrid.y--;
            SetPlayerAnim(PLAYERSPR_R+PLAYERSPR_RUN_ANIM,PLAYERDIR_BR,FIXED_POINT_ONE,-FIXED_POINT_ONE*5);
            return;
        }
        if (buttons & (1<<JOYPAD_L_UP))
        {
            player.playgrid.x--;
            player.playgrid.y--;
            SetPlayerAnim(PLAYERSPR_U+PLAYERSPR_RUN_ANIM,PLAYERDIR_TL,-FIXED_POINT_ONE,-FIXED_POINT_ONE*5);
            return;
        }
        if (buttons & (1<<JOYPAD_L_DOWN))
        {
            player.playgrid.x++;
            player.playgrid.y++;
            SetPlayerAnim(PLAYERSPR_D+PLAYERSPR_RUN_ANIM,PLAYERDIR_BL,FIXED_POINT_ONE,-FIXED_POINT_ONE*2);
            return;
        }
    }
}


//---------------------------------------------------------
void AnimatePlayer(void)
{
    player.object.animDelay--;
    if (player.object.animDelay==0)
    {
        player.object.animDelay = player.object.animSpeed;
        player.object.frameIndex++;
        if (player.object.frameIndex==player.object.totalFrames)
        {
            player.object.frameIndex = 0;
        }
    }
}

//---------------------------------------------------------
void UpdatePlayer(void)
{
    MovePlayer();
    AnimatePlayer();
}

//---------------------------------------------------------
void RenderPlayer(void)
{
    u8 animIndex = player.object.baseIndex+player.object.frameIndex;
    u8 page = (animIndex*8)+PLAYER_ANIM_PAGE;
    nextreg(MMU_SLOT_6,page);
    u8* pPattern = (u8*)SWAP_BANK_0+(page&7<<2);
    CopySprite(pPattern, PLAYER_SPRITE_PATTERN, 4);
    nextreg(SPRITE_INDEX, PLAYER_SPRITE_SLOT);
    s16 x = player.object.position.x>>FIXED_POINT_BITS;
    s16 y = player.object.position.y>>FIXED_POINT_BITS;
    nextreg(SPRITE_ATTR_0, x&0xff);
    nextreg(SPRITE_ATTR_1, y);
    nextreg(SPRITE_ATTR_2, (x>>8)&1);
}
