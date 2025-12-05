typedef struct
{
	enemy_controller controller;
	coord position;
} bear;

void InitBear(bear* pObject);
bool UpdateBear(bear* pObject);
void DestroyBear(bear* pObject);
