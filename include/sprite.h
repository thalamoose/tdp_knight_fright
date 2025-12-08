#if !defined(__SPRITE_H)
#define __SPRITE_H

typedef struct s_anim_sprite
{
	u8 slot;
	u8 pattern;
	u8 palette;
	u8 frameIndex;
	u8 baseIndex;
	u8 lastIndex;
	s8 animDelay;
	u8 animSpeed;
	u8 totalFrames;
} anim_sprite;

void InitializeSprites(void);
void ResetSprites(void);
void SetupSprite(u8 slot, u8 pattern, u8 attr0, u8 attr1, u8 attr2, u8 attr3, u8 attr4);
void UpdateSprites(void);
void RenderSprites(void);

#endif
