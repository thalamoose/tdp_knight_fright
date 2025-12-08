typedef struct
{
	object object;
} big_hopper;

void CreateBigHopper(big_hopper* pObject);
bool UpdateBigHopper(big_hopper* pObject);
void RenderBigHopper(big_hopper* pObject);
void DestroyBigHopper(big_hopper* pObject);
void BlowupBigHopper(big_hopper* pObject);
void CollideBigHopper(big_hopper* pObject, object* pCollider);

