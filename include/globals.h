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
	coord tileMap;
	coord charScreen;
	u16 pulseColour;
	u16 pulseTarget;
	u8 level;
	u8 playAreaIndex;
	coord pulseCoord;
	u8 particleIndex;
} globals;

extern globals global;

#endif

