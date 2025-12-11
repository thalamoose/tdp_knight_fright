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
#include "level_manager.h"
#include "hud.h"
#include "enemies/spike.h"

#define PLAYER_TO_TILE_X_OFFSET ( 2)
#define PLAYER_TO_TILE_Y_OFFSET (-22)

game_object* player;
void MovePlayer(game_object* pPlayer);

//---------------------------------------------------------
void SetPlayerAnimIdle(u8 baseIndex, s16 vx, s16 vy)
{
    player->trans.vel.x = vx;
    player->trans.vel.y = vy;
    player->anim.frameIndex = 0;
    player->anim.baseIndex = baseIndex;
    player->anim.lastIndex = 0xff;
    player->anim.animDelay = 4;
    player->anim.animSpeed = 4;
    player->anim.totalFrames = 8;
}

//---------------------------------------------------------
void SetPlayerAnim(u8 baseIndex, u8 direction, s16 vx, s16 vy)
{
    player->moveSteps = 16;
    player->direction = direction;
    SetPlayerAnimIdle(baseIndex, vx, vy);
}

//---------------------------------------------------------
void CreatePlayer(game_object* pPlayer, s8 px, s8 py)
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
    pPlayer->anim.sprite.page = PLAYER_ANIM_PAGE;
    pPlayer->anim.sprite.slot = PLAYER_SPRITE_SLOT;
    pPlayer->anim.sprite.pattern = PLAYER_SPRITE_PATTERN;
    pPlayer->anim.sprite.patternCount = 4;
    pPlayer->anim.sprite.centerOffset.x = PLAYER_TO_TILE_X_OFFSET;
    pPlayer->anim.sprite.centerOffset.y = PLAYER_TO_TILE_Y_OFFSET;
    
    pPlayer->anim.sprite.patternCount = 4;
    // Copies transparent color to the register.
    nextreg(MMU_SLOT_6, PLAYER_ANIM_PAGE);
    nextreg(TRANS_SPRITE_INDEX, *(u8 *)SWAP_BANK_0);
    pPlayer->moveSteps = 32;
    pPlayer->trans.pos.y -= I_TO_F(TILEMAP_PIX_HEIGHT/2+64);
    pPlayer->trans.vel.x = 0;
    pPlayer->trans.vel.y = FIXED_POINT_HALF*4;
    pPlayer->trans.gravity = FIXED_POINT_HALF/2;
    pPlayer->playGrid.x = px;
    pPlayer->playGrid.y = py;
}

//---------------------------------------------------------
bool UpdatePlayer(game_object* pPlayer)
{
    MovePlayer(pPlayer);
    AnimateComponent(&pPlayer->anim);
    return true;
}

//---------------------------------------------------------
void RenderPlayer(game_object* pPlayer)
{
    RenderComponent(&pPlayer->trans, &pPlayer->anim);
}

//---------------------------------------------------------
void DestroyPlayer(game_object* pPlayer)
{
    (void)pPlayer;

}

