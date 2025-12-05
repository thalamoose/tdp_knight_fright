typedef struct
{
	enemy_controller controller;
	coord position;
} follower;

void InitFollower(follower* pObject);
bool UpdateFollower(follower* pObject);
void DestroyFollower(follower* pObject);
