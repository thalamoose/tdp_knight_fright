typedef struct
{
	enemy_controller controller;
	coord position;
} follower;

void CreateFollower(follower* pObject);
bool UpdateFollower(follower* pObject);
void DestroyFollower(follower* pObject);
void BlowupFollower(follower* pObject);
void CollideFollower(follower* pObject, enemy_controller* pCollider);
