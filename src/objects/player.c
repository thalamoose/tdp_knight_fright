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

void MovePlayer(game_object* pPlayer);

anim_config idleConfig = {PLAYERSPR_IDLE_ANIM, 0, 0};

//---------------------------------------------------------
void SetPlayerAnimIdle(game_object* pObj, const anim_config* config)
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
void SetPlayerAnimRun(game_object* pObj, const anim_config* pConfig )
{
    pObj->moveSteps = 16;
    pObj->direction = pConfig->direction;
    SetPlayerAnimIdle(pObj, pConfig);
}

const sprite_config playerSpriteConfig[4]=
{
    {PLAYER_SPRITE_PATTERN,    0,  0, 0, 0x40, 0x00},
    {PLAYER_SPRITE_PATTERN+1, 16,  0, 0, 0xc0, 0x60},
    {PLAYER_SPRITE_PATTERN+2,  0, 16, 0, 0xc0, 0x60},
    {PLAYER_SPRITE_PATTERN+3, 16, 16, 0, 0xc0, 0x60}
};

//---------------------------------------------------------
void CreatePlayer(game_object* pPlayer, const coord_s8* mapPosition, u16 param)
{
    UNUSED(param);
    SetPlayerAnimIdle(pPlayer, &idleConfig);
    //
    // Set up sprites 64..67, so that only the minimum needs to be set up below.
    //
    for (u8 i=0; i<4; i++)
    {
        SetupSprite(PLAYER_SPRITE_SLOT+i,  &playerSpriteConfig[i]);
    }
    nextreg(SWAP_BANK_0_SLOT, MISC_DATA_PAGE);
    CopyPalettePartial(asset_PlayerPalette, PALETTE_SPRITE_PRIMARY, 0, 64);
    // Grab whatever colour is the background colour. This will be our transparent
    // index.
    pPlayer->anim.sprite.page = PLAYER_ANIM_PAGE;
    pPlayer->anim.sprite.slot = PLAYER_SPRITE_SLOT;
    pPlayer->anim.sprite.pattern = PLAYER_SPRITE_PATTERN;
    pPlayer->anim.sprite.patternCount = 4;
    pPlayer->anim.sprite.centerOffset.x = PLAYER_TO_TILE_X_OFFSET;
    pPlayer->anim.sprite.centerOffset.y = PLAYER_TO_TILE_Y_OFFSET;
    // Copies transparent color to the register.
    nextreg(SWAP_BANK_0_SLOT, PLAYER_ANIM_PAGE);
    nextreg(TRANS_SPRITE_INDEX, *(u8 *)SWAP_BANK_0);
    pPlayer->playGrid = *mapPosition;
    ResetPlayer(pPlayer);
}

//---------------------------------------------------------
bool UpdatePlayer(game_object* pPlayer)
{
    MovePlayer(pPlayer);
    AnimateComponent(&pPlayer->anim);
    return true;
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
	nextreg(SWAP_BANK_0_SLOT, MISC_DATA_PAGE);
	nextreg(SWAP_BANK_1_SLOT, VIRTUAL_TILEMAP_PAGE);
    // A palette pulse was in progress, so cancel it.
    if (global.pulseColour != global.pulseTarget)
    {
        RefreshPlayAreaCell(&global.pulseCoord, 0);
    }
    global.pulseColour = 0x1ff;
    global.pulseTarget = asset_TilemapPalette[4];
    global.pulseCoord = levelManager.player->playGrid;
    RefreshPlayAreaCell( &global.pulseCoord, 1);
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
        RefreshPlayAreaCell(&global.pulseCoord, 0);
    }
}

//---------------------------------------------------------
void HandlePickup(game_object* pObject)
{
    anim_config config;

    config.baseIndex = PLAYERSPR_IDLE_ANIM+pObject->direction*pObject->anim.totalFrames;
    config.direction = pObject->direction;
    config.vx = 0;
    config.vy = 0;
    SetPlayerAnimIdle(pObject, &config);
    for (int i=0; i<32; i++)
    {
        particle params;
        params.x = pObject->trans.pos.x+I_TO_F(16)+((s16)random8()<<3);
        params.y = pObject->trans.pos.y+I_TO_F(-16)+((s16)random8()<<3);
        params.x += I_TO_F(TILEMAP_PIX_WIDTH/2)+tileMap.position.x;
        params.y += I_TO_F(TILEMAP_PIX_HEIGHT/2)+tileMap.position.y;
        params.vx = random8();
        params.vy = random8();
        params.width = random8()&3+1;
        params.colour = random8() | 0xf0;
        params.life = (random8()&31)+24;
        params.flags = 0;
		params.prevPage = 0xff;
        AddParticle(&params);
    }
    play_cell* pCell = GetPlayAreaCell(&pObject->playGrid);
    game_object* pCollider = GetObjectFromIndex(pCell->objIndex);
    BlowupObject(pCollider);
    pCell->objIndex = 0;
    pCell->type = CELL_TILE;
}

