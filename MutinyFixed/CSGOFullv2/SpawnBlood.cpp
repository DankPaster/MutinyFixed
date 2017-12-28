#include "VTHook.h"
#include "SpawnBlood.h"
#include "Interfaces.h"
#include "player_lagcompensation.h"
#include "LocalPlayer.h"
#include "VMProtectDefs.h"

void OnSpawnBlood(C_TEEffectDispatch* thisptr, DataUpdateType_t updateType)
{
	auto stat = START_PROFILING("OnSpawnBlood");
	CBaseEntity* pEntity = thisptr->m_EffectData.m_hEntity != -1 ? Interfaces::ClientEntList->GetClientEntityFromHandle(thisptr->m_EffectData.m_hEntity) : nullptr;
	if (pEntity && pEntity->IsPlayer() && pEntity != LocalPlayer.Entity)
	{
#ifdef _DEBUG
		Vector* origin = &thisptr->m_EffectData.m_vOrigin;
		AllocateConsole();
		wchar_t PlayerName[32];
		GetPlayerName(GetRadar(), pEntity->index, PlayerName);
		printf("Spawn Blood:\nPlayer: %S\nBlood Origin: %.2f, %.2f, %.2f\nSimulation Time: %f\nTickcount: %i\n\n", PlayerName, origin->x, origin->y, origin->z, pEntity->GetSimulationTime(), Interfaces::Globals->tickcount);
#endif
		gLagCompensation.OnPlayerBloodSpawned(pEntity, &thisptr->m_EffectData.m_vOrigin, &thisptr->m_EffectData.m_vNormal);
	}
	oTE_EffectDispatch_PostDataUpdate(thisptr, updateType);
	END_PROFILING(stat);
}