void CreateSpike(game_object* pObject,  const coord_s8* mapPosition, u16 param);
bool UpdateSpike(game_object* pObject);
void RenderSpike(game_object* pObject);
void DestroySpike(game_object* pObject);
void BlowupSpike(game_object* pObject);
void CollideSpike(game_object* pObject, const game_object* pCollider);
