typedef struct
{
	enemy_controller controller;
	coord position;
} character;

void CreateCharacter(character* pObject);
void UpdateCharacter(character* pObject);
void DestroyCharacter(character* pObject);
