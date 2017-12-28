
#include "CBaseHandle.h"
#include "Interfaces.h"
// -------------------------------------------------------------------------------------------------- //
// Game-code CBaseHandle implementation.
// -------------------------------------------------------------------------------------------------- //
IHandleEntity* CBaseHandle::Get() const
{
	extern CBaseEntityList *g_pEntityList;
	return (IHandleEntity*)Interfaces::ClientEntList->GetClientEntityFromHandle((DWORD)this); //DYLAN FIXME
}