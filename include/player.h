#if !defined(__PLAYER_H)
#define __PLAYER_H

#include "kftypes.h"
#include "objects.h"

void InitializePlayer(void);
void UpdatePlayer(void);
void AnimatePlayer(void);
void RenderPlayer(void);
void MovePlayer(void);

typedef struct s_player_object
{
    object object;
    u8 playgrid_x;
    u8 playgrid_y;
    u8 moveSteps;
    u8 direction;

} player_object;

u8 GetPlaygridContent(u8 x, u8 y);

#endif
