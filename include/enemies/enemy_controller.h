

typedef struct s_enemy_controller
{
	struct 
	{
		bool active:1;
	} flags;
	u8 type;
	coord_s8 playPosition;
} enemy_controller;

enum enemy_type
{
		ENEMY_TYPE_BEAR,
		ENEMY_TYPE_BIG_HOPPER,
		ENEMY_TYPE_COLOR_CHANGER,
		ENEMY_TYPE_FOLLOWER,
		ENEMY_TYPE_SPIKE,
} ;

void InitEnemies(void);
enemy_controller* CreateEnemy(u8 type);
void UpdateEnemies(void);
void DestroyEnemy(enemy_controller* enemy);
