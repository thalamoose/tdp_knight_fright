#if !defined(__COIN_H)
#define __COIN_H

enum
{
	COIN_NORMAL,
	COIN_SUPER,
};
#define COIN_PALETTE 8

game_object* CreateCoinObject(s8 x, s8 y);

void RemoveCoin(u8 coinIndex);

#endif