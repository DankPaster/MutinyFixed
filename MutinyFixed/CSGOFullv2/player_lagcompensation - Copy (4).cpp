#include "player_lagcompensation.h"
#include "Interfaces.h"
#include "CreateMove.h"
#include "ConVar.h"
#include "utlvector.h"
#include "INetchannelInfo.h"
#include <math.h>
#include "BaseCombatWeapon.h"
#include "Aimbot.h"
#include "Triggerbot.h"

const Vector vecZero = { 0.0f, 0.0f, 0.0f };

ConVar *sv_unlag;
ConVar *sv_maxunlag;
ConVar *sv_lagflushbonecache;
ConVar *sv_lagpushticks;
ConVar *sv_client_min_interp_ratio;
ConVar *sv_client_max_interp_ratio;
ConVar *sv_maxupdaterate;
ConVar *sv_minupdaterate;
ConVar *cl_updaterate;
ConVar *sv_gravity = nullptr;
ConVar *cl_interp_ratio = nullptr;

char *sv_unlagstr = new char[9]{ 9, 12, 37, 15, 20, 22, 27, 29, 0 }; /*sv_unlag*/
char *sv_maxunlagstr = new char[12]{ 9, 12, 37, 23, 27, 2, 15, 20, 22, 27, 29, 0 }; /*sv_maxunlag*/
char *sv_lagflushbonecachestr = new char[21]{ 9, 12, 37, 22, 27, 29, 28, 22, 15, 9, 18, 24, 21, 20, 31, 25, 27, 25, 18, 31, 0 }; /*sv_lagflushbonecache*/
char *sv_lagpushticksstr = new char[16]{ 9, 12, 37, 22, 27, 29, 10, 15, 9, 18, 14, 19, 25, 17, 9, 0 }; /*sv_lagpushticks*/
char *sv_client_min_interp_ratio_str = new char[27]{ 9, 12, 37, 25, 22, 19, 31, 20, 14, 37, 23, 19, 20, 37, 19, 20, 14, 31, 8, 10, 37, 8, 27, 14, 19, 21, 0 }; /*sv_client_min_interp_ratio*/
char *sv_client_max_interp_ratio_str = new char[27]{ 9, 12, 37, 25, 22, 19, 31, 20, 14, 37, 23, 27, 2, 37, 19, 20, 14, 31, 8, 10, 37, 8, 27, 14, 19, 21, 0 }; /*sv_client_max_interp_ratio*/
char *sv_maxupdateratestr = new char[17]{ 9, 12, 37, 23, 27, 2, 15, 10, 30, 27, 14, 31, 8, 27, 14, 31, 0 }; /*sv_maxupdaterate*/
char *sv_minupdateratestr = new char[17]{ 9, 12, 37, 23, 19, 20, 15, 10, 30, 27, 14, 31, 8, 27, 14, 31, 0 }; /*sv_minupdaterate*/
char *cl_updateratestr = new char[14]{ 25, 22, 37, 15, 10, 30, 27, 14, 31, 8, 27, 14, 31, 0 }; /*cl_updaterate*/
char *cl_interp_ratiostr = new char[16]{ 25, 22, 37, 19, 20, 14, 31, 8, 10, 37, 8, 27, 14, 19, 21, 0 }; /*cl_interp_ratio*/
char *sv_gravitystr = new char[11]{ 9, 12, 37, 29, 8, 27, 12, 19, 14, 3, 0 }; /*sv_gravity*/

bool LagCompensationConvarsReceived = false;

