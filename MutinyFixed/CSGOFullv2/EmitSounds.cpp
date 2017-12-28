#include "VTHook.h"
#include "LocalPlayer.h"
#include "netchan.h"

EmitSoundFn oEmitSound;

void __fastcall Hooks::EmitSound(void* ecx, void* edx, void* filter, int iEntIndex, int iChannel, const char *pSoundEntry, unsigned int nSoundEntryHash, const char *pSample, float flVolume, float flAttenuation, int nSeed, int iFlags, int iPitch, const Vector *pOrigin, const Vector *pDirection, Vector * pUtlVecOrigins, bool bUpdatePositions, float soundtime, int speakerentity)
{
	CBaseEntity *pEnt = Interfaces::ClientEntList->GetClientEntity(iEntIndex);
	if (pEnt && pEnt != LocalPlayer.Entity && pEnt->IsPlayer() && pOrigin && MTargetting.IsPlayerAValidTarget(pEnt))
	{
		CustomPlayer *pCPlayer = &AllPlayers[pEnt->index];
		pCPlayer->iLastFootStepTick = Interfaces::Globals->tickcount;
		pCPlayer->vecLastFootStepOrigin = *pOrigin;
	}

	//<-- REMOVED -->
	oEmitSound(ecx, edx, filter, iEntIndex, iChannel, pSoundEntry, nSoundEntryHash, pSample, flVolume, flAttenuation, nSeed, iFlags, iPitch, pOrigin, pDirection, pUtlVecOrigins, bUpdatePositions, soundtime, speakerentity);
}
