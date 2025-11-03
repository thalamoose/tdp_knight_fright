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
#include "particles.h"

extern u8 asset_PlayerPalette[];

//---------------------------------------------------------
void SetPlayerAnimIdle(u8 baseIndex, s16 vx, s16 vy)
{
    global.player.object.velocity.x = vx;
    global.player.object.velocity.y = vy;
    global.player.object.baseIndex = baseIndex;
    global.player.object.totalFrames = 8;
    global.player.object.frameIndex = 0;
    global.player.object.animDelay = 4;
    global.player.object.animSpeed = 4;
}

//---------------------------------------------------------
void SetPlayerAnim(u8 baseIndex, u8 direction, s16 vx, s16 vy)
{
    global.player.moveSteps = 16;
    global.player.direction = direction;
    SetPlayerAnimIdle(baseIndex, vx, vy);
}

//---------------------------------------------------------
void SnapToGrid(void)
{
    global.player.object.position.x = ((global.player.playgrid.x-global.playArea.position.x-2)*16+LAYER_2_WIDTH/2+12)<<FIXED_POINT_BITS;
    global.player.object.position.y = ((global.player.playgrid.y-global.playArea.position.y-1)*24+LAYER_2_HEIGHT/2-14)<<FIXED_POINT_BITS;
}

//---------------------------------------------------------
void InitializePlayer(void)
{
    global.player.object.position.x = (320/2)<<FIXED_POINT_BITS;
    global.player.object.position.y = (256/2)<<FIXED_POINT_BITS;
    global.player.object.flags.direction = 0;
    global.player.object.frameIndex = 0;
    global.player.object.baseIndex = 32;
    global.player.object.animDelay = 4;
    global.player.object.animSpeed = 4;
    global.player.object.totalFrames = 8;
    global.player.object.gravity = FIXED_POINT_HALF;
	global.player.playgrid.x = PLAY_AREA_CELLS_WIDTH/2;
	global.player.playgrid.y = PLAY_AREA_CELLS_HEIGHT/2;
    global.player.moveSteps = 0;
//
// Set up sprites 64..67, so that only the minimum needs to be set up below. 
//
    SetupSprite(PLAYER_SPRITE_SLOT, PLAYER_SPRITE_PATTERN, 0, 0, 0, 0xc0, 0);
    SetupSprite(PLAYER_SPRITE_SLOT+1, PLAYER_SPRITE_PATTERN+1, 16, 0, 0, 0xc0, 0x60);
    SetupSprite(PLAYER_SPRITE_SLOT+2, PLAYER_SPRITE_PATTERN+2, 0, 16, 0, 0xc0, 0x60);
    SetupSprite(PLAYER_SPRITE_SLOT+3, PLAYER_SPRITE_PATTERN+3, 16,16, 0, 0xc0, 0x60);

    nextreg(MMU_SLOT_6, PALETTE_PAGE);
    CopyPalette(asset_PlayerPalette,2);
    // Grab whatever colour is the background colour. This will be our transparent
    // index.
    nextreg(MMU_SLOT_6,PLAYER_ANIM_PAGE);
    nextreg(TRANS_SPRITE_INDEX,*(u8*)SWAP_BANK_0);
    SnapToGrid();
}

