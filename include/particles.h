#include "kftypes.h"

void InitializeParticles(void);
void UpdateParticles(void);
void RenderParticles(void);

typedef struct 
{
	s16 vx;
	s16 x;
	s16 vy;
	s16 y;
	s8 life;
	s16 prev_address;
	s8 prev_page;
	s8 prev_colour;
	s8 colour;
	s8 width;
	s8 flags;
} particle;
