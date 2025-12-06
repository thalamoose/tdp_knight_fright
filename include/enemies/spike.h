typedef struct
{
	enemy_controller controller;
	coord position;
} spike;

void CreateSpike(spike* pObject);
bool UpdateSpike(spike* pObject);
void DestroySpike(spike* pObject);
void BlowupSpike(spike* pObject);
void CollideSpike(spike* pObject, enemy_controller* pCollider);
