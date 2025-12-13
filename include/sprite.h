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

typedef struct s_sprite_config
{
	u8 pattern;
	u8 attr0;
	u8 attr1;
	u8 attr2;
	u8 attr3;
	u8 attr4;
} sprite_config;

void InitializeSprites(void);
void ResetSprites(void);
void SetupSprite(u8 slot, const sprite_config* config);
void HideSprite(u8 slot);
void UpdateSprites(void);
void RenderSprites(void);

u8 AllocSpriteSlot(void);
u8 AllocSpritePattern(void);

#endif
