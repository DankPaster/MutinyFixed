#include "TE_FireBullets.h"
#include "VTHook.h"
#include "Interfaces.h"
#include "player_lagcompensation.h"
#include "LocalPlayer.h"

TE_FireBullets_PostDataUpdateFn oTE_FireBullets_PostDataUpdate;



void __stdcall FireBullets_PostDataUpdate(C_TEFireBullets* thisptr, DataUpdateType_t updateType)
{
	if (!DisableAllChk.Checked && ResolverChk.Checked && FireWhenEnemiesShootChk.Checked)
	{
		int iPlayer = ReadInt((uintptr_t)&thisptr->m_iPlayer) + 1;
		if (iPlayer <= MAX_PLAYERS)
		{
			QAngle	vecAngles;
			ReadAngle((uintptr_t)&thisptr->m_vecAngles, vecAngles);
			Vector vecOrigin;
			ReadVector((uintptr_t)&thisptr->_m_vecOrigin, vecOrigin);
			//CBaseEntity *pPlayer = Interfaces::ClientEntList->GetClientEntity(iPlayer);

			CustomPlayer *pCPlayer = &AllPlayers[iPlayer];

			if (pCPlayer && !pCPlayer->Dormant && pCPlayer->BaseEntity && pCPlayer->BaseEntity != LocalPlayer.Entity)// && (pCPlayer->Personalize.ResolvePitch || pCPlayer->Personalize.ResolveYaw))
			{
			//	Vector vel = pCPlayer->BaseEntity->GetVelocity();
			//	if (vel.Length() < 0.5f)
				{
					//pCPlayer->HandledFireBullet = false;
					//pCPlayer->IsFiring = true;
					//int tick_count = ReadInt((uintptr_t)&Interfaces::Globals->tickcount);
					//int sim_tick = TIME_TO_TICKS(pCPlayer->CurrentNetVars.simulationtime - (Interfaces::Globals->interval_per_tick * 2));
					//int diff = sim_tick - tick_count
					//int lag = TIME_TO_TICKS(pCPlayer->CurrentNetVars.simulationtime - pCPlayer->CurrentNetVars.animtime);
					//DWORD ClientState = (DWORD)ReadInt(ReadInt(pClientState));

					//float simtickshot = pCPlayer->m_flOldSimulationTime - Interfaces::Globals->interval_per_tick;// -0.000005;
					//pCPlayer->TickFiredBullet = TIME_TO_TICKS(simtickshot);
					//pCPlayer->LowerBodyYawWhenFiredBullet = pCPlayer->CurrentNetVars.lowerbodyyaw;
					//pCPlayer->TickFiredBullet = tick_count + diff;
					//pCPlayer->TickLaggedFiredBullet = sim_tick + TIME_TO_TICKS(pCPlayer->CurrentNetVars.simulationtime - pCPlayer->CurrentNetVars.animtime) - 1;
					//pCPlayer->TickFiredBullet = sim_tick + 1;
					//ReadAngle((uintptr_t)&thisptr->m_vecAngles, pCPlayer->AnglesFiredBullet);
					//SaveNetvars(&pCPlayer->LastUpdatedNetVars, pCPlayer->BaseEntity, pCPlayer, pCPlayer->CurrentLowerBodyYaw);
					//pCPlayer->LastUpdatedNetVars.eyeangles = vecAngles;
					//pCPlayer->LastUpdatedNetVars.tickcount = TIME_TO_TICKS(pCPlayer->BaseEntity->GetSimulationTime());//Interfaces::Globals->tickcount; //pCPlayer->BaseEntity->GetSimulationTime();
				}
			}
		}
	}
	oTE_FireBullets_PostDataUpdate(thisptr, updateType);
}

__declspec (naked) void __stdcall Hooks::TE_FireBullets_PostDataUpdate(DataUpdateType_t updateType)
{
	__asm
	{
		push[esp + 4]
		push ecx
		call FireBullets_PostDataUpdate
		retn 4
	}
}