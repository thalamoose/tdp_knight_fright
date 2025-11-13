#include "kftypes.h"
#include "defines.h"
#include "utilities.h"

u16 next2rgb(u16 nextColour)
{
    return (nextColour&0xff)<<1|(nextColour>>8);
}

u16 rgb2next(u16 rgbColour)
{
    return (rgbColour>>1)|((rgbColour&0x01)<<8);
}

u16 BlendColour(u16 current, u16 target)
{
    current = next2rgb(current);
    target = next2rgb(target);

    u16 newColour=current;

	if ((current & RED_MASK) < (target & RED_MASK)) newColour += (1<<RED_SHIFT);
    if ((current & RED_MASK) > (target & RED_MASK)) newColour -= (1<<RED_SHIFT);
    if ((current & GRN_MASK) < (target & GRN_MASK)) newColour += (1<<GRN_SHIFT);
    if ((current & GRN_MASK) > (target & GRN_MASK)) newColour -= (1<<GRN_SHIFT);
    if ((current & BLU_MASK) < (target & BLU_MASK)) newColour += (1<<BLU_SHIFT);
    if ((current & BLU_MASK) > (target & BLU_MASK)) newColour -= (1<<BLU_SHIFT);
    u16 cr = current>>RED_SHIFT & 7;
    u16 cg =  current>>GRN_SHIFT & 7;
    u16 cb =  current>>BLU_SHIFT & 7;
    u16 tr = target>>RED_SHIFT & 7;
    u16 tg =  target>>GRN_SHIFT & 7;
    u16 tb =  target>>BLU_SHIFT & 7;
    u16 nr = newColour>>RED_SHIFT & 7;
    u16 ng =  newColour>>GRN_SHIFT & 7;
    u16 nb =  newColour>>BLU_SHIFT & 7;

	return rgb2next(newColour);
}