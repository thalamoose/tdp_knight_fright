typedef struct
{
	enemy_controller controller;
	coord position;
} color_changer;

void CreateColorChanger(color_changer* pObject);
bool UpdateColorChanger(color_changer* pObject);
void DestroyColorChanger(color_changer* pObject);
void BlowupColorChanger(color_changer* pObject);
void CollideColorChanger(color_changer* pObject, enemy_controller* pCollider);
