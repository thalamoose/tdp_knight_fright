#if !defined(__COIN_H)
#define __COIN_H

enum
{
	COIN_NORMAL,
	COIN_SUPER,
};

u8 AddCoin(u8 type, s8 x, s8 y);
void RemoveCoin(u8 coinIndex);

#endif