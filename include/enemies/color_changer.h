typedef struct
{
	enemy_controller controller;
	coord position;
} color_changer;

void InitColorChanger(color_changer* pObject);
bool UpdateColorChanger(color_changer* pObject);
void DestroyColorChanger(color_changer* pObject);
