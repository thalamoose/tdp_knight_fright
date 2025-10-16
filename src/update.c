#include "update.h"
#include "player.h"
#include "particles.h"

void UpdateObjects(void)
{
	AnimateObjects();
	UpdatePlayer();
	UpdateSprites();
	UpdateParticles();
}		