#include "kftypes.h"
#include "defines.h"
#include "particles.h"
#include "utilities.h"
#include "globals.h"


// Update and render functions in particles.asm

void InitializeParticles(void)
{
	memset( global.particles, 0, sizeof(global.particles));
	global.particleIndex = 0;
}

u8 AddParticle(s16 x, s16 y, s16 vx, s16 vy, s8 life, s8 colour, s8 width, s8 flags)
{
	u8 index = global.particleIndex;
	particle* pParticle = &global.particles[index];
	do
	{
		if (!(pParticle->flags & PARTICLE_ACTIVE))
		{
			break;
		}
		index = (index+1) & (MAX_PARTICLES-1);
		if (index==0)
		{
			pParticle = &global.particles[0];
		}
	} while (index!=global.particleIndex);
	if (pParticle->prevPage) 
	{
		RemoveParticle(pParticle);
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
	global.particleIndex = (index+1)&(MAX_PARTICLES-1);

	return index;
}
