#include "kftypes.h"

void InitializeParticles(void);
u8  UpdateParticles(void);
void RenderParticles(void);


//
// This MUST match the structure definition in particles.asm
typedef struct s_particle
{
	s16 vx;
	s16 x;
	s16 vy;
	s16 y;
	s8 life;
	s16 prevAddress;
	s8 prevPage;
	s8 prevColour;
	s8 colour;
	s8 width;
	s8 flags;
} particle;

enum
{
	PARTICLE_ACTIVE = (1<<0)
};

u8 AddParticle(s16 x, s16 y, s16 vx, s16 vy, s8 life, s8 colour, s8 width, s8 flags);
