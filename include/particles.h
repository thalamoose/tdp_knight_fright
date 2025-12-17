#if !defined(__PARTICLES_H)
#define __PARTICLES_H
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
	PARTICLE_ACTIVE = (1 << 0)
};

void InitializeParticles(void);
void ResetParticles(void);
u8 UpdateParticles(particle *particles);
void RenderParticles(particle *particles);
u8 AddParticle(const particle* pTemplate);
void RemoveParticle(particle *pParticle);
extern particle particles[];
#endif
