#if !defined(__COINS_H)
#define __COINS_H
#include "objects.h"

typedef struct
{
	object object;
	sprite sprite;
	coord_s8 playmapCell;
	u8 type;
} coin;

typedef struct
{
	u8 itemIndex;
	coin coin[MAX_COINS];
} coin_manager;

void InitializeCoins(void);
void ResetCoins(void);

enum
{
	COIN_NORMAL,
	COIN_SUPER,
};

u8 AddCoin(u8 type, s8 x, s8 y);
void RemoveCoin(u8 coinIndex);
void UpdateCoins(void);
void RenderCoins(void);

#endif