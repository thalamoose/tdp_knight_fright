#if !defined(__UTILITIES_H)
#define __UTILITIES_H
#include "kftypes.h"
void *memcpy_dma(void *tgt, const void *src, u16 length);
void *memset(void *tgt, u8 value, u16 length);
void nextreg(u8 reg, u8 val);
void nextreg16(u8 reg, u16 val);
void CopyPalette(void *palette, u8 paletteType);
void CopySprite(void *srcData, u8 slot, u8 totalSlots);
void WaitVSync(void);
void x_printf(const char *format, ...);

void SetColour(u8 palette, u8 index, u16 colour);
u16 BlendColour(u16 current, u16 target);

s16 random16(void);
s8 random8(void);

s16 fp_mul_f88(s16 a, s16 b);

void bcd_add(u8 bcd_array[], u8 value);

#endif
