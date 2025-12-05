typedef struct
{
	enemy_controller controller;
	coord position;
} spike;

void InitSpike(spike* pObject);
bool UpdateSpike(spike* pObject);
void DestroySpike(spike* pObject);
