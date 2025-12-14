void CreateFollower(game_object* pObject,  const coord_s8* mapPosition, u16 param);
bool UpdateFollower(game_object* pObject);
void RenderFollower(game_object* pObject);
void DestroyFollower(game_object* pObject);
void BlowupFollower(game_object* pObject);
void CollideFollower(game_object* pObject, const game_object* pCollider);
