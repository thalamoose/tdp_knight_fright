struct s_enemy_controller;
typedef struct s_enemy_controller enemy_controller;

typedef void(enemy_create_fn)(enemy_controller* enemy);
typedef void(enemy_update_fn)(enemy_controller* enemy);
typedef void(enemy_destroy_fn)(enemy_controller* enemy);
typedef void(enemy_blowup_fn)(enemy_controller* enemy);
typedef void(enemy_collide_fn)(enemy_controller* enemy, enemy_controller* collider);

typedef struct s_enemy_controller
{
	struct 
	{
		bool active:1;
	} flags;
	u8 type;
	coord_s8 playPosition;
	enemy_create_fn* Create;
	enemy_update_fn* Update;
	enemy_destroy_fn* Destroy;
	enemy_blowup_fn* Blowup;
	enemy_collide_fn* Collide;
} enemy_controller;

enum enemy_type
{
	ENEMY_BEAR=(1<<0),
	ENEMY_BIG_HOPPER=(1<<1),
	ENEMY_COLOR_CHANGER=(1<<2),
	ENEMY_FOLLOWER=(1<<3),
	ENEMY_SPIKE=(1<<4),
} ;

void InitEnemies(void);
enemy_controller* CreateEnemy(u8 type);
void UpdateEnemies(void);
void DestroyEnemy(enemy_controller* enemy);
enemy_controller* GetEnemyFromIndex(u8 enemyIndex);
