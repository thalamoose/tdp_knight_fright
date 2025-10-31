#if !defined(__GLOBALS_H)
#define __GLOBALS_H

#include "kftypes.h"
#include "defines.h"
#include "objects.h"
#include "player.h"
#include "playarea.h"

typedef struct s_globals
{
	u8 particlesActive;
	u8 debugMaxParticles;
	u8 activeSpriteCount;
	bool gameIsRunning;
	coord tileMap;
	coord charScreen;
	player_object player;
	object objects[MAX_OBJECTS];
	play_area playArea;
} globals;

extern globals global;

#endif

