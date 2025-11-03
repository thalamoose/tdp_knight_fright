#include "kftypes.h"
#include "defines.h"
#include "particles.h"
#include "utilities.h"


// Update and render functions in particles.asm

particle particleObjects[MAX_PARTICLES];
u8 particleIndex;

void InitializeParticles(void)
{
	memset( particleObjects, 0, sizeof(particleObjects));
	particleIndex = 0;
}

u8 AddParticle(s16 x, s16 y, s16 vx, s16 vy, s8 life, s8 colour, s8 width, s8 flags)
{
	u8 index = particleIndex;
	particle* pParticle = &particleObjects[index];
	while (index!=particleIndex)
	{
		if (!(pParticle->flags & PARTICLE_ACTIVE))
		{
			break;
		}
		index++;
		if (index>=MAX_PARTICLES)
		{
			index = 0;
			pParticle = &particleObjects[0];
		}
	}
	pParticle->x = x;
	pParticle->y = y;
	pParticle->vx = vx;
	pParticle->vy = vy;
	pParticle->life = life;
	pParticle->flags = flags;
	pParticle->colour = colour;
	pParticle->width = width;
	pParticle->prevColour = colour;
	pParticle->prevPage = 0;
	pParticle->prevAddress = 0;
	pParticle->flags = PARTICLE_ACTIVE;
	particleIndex = (index+1)&(MAX_PARTICLES-1);

	return index;
}