bool GetLagCompensationConVars()
{
	if (LagCompensationConvarsReceived)
		return true;

	DecStr(sv_unlagstr);
	DecStr(sv_maxunlagstr);
	DecStr(sv_lagflushbonecachestr);
	DecStr(sv_lagpushticksstr);
	DecStr(sv_client_min_interp_ratio_str);
	DecStr(sv_client_max_interp_ratio_str);
	DecStr(sv_maxupdateratestr);
	DecStr(sv_minupdateratestr);
	DecStr(cl_updateratestr);
	DecStr(cl_interp_ratiostr);
	DecStr(sv_gravitystr);
	sv_unlag = Interfaces::Cvar->FindVar(sv_unlagstr);
	sv_maxunlag = Interfaces::Cvar->FindVar(sv_maxunlagstr);
	sv_lagflushbonecache = Interfaces::Cvar->FindVar(sv_lagflushbonecachestr);
	sv_lagpushticks = Interfaces::Cvar->FindVar(sv_lagpushticksstr);
	sv_client_min_interp_ratio = Interfaces::Cvar->FindVar(sv_client_min_interp_ratio_str);
	sv_client_max_interp_ratio = Interfaces::Cvar->FindVar(sv_client_max_interp_ratio_str);
	sv_maxupdaterate = Interfaces::Cvar->FindVar(sv_maxupdateratestr);
	sv_minupdaterate = Interfaces::Cvar->FindVar(sv_minupdateratestr);
	cl_updaterate = Interfaces::Cvar->FindVar(cl_updateratestr);
	cl_interp_ratio = Interfaces::Cvar->FindVar(cl_interp_ratiostr);
	sv_gravity = Interfaces::Cvar->FindVar(sv_gravitystr);
	EncStr(sv_unlagstr);
	EncStr(sv_maxunlagstr);
	EncStr(sv_lagflushbonecachestr);
	EncStr(sv_lagpushticksstr);
	EncStr(sv_client_min_interp_ratio_str);
	EncStr(sv_client_max_interp_ratio_str);
	EncStr(sv_maxupdateratestr);
	EncStr(sv_minupdateratestr);
	EncStr(cl_updateratestr);
	EncStr(cl_interp_ratiostr);
	EncStr(sv_gravitystr);
	if (cl_interp_ratio && sv_gravity)
	{
		LagCompensationConvarsReceived = true;
		return true;
	}
	return false;
}

void RestoreNetvars(StoredNetvars *src, CBaseEntity *Entity)
{
	if (src->origin != vecZero)
	{
		Entity->SetViewOffset(src->viewoffset);
		//Entity->SetVelocity(src->velocity);
		//Entity->SetVelocityModifier(src->velocitymodifier);
		//Entity->SetFallVelocity(src->fallvelocity);
		//Entity->SetFlags(src->flags);
		Entity->SetSimulationTime(src->simulationtime);
		//Entity->SetBaseVelocity(src->basevelocity);
		//Entity->SetCycle(src->cycle);
		Entity->SetMaxs(src->maxs);
		Entity->SetMins(src->mins);
		//Entity->SetSequence(src->sequence);
		//Entity->SetPlaybackRate(src->playbackrate);
		//Entity->SetPunch(src->aimpunch);
		//Entity->SetAngleRotation(src->angles);
		//Entity->SetLowerBodyYaw(src->lowerbodyyaw);
		//Entity->SetGroundEntity(src->groundentity); //could possibly crash if entity is gone
		Entity->SetEyeAngles(src->eyeangles);
		//Entity->SetVecLadderNormal(src->laddernormal);
		//Entity->SetDucked(src->isducked);
		//Entity->SetDucking(src->isducking);
		//Entity->SetInDuckJump(src->isinduckjump);
		//Entity->SetDuckTimeMsecs(src->ducktimemsecs);
		//Entity->SetJumpTimeMsecs(src->jumptimemsecs);
		Entity->SetDuckAmount(src->duckamount);
		Entity->SetDuckSpeed(src->duckspeed);
		//Entity->SetLaggedMovement(src->laggedmovement);
		//Entity->SetAnimTime(src->animtime);
		//Entity->SetNetworkOrigin(src->networkorigin);//(src->networkorigin);
		//Entity->SetOrigin(src->origin);// src->networkorigin);// src->origin);
		//Entity->SetAbsOrigin(src->origin);
#if 0
		float* flPose = (float*)((DWORD)Entity + m_flPoseParameter);
		for (int i = 0; i < 24; i++)
		{
			WriteFloat((uintptr_t)&flPose[i], src->flPoseParameter[i]);
		}
#endif
	}
}

int GetEstimatedServerTickCount(float latency)
{
	return (int)floorf((float)((float)(latency) / ReadFloat((uintptr_t)&Interfaces::Globals->interval_per_tick)) + 0.5) + 1 + ReadInt((uintptr_t)&Interfaces::Globals->tickcount);
}

