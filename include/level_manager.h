

typedef struct 
{
	u8 numCoins;
	u8 numHoles;
	u8 numObstacles;
	u8 maxNumEnemies;
	u8 spawnRate;
	u8 numSpikes;
	u8 enabledEnemies;
} level_configuration;

typedef struct
{
	u16 levelNum;
	u8 levelComplete;
	bool isIntroLevel;
	bool playerIsDead;
	game_object* player;
	u8 currentLevelType;
	u8 currentMaxEnemies;
	u8 currentSpawnRate;
	u16 tilesFlipped;
	u16 coinsFound;
	u16 tilesRemaining;
	u16 randomDropCounter;
	bool dropEnemies;
	u8 enabledEnemiesCount;
	u8 enabledEnemies[8];
	level_configuration config;
	coord_s8 playerSpawnPos;
	u8 state;
	u8 stateCounter;
	u8 enemiesActive;
	u8 enemyDropDelay;
} level_manager;

typedef struct
{
    const play_area_template* template;
    level_configuration config;
} level_template;

void InitializeLevelManager(void);
void ResetLevelManager(void);
void UpdateLevelManager(void);
void DestroyLevelManager(void);

const level_template* GetLevelTemplate(u8 levelIndex);

void GenerateLevel(u8 levelNum, level_configuration* config);
bool CheckLevelComplete(void);
void NewLevel(void);

extern level_manager levelManager;
