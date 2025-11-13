#if !defined(__GLOBALS_H)
#define __GLOBALS_H

#include "kftypes.h"
#include "defines.h"
#include "objects.h"
#include "player.h"
#include "playarea.h"
#include "particles.h"

typedef struct s_globals
{
	u8 particlesActive;
	u8 activeSpriteCount;
	bool gameIsRunning;
	coord tileMap;
	coord charScreen;
	u16 pulseColour;
	u16 pulseTarget;
	coord pulseCoord;
	u8 particleIndex;
	play_area playArea;
} globals;

extern globals global;

#endif