float GetEstimatedServerTime()
{
	return LocalPlayer->GetTickBase() * ReadFloat((uintptr_t)&Interfaces::Globals->interval_per_tick);
}

float GetNetworkLatency()
{
	// Get true latency
	INetChannelInfo *nci = Interfaces::Engine->GetNetChannelInfo();
	if (nci)
	{
		float IncomingLatency = nci->GetAvgLatency(FLOW_INCOMING);
		//float OutgoingLatency = nci->GetAvgLatency(FLOW_OUTGOING);
		return IncomingLatency;// +OutgoingLatency;
	}
	return 0.0f;
}

//See if a tick is within limits to be accepted by the server for hit registration
bool bTickIsValid(int tick)
{
#define ALIENSWARM_VERSION
#ifdef ALIENSWARM_VERSION
	float latency = GetNetworkLatency();
	const float SV_MAXUNLAG = 1.0f;
	float m_flLerpTime = cl_interp_ratio->GetFloat() / cl_updaterate->GetFloat();
	if (RemoveInterpChk.Checked)
		tick += TIME_TO_TICKS(m_flLerpTime);
	float correct = clamp(latency + m_flLerpTime, 0.0f, SV_MAXUNLAG);
	float flTargetTime = TICKS_TO_TIME(tick) - m_flLerpTime;
	float deltaTime = correct - (GetEstimatedServerTime() - flTargetTime);
	if (fabs(deltaTime) > 0.2f)
	{
		return false;
	}
	return true;
#else
	//SDK 2013 VERSION
	float latency = GetNetworkLatency();
	const float SV_MAXUNLAG = 1.0f;
	float m_flLerpTime = cl_interp_ratio->GetFloat() / cl_updaterate->GetFloat();
	int lerpTicks = TIME_TO_TICKS(m_flLerpTime);
	if (RemoveInterpChk.Checked)
		tick += lerpTicks;
	float correct = clamp(latency + TICKS_TO_TIME(lerpTicks), 0.0f, SV_MAXUNLAG);
	int targettick = tick - lerpTicks;
	float deltaTime = correct - TICKS_TO_TIME(GetEstimatedServerTickCount(latency) - targettick);
	if (fabs(deltaTime) > 0.2f)
	{
		return false;
	}
	return true;
#endif
}

void FixPlayerZVelocity(CustomPlayer* pCPlayer, CBaseEntity* Entity, bool StoreBaseVel)
{
	return;

	Vector pVel = Entity->GetVelocity();
	Vector tmp = Entity->GetBaseVelocity();
	float tick_interval = ReadFloat((uintptr_t)&Interfaces::Globals->interval_per_tick);
	pVel.z -= (sv_gravity->GetFloat() * 0.5f * tick_interval);
	pVel.z += (tmp.x * tick_interval);

	Entity->SetVelocity(pVel);

	tmp.z = 0.f;
	Entity->SetBaseVelocity(tmp);
	if (StoreBaseVel)
		pCPlayer->CurrentNetVars.basevelocity = tmp;
}

extern void GetIdealYaw(float& yaw, int correctedindex, CustomPlayer* pCPlayer, CBaseEntity* Entity, CBaseEntity* pLocalEntity);
extern void GetIdealPitch(float& pitch, int correctedindex, CustomPlayer* pCPlayer, CBaseEntity* Entity, CBaseEntity* pLocalEntity);

