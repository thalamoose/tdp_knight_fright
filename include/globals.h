#if !defined(__GLOBALS_H)
#define __GLOBALS_H

typedef struct s_globals
{
	u8 particlesActive;
	u8 activeSpriteCount;
	coord charScreen;
	u16 pulseColour;
	u16 pulseTarget;
	u16 level;
	u16 bonusTracker;
	bool useSuperCoins;
	coord_s8 pulseCoord;
	u8 particleIndex;
} globals;

extern globals global;

#endif
