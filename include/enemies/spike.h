typedef struct
{
	object object;
} spike;

void CreateSpike(spike* pObject);
bool UpdateSpike(spike* pObject);
void DestroySpike(spike* pObject);
void BlowupSpike(spike* pObject);
void CollideSpike(spike* pObject, object* pCollider);
