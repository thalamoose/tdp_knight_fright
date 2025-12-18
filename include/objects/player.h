#if !defined(__PLAYER_H)
#define __PLAYER_H

#include "object_manager.h"


void ResetPlayer(game_object* pObject);
game_object* CreatePlayerObject(void);
bool CheckReset(void);
#endif
