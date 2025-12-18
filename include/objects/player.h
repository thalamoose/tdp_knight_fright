#if !defined(__PLAYER_H)
#define __PLAYER_H

#include "object_manager.h"


void ResetPlayer(game_object* pObject);
void HandleControllerInput(game_object* pObj, u8 spriteBase, u8 buttons);
game_object* CreatePlayerObject(void);
bool CheckReset(void);
#endif
