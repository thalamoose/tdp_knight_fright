#include "structs.h"

extern void update_position(player_position* obj);

player_position player;
player_position* pPlayer =&player;
void main(void)
{
	update_position(&player);
	update_position(pPlayer);
	
}