//---------------------------------------------------------
void MovePlayer(void)
{
    if (global.player.moveSteps)
    {
        global.player.object.position.x += global.player.object.velocity.x;
        global.player.object.position.y += global.player.object.velocity.y;
        global.player.object.velocity.y += global.player.object.gravity;
        global.player.moveSteps--;
        if (global.player.moveSteps!=0)
        {
            if (global.player.object.position.y>(240<<FIXED_POINT_BITS))
            {
                // We must have been in a die. Respawn now.
                InitializePlayer();
            }
            return;
        }
        SnapToGrid();
        u8 content = GetPlayAreaContent(global.player.playgrid.x, global.player.playgrid.y);
        x_printf("Coord:(%d,%d), content:%c\n", global.player.playgrid.x, global.player.playgrid.y, content);
        if (content==2)
        {
            // dead now.
            SetPlayerAnimIdle(global.player.direction*8+PLAYERSPR_IDLE_ANIM,0,0);
            for (int i=0; i<32; i++)
            {
                s16 px = global.player.object.position.x+(16<<FIXED_POINT_BITS)+(get_random_16()>>10);
                s16 py = global.player.object.position.y+(8<<FIXED_POINT_BITS)+(get_random_16()>>10);
                s16 vx = get_random_16()>>8;
                s16 vy = get_random_16()>>8;
                s8 width = get_random()&3+1;
                s8 colour = (get_random()&63)+1;
                s8 age = (get_random()&31)+24;
                AddParticle(px, py, vx, vy, age, colour, width, 0);
            }
        }
        else if (content==0)
        {
            SetPlayerAnimIdle(global.player.direction*8+PLAYERSPR_IDLE_ANIM,0,-FIXED_POINT_ONE*2);
            global.player.moveSteps = 64;
            global.player.object.gravity = FIXED_POINT_HALF/2;
        }
        else
        {
            SetPlayerAnimIdle(global.player.direction*8+PLAYERSPR_IDLE_ANIM,0,0);
        }
    }
    u8 buttons = ReadController();
    if (buttons & (1<<JOYPAD_L_LEFT))
    {
        global.player.playgrid.x--;
        global.player.playgrid.y++;
        SetPlayerAnim(PLAYERSPR_L+PLAYERSPR_RUN_ANIM,PLAYERDIR_BL,-FIXED_POINT_ONE,-FIXED_POINT_ONE*2);
        return;
    }
    if (buttons & (1<<JOYPAD_L_RIGHT))
    {
        global.player.playgrid.x++;
        global.player.playgrid.y--;
        SetPlayerAnim(PLAYERSPR_R+PLAYERSPR_RUN_ANIM,PLAYERDIR_BR,FIXED_POINT_ONE,-FIXED_POINT_ONE*5);
        return;
    }
    if (buttons & (1<<JOYPAD_L_UP))
    {
        global.player.playgrid.x--;
        global.player.playgrid.y--;
        SetPlayerAnim(PLAYERSPR_U+PLAYERSPR_RUN_ANIM,PLAYERDIR_TL,-FIXED_POINT_ONE,-FIXED_POINT_ONE*5);
        return;
    }
    if (buttons & (1<<JOYPAD_L_DOWN))
    {
        global.player.playgrid.x++;
        global.player.playgrid.y++;
        SetPlayerAnim(PLAYERSPR_D+PLAYERSPR_RUN_ANIM,PLAYERDIR_BL,FIXED_POINT_ONE,-FIXED_POINT_ONE*2);
        return;
    }
}


//---------------------------------------------------------
void AnimatePlayer(void)
{
    global.player.object.animDelay--;
    if (global.player.object.animDelay<0)
    {
        global.player.object.animDelay = global.player.object.animSpeed;
        global.player.object.frameIndex++;
        if (global.player.object.frameIndex==global.player.object.totalFrames)
        {
            global.player.object.frameIndex = 0;
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
    u8 animIndex = global.player.object.baseIndex+global.player.object.frameIndex;
    u8 page = (animIndex>>3)+PLAYER_ANIM_PAGE;
    nextreg(MMU_SLOT_6,page);
    u8* pPattern = (u8*)SWAP_BANK_0+((animIndex&7)<<10);
    CopySprite(pPattern, PLAYER_SPRITE_PATTERN, 4);
    nextreg(SPRITE_INDEX, PLAYER_SPRITE_SLOT);
    s16 x = global.player.object.position.x>>FIXED_POINT_BITS;
    s16 y = global.player.object.position.y>>FIXED_POINT_BITS;
    nextreg(SPRITE_ATTR_0, x&0xff);
    nextreg(SPRITE_ATTR_1, y);
    nextreg(SPRITE_ATTR_2, (x>>8)&1);
    nextreg(SPRITE_ATTR_3, 0xc0);
}
