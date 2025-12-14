#include "kftypes.h"
#include "defines.h"
#include "particles.h"
#include "utilities.h"
#include "globals.h"

extern particle particles[MAX_PARTICLES];
// Update and render functions in particles.asm

//---------------------------------------------------------
void InitializeParticles(void)
{
	memset(particles, 0, sizeof(particles));
	global.particleIndex = 0;
}

//---------------------------------------------------------
void ResetParticles(void)
{
	InitializeParticles();
}

//---------------------------------------------------------
u8 AddParticle(const particle* params)
{
	u8 index = global.particleIndex;
	particle *pParticle = &particles[index];
	do
	{
		if (!(pParticle->flags & PARTICLE_ACTIVE))
		{
			break;
		}
		index = (index + 1) & (MAX_PARTICLES - 1);
		if (index == 0)
		{
			pParticle = &particles[0];
		}
	} while (index != global.particleIndex);
	if (pParticle->prevPage)
	{
		RemoveParticle(pParticle);
	}
	*pParticle = *params;
	pParticle->flags = PARTICLE_ACTIVE;
	global.particleIndex = (index + 1) & (MAX_PARTICLES - 1);

	return index;
}
