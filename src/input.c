#include "kftypes.h"
#include "input.h"
#include "hardware.h"

static const u8* ReadKeyboardMatrix(void);

//---------------------------------------------------------
u8 ReadController(void)
{
	const u8* scanCodes = ReadKeyboardMatrix();
	u8  buttons = port_in(KEMPSTON_JOYSTICK_PORT);
	if ((scanCodes[1] & (1<<0))==0)
	{
		buttons |= (1<<JOYPAD_L_UP);
	}
	if ((scanCodes[0] & (1<<1))==0)
	{
		buttons |= (1<<JOYPAD_L_DOWN);
	}
	if ((scanCodes[6] & (1<<1))==0)
	{
		buttons |= (1<<JOYPAD_L_LEFT);
	}
	if ((scanCodes[5] & (1<<0))==0)
	{
		buttons |= (1<<JOYPAD_L_RIGHT);
	}
	return buttons;
}

//---------------------------------------------------------
static const u8* ReadKeyboardMatrix(void)
{
	static u8 scancodes[8];
	u8 bitLine=0xfe;
	for(int i=0; i<8; i++)
	{
		scancodes[i] = port_in(bitLine<<8|ULA_PORT);
		bitLine = bitLine << 1 | 1;
	}
	return scancodes;
}

//---------------------------------------------------------
u8 ReadKeyboard(void)
{
	static const u8 keyTable[48]=
	{
		'^','Z','X','C','V',                    // Port FEFE
		'A','S','D','F','G',                    // Port FDFE
		'Q','W','E','R','T',                    // Port FBFE
		'1','2','3','4','5',                    // Port F7FE
		'0','9','8','7','6',                    // Port EFFE 
		'P','O','I','U','Y',                    // Port DFFE
		13 ,'L','K','J','H',                    // Port BFFE
		' ','~','M','N','B'                    // Port 7FFE
	};
	const u8* scanCodes = ReadKeyboardMatrix();
	for (u8 row=0; row<8; row++)
	{
		u8 scanCode = ~scanCodes[row] & 0x1f;
		if (scanCode)
		{
			u8 bit=0;
			while (scanCode)
			{
				if (scanCode & 1)
				{
					return keyTable[row*5+bit];
				}
				scanCode=scanCode>>1;
				bit++;
			}
		}
	}
	return 0;
}

//---------------------------------------------------------
const mouse* ReadMouse(void)
{
	static mouse mouseValue;
	mouseValue.x = port_in(KEMPSTON_MOUSE_X_PORT);
	mouseValue.y = port_in(KEMPSTON_MOUSE_Y_PORT);
	mouseValue.button = port_in(KEMPSTON_MOUSE_BTN_PORT);
	return &mouseValue;
}
