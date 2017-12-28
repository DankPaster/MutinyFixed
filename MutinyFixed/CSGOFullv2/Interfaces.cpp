#include "Interfaces.h"

void(__fastcall *pUTIL_ClipTraceToPlayers) (const Vector&, const Vector&, unsigned int, ITraceFilter *, trace_t *);
DWORD pClientState;

BOOLEAN(__thiscall *IsPaused) (DWORD ClientState);
int host_tickcount;
int m_pPredictionRandomSeed;
DWORD m_pPredictionPlayer;


IBaseClientDll*			Interfaces::Client;
IClientModeShared*		Interfaces::ClientMode;
IClientEntityList*		Interfaces::ClientEntList;
ICVar*					Interfaces::Cvar;
//IInputSystem*			Interfaces::InputSystem;
IEngineClient*			Interfaces::Engine;
IEngineTrace*			Interfaces::EngineTrace;
IGlobalVarsBase*		Interfaces::Globals;
//ISurface*				Interfaces::Surface;
IVPanel*				Interfaces::VPanel;
//IVModelRender*			Interfaces::ModelRender;
//IVModelInfo*			Interfaces::ModelInfo;
//IMaterialSystem*		Interfaces::MaterialSystem;
//IMaterial*				Interfaces::Material;
IVRenderView*			Interfaces::RenderView;
CPrediction*			Interfaces::Prediction;
CGameMovement*          Interfaces::GameMovement;
IPhysicsSurfaceProps*	Interfaces::Physprops;
IVModelInfoClient*       Interfaces::ModelInfoClient;
IVDebugOverlay*			Interfaces::DebugOverlay;
//IStudioRender*			Interfaces::StudioRender;
IGameEventManager2*		Interfaces::GameEventManager;
C_TEFireBullets* Interfaces::TE_FireBullets;
C_TEEffectDispatch* Interfaces::TE_EffectDispatch;
IMDLCache* Interfaces::MDLCache;
void* Interfaces::IEngineSoundClient;
void* Interfaces::Surface;

//IMaterial* visible_tex;
//IMaterial* hidden_tex;
//IMaterial* visible_flat;
//IMaterial* hidden_flat;
/*
void UTIL_ClipTraceToPlayers(const Vector& vecAbsStart, const Vector& vecAbsEnd, unsigned int mask, ITraceFilter *filter, trace_t *tr)
{
	trace_t playerTrace;
	Ray_t ray;
	float smallestFraction = tr->fraction;
	const float maxRange = 60.0f;

	ray.Init(vecAbsStart, vecAbsEnd);

	for (int k = 1; k <= Interfaces::Globals->maxClients; ++k)
	{
		CBasePlayer *player = UTIL_PlayerByIndex(k);

		if (!player || !player->IsAlive())
			continue;

#ifdef CLIENT_DLL
		if (player->IsDormant())
			continue;
#endif // CLIENT_DLL

		if (filter && filter->ShouldHitEntity(player, mask) == false)
			continue;

		float range = DistanceToRay(player->WorldSpaceCenter(), vecAbsStart, vecAbsEnd);
		if (range < 0.0f || range > maxRange)
			continue;

		enginetrace->ClipRayToEntity(ray, mask | CONTENTS_HITBOX, player, &playerTrace);
		if (playerTrace.fraction < smallestFraction)
		{
			// we shortened the ray - save off the trace
			*tr = playerTrace;
			smallestFraction = playerTrace.fraction;
		}
	}
}
*/