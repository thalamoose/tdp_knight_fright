#include "kftypes.h"
#include "copper.h"
#include "hardware.h"
#include "defines.h"
#include "utilities.h"

#if 0
struct
{
	u8 playerPriorityLine;
} copper;

#define CU_WAIT(ver, hor) ((u16)((((ver) & 0xff) << 8) + 0x80 + (((hor) & 0x3f) << 1) + (((ver) & 0x100) >> 8)))
#define CU_MOVE(reg, val) ((u16)((((val) & 0xff) << 8) + ((reg) & 0x7f)))
#define CU_NOP() ((u16)0x0000)
#define CU_STOP() ((u16)0xffff)

void InitializeCopper(void)
{
	memset(&copper, 0, sizeof(copper));
	nextreg(COPPER_CONTROL_L, 0);
	nextreg(COPPER_CONTROL_H, 0);
}

void RenderCopper(void)
{
	if (copper.playerPriorityLine)
	{
		nextreg(COPPER_CONTROL_L, 0);
		nextreg(COPPER_CONTROL_H, 0xc0);
		nextreg16(COPPER_DATA, CU_WAIT(0, copper.playerPriorityLine));
		nextreg16(COPPER_DATA, CU_MOVE(SPRITE_INDEX, PLAYER_SPRITE_SLOT));
		nextreg16(COPPER_DATA, CU_MOVE(SPRITE_INDEX, PLAYER_SPRITE_SLOT));
		nextreg16(COPPER_DATA, CU_STOP());
	}
	else
	{
		nextreg(COPPER_CONTROL_H, 0);
	}
}

void SetPlayerPriorityLine(u8 line)
{
	copper.playerPriorityLine = line;
}

#endif