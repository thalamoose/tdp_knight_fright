#if !defined(__GAME_MANAGER_H)
#define __GAME_MANAGER_H

void InitializeGameManager(void);
void ResetGameManager(void);
void UpdateGameManager(void);
void RenderGameManager(void);
void EndGame(void);

typedef struct
{
	bool isPaused;
	bool isRunning;
	u8 particlesActive;
	u8 ticksPerSecond;
} game_manager;

extern game_manager gameManager;
#endif