anim_config deathAnimConfig = {PLAYERSPR_IDLE_ANIM, PLAYERDIR_BL, 0, -FIXED_POINT_ONE*2};
//---------------------------------------------------------
void HandleDeath(game_object* pObject, bool fallThrough)
{
    (void)fallThrough;
    deathAnimConfig.direction = pObject->direction;
    SetPlayerAnimIdle(pObject, &deathAnimConfig);
    pObject->moveSteps = 64;
    pObject->trans.gravity = FIXED_POINT_HALF/2;
}


anim_config playerMoves[4]=
{
    {PLAYERSPR_L+PLAYERSPR_RUN_ANIM, PLAYERDIR_TL,-FIXED_POINT_ONE, -FIXED_POINT_ONE*2},
    {PLAYERSPR_R+PLAYERSPR_RUN_ANIM, PLAYERDIR_BR, FIXED_POINT_ONE, -FIXED_POINT_ONE*5},
    {PLAYERSPR_U+PLAYERSPR_RUN_ANIM, PLAYERDIR_TR,-FIXED_POINT_ONE, -FIXED_POINT_ONE*5},
    {PLAYERSPR_D+PLAYERSPR_RUN_ANIM, PLAYERDIR_BL, FIXED_POINT_ONE, -FIXED_POINT_ONE*2}
};

//---------------------------------------------------------
void HandleControllerInput(game_object* pObject, u8 spriteBase, u8 buttons)
{
    if (hud.transitionIsRunning)
        return;
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
            SetPlayerAnimRun(pObject, pAnimConfig);
        }
    }
    if (buttons&JOYPAD_R_DOWN)
    {
        levelManager.tilesRemaining = 1;
    }
}

//---------------------------------------------------------
void CollidePlayer(game_object* pPlayer, const game_object* pCollider)
{
    if (hud.transitionIsRunning)
        return;
    //x_printf("Coord:(%d,%d), content:%c\n", player->playGrid.x, player->playGrid.y, *(u8 *)pCell);
    play_cell* pCell = GetPlayAreaCell(&pPlayer->playGrid);
    switch (pCell->type)
    {
        case CELL_HOLE:
        {
            HandleDeath(pPlayer, true);
            break;
        }
        case CELL_ENEMY:
        {
            HandleDeath(pPlayer, true);
            break;
        }
        case CELL_SPIKE:
        {
            if (pCollider->anim.frameIndex!=0)
            {
                HandleDeath(pPlayer, true);
            }
            break;
        }
        case CELL_COIN:
        {
            HandlePickup(pPlayer);
            break;
        }
        case CELL_TILE:
        {
            if (!pCell->isDark)
            {
                pCell->isDark = true;
                BeginPulsePalette();
                bool tileFull = IncrementHudTileCount();
                if (tileFull)
                {
                    x_printf("Tile full bonus.\n");
                }
                levelManager.tilesRemaining--;
            }
            anim_config config;
            config.direction = pPlayer->direction;
            config.baseIndex = PLAYERSPR_IDLE_ANIM+pPlayer->direction*8;
            config.vx = 0;
            config.vy = 0;
            SetPlayerAnimIdle(pPlayer, &config);
            break;
        }
    }
}

//---------------------------------------------------------
object_vtable playerVirtualTable = 
{
    CreatePlayer,
    UpdatePlayer,
    RenderComponent,
    DestroyPlayer,
    CollidePlayer,
    BlowupPlayer,
};

//---------------------------------------------------------
game_object* CreatePlayerObject(void)
{
    return CreateObject(&playerVirtualTable, &playArea.start, 0);
}

//---------------------------------------------------------
void ResetPlayer(game_object* pObject)
{
	pObject->playGrid = playArea.start;
	SnapToPlayAreaGrid(pObject);
    pObject->moveSteps = 32;
    pObject->trans.pos.y -= I_TO_F(TILEMAP_PIX_HEIGHT/2+64);
    pObject->trans.vel.x = 0;
    pObject->trans.vel.y = FIXED_POINT_HALF*4;
    pObject->trans.gravity = FIXED_POINT_HALF/2;
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
                ResetPlayer(player);
            }
            return;
        }
        SnapToPlayAreaGrid(player);
        player->trans.gravity = FIXED_POINT_HALF;
        play_cell *pCell = GetPlayAreaCell(&player->playGrid);
        if (pCell)
        {
            game_object* pCollider = GetObjectFromIndex(pCell->objIndex);
            player->object.vtable->Collide(player, pCollider);
        }
        return;
    }
    HandleControllerInput(player, 0, ReadController());
}

