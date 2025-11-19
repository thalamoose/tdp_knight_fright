#include "update.h"
#include "player.h"
#include "particles.h"
#include "globals.h"
#include "npcs.h"

void UpdateObjects(void)
{

	UpdateNpcs();
	UpdatePlayer();
	UpdateSprites();
	global.particlesActive = UpdateParticles(&particles[0]);
}