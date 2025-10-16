#if !defined(__UTILITIES_H)
#define __UTILITIES_H

extern void memcpy(void* tgt, const void* src, u16 length);
extern void memset(void* tgt, u8 value, u16 length);
extern void nextreg(u8 reg,u8 val);
extern void CopyPalette(void* palette,u8 paletteType);
extern void CopySprite(void* srcData, u8 slot, u8 totalSlots);

#endif
