#if !defined(__PLAYER_H)
#define __PLAYER_H

#include "object_manager.h"

void SetPlayerAnimIdle(u8 baseIndex, s16 vx, s16 vy);
void SetPlayerAnim(u8 baseIndex, u8 direction, s16 vx, s16 vy);
void InitializePlayer(void);
void ResetPlayer(void);

u8 GetPlaygridContent(u8 x, u8 y);

bool CheckReset(void);
extern game_object* player;
#endif
