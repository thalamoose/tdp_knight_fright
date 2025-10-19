#include "kftypes.h"
#include "objects.h"
typedef struct s_globals
{
	u8 particlesActive;
	u8 debugMaxParticles;
	u8 activeSpriteCount;
	bool gameIsRunning;
	struct 
	{
		s8 x;
		s8 y;
	} playArea;
	coordinate charScreen;
} globals;

extern globals global;