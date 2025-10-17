#include "kftypes.h"

typedef struct s_globals
{
	u8 particlesActive;
	u8 debugMaxParticles;
	bool gameIsRunning;
	struct 
	{
		s8 x;
		s8 y;
	} playArea;

} globals;

extern globals global;