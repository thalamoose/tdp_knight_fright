typedef struct
{
	object object;
} bear;

void CreateBear(bear* pObject);
bool UpdateBear(bear* pObject);
void RenderBear(bear* pObject);
void DestroyBear(bear* pObject);
void BlowupBear(bear* pObject);
void CollideBear(bear* pObject, object* pCollider);
