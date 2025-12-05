typedef struct
{
	enemy_controller controller;
} big_hopper;

void InitBigHopper(big_hopper* pObject);
bool UpdateBigHopper(big_hopper* pObject);
void DestroyBigHopper(big_hopper* pObject);
