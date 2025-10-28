#include "objects.h"
#include "defines.h"


void AnimateObject(object* pObj)
{
	if (!pObj->flags.active)
		return;
	pObj->frameIndex++;
	if (pObj->animDelay)
	{
		pObj->animDelay--;
		return;
	}
	pObj->animDelay = pObj->animSpeed;
	if (pObj->flags.direction)
	{
		pObj->frameIndex--;
		if( pObj->frameIndex==0)
			pObj->flags.direction = false;
	}
	else
	{
		pObj->frameIndex++;
		if (pObj->frameIndex==pObj->totalFrames)
		{
			if (pObj->flags.pingpong)
			{
				pObj->flags.direction = true;
				pObj->frameIndex = pObj->totalFrames-1;
			}
			else
				pObj->frameIndex=0;
		}
	}
}