//Update was received from the server, so update netvars
void BeginLagCompensation(CustomPlayer *pCPlayer, CBaseEntity* Entity)
{
	if (Entity->GetDormant() || Entity->GetHealth() <= 0)
	{
		pCPlayer->FireAtPelvis = false;
		pCPlayer->IsAimingAtUs = false;
		return;
	}

	bool bResolve = pCPlayer->Personalize.AutomaticPitch || pCPlayer->Personalize.AutomaticYaw;
	float curtime = ReadFloat((uintptr_t)&Interfaces::Globals->curtime);
	int tickcount = ReadInt((uintptr_t)&Interfaces::Globals->tickcount);
	float flCurrentBodyYaw = pCPlayer->CurrentNetVars.lowerbodyyaw;
	StoredNetvars *LastUpdatedNetVars = &pCPlayer->LastUpdatedNetVars;
	StoredNetvars *CurrentNetVars = &pCPlayer->CurrentNetVars;

	if (tickcount - pCPlayer->TickFiredBullet > 2)
	{
		pCPlayer->IsFiring = false;
	}

	//Find out how many ticks player is choking
	float flSimulationTime = pCPlayer->CurrentNetVars.simulationtime;
	float flSimDiff = curtime - flSimulationTime;
	float latency = GetNetworkLatency();
	pCPlayer->tickschoked = TIME_TO_TICKS(max(0.0f, flSimDiff - latency));

	if (pCPlayer->DataUpdated)
	{
		pCPlayer->m_flOldAnimTime = CurrentNetVars->animtime;
		CurrentNetVars->animtime = oGetLastTimeStamp();
		if (!pCPlayer->Backtracked)
		{
			CurrentNetVars->velocity = Entity->GetVelocity();
			FixPlayerZVelocity(pCPlayer, Entity, true);
		}
		pCPlayer->DataUpdated = false;
	}
	else
	{
		if (!pCPlayer->Backtracked)
		{
			FixPlayerZVelocity(pCPlayer, Entity, false);
		}
	}

	bool AimingAtUsThisTick = false;

	if (pCPlayer->EyeAnglesUpdated)
	{
		QAngle AngleToOurHead = CalcAngle(Entity->GetEyePosition(), LocalPlayer->GetEyePosition());
		float FOVfromourhead = GetFov(AngleToOurHead, pCPlayer->CurrentNetVars.eyeangles);
		if (FOVfromourhead < 5.0f || Entity->IsTargetingLocal())
		{
			pCPlayer->IsAimingAtUs = true;
			AimingAtUsThisTick = true;
		}
		else
		{
			pCPlayer->IsAimingAtUs = false;
		}
		pCPlayer->EyeAnglesUpdated = false;
	}

	if (Entity->GetVelocity().Length() > 1.0f)
	{
		LastUpdatedNetVars->tickcount = 0;
		pCPlayer->IsMoving = true;
	}
	else
	{
		pCPlayer->IsMoving = false;
	}

	int newtime = RemoveInterpChk.Checked ? TIME_TO_TICKS(flSimulationTime) : tickcount + TIME_TO_TICKS(flSimulationTime - pCPlayer->m_flOldAnimTime) + 1;

	if (pCPlayer->LowerBodyUpdated)
	{
		float lbd = flCurrentBodyYaw - pCPlayer->LastUpdatedNetVars.lowerbodyyaw;
		ClampY(lbd);

		if (lbd == 0.0f && newtime - pCPlayer->LastUpdatedNetVars.tickcount < 12) // ~200ms
		{
			pCPlayer->LowerBodyUpdated = false;
		}
		else
		{
			pCPlayer->LowerBodyDelta = lbd;
		}
	}

	if (pCPlayer->LowerBodyUpdated || AimingAtUsThisTick)
	{
		*LastUpdatedNetVars = *CurrentNetVars;

		if (flCurrentBodyYaw < 0)
			flCurrentBodyYaw += 360.0f;

		if (!AimingAtUsThisTick)
		{
			pCPlayer->LastUpdatedNetVars.eyeangles.y = flCurrentBodyYaw;// -35.0f;
		}

		if (!pCPlayer->IsMoving)
		{
			LastUpdatedNetVars->tickcount = newtime;
		}
		pCPlayer->LowerBodyUpdated = false;
		pCPlayer->FireAtPelvis = false;
	}
	else
	{
		if (!bResolve)
		{
			pCPlayer->FireAtPelvis = false;
		}
		else
		{
			if (AutoPelvisChk.Checked && !bTickIsValid(LastUpdatedNetVars->tickcount))
			{
				if ((GetAsyncKeyState(VK_LBUTTON) & (1 << 16)))
				{
					//Force fire at best bone when manual firing
					pCPlayer->FireAtPelvis = false;
				}
				else
				{
					if (!(GetAsyncKeyState(VK_CAPITAL) & (1 << 16)))
						//Let auto fire shoot at pelvis
						pCPlayer->FireAtPelvis = true;
					else
						pCPlayer->FireAtPelvis = false;
				}
			}
			else
			{
				pCPlayer->FireAtPelvis = false;
			}
		}
	}
}

