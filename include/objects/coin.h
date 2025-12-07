#if !defined(__COIN_H)
#define __COIN_H

typedef struct
{
	object object;
	u8 type;
} coin;

enum
{
	COIN_NORMAL,
	COIN_SUPER,
};

u8 AddCoin(u8 type, s8 x, s8 y);
void RemoveCoin(u8 coinIndex);

#endif