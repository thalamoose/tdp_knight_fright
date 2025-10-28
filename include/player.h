#if !defined(__PLAYER_H)
#define __PLAYER_H

#include "kftypes.h"
#include "objects.h"

void InitializePlayer(void);
void UpdatePlayer(void);
void RenderPlayer(void);
void SetPlayerAnimIdle(u8 baseIndex, s16 vx, s16 vy);
void SetPlayerAnim(u8 baseIndex, u8 direction, s16 vx, s16 vy);

typedef struct s_player_object
{
    object object;
    coord playgrid;
    u8 moveSteps;
    u8 direction;

} player_object;

u8 GetPlaygridContent(u8 x, u8 y);

bool CheckReset(void);
#endif
