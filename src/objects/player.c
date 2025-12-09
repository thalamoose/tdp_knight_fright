#include "kftypes.h"
#include "defines.h"
#include "hardware.h"
#include "memorymap.h"
#include "utilities.h"
#include "objects/player.h"
#include "objects/components.h"
#include "sprite.h"
#include "input.h"
#include "globals.h"
#include "particles.h"
#include "assets.h"
#include "playarea.h"
#include "game_manager.h"
#include "hud.h"

#define PLAYER_TO_TILE_X_OFFSET ( 2)
#define PLAYER_TO_TILE_Y_OFFSET (-22)

player_object* player;
void MovePlayer(player_object* pPlayer);

//---------------------------------------------------------
void SetPlayerAnimIdle(u8 baseIndex, s16 vx, s16 vy)
{
    player->object.trans.vel.x = vx;
    player->object.trans.vel.y = vy;
    player->object.anim.frameIndex = 0;
    player->object.anim.baseIndex = baseIndex;
    player->object.anim.lastIndex = 0xff;
    player->object.anim.animDelay = 4;
    player->object.anim.animSpeed = 4;
    player->object.anim.totalFrames = 8;
}

//---------------------------------------------------------
void SetPlayerAnim(u8 baseIndex, u8 direction, s16 vx, s16 vy)
{
    player->object.moveSteps = 16;
    player->object.direction = direction;
    SetPlayerAnimIdle(baseIndex, vx, vy);
}

//---------------------------------------------------------
void CreatePlayer(player_object* pPlayer)
{
    player = pPlayer;
    SetPlayerAnimIdle(PLAYERSPR_IDLE_ANIM, 0, 0);
    //
    // Set up sprites 64..67, so that only the minimum needs to be set up below.
    //
    SetupSprite(PLAYER_SPRITE_SLOT, PLAYER_SPRITE_PATTERN, 0, 0, 0, 0x40, 0);
    SetupSprite(PLAYER_SPRITE_SLOT+1, PLAYER_SPRITE_PATTERN+1, 16, 0, 0, 0xc0, 0x60);
    SetupSprite(PLAYER_SPRITE_SLOT+2, PLAYER_SPRITE_PATTERN+2, 0, 16, 0, 0xc0, 0x60);
    SetupSprite(PLAYER_SPRITE_SLOT+3, PLAYER_SPRITE_PATTERN+3, 16, 16, 0, 0xc0, 0x60);

    nextreg(MMU_SLOT_6, PALETTE_PAGE);
    CopyPalettePartial(asset_PlayerPalette, PALETTE_SPRITE_PRIMARY, 0, 64);
    // Grab whatever colour is the background colour. This will be our transparent
    // index.
    pPlayer->object.anim.sprite.page = PLAYER_ANIM_PAGE;
    pPlayer->object.anim.sprite.slot = PLAYER_SPRITE_SLOT;
    pPlayer->object.anim.sprite.pattern = PLAYER_SPRITE_PATTERN;
    pPlayer->object.anim.sprite.patternCount = 4;
    pPlayer->object.anim.sprite.centerOffset.x = PLAYER_TO_TILE_X_OFFSET;
    pPlayer->object.anim.sprite.centerOffset.y = PLAYER_TO_TILE_Y_OFFSET;
    
    pPlayer->object.anim.sprite.patternCount = 4;
    // Copies transparent color to the register.
    nextreg(MMU_SLOT_6, PLAYER_ANIM_PAGE);
    nextreg(TRANS_SPRITE_INDEX, *(u8 *)SWAP_BANK_0);
    pPlayer->object.moveSteps = 32;
    pPlayer->object.trans.pos.y -= I_TO_F(TILEMAP_PIX_HEIGHT/2+64);
    pPlayer->object.trans.vel.x = 0;
    pPlayer->object.trans.vel.y = FIXED_POINT_HALF*4;
    pPlayer->object.trans.gravity = FIXED_POINT_HALF/2;
}

//---------------------------------------------------------
bool UpdatePlayer(player_object* pPlayer)
{
    MovePlayer(pPlayer);
    AnimateComponent(&pPlayer->object.anim);
    return true;
}

//---------------------------------------------------------
void RenderPlayer(player_object* pPlayer)
{
    RenderComponent(&pPlayer->object.trans, &pPlayer->object.anim);
}

//---------------------------------------------------------
void DestroyPlayer(player_object* pPlayer)
{
    (void)pPlayer;

}

//---------------------------------------------------------
void BlowupPlayer(player_object* pPlayer)
{
    (void)pPlayer;

}

//---------------------------------------------------------
void CollidePlayer(player_object* pPlayer)
{
    (void)pPlayer;

}

//---------------------------------------------------------
object_vtable playerVirtualTable = 
{
    (object_create_fn*)CreatePlayer,
    (object_update_fn*)UpdatePlayer,
    (object_render_fn*)RenderPlayer,
    (object_destroy_fn*)DestroyPlayer,
    (object_collide_fn*)CollidePlayer,
    (object_blowup_fn*)BlowupPlayer,
};

//---------------------------------------------------------
void InitializePlayer(void)
{
    player = (player_object*)CreateObject(&playerVirtualTable, playArea.start.x, playArea.start.y);
}

//---------------------------------------------------------
void ResetPlayer(void)
{
	player->object.playGrid = playArea.start;
	SnapToPlayAreaGrid(&player->object);
    CreatePlayer(player);
}

