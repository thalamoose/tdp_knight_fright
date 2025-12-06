typedef struct
{
	enemy_controller controller;
	coord position;
} bear;

void CreateBear(bear* pObject);
bool UpdateBear(bear* pObject);
void DestroyBear(bear* pObject);
void BlowupBear(bear* pObject);
void CollideBear(bear* pObject, enemy_controller* pCollider);