//Restores player back to most recent server update if possible
bool BacktrackPlayer(CustomPlayer *pCPlayer, CBaseEntity* pLocalEntity, CBaseEntity* Entity)
{
	if (pCPlayer->Dormant || !Entity->GetAlive() || (!pCPlayer->Personalize.ResolvePitch && !pCPlayer->Personalize.ResolveYaw))
	{
		if (pCPlayer->Backtracked)
		{
			RestoreNetvars(&pCPlayer->CurrentNetVars, Entity);
			pCPlayer->Backtracked = false;
		}
	}
	else
	{
		StoredNetvars *LastUpdatedNetVars = &pCPlayer->LastUpdatedNetVars;
		StoredNetvars *CurrentNetVars = &pCPlayer->CurrentNetVars;
		int tickcount = ReadInt((uintptr_t)&Interfaces::Globals->tickcount);

		bool TickIsValid = bTickIsValid(LastUpdatedNetVars->tickcount);

		//If we have new enough data to backtrack to, then force the player back in time as long as we can.

		if (TickIsValid)// || IsUsingFakeAngles)
		{
			//Restore player's netvars to the last values received from the server
			RestoreNetvars(&pCPlayer->LastUpdatedNetVars, Entity);

			//If player forces lower body or they are using fake angles, then force the eye yaw to the lower body
			QAngle neweyeangles = LastUpdatedNetVars->eyeangles;

			Personalize *pPersonalize = &pCPlayer->Personalize;

			pPersonalize->DontStatsThisShot = true;

			int correctedindex = pPersonalize->isCorrected ? pPersonalize->correctedindex : pPersonalize->miss;
			GetIdealPitch(neweyeangles.x, correctedindex, pCPlayer, Entity, pLocalEntity);
			//neweyeangles.x = fmodf(neweyeangles.x + 180.0f, 360.0f);
			//	}
			Entity->SetEyeAngles(neweyeangles);
			//Let other functions know that the player is backtracked

			pCPlayer->Backtracked = true;

			//Successfully restored player to old position
			return true;
		}
		else if (pCPlayer->Backtracked)
		{
			RestoreNetvars(&pCPlayer->CurrentNetVars, Entity);
		}
		pCPlayer->Backtracked = false;
	}
	pCPlayer->IsAimingAtUs = false;
	pCPlayer->IsFiring = false;
	return false;
}

//Called on frame end to restore players back to current netvars
void FinishLagCompensation(CBaseEntity *pLocalEntity)
{
	//Do we need this anymore now that we restore on new frame already?
	for (int i = 0; i < MAX_PLAYERS; i++)
	{
		CBaseEntity* Entity = Interfaces::ClientEntList->GetClientEntity(i);
		if (Entity && Entity != pLocalEntity && Entity->IsPlayer())
		{
			CustomPlayer* pCPlayer = &AllPlayers[ReadInt((uintptr_t)&Entity->index)];
#ifdef _DEBUG
			if (RunningFrame && pCPlayer->Personalize.ResolvePitch || pCPlayer->Personalize.ResolveYaw)
			{
				Vector screen;
				if (WorldToScreenCapped(Entity->GetBonePosition(HITBOX_CHEST), screen))
				{
					char test[32];
					sprintf(test, "Lower Body Delta: %.0f", pCPlayer->LowerBodyDelta);
					DrawStringUnencrypted(test, Vector2D(screen.x - 200, screen.y - 25), ColorRGBA(200, 200, 200, 255), pFont);
				}
			}
#endif
		}
	}
}

//Called on new frame to backtrack player if possible
void BacktrackPlayersOnNewFrame(CBaseEntity *pLocalEntity)
{
	for (int i = 0; i < MAX_PLAYERS; i++)
	{
		CBaseEntity* Entity = Interfaces::ClientEntList->GetClientEntity(i);
		if (Entity && Entity != pLocalEntity && Entity->IsPlayer())
		{
			CustomPlayer* pCPlayer = &AllPlayers[ReadInt((uintptr_t)&Entity->index)];
			if (!BacktrackPlayer(pCPlayer, pLocalEntity, Entity))
			{
				Personalize *pPersonalize = &pCPlayer->Personalize;
				QAngle neweyeangles = Entity->GetEyeAngles();
				int correctedindex = pPersonalize->isCorrected ? pPersonalize->correctedindex : pPersonalize->miss;
				GetIdealPitch(neweyeangles.x, correctedindex, pCPlayer, Entity, pLocalEntity);
				GetIdealYaw(neweyeangles.y, correctedindex, pCPlayer, Entity, pLocalEntity);
				Entity->SetEyeAngles(neweyeangles);
			}
		}
	}
}

