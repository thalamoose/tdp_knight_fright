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
#include "hud.h"

#define PLAYER_TO_TILE_X_OFFSET (-1)
#define PLAYER_TO_TILE_Y_OFFSET (-22)

player_object player;

//---------------------------------------------------------
void SnapToGrid(void)
{
    s16 x = player.playgrid.x-playArea.position.x;
    s16 y = player.playgrid.y-playArea.position.y;

    s16 sx = (x+y)*16+LAYER_2_WIDTH/2;
    s16 sy = (y-x)*24+LAYER_2_HEIGHT/2;
    sx += global.tileMap.x+PLAYER_TO_TILE_X_OFFSET;
    sy += global.tileMap.y+PLAYER_TO_TILE_Y_OFFSET;

    player.object.position.x = sx<<FIXED_POINT_BITS;
    player.object.position.y = sy<<FIXED_POINT_BITS;

}

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
void InitializePlayer(void)
{
    player.object.position.x = 0;
    player.object.position.y = 0;
    player.object.flags.direction = 0;
    player.object.frameIndex = 0;
    player.object.baseIndex = 32;
    player.object.lastIndex = 0xff;
    player.object.animDelay = 4;
    player.object.animSpeed = 4;
    player.object.totalFrames = 8;
    player.object.gravity = FIXED_POINT_HALF;
	player.playgrid.x = 0;
	player.playgrid.y = 0;
    player.moveSteps = 0;
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
    global.pulseCoord = player.playgrid;
    RefreshPlayAreaBlock(player.playgrid.x, player.playgrid.y, 1);
    SetColour(PALETTE_TILE_PRIMARY, 16+4, global.pulseColour);
}

//---------------------------------------------------------
void PulsePalette(void)
{
    if (global.pulseColour==global.pulseTarget)
        return;

    global.pulseColour = BlendColour(global.pulseColour, global.pulseTarget);
    SetColour(PALETTE_TILE_PRIMARY, 16+4, global.pulseColour);
    if (global.pulseColour==global.pulseTarget)
    {
        RefreshPlayAreaBlock(global.pulseCoord.x, global.pulseCoord.y, 0);
    }
}

//---------------------------------------------------------
void HandlePickup(void)
{
    SetPlayerAnimIdle(player.direction*8+PLAYERSPR_IDLE_ANIM,0,0);
    u8 vxlz=0, vxgz=0, vylz=0, vygz=0;
    for (int i=0; i<32; i++)
    {
        s16 px = player.object.position.x+(16<<FIXED_POINT_BITS)+((s16)random8()<<3);
        s16 py = player.object.position.y+(8<<FIXED_POINT_BITS)+((s16)random8()<<3);
        //x_printf("px:%d, py:%d\n", px>>FIXED_POINT_BITS, py>>FIXED_POINT_BITS);
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
    SetPlayerAnimIdle(player.direction*8+PLAYERSPR_IDLE_ANIM,0,-FIXED_POINT_ONE*2);
    player.moveSteps = 64;
    player.object.gravity = FIXED_POINT_HALF/2;
}

//---------------------------------------------------------
void HandleControllerInput(void)
{
    u8 buttons = ReadController();
    if (hud.transitionIsRunning)
        return;
    if (buttons & (1<<JOYPAD_L_LEFT))
    {
        player.playgrid.x--;
        SetPlayerAnim(PLAYERSPR_L+PLAYERSPR_RUN_ANIM,PLAYERDIR_BL,-FIXED_POINT_ONE,-FIXED_POINT_ONE*2);
        return;
    }
    if (buttons & (1<<JOYPAD_L_RIGHT))
    {
        player.playgrid.x++;
        SetPlayerAnim(PLAYERSPR_R+PLAYERSPR_RUN_ANIM,PLAYERDIR_BR,FIXED_POINT_ONE,-FIXED_POINT_ONE*5);
        return;
    }
    if (buttons & (1<<JOYPAD_L_UP))
    {
        player.playgrid.y--;
        SetPlayerAnim(PLAYERSPR_U+PLAYERSPR_RUN_ANIM,PLAYERDIR_TL,-FIXED_POINT_ONE,-FIXED_POINT_ONE*5);
        return;
    }
    if (buttons & (1<<JOYPAD_L_DOWN))
    {
        player.playgrid.y++;
        SetPlayerAnim(PLAYERSPR_D+PLAYERSPR_RUN_ANIM,PLAYERDIR_BL,FIXED_POINT_ONE,-FIXED_POINT_ONE*2);
        return;
    }
}

//---------------------------------------------------------
void MovePlayer(void)
{
    PulsePalette();
    if (player.moveSteps)
    {
        player.object.position.x += player.object.velocity.x;
        player.object.position.y += player.object.velocity.y;
        player.object.velocity.y += player.object.gravity;
        player.moveSteps--;
        if (player.moveSteps!=0)
        {
            if (player.object.position.y>(240<<FIXED_POINT_BITS))
            {
                // We must have been in a die. Respawn now.
                InitializePlayer();
            }
            return;
        }
        SnapToGrid();
        play_cell* pCell = GetPlayAreaCell(player.playgrid.x, player.playgrid.y);
        //x_printf("Coord:(%d,%d), content:%c\n", player.playgrid.x, player.playgrid.y, content);
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
            SetPlayerAnimIdle(player.direction*8+PLAYERSPR_IDLE_ANIM,0,0);
        }
        if (pCell->type && !pCell->dark)
        {
            pCell->dark = 1;
            BeginPulsePalette();
            bool tileFull = IncrementHudTileCount();
            if (tileFull)
            {
                x_printf("Tile full bonus.\n");
            }
            playArea.tilesToFlip--;
            if (playArea.tilesToFlip==0)
            {
                x_printf("No more tiles. Restart.\n");
                // End of map, needs to go on to the next
                hud.gameIsRunning = false;
                StartTransition(50, 0, 0, I_TO_F(5), I_TO_F(-1), I_TO_F(1)/4);
            }
        }
        return;
    }
    HandleControllerInput();
}


//---------------------------------------------------------
void AnimatePlayer(void)
{
    player.object.animDelay--;
    if (player.object.animDelay<0)
    {
        player.object.animDelay = player.object.animSpeed;
        player.object.frameIndex++;
        if (player.object.frameIndex!=player.object.totalFrames)
            return;
        player.object.frameIndex = 0;
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
    if (animIndex!=player.object.lastIndex)
    {
        player.object.lastIndex = animIndex;
        u8 page = (animIndex>>3)+PLAYER_ANIM_PAGE;
        nextreg(MMU_SLOT_6,page);
        u8* pPattern = (u8*)SWAP_BANK_0+((animIndex&7)<<10);
        CopySprite(pPattern, PLAYER_SPRITE_PATTERN, 4);
    }
    nextreg(SPRITE_INDEX, PLAYER_SPRITE_SLOT);
    s16 x = (player.object.position.x>>FIXED_POINT_BITS)+hud.shake.x;
    s16 y = (player.object.position.y>>FIXED_POINT_BITS)+hud.shake.y;
    nextreg(SPRITE_ATTR_0, x&0xff);
    nextreg(SPRITE_ATTR_1, y);
    nextreg(SPRITE_ATTR_2, (x>>8)&1);
    nextreg(SPRITE_ATTR_3, 0xc0);
}
