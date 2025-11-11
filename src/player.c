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
#include "assets.h"

#define PLAYER_TO_TILE_X_OFFSET (-1)
#define PLAYER_TO_TILE_Y_OFFSET (-22)
//---------------------------------------------------------
void SnapToGrid(void)
{
    s16 x = global.player.playgrid.x-global.playArea.position.x;
    s16 y = global.player.playgrid.y-global.playArea.position.y;

    s16 sx = (x+y)*16+LAYER_2_WIDTH/2;
    s16 sy = (y-x)*24+LAYER_2_HEIGHT/2;
    sx += global.tileMap.x+PLAYER_TO_TILE_X_OFFSET;
    sy += global.tileMap.y+PLAYER_TO_TILE_Y_OFFSET;

    global.player.object.position.x = sx<<FIXED_POINT_BITS;
    global.player.object.position.y = sy<<FIXED_POINT_BITS;

}

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
void InitializePlayer(void)
{
    global.player.object.position.x = 0;
    global.player.object.position.y = 0;
    global.player.object.flags.direction = 0;
    global.player.object.frameIndex = 0;
    global.player.object.baseIndex = 32;
    global.player.object.lastIndex = 0xff;
    global.player.object.animDelay = 4;
    global.player.object.animSpeed = 4;
    global.player.object.totalFrames = 8;
    global.player.object.gravity = FIXED_POINT_HALF;
	global.player.playgrid.x = 0;
	global.player.playgrid.y = 0;
    global.player.moveSteps = 0;
//
// Set up sprites 64..67, so that only the minimum needs to be set up below. 
//
    SetupSprite(PLAYER_SPRITE_SLOT, PLAYER_SPRITE_PATTERN, 0, 0, 0, 0xc0, 0);
    SetupSprite(PLAYER_SPRITE_SLOT+1, PLAYER_SPRITE_PATTERN+1, 16, 0, 0, 0xc0, 0x60);
    SetupSprite(PLAYER_SPRITE_SLOT+2, PLAYER_SPRITE_PATTERN+2, 0, 16, 0, 0xc0, 0x60);
    SetupSprite(PLAYER_SPRITE_SLOT+3, PLAYER_SPRITE_PATTERN+3, 16,16, 0, 0xc0, 0x60);

    nextreg(MMU_SLOT_6, PALETTE_PAGE);
    CopyPalette(asset_PlayerPalette, PALETTE_SPRITE_PRIMARY);
    // Grab whatever colour is the background colour. This will be our transparent
    // index.
    nextreg(MMU_SLOT_6,PLAYER_ANIM_PAGE);
    nextreg(TRANS_SPRITE_INDEX, *(u8*)SWAP_BANK_0);
    SnapToGrid();
}

//---------------------------------------------------------

//---------------------------------------------------------
void SetColour(const u16 colour)
{
    nextreg(PALETTE_CONTROL, PALETTE_TILE_PRIMARY<<4);
    nextreg(PALETTE_INDEX, 4+16);
    nextreg(PALETTE_VALUE_9, colour>>1);
    nextreg(PALETTE_VALUE_9, colour&1);
}

//---------------------------------------------------------
void BeginPulsePalette(void)
{
    nextreg(MMU_SLOT_6, PALETTE_PAGE);
    // A palette pulse was in progress, so cancel it.
    if (global.pulseColour!=global.pulseTarget)
    {
        RefreshPlayAreaBlock(global.pulseCoord.x, global.pulseCoord.y, 0);
    }
    global.pulseColour = 0x1ff;
    global.pulseTarget = asset_TilemapPalette[4];
    global.pulseCoord = global.player.playgrid;
    RefreshPlayAreaBlock(global.player.playgrid.x, global.player.playgrid.y, 1);
    SetColour(global.pulseColour);
}

//---------------------------------------------------------
void PulsePalette(void)
{
    if (global.pulseColour==global.pulseTarget)
        return;

    nextreg(MMU_SLOT_6, PALETTE_PAGE);
    if ((global.pulseColour & RED_MASK) < (global.pulseTarget & RED_MASK)) global.pulseColour += (1<<RED_SHIFT);
    if ((global.pulseColour & RED_MASK) > (global.pulseTarget & RED_MASK)) global.pulseColour -= (1<<RED_SHIFT);
    if ((global.pulseColour & GRN_MASK) < (global.pulseTarget & GRN_MASK)) global.pulseColour += (1<<GRN_SHIFT);
    if ((global.pulseColour & GRN_MASK) > (global.pulseTarget & GRN_MASK)) global.pulseColour -= (1<<GRN_SHIFT);
    if ((global.pulseColour & BLU_MASK) < (global.pulseTarget & BLU_MASK)) global.pulseColour += (1<<BLU_SHIFT);
    if ((global.pulseColour & BLU_MASK) > (global.pulseTarget & BLU_MASK)) global.pulseColour -= (1<<BLU_SHIFT);

    SetColour(global.pulseColour);
    if (global.pulseColour==global.pulseTarget)
    {
        RefreshPlayAreaBlock(global.pulseCoord.x, global.pulseCoord.y, 0);
    }
}

