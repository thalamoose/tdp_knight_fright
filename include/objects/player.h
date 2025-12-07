#if !defined(__PLAYER_H)
#define __PLAYER_H

#include "objects.h"

void SetPlayerAnimIdle(u8 baseIndex, s16 vx, s16 vy);
void SetPlayerAnim(u8 baseIndex, u8 direction, s16 vx, s16 vy);
void InitializePlayer(void);

typedef struct s_player_object
{
    object object;
    u8 moveSteps;
    u8 direction;
} player_object;

u8 GetPlaygridContent(u8 x, u8 y);

bool CheckReset(void);
extern player_object* player;
#endif