//---------------------------------------------------------
void BlowupPlayer(game_object* pPlayer)
{
    (void)pPlayer;

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
    global.pulseCoord = player->playGrid;
    RefreshPlayAreaBlock(player->playGrid.x, player->playGrid.y, 1);
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
    SetPlayerAnimIdle(player->direction * 8+PLAYERSPR_IDLE_ANIM, 0, 0);
    u8 vxlz = 0, vxgz = 0, vylz = 0, vygz = 0;
    for (int i = 0; i < 32; i++)
    {
        s16 px = player->trans.pos.x+I_TO_F(16)+((s16)random8()<<3);
        s16 py = player->trans.pos.y+I_TO_F(-16)+((s16)random8()<<3);
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
    SetPlayerAnimIdle(player->direction*8+PLAYERSPR_IDLE_ANIM, 0, -FIXED_POINT_ONE*2);
    player->moveSteps = 64;
    player->trans.gravity = FIXED_POINT_HALF/2;
}

//---------------------------------------------------------
void HandleControllerInput(void)
{
    u8 buttons = ReadController();
    if (hud.transitionIsRunning)
        return;
    if (buttons&JOYPAD_L_LEFT)
    {
        player->playGrid.x--;
        SetPlayerAnim(PLAYERSPR_L+PLAYERSPR_RUN_ANIM, PLAYERDIR_BL, -FIXED_POINT_ONE, -FIXED_POINT_ONE * 2);
        return;
    }
    if (buttons&JOYPAD_L_RIGHT)
    {
        player->playGrid.x++;
        SetPlayerAnim(PLAYERSPR_R+PLAYERSPR_RUN_ANIM, PLAYERDIR_BR, FIXED_POINT_ONE, -FIXED_POINT_ONE * 5);
        return;
    }
    if (buttons&JOYPAD_L_UP)
    {
        player->playGrid.y--;
        SetPlayerAnim(PLAYERSPR_U+PLAYERSPR_RUN_ANIM, PLAYERDIR_TL, -FIXED_POINT_ONE, -FIXED_POINT_ONE * 5);
        return;
    }
    if (buttons&JOYPAD_L_DOWN)
    {
        player->playGrid.y++;
        SetPlayerAnim(PLAYERSPR_D+PLAYERSPR_RUN_ANIM, PLAYERDIR_BL, FIXED_POINT_ONE, -FIXED_POINT_ONE * 2);
        return;
    }
    if (buttons&JOYPAD_R_DOWN)
    {
        levelManager.tilesRemaining = 1;
    }
}

//---------------------------------------------------------
void CollidePlayer(game_object* pPlayer, play_cell* pCollider)
{
    if (hud.transitionIsRunning)
        return;
    //x_printf("Coord:(%d,%d), content:%c\n", player->playGrid.x, player->playGrid.y, *(u8 *)pCell);
    switch (pCollider->type)
    {
        case CELL_HOLE:
        {
            HandleDeath(true);
            break;
        }
        case CELL_ENEMY:
        {
            HandleDeath(true);
            break;
        }
        case CELL_SPIKE:
        {
            game_object* pSpike = GetObjectFromIndex(pCollider->objIndex);
            if (pSpike->anim.frameIndex!=0)
            {
                HandleDeath(true);
            }
            break;
        }
        case CELL_COIN:
        {
            HandlePickup();
            break;
        }
        case CELL_TILE:
        {
            if (!pCollider->isDark)
            {
                pCollider->isDark = true;
                BeginPulsePalette();
                bool tileFull = IncrementHudTileCount();
                if (tileFull)
                {
                    x_printf("Tile full bonus.\n");
                }
                levelManager.tilesRemaining--;
                //x_printf("%d tiles remaining.", (s16)playArea.tilesToFlip);
                if (levelManager.tilesRemaining == 0)
                {
                    x_printf("No more tiles. Restart.\n");
                    // End of map, needs to go on to the next
                    gameManager.isRunning = false;
                }
            }
            SetPlayerAnimIdle(pPlayer->direction * 8+PLAYERSPR_IDLE_ANIM, 0, 0);
            break;
        }
    }
}

//---------------------------------------------------------
object_vtable playerVirtualTable = 
{
    CreatePlayer,
    UpdatePlayer,
    RenderPlayer,
    DestroyPlayer,
    CollidePlayer,
    BlowupPlayer,
};

//---------------------------------------------------------
void InitializePlayer(void)
{
    player = CreateObject(&playerVirtualTable, playArea.start.x, playArea.start.y);
}

//---------------------------------------------------------
void ResetPlayer(void)
{
	player->playGrid = playArea.start;
	SnapToPlayAreaGrid(player);
    CreatePlayer(player, playArea.start.x, playArea.start.y);
}


//---------------------------------------------------------
void MovePlayer(game_object* player)
{
    PulsePalette();
    if (player->moveSteps)
    {
        TransformComponent(&player->trans);
        player->moveSteps--;
        if (player->moveSteps != 0)
        {
            s16 dy = F_TO_I(player->trans.pos.y+tileMap.position.y);
            if (dy > 256+32)
            {
                // We must have been in a die. Respawn now.
                ResetPlayer();
            }
            return;
        }
        SnapToPlayAreaGrid(player);
        player->trans.gravity = FIXED_POINT_HALF;
        play_cell *pCell = GetPlayAreaCell(player->playGrid.x, player->playGrid.y);
        if (pCell)
        {
            player->object.vtable->Collide(player, pCell);
        }
        return;
    }
    HandleControllerInput();
}

