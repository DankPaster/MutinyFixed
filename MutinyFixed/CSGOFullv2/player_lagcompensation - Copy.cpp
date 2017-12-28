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
	if (cl_interp_ratio)
	{
		LagCompensationConvarsReceived = true;
		return true;
	}
	return false;
}

bool SaveNetvars(StoredNetvars *dest, CBaseEntity *Entity, CustomPlayer *pCPlayer, float CurrentLowerBodyYaw, float CurrentSimulationTime, Vector origin)
{

	if (origin == Vector(0, 0, 0) && CurrentSimulationTime == 0)
		return false;

	dest->aimpunch = Entity->GetPunch();
	dest->angles = Entity->GetAngleRotation();
	dest->mins = Entity->GetMins();
	dest->maxs = Entity->GetMaxs();
	dest->lowerbodyyaw = CurrentLowerBodyYaw; // Entity->GetLowerBodyYaw();
	dest->fallvelocity = Entity->GetFallVelocity();
	dest->origin = origin; //Entity->GetOrigin();
	dest->networkorigin = Entity->GetNetworkOrigin();
	dest->viewoffset = Entity->GetViewOffset();
	dest->velocity = Entity->GetVelocity();
	dest->velocitymodifier = Entity->GetVelocityModifier();
	dest->flags = Entity->GetFlags();
	//dest->headorigin = Entity->GetBonePosition(HITBOX_HEAD);
	dest->cycle = Entity->GetCycle();
	dest->basevelocity = Entity->GetBaseVelocity();
	dest->playbackrate = Entity->GetPlaybackRate();
	dest->sequence = Entity->GetSequence();
	dest->simulationtime = CurrentSimulationTime;
	dest->groundentity = Entity->GetGroundEntity();
	dest->eyeangles = pCPlayer->EyeAngles;
	dest->eyeyaw = pCPlayer->EyeAngles.y;
	dest->isducked = Entity->IsDucked();
	dest->isducking = Entity->IsDucking();
	dest->isinduckjump = Entity->IsInDuckJump();
	dest->ducktimemsecs = Entity->GetDuckTimeMsecs();
	dest->jumptimemsecs = Entity->GetJumpTimeMsecs();
	dest->duckamount = Entity->GetDuckAmount();
	dest->duckspeed = Entity->GetDuckSpeed();
	dest->laggedmovement = Entity->GetLaggedMovement();
	dest->laddernormal = Entity->GetVecLadderNormal();
	dest->animtime = Entity->GetAnimTime();
	float* flPose = (float*)((DWORD)Entity + m_flPoseParameter);
	for (int i = 0; i < 24; i++)
	{
		dest->flPoseParameter[i] = ReadFloat((uintptr_t)&flPose[i]);
	}

	return true;
}