//---------------------------------------------------------
void HandlePickup(void)
{
    SetPlayerAnimIdle(global.player.direction*8+PLAYERSPR_IDLE_ANIM,0,0);
    u8 vxlz=0, vxgz=0, vylz=0, vygz=0;
    for (int i=0; i<32; i++)
    {
        s16 px = global.player.object.position.x+(16<<FIXED_POINT_BITS)+((s16)random8()<<3);
        s16 py = global.player.object.position.y+(8<<FIXED_POINT_BITS)+((s16)random8()<<3);
        x_printf("px:%d, py:%d\n", px>>FIXED_POINT_BITS, py>>FIXED_POINT_BITS);
        s16 vx = random8();
        s16 vy = random8();
        s8 width = random8()&3+1;
        s8 colour = random8()|0xf0;
        s8 age = (random8()&31)+24;
        AddParticle(px, py, vx, vy, age, colour, width, 0);
    }
}

//---------------------------------------------------------
void HandleDeath(bool fallThrough)
{
    (void)fallThrough;
    SetPlayerAnimIdle(global.player.direction*8+PLAYERSPR_IDLE_ANIM,0,-FIXED_POINT_ONE*2);
    global.player.moveSteps = 64;
    global.player.object.gravity = FIXED_POINT_HALF/2;
}

//---------------------------------------------------------
void HandleControllerInput(void)
{
    u8 buttons = ReadController();
    if (buttons & 0x0f)
    {

    }
    if (buttons & (1<<JOYPAD_L_LEFT))
    {
        global.player.playgrid.x--;
        SetPlayerAnim(PLAYERSPR_L+PLAYERSPR_RUN_ANIM,PLAYERDIR_BL,-FIXED_POINT_ONE,-FIXED_POINT_ONE*2);
        return;
    }
    if (buttons & (1<<JOYPAD_L_RIGHT))
    {
        global.player.playgrid.x++;
        SetPlayerAnim(PLAYERSPR_R+PLAYERSPR_RUN_ANIM,PLAYERDIR_BR,FIXED_POINT_ONE,-FIXED_POINT_ONE*5);
        return;
    }
    if (buttons & (1<<JOYPAD_L_UP))
    {
        global.player.playgrid.y--;
        SetPlayerAnim(PLAYERSPR_U+PLAYERSPR_RUN_ANIM,PLAYERDIR_TL,-FIXED_POINT_ONE,-FIXED_POINT_ONE*5);
        return;
    }
    if (buttons & (1<<JOYPAD_L_DOWN))
    {
        global.player.playgrid.y++;
        SetPlayerAnim(PLAYERSPR_D+PLAYERSPR_RUN_ANIM,PLAYERDIR_BL,FIXED_POINT_ONE,-FIXED_POINT_ONE*2);
        return;
    }
}

//---------------------------------------------------------
void MovePlayer(void)
{
    PulsePalette();
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
        play_cell* pCell = GetPlayAreaCell(global.player.playgrid.x, global.player.playgrid.y);
        //x_printf("Coord:(%d,%d), content:%c\n", global.player.playgrid.x, global.player.playgrid.y, content);
        if (pCell->type==2)
        {
            HandlePickup();
        }
        else if (pCell->type==0)
        {
            HandleDeath(true);
        }
        else
        {
            SetPlayerAnimIdle(global.player.direction*8+PLAYERSPR_IDLE_ANIM,0,0);
        }
        if (pCell->type && !pCell->dark)
        {
            pCell->dark = 1;
            BeginPulsePalette();
        }
        return;
    }
    HandleControllerInput();
}


//---------------------------------------------------------
void AnimatePlayer(void)
{
    global.player.object.animDelay--;
    if (global.player.object.animDelay<0)
    {
        global.player.object.animDelay = global.player.object.animSpeed;
        global.player.object.frameIndex++;
        if (global.player.object.frameIndex!=global.player.object.totalFrames)
            return;
        global.player.object.frameIndex = 0;
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
    if (animIndex!=global.player.object.lastIndex)
    {
        global.player.object.lastIndex = animIndex;
        u8 page = (animIndex>>3)+PLAYER_ANIM_PAGE;
        nextreg(MMU_SLOT_6,page);
        u8* pPattern = (u8*)SWAP_BANK_0+((animIndex&7)<<10);
        CopySprite(pPattern, PLAYER_SPRITE_PATTERN, 4);
    }
    nextreg(SPRITE_INDEX, PLAYER_SPRITE_SLOT);
    s16 x = global.player.object.position.x>>FIXED_POINT_BITS;
    s16 y = global.player.object.position.y>>FIXED_POINT_BITS;
    nextreg(SPRITE_ATTR_0, x&0xff);
    nextreg(SPRITE_ATTR_1, y);
    nextreg(SPRITE_ATTR_2, (x>>8)&1);
    nextreg(SPRITE_ATTR_3, 0xc0);
}
