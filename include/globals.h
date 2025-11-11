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
	struct
	{
		u16 activeColours[2];
		u16 inactiveColours[2];8
		u16 pulseColours[2];
		u16 pulseTarget[2];
		u8 segmentsLit;
	} hud;
	
	player_object player;
	object objects[MAX_OBJECTS];
	play_area playArea;
	particle particles[MAX_PARTICLES];
} globals;

extern globals global;

#endif

