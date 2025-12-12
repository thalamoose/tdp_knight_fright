#if !defined(__PLAYER_H)
#define __PLAYER_H

#include "object_manager.h"

void SetPlayerAnimIdle(game_object* pObject, u8 baseIndex, s16 vx, s16 vy);
void SetPlayerAnim(game_object* pObject, u8 baseIndex, u8 direction, s16 vx, s16 vy);
void ResetPlayer(game_object* pObject);
void HandleControllerInput(game_object* pObj, u8 spriteBase, u8 buttons);
game_object* CreatePlayerObject(void);
bool CheckReset(void);
#endif
