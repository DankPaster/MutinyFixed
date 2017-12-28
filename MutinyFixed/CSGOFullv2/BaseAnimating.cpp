#include "BaseAnimating.h"
#include "GameMemory.h"

int CBaseAnimating::GetHitboxSet()
{
	return ReadInt((uintptr_t)this + m_nHitboxSet);
}