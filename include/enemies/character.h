typedef struct
{
	enemy_controller controller;
	coord position;
} character;

character* InitCharacter(void);
void UpdateCharacter(character* pObject);
void DestroyCharacter(character* pObject);
