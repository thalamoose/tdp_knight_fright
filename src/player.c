#include "kftypes.h"
#include "defines.h"
#include "hardware.h"
#include "memorymap.h"
#include "utilities.h"
#include "objects.h"
#include "player.h"
#include "sprites.h"


player_object player;

extern void* player_palette;

void SnapToGrid(void);
void SetPlayerAnim(u8 baseIndex, u8 direction, s16 vx, s16 vy);
void SetPlayerAnimIdle(u8 baseIndex, s16 vx, s16 vy);
void UpdatePlayerMovement(void);
void UpdatePlayerAnimation(void);

void InitializePlayer(void)
{
    player.object.x = (320/2)<<FIXED_POINT_BITS;
    player.object.y = (256/2)<<FIXED_POINT_BITS;
    player.object.direction = 0;
    player.playgrid_x = PLAY_AREA_CELLS_WIDTH/2;
    player.playgrid_y = PLAY_AREA_CELLS_HEIGHT/2;
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

    CopyPalette(player_palette,2);
    nextreg(MMU_SLOT_6,PLAYER_ANIM_PAGE);
    nextreg(TRANS_SPRITE_INDEX,*(u8*)SWAP_BANK_0);
    SnapToGrid();
}

s8 play_area_center_x;
s8 play_area_center_y;
void SnapToGrid(void)
{
    player.object.x = ((play_area_center_x+player.playgrid_x)*24+LAYER_2_WIDTH/2+12)<<FIXED_POINT_BITS;
    player.object.y = ((play_area_center_y+player.playgrid_y)*24+LAYER_2_HEIGHT/2)<<FIXED_POINT_BITS;
}

void UpdatePlayer(void)
{
    MovePlayer();
    AnimatePlayer();
}

void MovePlayer(void)
{
    if (player.moveSteps)
    {
        player.object.x += player.object.vx;
        player.object.y += player.object.vy;
        player.object.vy += player.object.gravity;
        player.moveSteps--;
        if (player.moveSteps==0)
        {
            SnapToGrid();
            u8 content = GetPlaygridContent(player.playgrid_x, player.playgrid_y);
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
}



u8 GetPlaygridContent(u8 x, u8 y)
{
    u8* tileEntry = (u8*)(SWAP_BANK_0)+(y*40+x)*2;
    nextreg(MMU_SLOT_6,TILEMAP_PAGE);
    nextreg(MMU_SLOT_7,TILEMAP_PAGE+1);
    return *tileEntry;
}

void SetPlayerAnimIdle(u8 baseIndex, s16 vx, s16 vy)
{
    player.object.vx = vx;
    player.object.vy = vy;
    player.object.baseIndex = baseIndex;
    player.object.totalFrames = 8;
    player.object.frameIndex = 0;
    player.object.animDelay = 4;
    player.object.animSpeed = 4;
}

void SetPlayerAnim(u8 baseIndex, u8 direction, s16 vx, s16 vy)
{
    SetPlayerAnimIdle(baseIndex, vx, vy);
    player.moveSteps = 16;
    player.direction = direction;
}

void AnimatePlayer(void)
{
    player.object.animDelay--;
    if (player.object.animDelay)
    {
        player.object.animDelay = player.object.animSpeed;
        player.object.frameIndex++;
        if (player.object.frameIndex==player.object.totalFrames)
        {
            player.object.frameIndex = 0;
        }
    }
}

void RenderPlayer(void)
{
    u8 animIndex = player.object.baseIndex+player.object.frameIndex;
    u8 page = (animIndex*8)+PLAYER_ANIM_PAGE;
    nextreg(MMU_SLOT_6,page);
    u8* pPattern = (u8*)SWAP_BANK_0+(page&7<<2);
    CopySprite(pPattern, PLAYER_SPRITE_PATTERN, 4);
    nextreg(SPRITE_INDEX, PLAYER_SPRITE_SLOT);
    s16 x = player.object.x>>FIXED_POINT_BITS;
    s16 y = player.object.y>>FIXED_POINT_BITS;
    nextreg(SPRITE_ATTR_0, x&0xff);
    nextreg(SPRITE_ATTR_1, y);
    nextreg(SPRITE_ATTR_2, (x>>8)&1);
}
