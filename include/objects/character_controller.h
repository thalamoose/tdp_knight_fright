enum enemy_type
{
	ENEMY_BEAR=(1<<0),
	ENEMY_BIG_HOPPER=(1<<1),
	ENEMY_COLOR_CHANGER=(1<<2),
	ENEMY_FOLLOWER=(1<<3),
	ENEMY_SPIKE=(1<<4),
};

game_object* CreateEnemyObject(u8 type, const coord_s8* mapPosition, u16 param);
void DestroyEnemy(game_object* enemy);
void MoveCharacter(game_object* pObj, u8 spriteBase, u8 buttons);
