#include "kftypes.h"

typedef struct s_mouse
{
	u8 x;
	u8 y;
	u8 button;
} mouse;

u8 ReadController(void);
u8 ReadKeyboard(void);
const mouse *ReadMouse(void);