void AdjustTickCountForCmd(CBaseEntity *pPlayerFiredAt, bool bLocalPlayerIsFiring)
{
	int lerpTicks = RemoveInterpChk.Checked ? TIME_TO_TICKS(cl_interp_ratio->GetFloat() / cl_updaterate->GetFloat()) : 0;
	//pPlayerFiredAt is the player we are aimbotting or triggerbotting and we are definitely firing a bullet this tick
	if (pPlayerFiredAt)
	{
		CustomPlayer *pCPlayer = &AllPlayers[ReadInt((uintptr_t)&pPlayerFiredAt->index)];
		int tickcount = ReadInt((uintptr_t)&Interfaces::Globals->tickcount);
		if (pCPlayer->Backtracked)
		{
			WriteInt((uintptr_t)&CurrentUserCmd->tick_count, pCPlayer->LastUpdatedNetVars.tickcount + lerpTicks);
		}
		else
		{
			int cmdtick = ReadInt((uintptr_t)&CurrentUserCmd->tick_count);
			int targettick = RemoveInterpChk.Checked ? TIME_TO_TICKS(pCPlayer->CurrentNetVars.simulationtime) : cmdtick + TIME_TO_TICKS(pCPlayer->CurrentNetVars.simulationtime - pCPlayer->m_flOldAnimTime) + 1;
			if (!bTickIsValid(targettick))
			{
				if (RemoveInterpChk.Checked)
				{
					targettick = cmdtick + TIME_TO_TICKS(pCPlayer->CurrentNetVars.simulationtime - pCPlayer->m_flOldAnimTime) + 1;
					if (!bTickIsValid(targettick))
					{
						targettick = cmdtick;
					}
				}
				else
				{
					targettick = cmdtick;
				}
			}
			WriteInt((uintptr_t)&CurrentUserCmd->tick_count, targettick + lerpTicks);
		}
	}
	else
	{
		if (bLocalPlayerIsFiring)
		{
			Vector vecDir;
			Vector EyePos = LocalPlayer->GetEyePosition();
			AngleVectors(LocalPlayer->GetEyeAngles(), &vecDir);
			VectorNormalizeFast(vecDir);
			Vector EndPos = EyePos + (vecDir * 8192.0f);
			trace_t tr;
			UTIL_TraceLine(EyePos, EndPos, MASK_SHOT, LocalPlayer, &tr);
			CTraceFilterPlayersOnlyNoWorld filter;
			filter.pSkip = (IHandleEntity*)LocalPlayer;
			filter.m_icollisionGroup = COLLISION_GROUP_NONE;
			UTIL_ClipTraceToPlayers(EyePos, EndPos + vecDir * 40.0f, 0x4600400B, (ITraceFilter*)&filter, &tr);
			if (tr.m_pEnt && tr.m_pEnt->IsPlayer())
			{
				int index = ReadInt((uintptr_t)&tr.m_pEnt->index);
				if (index <= MAX_PLAYERS)
				{
					CustomPlayer* pCPlayer = &AllPlayers[index];
					int cmdtick = ReadInt((uintptr_t)&CurrentUserCmd->tick_count);
					int targettick = RemoveInterpChk.Checked ? TIME_TO_TICKS(pCPlayer->CurrentNetVars.simulationtime) : cmdtick + TIME_TO_TICKS(pCPlayer->CurrentNetVars.simulationtime - pCPlayer->m_flOldAnimTime) + 1;
					if (!bTickIsValid(targettick))
					{
						if (RemoveInterpChk.Checked)
						{
							targettick = TIME_TO_TICKS(pCPlayer->CurrentNetVars.simulationtime);
							if (!bTickIsValid(targettick))
							{
								targettick = cmdtick;
							}
						}
						else
						{
							targettick = cmdtick;
						}
					}
					WriteInt((uintptr_t)&CurrentUserCmd->tick_count, targettick + lerpTicks);
				}
			}
		}
	}
}