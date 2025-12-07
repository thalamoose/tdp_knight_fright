enum enemy_type
{
	ENEMY_BEAR=(1<<0),
	ENEMY_BIG_HOPPER=(1<<1),
	ENEMY_COLOR_CHANGER=(1<<2),
	ENEMY_FOLLOWER=(1<<3),
	ENEMY_SPIKE=(1<<4),
};

object* CreateEnemy(u8 type);
void DestroyEnemy(object* enemy);
