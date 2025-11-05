#if !defined __COPPER_H
#define __COPPER_H

#include "kftypes.h"
void InitializeCopper(void);
void SetLineRegister(u8 line, u8 reg, u8 value);
void SetPlayerPrioritySplit(u8 line);
void RenderCopper(void);

#endif // __COPPER_H
