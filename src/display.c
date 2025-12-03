#include "kftypes.h"
#include "defines.h"
#include "globals.h"
#include "display.h"
#include "utilities.h"

void ClearScreen(void)
{
	global.charScreen.x = 0;
	global.charScreen.y = 0;
	memset(SCREEN_BASE, 0, SCREEN_LENGTH);
	memset(ATTR_BASE, 0x47, ATTR_LENGTH);
}
