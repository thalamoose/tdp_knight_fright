typedef struct
{
	object object;
} color_changer;

void CreateColorChanger(color_changer* pObject);
bool UpdateColorChanger(color_changer* pObject);
void RenderColorChanger(color_changer* pObject);
void DestroyColorChanger(color_changer* pObject);
void BlowupColorChanger(color_changer* pObject);
void CollideColorChanger(color_changer* pObject, object* pCollider);