void RestoreNetvars(StoredNetvars *src, CBaseEntity *Entity)
{
	if (src->origin != vecZero)
	{
		Entity->SetViewOffset(src->viewoffset);
		Entity->SetVelocity(src->velocity);
		Entity->SetVelocityModifier(src->velocitymodifier);
		Entity->SetFallVelocity(src->fallvelocity);
		//Entity->SetFlags(src->flags);
		Entity->SetSimulationTime(src->simulationtime); //DO NOT DO THIS
		Entity->SetBaseVelocity(src->basevelocity);
		Entity->SetCycle(src->cycle);
		Entity->SetMaxs(src->maxs);
		Entity->SetMins(src->mins);
		//Entity->SetOrigin(src->origin);// src->networkorigin);// src->origin);
		//Entity->SetNetworkOrigin(src->origin);//(src->networkorigin);
		Entity->SetSequence(src->sequence);
		Entity->SetPlaybackRate(src->playbackrate);
		Entity->SetPunch(src->aimpunch);
		//Entity->SetAngleRotation(src->angles);
		//Entity->SetLowerBodyYaw(src->lowerbodyyaw);
		//Entity->SetGroundEntity(src->groundentity); //could possibly crash if entity is gone
		Entity->SetEyeAngles(src->eyeangles);
		Entity->SetVecLadderNormal(src->laddernormal);
		Entity->SetDucked(src->isducked);
		Entity->SetDucking(src->isducking);
		Entity->SetInDuckJump(src->isinduckjump);
		Entity->SetDuckTimeMsecs(src->ducktimemsecs);
		Entity->SetJumpTimeMsecs(src->jumptimemsecs);
		Entity->SetDuckAmount(src->duckamount);
		Entity->SetDuckSpeed(src->duckspeed);
		Entity->SetLaggedMovement(src->laggedmovement);
		Entity->SetAnimTime(src->animtime);

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

float inline GetEstimatedServerTime()
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

extern void GetIdealYaw(float& yaw, int correctedindex, CustomPlayer* pCPlayer, CBaseEntity* Entity, CBaseEntity* pLocalEntity);
extern void GetIdealPitch(float& pitch, int correctedindex, CustomPlayer* pCPlayer, CBaseEntity* Entity, CBaseEntity* pLocalEntity);

//Update was received from the server, so update netvars
void BeginLagCompensation(CustomPlayer *pCPlayer, CBaseEntity* Entity)
{
	if (!pCPlayer->Dormant && pCPlayer->Health > 0)
	{
		bool bResolve = pCPlayer->Personalize.AutomaticPitch || pCPlayer->Personalize.AutomaticYaw;
		pCPlayer->IsAimingAtUs = Entity->IsTargetingLocal();
		float curtime = ReadFloat((uintptr_t)&Interfaces::Globals->curtime);
		int tickcount = ReadInt((uintptr_t)&Interfaces::Globals->tickcount);
		float flCurrentBodyYaw = pCPlayer->CurrentLowerBodyYaw;
		StoredNetvars *LastUpdatedNetVars = &pCPlayer->LastUpdatedNetVars;
		StoredNetvars *CurrentNetVars = &pCPlayer->CurrentNetVars;

		if (tickcount - pCPlayer->TickFiredBullet > 2)
		{
			pCPlayer->IsFiring = false;
		}

		Vector origin = Entity->GetOrigin();

		if (origin != vecZero)
		{
			//Find out how many ticks player is choking
			float flSimulationTime = pCPlayer->CurrentSimulationTime;//Entity->GetSimulationTime();
			float flSimDiff = curtime - flSimulationTime;
			float latency = GetNetworkLatency();
			pCPlayer->tickschoked = TIME_TO_TICKS(max(0.0f, flSimDiff - latency));

#if 0
#ifdef _DEBUG
			if (GetAsyncKeyState(VK_F10))
			{
				char tmp[128];
				sprintf(tmp, "Lower Body Yaw Address: %02X", (DWORD)((DWORD)Entity + m_flLowerBodyYawTarget));
				MessageBoxA(NULL, tmp, "TEST", MB_OK);
			}
#endif
#endif
			//bool PlayerIsInAir = !(Entity->GetFlags() & FL_ONGROUND || Entity->GetGroundEntity() == nullptr);

			//Save current netvars received from the server


			//if (pCPlayer->tickschoked)
			//	return;

			bool bLowerBodyUpdated = pCPlayer->LowerBodyUpdated || (LastUpdatedNetVars->lowerbodyyaw != flCurrentBodyYaw);

			if (!bLowerBodyUpdated && pCPlayer->CurrentSimulationTime == CurrentNetVars->simulationtime && pCPlayer->CurrentOrigin == CurrentNetVars->origin)
				return;

			Vector oldvelocity = CurrentNetVars->velocity;

			if (!SaveNetvars(CurrentNetVars, Entity, pCPlayer, flCurrentBodyYaw, flSimulationTime, pCPlayer->CurrentOrigin))
				return;

			//CurrentNetVars->velocity = pCPlayer->CurrentVelocity;

			if (pCPlayer->Backtracked)
			{
				CurrentNetVars->velocity = oldvelocity;
			}

			bool PlayerIsMoving = (CurrentNetVars->velocity.Length() > 1.0f); // || !(Entity->GetFlags() & FL_ONGROUND); //<= absolutely shit, makes it impossible to hit while they bhop with fakeangles

			if (bLowerBodyUpdated)
			{
				*LastUpdatedNetVars = *CurrentNetVars;
				LastUpdatedNetVars->eyeangles.y = flCurrentBodyYaw;
				LastUpdatedNetVars->tickcount = TIME_TO_TICKS(flSimulationTime); // TIME_TO_TICKS(flSimulationTime);
				pCPlayer->LowerBodyUpdated = false;
				pCPlayer->FireAtPelvis = false;
			}
			else
			{
				if (bResolve)
				{
					if (!bTickIsValid(LastUpdatedNetVars->tickcount))
					{
						if (AutoPelvisChk.Checked)
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
					else
					{
						pCPlayer->FireAtPelvis = false;
					}
				}
				else
				{
					pCPlayer->FireAtPelvis = false;
				}
			}



#if 0

#if 0
			int pose = LookupPoseParameter(Entity, "lean");
			float flmin, flmax;
			GetPoseParameterRange(Entity, pose, flmin, flmax);

			float flValue = Entity->GetPoseParameter(pose);

			flValue = flValue * (flmax - flmin) + flmin;

			char tmp[128];
			sprintf(tmp, "%.5f", flValue);
			DrawStringUnencrypted(tmp, Vector2D(1024, 768), ColorRGBA(255, 0, 0, 255), pFont);
#endif

			bool bLowerBodyUpdated = LastUpdatedNetVars->lowerbodyyaw != pCPlayer->CurrentLowerBodyYaw;

			float bodyeyedelta = pCPlayer->EyeAngles.y - flCurrentBodyYaw;
			NormalizeAngle(bodyeyedelta);
			bodyeyedelta = fabsf(bodyeyedelta);


			if (bLowerBodyUpdated)
			{
				*LastUpdatedNetVars = *CurrentNetVars;
				LastUpdatedNetVars->eyeangles.y = flCurrentBodyYaw;
				if (!PlayerIsMoving)
				{
					LastUpdatedNetVars->eyeangles.y -= 35.0f;
					ClampY(LastUpdatedNetVars->eyeangles.y);
				}
				LastUpdatedNetVars->tickcount = tickcount;
			}
			else
			{
				if (PlayerIsMoving)
				{
					if (!bTickIsValid(LastUpdatedNetVars->tickcount))
					{
						if (bodyeyedelta < 35.0f)
						{
							*LastUpdatedNetVars = *CurrentNetVars;
							//LastUpdatedNetVars->eyeangles.y = flCurrentBodyYaw;
							LastUpdatedNetVars->tickcount = tickcount;
						}
					}
				}
			}

#endif
#if 0
			if (LastUpdatedNetVars->lowerbodyyaw != flCurrentBodyYaw)
			{
				bLowerBodyUpdated = true;
				pCPlayer->TimeLowerBodyUpdated = curtime;
			}

			//float flYawFromPose = CurrentNetVars->flPoseParameter[2] * 360.f - CurrentNetVars->flPoseParameter[11] * 360.f;

			float bodyeyedelta = pCPlayer->EyeAngles.y - flCurrentBodyYaw;
			NormalizeAngle(bodyeyedelta);
			bodyeyedelta = fabsf(bodyeyedelta);

			if (bResolve && GetAsyncKeyState(VK_CAPITAL))
			{
				//Force eye angles to lower body yaw..
				Entity->SetEyeAngles(QAngle(pCPlayer->EyeAngles.x, flCurrentBodyYaw, 0.0f));
			}

			if (bLowerBodyUpdated || pCPlayer->IsAimingAtUs || pCPlayer->IsFiring )// || LastUpdatedNetVars->eyeangles.x != CurrentNetVars->eyeangles.x || LastUpdatedNetVars->eyeyaw != CurrentNetVars->eyeangles.y)
			{
				//static float lastbodyyaw = 0.0f;
				static QAngle prevangles = { 0.0f, 0.0f, 0.0f };
				if (!pCPlayer->IsFiring)
				{
					//lastbodyyaw = LastUpdatedNetVars->lowerbodyyaw;
					prevangles = LastUpdatedNetVars->eyeangles;
				}

				*LastUpdatedNetVars = *CurrentNetVars;

				if (pCPlayer->IsFiring && !bLowerBodyUpdated)
				{
					//LastUpdatedNetVars->eyeangles.x = prevangles.x;
					LastUpdatedNetVars->eyeangles = prevangles;
					//LastUpdatedNetVars->eyeangles.y = lastbodyyaw;
					LastUpdatedNetVars->tickcount = TIME_TO_TICKS(flSimulationTime);// pCPlayer->tickschoked ? TIME_TO_TICKS(flSimulationTime) : tickcount;
				}
				else if (bLowerBodyUpdated && !pCPlayer->IsAimingAtUs && !pCPlayer->IsFiring)
				{
					LastUpdatedNetVars->eyeangles.y = flCurrentBodyYaw;
					if (!PlayerIsMoving)
					{
						LastUpdatedNetVars->eyeangles.y -= 35.0f; //Fixes weird spins on evolve cheat
					}
					ClampY(LastUpdatedNetVars->eyeangles.y);
					LastUpdatedNetVars->tickcount = TIME_TO_TICKS(flSimulationTime);// pCPlayer->tickschoked ? TIME_TO_TICKS(flSimulationTime) : tickcount;
				}

				pCPlayer->FireAtPelvis = false;
				pCPlayer->IsUsingFakeAngles = false;
			}
			else
			{
				if (!bResolve)
				{
					pCPlayer->FireAtPelvis = false;
					pCPlayer->IsUsingFakeAngles = false;
				}
				else
				{
					LastUpdatedNetVars->eyeangles.x = pCPlayer->EyeAngles.x;
					if (PlayerIsInAir || PlayerIsMoving)
					{
						if (bodyeyedelta >= 35.0f)
						{
							LastUpdatedNetVars->eyeangles.y = pCPlayer->CurrentLowerBodyYaw;
							pCPlayer->IsUsingFakeAngles = true;
						}
					}
					else
					{
						//See if player is faking their angles
						float TimeSinceUpdate = curtime - pCPlayer->TimeLowerBodyUpdated;
						if (TimeSinceUpdate > 0.5f)
						{
							//LastUpdatedNetVars->tickcount = pCPlayer->tickschoked ? TIME_TO_TICKS(flSimulationTime) : tickcount;
							//pCPlayer->TimeLowerBodyUpdated = curtime;
							if (bodyeyedelta >= 35.0f)
							{
								//Force an eye angle update
								LastUpdatedNetVars->eyeangles.y = LastUpdatedNetVars->lowerbodyyaw;
								pCPlayer->IsUsingFakeAngles = true;
							}

							if (AutoPelvisChk.Checked && !bTickIsValid(LastUpdatedNetVars->tickcount))
							{
								if (GetAsyncKeyState(VK_LBUTTON))
								{
									//Force fire at best bone when manual firing
									pCPlayer->FireAtPelvis = false;
								}
								else
								{
									//Let auto fire shoot at pelvis
									pCPlayer->FireAtPelvis = true;
								}
							}
						}
						else
						{
							pCPlayer->FireAtPelvis = false;
							pCPlayer->IsUsingFakeAngles = false;
						}
					}
				}
			}
#endif
		}
	}
}

//Restores player back to most recent server update if possible
bool BacktrackPlayer(CustomPlayer *pCPlayer, CBaseEntity* pLocalEntity, CBaseEntity* Entity)
{
	if (!pCPlayer->Dormant && Entity->GetAlive())
	{
		StoredNetvars *LastUpdatedNetVars = &pCPlayer->LastUpdatedNetVars;
		StoredNetvars *CurrentNetVars = &pCPlayer->CurrentNetVars;
		int tickcount = ReadInt((uintptr_t)&Interfaces::Globals->tickcount);		

		bool IsUsingFakeAngles = (GetAsyncKeyState(VK_CAPITAL) & (1 << 16)) || pCPlayer->IsUsingFakeAngles;
		bool TickIsValid = bTickIsValid(LastUpdatedNetVars->tickcount);

		//If we have new enough data to backtrack to, then force the player back in time as long as we can.

		if (IsUsingFakeAngles || TickIsValid)
		{
			Entity->InvalidateBoneCache();
			if (TickIsValid)
			{
				//Restore player's netvars to the last values received from the server
				RestoreNetvars(&pCPlayer->LastUpdatedNetVars, Entity);
			}
			else
			{
				//Restore to current netvars
				RestoreNetvars(&pCPlayer->CurrentNetVars, Entity);
			}

			//If player forces lower body or they are using fake angles, then force the eye yaw to the lower body
			QAngle neweyeangles = LastUpdatedNetVars->eyeangles;
			if ((IsUsingFakeAngles && !pCPlayer->IsFiring && !pCPlayer->IsAimingAtUs))
			{
				neweyeangles.y = LastUpdatedNetVars->lowerbodyyaw;
			}

			Personalize *pPersonalize = &pCPlayer->Personalize;
			//Allow player to correct their pitch, or let automatic pitch do its thing
			int correctedindex = pPersonalize->isCorrected ? pPersonalize->correctedindex : pPersonalize->miss;
			if (!pCPlayer->IsFiring && !pCPlayer->IsAimingAtUs)
			{
				GetIdealPitch(neweyeangles.x, correctedindex, pCPlayer, Entity, pLocalEntity);
				ClampX(neweyeangles.x);
			}
			Entity->SetEyeAngles(neweyeangles);
			//Let other functions know that the player is backtracked
			//if (TickIsValid || !IsUsingFakeAngles)
			{
				pCPlayer->Backtracked = true;
			}
			//Successfully restored player to old position
			Entity->SetupBones(NULL, MAXSTUDIOBONES, BONE_USED_BY_HITBOX, ReadFloat((uintptr_t)&Interfaces::Globals->curtime));
			return true;
		}
		//Check to see if we are backtracked, if we are then restore back to current netvars
		if (pCPlayer->Backtracked)
		{
			//if (!IsUsingFakeAngles)
			Entity->InvalidateBoneCache();
			RestoreNetvars(&pCPlayer->CurrentNetVars, Entity);
			Entity->SetupBones(NULL, MAXSTUDIOBONES, BONE_USED_BY_HITBOX, ReadFloat((uintptr_t)&Interfaces::Globals->curtime));
			pCPlayer->Backtracked = false;

#if 0
			Vector pVel = Entity->GetVelocity();

			pVel[2] -= (Interfaces::Cvar->FindVar("sv_gravity")->GetFloat() * 0.5f * Interfaces::Globals->interval_per_tick);
			pVel[2] += (Entity->GetBaseVelocity()[2] * Interfaces::Globals->interval_per_tick);

			Entity->SetVelocity(pVel);

			Vector tmp = Entity->GetBaseVelocity();
			tmp[2] = 0.f;
			Entity->SetBaseVelocity(tmp);
#endif
		}
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
			if (pCPlayer->Backtracked)
			{
				Entity->InvalidateBoneCache();
				//FIXME: THIS BREAKS DOWN FAKE STATIC SOMETIMES FROM EVOLVE
				RestoreNetvars(&pCPlayer->CurrentNetVars, Entity);
				Entity->SetupBones(NULL, MAXSTUDIOBONES, BONE_USED_BY_HITBOX, ReadFloat((uintptr_t)&Interfaces::Globals->curtime));
			}
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
			if (!pCPlayer->Dormant && Entity->GetAlive() && pCPlayer->Personalize.ResolvePitch || pCPlayer->Personalize.ResolveYaw)
			{
				if (!BacktrackPlayer(pCPlayer, pLocalEntity, Entity))
				{
					Entity->InvalidateBoneCache();
					Personalize *pPersonalize = &pCPlayer->Personalize;
					QAngle neweyeangles = Entity->GetEyeAngles();
					int correctedindex = pPersonalize->isCorrected ? pPersonalize->correctedindex : pPersonalize->miss;
					GetIdealPitch(neweyeangles.x, correctedindex, pCPlayer, Entity, pLocalEntity);
					GetIdealYaw(neweyeangles.y, correctedindex, pCPlayer, Entity, pLocalEntity);
					neweyeangles.Clamp();
					Entity->SetEyeAngles(neweyeangles);
					Entity->SetupBones(NULL, MAXSTUDIOBONES, BONE_USED_BY_HITBOX, ReadFloat((uintptr_t)&Interfaces::Globals->curtime));
#if 0
					StoredNetvars *LastUpdatedNetVars = &pCPlayer->LastUpdatedNetVars;
					StoredNetvars *CurrentNetVars = &pCPlayer->CurrentNetVars;
					//If data updated since last lower body yaw, check to see if it's been long enough to force an update
					//if (LastUpdatedNetVars->eyeangles.x != CurrentNetVars->eyeangles.x || LastUpdatedNetVars->eyeyaw != CurrentNetVars->eyeangles.y)
					{
						int tickcount = ReadInt((uintptr_t)&Interfaces::Globals->tickcount);
						if (/*LastUpdatedNetVars->eyeangles.x == CurrentNetVars->eyeangles.x && LastUpdatedNetVars->eyeyaw == CurrentNetVars->eyeangles.y || */tickcount - LastUpdatedNetVars->tickcount > TIME_TO_TICKS(5.0f))
						{
							//Too long since lower body yaw update, force an update
							pCPlayer->LastUpdatedNetVars = pCPlayer->CurrentNetVars;
							LastUpdatedNetVars->tickcount = tickcount;
						}
					}
#endif
				}
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
			int targettick = TIME_TO_TICKS(pPlayerFiredAt->GetSimulationTime());
			//if (!bTickIsValid(targettick))
			//	targettick = cmd->tick_count;
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
			Vector EndPos = EyePos + (vecDir * 8192);
			trace_t tr;
			UTIL_TraceLine(EyePos, EndPos, MASK_SHOT, LocalPlayer, &tr);
			CTraceFilterPlayersOnlyNoWorld filter;
			filter.pSkip = (IHandleEntity*)LocalPlayer;
			filter.m_icollisionGroup = COLLISION_GROUP_NONE;
			UTIL_ClipTraceToPlayers(EyePos, EndPos + vecDir * 40.0f, 0x4600400B, &filter, &tr);

			WriteInt((uintptr_t)&CurrentUserCmd->tick_count, tr.m_pEnt ? TIME_TO_TICKS(tr.m_pEnt->GetSimulationTime()) + lerpTicks : ReadInt((uintptr_t)&CurrentUserCmd->tick_count) + lerpTicks);
		}
	}
}