//---------------------------------------------------------
void BeginPulsePalette(void)
{
    nextreg(MMU_SLOT_6, PALETTE_PAGE);
    // A palette pulse was in progress, so cancel it.
    if (global.pulseColour != global.pulseTarget)
    {
        RefreshPlayAreaBlock(global.pulseCoord.x, global.pulseCoord.y, 0);
    }
    global.pulseColour = 0x1ff;
    global.pulseTarget = asset_TilemapPalette[4];
    global.pulseCoord = player->object.playGrid;
    RefreshPlayAreaBlock(player->object.playGrid.x, player->object.playGrid.y, 1);
    SetColour(PALETTE_TILE_PRIMARY, 16+4, global.pulseColour);
}

//---------------------------------------------------------
void PulsePalette(void)
{
    if (global.pulseColour == global.pulseTarget)
        return;

    global.pulseColour = BlendColour(global.pulseColour, global.pulseTarget);
    SetColour(PALETTE_TILE_PRIMARY, 16+4, global.pulseColour);
    if (global.pulseColour == global.pulseTarget)
    {
        RefreshPlayAreaBlock(global.pulseCoord.x, global.pulseCoord.y, 0);
    }
}

//---------------------------------------------------------
void HandlePickup(void)
{
    SetPlayerAnimIdle(player->object.direction * 8+PLAYERSPR_IDLE_ANIM, 0, 0);
    u8 vxlz = 0, vxgz = 0, vylz = 0, vygz = 0;
    for (int i = 0; i < 32; i++)
    {
        s16 px = player->object.trans.pos.x+I_TO_F(16)+((s16)random8()<<3);
        s16 py = player->object.trans.pos.y+I_TO_F(-16)+((s16)random8()<<3);
        s16 vx = random8();
        s16 vy = random8();
        s8 width = random8()&3+1;
        s8 colour = random8() | 0xf0;
        s8 age = (random8()&31)+24;
        px += I_TO_F(TILEMAP_PIX_WIDTH/2)+tileMap.position.x;
        py += I_TO_F(TILEMAP_PIX_HEIGHT/2)+tileMap.position.y;
        AddParticle(px, py, vx, vy, age, colour, width, 0);
    }
}

//---------------------------------------------------------
void HandleDeath(bool fallThrough)
{
    (void)fallThrough;
    SetPlayerAnimIdle(player->object.direction*8+PLAYERSPR_IDLE_ANIM, 0, -FIXED_POINT_ONE*2);
    player->object.moveSteps = 64;
    player->object.trans.gravity = FIXED_POINT_HALF/2;
}

//---------------------------------------------------------
void HandleControllerInput(void)
{
    u8 buttons = ReadController();
    if (hud.transitionIsRunning)
        return;
    if (buttons&JOYPAD_L_LEFT)
    {
        player->object.playGrid.x--;
        SetPlayerAnim(PLAYERSPR_L+PLAYERSPR_RUN_ANIM, PLAYERDIR_BL, -FIXED_POINT_ONE, -FIXED_POINT_ONE * 2);
        return;
    }
    if (buttons&JOYPAD_L_RIGHT)
    {
        player->object.playGrid.x++;
        SetPlayerAnim(PLAYERSPR_R+PLAYERSPR_RUN_ANIM, PLAYERDIR_BR, FIXED_POINT_ONE, -FIXED_POINT_ONE * 5);
        return;
    }
    if (buttons&JOYPAD_L_UP)
    {
        player->object.playGrid.y--;
        SetPlayerAnim(PLAYERSPR_U+PLAYERSPR_RUN_ANIM, PLAYERDIR_TL, -FIXED_POINT_ONE, -FIXED_POINT_ONE * 5);
        return;
    }
    if (buttons&JOYPAD_L_DOWN)
    {
        player->object.playGrid.y++;
        SetPlayerAnim(PLAYERSPR_D+PLAYERSPR_RUN_ANIM, PLAYERDIR_BL, FIXED_POINT_ONE, -FIXED_POINT_ONE * 2);
        return;
    }
    if (buttons&JOYPAD_R_DOWN)
    {
        playArea.tilesToFlip = 1;
    }
}

//---------------------------------------------------------
void MovePlayer(player_object* player)
{
    PulsePalette();
    if (player->object.moveSteps)
    {
        TransformComponent(&player->object.trans);
        player->object.moveSteps--;
        if (player->object.moveSteps != 0)
        {
            s16 dy = F_TO_I(player->object.trans.pos.y+tileMap.position.y);
            if (dy > 256+32)
            {
                // We must have been in a die. Respawn now.
                ResetPlayer();
            }
            return;
        }
        SnapToPlayAreaGrid(&player->object);
        player->object.trans.gravity = FIXED_POINT_HALF;
        play_cell *pCell = GetPlayAreaCell(player->object.playGrid.x, player->object.playGrid.y);
        //x_printf("Coord:(%d,%d), content:%c\n", player->object.playGrid.x, player->object.playGrid.y, *(u8 *)pCell);
        if (pCell->type==CELL_COIN)
        {
            HandlePickup();
        }
        else if (pCell->type==CELL_HOLE)
        {
            HandleDeath(true);
        }
        else
        {
            SetPlayerAnimIdle(player->object.direction * 8+PLAYERSPR_IDLE_ANIM, 0, 0);
        }
        if (!pCell->isDark && pCell->type==CELL_TILE && hud.transitionIsRunning==false)
        {
            pCell->isDark = true;
            BeginPulsePalette();
            bool tileFull = IncrementHudTileCount();
            if (tileFull)
            {
                x_printf("Tile full bonus.\n");
            }
            playArea.tilesToFlip--;
            //x_printf("%d tiles remaining.", (s16)playArea.tilesToFlip);
            if (playArea.tilesToFlip == 0)
            {
                x_printf("No more tiles. Restart.\n");
                // End of map, needs to go on to the next
                gameManager.isRunning = false;
                StartTransition(50, 0, 0, I_TO_F(5), I_TO_F(-1), I_TO_F(1) / 4);
            }
        }
        return;
    }
    HandleControllerInput();
}

