#if !defined(__COIN_H)
#define __COIN_H

enum
{
	COIN_NORMAL,
	COIN_SUPER,
};
#define COIN_PALETTE 8

game_object* CreateCoinObject(const coord_s8* mapPosition);

void RemoveCoin(u8 coinIndex);

#endif