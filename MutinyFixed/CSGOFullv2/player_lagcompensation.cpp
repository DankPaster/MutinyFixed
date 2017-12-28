#include "player_lagcompensation.h"
#include "Interfaces.h"
#include "CreateMove.h"
#include "ConVar.h"
#include "utlvectorsimple.h"
#include "INetchannelInfo.h"
#include <math.h>
#include "BaseCombatWeapon.h"
#include "Aimbot.h"
#include "Triggerbot.h"
#include "HitChance.h"
#include "Animation.h"
#include <fstream>
#include "LocalPlayer.h"
#include "ClientSideAnimationList.h"
#include "VMProtectDefs.h"
#include "Intersection.h"
#include "Netchan.h"

extern CMoveHelperServer* pMoveHelperServer;

LagCompensation gLagCompensation;
#define acceleration
#define predictTicks
//Haven't test with test data
//should work tho

//TODO: MOVE ME TO PROPER PLACE
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

//Gets various convars used for lag compensation
bool LagCompensation::GetLagCompensationConVars()
{
	if (LagCompensationConvarsReceived)
		return true;

	DecStr(sv_unlagstr, 8);
	DecStr(sv_maxunlagstr, 11);
	DecStr(sv_lagflushbonecachestr, 20);
	DecStr(sv_lagpushticksstr, 15);
	DecStr(sv_client_min_interp_ratio_str, 26);
	DecStr(sv_client_max_interp_ratio_str, 26);
	DecStr(sv_maxupdateratestr, 16);
	DecStr(sv_minupdateratestr, 16);
	DecStr(cl_updateratestr, 13);
	DecStr(cl_interp_ratiostr, 15);
	DecStr(sv_gravitystr, 10);
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
	EncStr(sv_unlagstr, 8);
	EncStr(sv_maxunlagstr, 11);
	EncStr(sv_lagflushbonecachestr, 20);
	EncStr(sv_lagpushticksstr, 15);
	EncStr(sv_client_min_interp_ratio_str, 26);
	EncStr(sv_client_max_interp_ratio_str, 26);
	EncStr(sv_maxupdateratestr, 16);
	EncStr(sv_minupdateratestr, 16);
	EncStr(cl_updateratestr, 13);
	EncStr(cl_interp_ratiostr, 15);
	EncStr(sv_gravitystr, 10);
	if (cl_interp_ratio && sv_gravity)
	{
		LagCompensationConvarsReceived = true;
		return true;
	}
	return false;
}

//Called from FrameStageNotify to set a player's netvars
void LagCompensation::FSN_RestoreNetvars(StoredNetvars *src, CBaseEntity *const Entity, QAngle EyeAngles, float LowerBodyYaw)
{
	Entity->SetMaxs(src->maxs);
	Entity->SetMins(src->mins);
	Entity->SetDuckAmount(src->duckamount);
	Entity->SetDuckSpeed(src->duckspeed);
	Entity->SetEyeAngles(EyeAngles);
	*Entity->EyeAngles() = EyeAngles;
	Entity->SetLowerBodyYaw(LowerBodyYaw);
}

//Restores various netvars and animations for a specific entity
void LagCompensation::CM_RestoreNetvars(StoredNetvars *const src, CBaseEntity *const Entity, Vector *const Origin)
{
#ifdef PRINT_FUNCTION_NAMES
	AllocateConsole();
	printf("CM_RestoreNetvars\n");
#endif
	//Restore netvars
	if (src->ResolvedFromBloodSpawning)
	{
		Entity->SetPoseParameterScaled(11, src->flPoseParameter[11]);
		Entity->SetAbsAngles(src->absangles);
	}
	//Entity->SetAngleRotation(src->angles); //FIXME: get this in render start?
	//Entity->SetViewOffset(src->viewoffset);
	//Entity->SetVelocity(src->velocity);
	//Entity->SetFallVelocity(src->fallvelocity);
	Entity->SetFlags(src->flags);
	//Entity->SetSimulationTime(src->simulationtime);
	//Entity->SetBaseVelocity(src->basevelocity);
	Entity->SetMaxs(src->maxs);
	Entity->SetMins(src->mins);
	Entity->SetLowerBodyYaw(src->lowerbodyyaw); //fixme?
	Entity->SetDuckAmount(src->duckamount);
	Entity->SetDuckSpeed(src->duckspeed);
	//Entity->SetVelocityModifier(src->velocitymodifier);
	//Entity->SetCycle(src->cycle);
	//Entity->SetSequence(src->sequence);
	//Entity->SetPlaybackRate(src->playbackrate);
	//Entity->SetPunch(src->aimpunch);
	//Entity->SetAngleRotation(src->angles);
	//Entity->SetGroundEntity(src->groundentity); //could possibly crash if entity is gone
	//Entity->SetEyeAngles(EyeAngles);
	//Entity->SetVecLadderNormal(src->laddernormal);
	//Entity->SetDucked(src->isducked);
	//Entity->SetDucking(src->isducking);
	//Entity->SetInDuckJump(src->isinduckjump);
	//Entity->SetDuckTimeMsecs(src->ducktimemsecs);
	//Entity->SetJumpTimeMsecs(src->jumptimemsecs);
	//Entity->UpdateClientSideAnimation();
	//Entity->SetLaggedMovement(src->laggedmovement);
	//Entity->SetAnimTime(src->animtime);
	//Entity->SetNetworkOrigin(src->networkorigin);//(src->networkorigin);
	//Entity->SetOrigin(src->origin);// src->networkorigin);// src->origin);
	//Entity->SetAbsOrigin(src->origin);
	//if (Absangles.y < 0)
	//	Absangles.y += 360.0f;
	//Entity->SetRenderAngles(EyeAngles);
	//Entity->SetAbsAngles(Absangles);
	//Entity->SetNetworkOrigin(src->networkorigin);

#if 1
		//Avoid using absorigin, it causes massive issues with bones
	//	Entity->SetOrigin(*Origin);
		Entity->SetAbsOrigin(*Origin);
	//	Entity->InvalidatePhysicsRecursive(POSITION_CHANGED);
#else
	Vector NewOrigin = *Origin;
	if (Entity->GetOrigin() != NewOrigin && NewOrigin != vecZero)
	{
		Entity->SetOrigin(NewOrigin);
		Entity->InvalidatePhysicsRecursive(POSITION_CHANGED);
	}
#endif
#ifdef PRINT_FUNCTION_NAMES
	printf("FinishRestoreNetvars\n");
#endif
}

void LagCompensation::CM_RestoreAnimations(StoredNetvars *const src, CBaseEntity *const Entity)
{
	//Restore pose parameters
	//if (!src->ResolvedFromBloodSpawning)
	{
		float* flPose = (float*)((uintptr_t)Entity + m_flPoseParameter);
		memcpy((void*)flPose, &src->flPoseParameter[0], (sizeof(float) * 24));
		//flPose[11] = body_yaw;
		//flPose[12] = body_pitch;
	}

	//Restore animations
	int animcount = Entity->GetNumAnimOverlays();
	for (int i = 0; i < animcount; i++)
	{
		C_AnimationLayer* pLayer = Entity->GetAnimOverlay(i);
	#if 1
		* pLayer = src->AnimLayer[i];
	#else
		C_AnimationLayer* pStoredLayer = &src->AnimLayer[i];
		if (pLayer->_m_flCycle != pStoredLayer->_m_flCycle)
			pLayer->SetCycle(pStoredLayer->_m_flCycle);
		if (pLayer->m_flWeight != pStoredLayer->m_flWeight)
			pLayer->SetWeight(pStoredLayer->m_flWeight);
		if (pLayer->m_nOrder != pStoredLayer->m_nOrder)
			pLayer->SetOrder(pStoredLayer->m_nOrder);
		if (pLayer->_m_nSequence != pStoredLayer->_m_nSequence)
			pLayer->SetSequence(pStoredLayer->_m_nSequence);
		pLayer->m_nInvalidatePhysicsBits = pStoredLayer->m_nInvalidatePhysicsBits;
		pLayer->m_flLayerAnimtime = pStoredLayer->m_flLayerAnimtime;
		pLayer->m_flPrevCycle = pStoredLayer->m_flPrevCycle;
		pLayer->m_flWeightDeltaRate = pStoredLayer->m_flWeightDeltaRate;
		pLayer->m_flBlendIn = pStoredLayer->m_flBlendIn;
		pLayer->m_flBlendOut = pStoredLayer->m_flBlendOut;
		pLayer->m_flLayerFadeOuttime = pStoredLayer->m_flLayerFadeOuttime;
		pLayer->m_pOwner = pStoredLayer->m_pOwner;
		pLayer->_m_flPlaybackRate = pStoredLayer->_m_flPlaybackRate;
	#endif
	}
}

//Gets the estimated server tickcount
int LagCompensation::GetEstimatedServerTickCount(float latency)
{
	return TIME_TO_TICKS(latency) + 1 + ReadInt((uintptr_t)&Interfaces::Globals->tickcount);
}

//Gets the estimated server curtime
float LagCompensation::GetEstimatedServerTime()
{
	return LocalPlayer.Entity->GetTickBase() * ReadFloat((uintptr_t)&Interfaces::Globals->interval_per_tick);
}

//Get latency for local player
float LagCompensation::GetNetworkLatency(int DIRECTION)
{
	// Get true latency
	INetChannelInfo *nci = Interfaces::Engine->GetNetChannelInfo();
	if (nci)
	{
		//float IncomingLatency = nci->GetAvgLatency(FLOW_INCOMING);
		float OutgoingLatency = nci->GetAvgLatency(DIRECTION);
		return OutgoingLatency;// + IncomingLatency;
	}
	return 0.0f;
}
float LagCompensation::GetTrueNetworkLatency(int DIRECTION)
{
	// Get true latency
	INetChannelInfo *nci = Interfaces::Engine->GetNetChannelInfo();
	if (nci)
	{
		//float IncomingLatency = nci->GetAvgLatency(FLOW_INCOMING);
		float OutgoingLatency = nci->GetLatency(DIRECTION);
		return OutgoingLatency;// + IncomingLatency;
}
	return 0.0f;
}

//See if a tick is within limits to be accepted by the server for hit registration
bool LagCompensation::bTickIsValid(int tick)
{
#ifdef PRINT_FUNCTION_NAMES
	AllocateConsole();
	printf("bTickIsValid\n");
#endif
#define NEW_VERSION
#ifdef NEW_VERSION
	if (tick > 0)
	{

		float outlatency;
		float inlatency;
		INetChannelInfo *nci = Interfaces::Engine->GetNetChannelInfo();
		if (nci)
		{
			inlatency = nci->GetLatency(FLOW_INCOMING);
			outlatency = nci->GetLatency(FLOW_OUTGOING);
		}
		else
		{
			inlatency = outlatency = 0.0f;
		}

		float totaloutlatency = outlatency;
		if (BacktrackExploitChk.Checked)
			totaloutlatency += FAKE_LATENCY_AMOUNT;

		//int servertickcount = TIME_TO_TICKS(latency) + 1 + Interfaces::Globals->tickcount;

		float servertime = TICKS_TO_TIME(GetEstimatedServerTickCount(outlatency + inlatency));

		float correct = clamp(totaloutlatency + GetLerpTime(), 0.0f, 1.0f);
		float flTargetTime = TICKS_TO_TIME(tick);
		float deltaTime = correct - (servertime - flTargetTime);
		return (fabsf(deltaTime) <= 0.2f);
	}

	return false;


	//return (ReadInt((uintptr_t)&Interfaces::Globals->tickcount) - tick <= TIME_TO_TICKS(0.2f));
#else
#define ALIENSWARM_VERSION
#ifdef ALIENSWARM_VERSION
	float latency = GetNetworkLatency(FLOW_OUTGOING);
#define SV_MAXUNLAG 1.0f
	float m_flLerpTime = cl_interp_ratio->GetFloat() / cl_updaterate->GetFloat();
	float correct = clamp(latency + m_flLerpTime, 0.0f, SV_MAXUNLAG);
	float flTargetTime = TICKS_TO_TIME(tick) - m_flLerpTime;
	float deltaTime = correct - (GetEstimatedServerTime() - flTargetTime);
	////^oRIGINAL

	//float latency2 = GetNetworkLatency(FLOW_INCOMING) + GetNetworkLatency(FLOW_OUTGOING);
	//float TimeUntilNextFrame = 0;// Interfaces::Globals->absoluteframetime * 0.5f;
	//float TimeItWillBeWhenPacketReachesServer = TICKS_TO_TIME(CurrentUserCmd.cmd->tick_count + TIME_TO_TICKS(latency2 + TimeUntilNextFrame) + 1);
	////Time we reach the server - time the player was simulated on the server , clamped to sv_maxunlag (1.0 seconds)
	//float deltaTime2 = fminf(TimeItWillBeWhenPacketReachesServer - TICKS_TO_TIME(tick), 1.0f);
	////^ fake lag fix


	//float tickInterval = 1.0 / Interfaces::Globals->interval_per_tick;

	//float deltaTime3 = ((floorf(((GetNetworkLatency(FLOW_INCOMING) + GetNetworkLatency(FLOW_OUTGOING)) * tickInterval) + 0.5)
	//	+ CurrentUserCmd.cmd->tick_count
	//	+ 1)
	//	* Interfaces::Globals->interval_per_tick)
	//	- TICKS_TO_TIME(tick);

	//if (deltaTime3 > 1.0f)
	//	deltaTime3 = 1.0f;

	////^aimware
	//AllocateConsole();
	//std::cout << deltaTime << " " << deltaTime2 << " " << deltaTime3 << std::endl;
	return fabsf(deltaTime) <= 0.2f;
#else
	//SDK 2013 VERSION
	float latency = GetNetworkLatency();
	const float SV_MAXUNLAG = 1.0f;
	float m_flLerpTime = cl_interp_ratio->GetFloat() / cl_updaterate->GetFloat();
	int lerpTicks = TIME_TO_TICKS(m_flLerpTime);
	float correct = clamp(latency + TICKS_TO_TIME(lerpTicks), 0.0f, SV_MAXUNLAG);
	int targettick = tick - lerpTicks;
	float deltaTime = correct - TICKS_TO_TIME(GetEstimatedServerTickCount(latency) - targettick);
	return fabsf(deltaTime) <= 0.2f;
#endif
#endif
}

bool LagCompensation::bTickIsValid2(int tick)
{
	float latency = GetNetworkLatency(FLOW_OUTGOING);

	int servertickcount = TIME_TO_TICKS(latency) + 1 + Interfaces::Globals->tickcount;

	float correct = clamp(latency + TICKS_TO_TIME(1) + GetLerpTime(), 0.0f, 1.0f);
	float flTargetTime = TICKS_TO_TIME(tick);
	float deltaTime = correct - (TICKS_TO_TIME(servertickcount) - flTargetTime);
	return ((deltaTime) <= 0.2f);


	//return (ReadInt((uintptr_t)&Interfaces::Globals->tickcount) - tick <= TIME_TO_TICKS(0.2f));
}

//Attempts to predict what the player's lower body yaw will be using algorithms
bool LagCompensation::PredictLowerBodyYaw(CustomPlayer *const pCPlayer, CBaseEntity*const Entity, float LowerBodyDelta)
{
#ifdef PRINT_FUNCTION_NAMES
	AllocateConsole();
	printf("PredictLowerBodyYaw\n");
#endif
	VMP_BEGINMUTILATION("PLBY")

	auto stat = START_PROFILING("PredictLowerBodyYaw");
	StoredNetvars *CurrentNetVars = pCPlayer->GetCurrentRecord();
	auto LowerBodyRecords = &pCPlayer->Personalize.m_PlayerLowerBodyRecords;
	bool FoundPattern = false;

	float flCurrentBodyYaw = CurrentNetVars->lowerbodyyaw;

	bool bPlayerIsMoving = Entity->GetVelocity().Length() > 0.1f;

	if (bPlayerIsMoving)
	{
		QAngle VelocityAngles;
		VectorAngles(Entity->GetVelocity(), VelocityAngles);

		pCPlayer->Personalize.WasInverseFakingWhileMoving = fabsf(ClampYr(VelocityAngles.y - flCurrentBodyYaw)) >= 170.0f;

		if (!LowerBodyRecords->empty())
		{
			//Detects inverse very well
			StoredLowerBodyYaw *NewestLowerBodyRecord = LowerBodyRecords->front();
			if (NewestLowerBodyRecord->HasPreviousRecord && fabsf(NewestLowerBodyRecord->deltabetweenlastlowerbodyeyedelta) < 10.0f)
			{
				pCPlayer->Personalize.EstimateYawModeInAir = ResolveYawModes::StaticFakeDynamic;
			}
			else
			{
				pCPlayer->Personalize.EstimateYawModeInAir = pCPlayer->Personalize.EstimateYawMode;
			}
		}
	}
	else
	{
		if (pCPlayer->LowerBodyEyeDelta != -999.9f)
		{
			//Not a very reliable resolve method at all
			if (pCPlayer->PredictedRandomFakeAngle != -999.9f)
			{
				if (fabsf(ClampYr(flCurrentBodyYaw - pCPlayer->PredictedRandomFakeAngle)) < 10.0f)
				{
					//random fake
					pCPlayer->Personalize.EstimateYawMode = ResolveYawModes::RandomFake;
					FoundPattern = true;
				}
			}
			pCPlayer->PredictedRandomFakeAngle = ClampYr(CurrentNetVars->eyeangles.y + (pCPlayer->LowerBodyEyeDelta * 0.5f));
		}

		bool FoundToBeFakeSpinning = false; //ugh shit code

		if (pCPlayer->LowerBodyDelta != -999.9f)
		{
			if (CurrentNetVars->bSpinbotting && fabsf(ClampYr(LowerBodyDelta - pCPlayer->LowerBodyDelta)) < 10.0f)
			{
				//fake spin
				pCPlayer->Personalize.EstimateYawMode = ResolveYawModes::FakeSpins;
				FoundPattern = true;
				FoundToBeFakeSpinning = true;
			}
			else
			{
				if (pCPlayer->PredictedLinearFakeAngle != -999.9f)
				{
					if (fabsf(ClampYr(flCurrentBodyYaw - pCPlayer->PredictedLinearFakeAngle)) < 15.0f)
					{
						//linear fake
						pCPlayer->Personalize.EstimateYawMode = ResolveYawModes::LinearFake;
						FoundPattern = true;
					}
				}

				if (pCPlayer->PredictedStaticFakeAngle != -999.9f)
				{
					if (fabsf(ClampYr(flCurrentBodyYaw - pCPlayer->PredictedStaticFakeAngle)) < 10.0f)
					{
						//static fake
						pCPlayer->Personalize.EstimateYawMode = ResolveYawModes::FakeSpins;
						FoundPattern = true;
					}
				}
			}
			pCPlayer->PredictedLinearFakeAngle = ClampYr(flCurrentBodyYaw + LowerBodyDelta);
			pCPlayer->PredictedStaticFakeAngle = ClampYr(ClampYr(CurrentNetVars->eyeangles.y) - LowerBodyDelta);

			if (!FoundToBeFakeSpinning && !LowerBodyRecords->empty())
			{
				//Detects inverse very well
				StoredLowerBodyYaw *NewestLowerBodyRecord = LowerBodyRecords->front();
				if (NewestLowerBodyRecord->HasPreviousRecord && fabsf(NewestLowerBodyRecord->deltabetweenlastlowerbodyeyedelta) < 10.0f)
				{
					pCPlayer->Personalize.EstimateYawMode = ResolveYawModes::StaticFakeDynamic;
					FoundPattern = true;
				}
			}

			//Detect average lower body changes
			int LowerBodyRecordCount = LowerBodyRecords->size();
			if (LowerBodyRecordCount > 1)
			{
				float TotalYaws = 0.0f;
				int maxindex = min(LowerBodyRecordCount - 1, 4);
				int samples = 0;
				for (samples; samples < maxindex; samples++)
				{
					StoredLowerBodyYaw *record = LowerBodyRecords->at(samples);
					StoredLowerBodyYaw *previousrecord = LowerBodyRecords->at(samples + 1);
					if (record->networkorigin != previousrecord->networkorigin)
					{
						//If player moved, don't count it
						break;
					}
					TotalYaws += ClampYr(record->lowerbodyyaw - previousrecord->lowerbodyyaw);
				}

				if (samples)
				{
					float AverageYaw = ClampYr(TotalYaws / samples);
					if (!FoundPattern)
					{
						float Delta = ClampYr(flCurrentBodyYaw - pCPlayer->PredictedAverageFakeAngle);
						if (fabsf(Delta) <= 25.0f)
						{
							pCPlayer->Personalize.EstimateYawMode = ResolveYawModes::AverageLBYDelta;
							FoundPattern = true;
						}
					}

					pCPlayer->PredictedAverageFakeAngle = ClampYr(flCurrentBodyYaw + AverageYaw);
				}
			}

			if (pCPlayer->Personalize.EstimateYawMode == ResolveYawModes::AverageLBYDelta && pCPlayer->PredictedAverageFakeAngle == -999.9f)
			{
				//Don't allow invalidated angles
				pCPlayer->Personalize.EstimateYawMode = ResolveYawModes::LinearFake;
			}
		}

		pCPlayer->LowerBodyDelta = LowerBodyDelta;

		float lbed = ClampYr(flCurrentBodyYaw - CurrentNetVars->eyeangles.y);
		pCPlayer->LowerBodyEyeDelta = lbed;
	}
	
	if (pCPlayer->Personalize.EstimateYawModeInAir == ResolveYawModes::NoYaw)
	{
		pCPlayer->Personalize.EstimateYawModeInAir = pCPlayer->Personalize.EstimateYawMode;
	}

	END_PROFILING(stat);
	return FoundPattern;
	VMP_END
}

void Normalise(double &value)
{

	if (!std::isfinite(value))
	{
		value = 0;
	}

	while (value <= -180) value += 360;
	while (value > 180) value -= 360;
	if (value < -180) value = -179.999;
	if (value > 180) value = 179.999;

}
float Vec2Ang(Vector Velocity)
{
	if (Velocity.x == 0 || Velocity.y == 0)
		return 0;
	float rise = Velocity.x;
	float run = Velocity.y;
	float value = rise / run;
	float theta = atan(value);
	theta = RAD2DEG(theta) + 90;
	if (Velocity.y < 0 && Velocity.x > 0 || Velocity.y < 0 && Velocity.x < 0)
		theta *= -1;
	else
		theta = 180 - theta;
	return theta;
}

double difference(double first, double second)
{
	Normalise(first);
	Normalise(second);
	double returnval = first - second;
	if (first < -91 && second> 91 || first > 91 && second < -91)
	{
		double negativedifY = 180 - abs(first);
		double posdiffenceY = 180 - abs(second);
		returnval = negativedifY + posdiffenceY;
	}
	return returnval;
}

//Probably double handling with these value storing
//Call this every single tick that the player's information updates (ie they send a tick again).
int usingAimwareAdaptive(CustomPlayer*const pCPlayer)
{

	float VelocityY = pCPlayer->BaseEntity->GetVelocity().y;
	float VelocityX = pCPlayer->BaseEntity->GetVelocity().x;
	int WishTicks_1 = 0;
	int AdaptTicks = 2;
	double ExtrapolatedSpeed = sqrt((VelocityX * VelocityX) + (VelocityY * VelocityY))
		* Interfaces::Globals->interval_per_tick;
	while ((WishTicks_1 * ExtrapolatedSpeed) <= 68.0)
	{
		if (((AdaptTicks - 1) * ExtrapolatedSpeed) > 68.0)
		{
			++WishTicks_1;
			break;
		}
		if ((AdaptTicks * ExtrapolatedSpeed) > 68.0)
		{
			WishTicks_1 += 2;
			break;
		}
		if (((AdaptTicks + 1) * ExtrapolatedSpeed) > 68.0)
		{
			WishTicks_1 += 3;
			break;
		}
		if (((AdaptTicks + 2) * ExtrapolatedSpeed) > 68.0)
		{
			WishTicks_1 += 4;
			break;
		}
		AdaptTicks += 5;
		WishTicks_1 += 5;
		if (AdaptTicks > 16)
			break;
	}
	if (WishTicks_1 > 16)
		WishTicks_1 = 15;
	return WishTicks_1;
}

void recordChokedTicks(CustomPlayer*const pCPlayer, int ChockedTics)
{
	//inacurate first 20 logs but meh
	pCPlayer->Personalize.fakeLagHistoryCounter++;
	if (pCPlayer->Personalize.fakeLagHistoryCounter > 19)
		pCPlayer->Personalize.fakeLagHistoryCounter = 0;
	pCPlayer->Personalize.historyPacketsChoked[pCPlayer->Personalize.fakeLagHistoryCounter] = ChockedTics;

	if (usingAimwareAdaptive(pCPlayer) == ChockedTics)
		pCPlayer->Personalize.UsingAwareAdaptive = true;
	else
		pCPlayer->Personalize.UsingAwareAdaptive = false;


}
//int ChockedTics = number of ticks chocked since last sending a packet
int predictedChokedTicks(CustomPlayer*const pCPlayer, int CurrentChockedTics)
{
#ifdef PRINT_FUNCTION_NAMES
	AllocateConsole();
	printf("predictChokedTicks\n");
#endif
	//Assuming polnomial expansions, lets just keep differentialting the equation until we get something which works ;)
	int total = 0;
	for (int i = 0; i < 20; i++)
	{
		total += pCPlayer->Personalize.historyPacketsChoked[i];
	}
	int average = total / 20;
	double SD = 0;
	for (int i = 0; i < 20; i++)
	{
		SD += ((pCPlayer->Personalize.historyPacketsChoked[i] - average)*(pCPlayer->Personalize.historyPacketsChoked[i] - average));
	}
	SD /= (SD - 1);
	SD = sqrt(SD);

	if (SD <= 2 && CurrentChockedTics <= average)
		return average; // Probably average if SD is so low

	if (pCPlayer->Personalize.UsingAwareAdaptive)
		return usingAimwareAdaptive(pCPlayer);

	//See if its a linear step
	int totalStep = 0;
	//2nd degree step
	int difference2[19];
	int totalstep2 = 0;
	//3rd degree step
	int difference3[18];
	int totalstep3 = 0;
	for (int i = 0; i < 19; i++)
	{
		difference2[i] += pCPlayer->Personalize.historyPacketsChoked[i + 1];
		difference2[i] -= pCPlayer->Personalize.historyPacketsChoked[i];
		totalStep += difference2[i];
	}
	int averageStep = totalStep / 19;
	double SDStep = 0;
	for (int i = 0; i < 19; i++)
	{
		SDStep += ((difference2[i] - averageStep)*(difference2[i] - averageStep));
	}
	SDStep /= (SDStep - 1);
	SDStep = sqrt(SDStep);

	if (SDStep <= 1 && CurrentChockedTics <= averageStep)
		return averageStep; // Probably average if SD is so l



	for (int i = 0; i < 18; i++)
	{
		difference3[i] += difference2[i + 1];
		difference3[i] -= difference2[i];
		totalstep2 += difference3[i];
	}
	int averageStep2 = totalstep2 / 18;
	double SDStep2 = 0;
	for (int i = 0; i < 18; i++)
	{
		SDStep2 += ((difference2[i] - averageStep2)*(difference2[i] - averageStep2));
	}
	SDStep2 /= (SDStep2 - 1);
	SDStep2 = sqrt(SDStep2);

	if (SDStep2 <= 1 && CurrentChockedTics <= averageStep2)
		return averageStep2; // Probably average if SD is so l


	int difference4[17];
	for (int i = 0; i < 17; i++)
	{
		difference4[i] += difference3[i + 1];
		difference4[i] -= difference3[i];
		totalstep3 += difference3[i];
	}
	int averageStep3 = totalstep3 / 17;
	double SDStep3 = 0;
	for (int i = 0; i < 17; i++)
	{
		SDStep3 += ((difference3[i] - averageStep3)*(difference3[i] - averageStep3));
	}
	SDStep3 /= (SDStep3 - 1);
	SDStep3 = sqrt(SDStep3);

	if (SDStep3 <= 1 && CurrentChockedTics <= averageStep3)
		return averageStep3; // Probably average if SD is so l

							 //We have now checked if they are following a polynomial up to degree 4
							 //See here for complete complex algorthym http://www.johansens.us/sane/education/formula.htm
	int lastamount = pCPlayer->Personalize.historyPacketsChoked[pCPlayer->Personalize.fakeLagHistoryCounter];
	if (CurrentChockedTics <= lastamount)
		return lastamount;
	return CurrentChockedTics;

	//Check for aimware adaptive

}


float accuracy = 10;

void clamp(float &value)
{
	while (value > 180)
		value -= 360;
	while (value < -180)
		value += 360;
}
float clamp2(float value)
{
	while (value > 180)
		value -= 360;
	while (value < -180)
		value += 360;
	return value;
}
float difference(float first, float second)
{
	clamp(first);
	clamp(second);
	float returnval = first - second;
	if (first < -91 && second> 91 || first > 91 && second < -91)
	{
		double negativedifY = 180 - abs(first);
		double posdiffenceY = 180 - abs(second);
		returnval = negativedifY + posdiffenceY;
	}
	return returnval;
}

float mean(float real[scanstrength], int slots)
{
	float total = 0;
	for (int i = 0; i < slots; i++)
	{
		total += real[i];
	}
	return total / slots;
}
float SD(float real[scanstrength], float mean, int slots)
{
	float total = 0;
	for (int i = 0; i < slots; i++)
	{
		total += (real[i] - mean)*(real[i] - mean);
	}
	total /= slots;
	return sqrt(total);
}
bool checkSpin(float real[scanstrength], int ticks[scanstrength], float &meanvalue, int &level, int slots)
{
	float dif[scanstrength - 1];

	int ticks2[scanstrength - 1];
	float dif2[scanstrength - 2];

	for (int i = 0; i < slots - 1; i++)
	{
		ticks2[i] = abs(ticks[i + 1] - ticks[i]);
		if (ticks2[i] == 1)
			ticks2[i] = 1;
		dif[i] = clamp2(difference(real[i], real[i + 1]) / (ticks2[i]));
	}
	meanvalue = mean(dif, slots - 1);
	if (SD(dif, meanvalue, slots - 1) < accuracy)
	{
		level = 1;
		return true;
	}


	for (int i = 0; i < slots - 2; i++)
	{
		int ticks = abs(ticks2[i + 1] - ticks2[i]);
		if (ticks == 0)
			ticks = 1;

		dif2[i] = clamp2(difference(dif[i], dif[i + 1]) / (ticks));
	}
	meanvalue = mean(dif2, slots - 2);
	if (SD(dif2, meanvalue, slots - 2) < accuracy)
	{
		level = 2;
		return true;
	}

	return false;
}
bool checkJitter(float real[scanstrength], int ticks[scanstrength], float &meanvalue, int &level, int slots)
{
	float dif[scanstrength - 1];

	int ticks2[scanstrength - 1];
	float dif2[scanstrength - 2];

	for (int i = 0; i < slots - 1; i++)
	{
		ticks2[i] = abs(ticks[i + 1] - ticks[i]);
		if (ticks2[i] == 1)
			ticks2[i] = 1;

		dif[i] = clamp2(abs(difference(real[i], real[i + 1]) / (ticks2[i])));
	}
	meanvalue = mean(dif, slots - 1);
	if (SD(dif, meanvalue, slots - 1) < accuracy)
	{
		level = 1;
		return true;
	}


	for (int i = 0; i < slots - 2; i++)
	{
		int ticks = abs(ticks2[i + 1] - ticks2[i]);
		if (ticks == 0)
			ticks = 1;

		dif2[i] = clamp2(abs(difference(dif[i], dif[i + 1]) / (ticks)));
	}
	meanvalue = mean(dif2, slots - 2);
	if (SD(dif2, meanvalue, slots - 2) < accuracy)
	{
		level = 2;
		return true;
	}

	return false;
}
bool checkStatic(float real[scanstrength], float meanvalue, int slots)
{
	meanvalue = mean(real, slots);
	if (SD(real, meanvalue, slots) < accuracy)
	{
		return true;
	}
	return false;
}
//TOFIX: level 2 jitter's and spins are not dont right
//Jitter's arent done right
//Add history to which of the options worked
void LogicResolver2_Log(QAngle angles, QAngle real, bool knowReal, CustomPlayer* pCPlayer, CBaseEntity* pLocalEntity)
{
#ifdef PRINT_FUNCTION_NAMES
	AllocateConsole();
	printf("LogicResolver2_Log\n");
#endif
	if (knowReal)
	{
		int history = WhaleDongTxt.iValue;
		CBaseEntity * Target = pCPlayer->BaseEntity;

		int counter = pCPlayer->Personalize.counter;
		pCPlayer->Personalize.counter++;
		if (pCPlayer->Personalize.counter >= history)
			pCPlayer->Personalize.counter = 0;
		pCPlayer->Personalize.realValues[counter] = real.y;
		pCPlayer->Personalize.fakeValues[counter] = angles.y;
		pCPlayer->Personalize.targetDValues[counter] = CalcAngle(Target->GetEyePosition(), pLocalEntity->GetEyePosition()).y;
		pCPlayer->Personalize.velocityDValues[counter] = Vec2Ang(Target->GetVelocity());
		pCPlayer->Personalize.ticksReal[counter] = TIME_TO_TICKS(Interfaces::Globals->curtime);
		pCPlayer->Personalize.lbyUpdate = Interfaces::Globals->curtime;
		pCPlayer->Personalize.curTimeUpdatedLast = Interfaces::Globals->curtime;
		pCPlayer->Personalize.lastlby = real.y;

	}
}


void LogicResolver2_Predict(QAngle &oAngles, bool predict, CustomPlayer* pCPlayer, CBaseEntity* pLocalEntity, int ticks)
{
#ifdef PRINT_FUNCTION_NAMES
	AllocateConsole();
	printf("LogicResolver2_Predict\n");
#endif
	int history = WhaleDongTxt.iValue;
	CBaseEntity * Target = pCPlayer->BaseEntity;

	float realValues[scanstrength];
	float fakeValues[scanstrength];
	float targetDValues[scanstrength];
	float velocityDValues[scanstrength];
	int ticksReal[scanstrength];

	for (int i = 0; i < history; i++)
	{
		realValues[i] = pCPlayer->Personalize.realValues[i];
		fakeValues[i] = pCPlayer->Personalize.fakeValues[i];
		targetDValues[i] = pCPlayer->Personalize.targetDValues[i];
		velocityDValues[i] = pCPlayer->Personalize.velocityDValues[i];
		ticksReal[i] = pCPlayer->Personalize.ticksReal[i];
	}
	


	float lastUpdate = pCPlayer->Personalize.curTimeUpdatedLast;
	float currentTime = Interfaces::Globals->curtime;

	float lastReal = pCPlayer->Personalize.lastlby;
	float currentFake = oAngles.y;
	float currentTargetD = CalcAngle(Target->GetEyePosition(), pLocalEntity->GetEyePosition()).y;
	float currentVelocityD = Vec2Ang(Target->GetVelocity());
	bool onGround = (Target->GetFlags() & FL_ONGROUND);

	int shotsMissed = pCPlayer->Personalize.ShotsnotHit;
	float ResolvedAngle = lastReal;
	float lastLBY = lastReal;
	StoredNetvars *CurrentNetVars = pCPlayer->GetCurrentRecord();
	if (CurrentNetVars)
	{
		auto LowerBodyRecords = &pCPlayer->Personalize.m_PlayerLowerBodyRecords;
		lastReal = CurrentNetVars->lowerbodyyaw;
		lastLBY = CurrentNetVars->lowerbodyyaw;
	}

	float testValues[scanstrength];



	int tick2predict = 1;

	//Do prediction shit
	//Real vs Fakes
	for (int i = 0; i < history; i++)
	{
		testValues[i] = clamp2(difference(realValues[i], fakeValues[i]));
	}

	float RFstaticMean = 0;;
	bool RFisStatic = checkStatic(testValues, RFstaticMean, history);
	int RFlevelJitter = 0;
	float RFlevelJitterMean;
	bool RFisJitter = checkJitter(testValues, ticksReal, RFlevelJitterMean, RFlevelJitter, history);
	int RFlevelSpin = 0;
	float RFlevelCheckMean;
	bool RFisSpin = checkSpin(testValues, ticksReal, RFlevelCheckMean, RFlevelSpin, history);

	//Real vs Target
	for (int i = 0; i < history; i++)
	{
		testValues[i] = clamp2(difference(realValues[i], targetDValues[i]));
	}

	float RTstaticMean = 0;;
	bool RTisStatic = checkStatic(testValues, RTstaticMean, history);
	int RTlevelJitter = 0;
	float RTlevelJitterMean;
	bool RTisJitter = checkJitter(testValues, ticksReal, RTlevelJitterMean, RTlevelJitter, history);
	int RTlevelSpin = 0;
	float RTlevelCheckMean;
	bool RTisSpin = checkSpin(testValues, ticksReal, RTlevelCheckMean, RTlevelSpin, history);

	//Delta Checks
		for (int i = 0; i < history; i++)
		{
			testValues[i] = clamp2(difference(realValues[i], 0));
		}

		float staticMean = 0;
		bool isStatic = checkStatic(testValues, staticMean, history);
		int levelJitter = 0;
		float levelJitterMean;
		bool isJitter = checkJitter(testValues, ticksReal, levelJitterMean, levelJitter, history);
		int levelSpin = 0;
		float levelCheckMean;
		bool isSpin = checkSpin(testValues, ticksReal, levelCheckMean, levelSpin, history);

	
		//Real vs Velocity

		for (int i = 0; i < history; i++)
		{
			testValues[i] = clamp2(difference(realValues[i], velocityDValues[i]));
		}

		float RVstaticMean = 0;;
		bool RVisStatic = checkStatic(testValues, RVstaticMean, history);
		int RVlevelJitter = 0;
		float RVlevelJitterMean;
		bool RVisJitter = checkJitter(testValues, ticksReal, RVlevelJitterMean, RVlevelJitter, history);
		int RVlevelSpin = 0;
		float RVlevelCheckMean;
		bool RVisSpin = checkSpin(testValues, ticksReal, RVlevelCheckMean, RVlevelSpin, history);


		//Let us predict!
		std::vector <float> angles;
		std::vector <int> resolveMode;
		float angle = 0;

		//Real vs fake
		if (RFisStatic)
		{
			angle = RFstaticMean;
			if (onGround && (currentTime - lastUpdate > 1.1f) && (difference(angle, lastLBY) < 35))
			{
				angles.push_back(angle);
				resolveMode.push_back(WD_StaticFake);
			}
			else if (!onGround || (currentTime - lastUpdate < 1.1f))
			{
				angles.push_back(angle);
				resolveMode.push_back(WD_StaticFake);
			}
		}
		if (RFisJitter)
		{
			if (RFlevelJitter == 2)
				angle = lastReal + RFlevelJitterMean * 1.5;
			else
				angle = lastReal + RFlevelJitterMean;
			if (onGround && (currentTime - lastUpdate > 1.1f) && (difference(angle, lastLBY) < 35))
			{
				angles.push_back(angle);
				resolveMode.push_back(WD_StaticFake);
			}
			else if (!onGround || (currentTime - lastUpdate < 1.1f))
			{
				angles.push_back(angle);
				resolveMode.push_back(WD_StaticFake);
			}
		}
		if (RFisSpin)
		{
			if (RFlevelSpin == 2)
				angle = lastReal + RFlevelCheckMean * 1.5;
			else
				angle = lastReal + RFlevelCheckMean;
			if (onGround && (currentTime - lastUpdate > 1.1f) && (difference(angle, lastLBY) < 35))
			{
				angles.push_back(angle);
				resolveMode.push_back(WD_StaticFake);
			}
			else if (!onGround || (currentTime - lastUpdate < 1.1f))
			{
				angles.push_back(angle);
				resolveMode.push_back(WD_StaticFake);
			}
		}

		//Real vs target
		if (RTisStatic)
		{
			angle = RTstaticMean;
			if (onGround && (currentTime - lastUpdate > 1.1f) && (difference(angle, lastLBY) < 35))
			{
				angles.push_back(angle);
				resolveMode.push_back(WD_Target);
			}
			else if (!onGround || (currentTime - lastUpdate < 1.1f))
			{
				angles.push_back(angle);
				resolveMode.push_back(WD_Target);
			}
		}
		if (RTisJitter)
		{
			if (RTlevelJitter == 2)
				angle = lastReal + RTlevelJitterMean * 1.5;
			else
				angle = lastReal + RTlevelJitterMean;
			if (onGround && (currentTime - lastUpdate > 1.1f) && (difference(angle, lastLBY) < 35))
			{
				angles.push_back(angle);
				resolveMode.push_back(WD_Target);
			}
			else if (!onGround || (currentTime - lastUpdate < 1.1f))
			{
				angles.push_back(angle);
				resolveMode.push_back(WD_Target);
			}
		}
		if (RTisSpin)
		{
			if (RTlevelSpin == 2)
				angle = lastReal + RTlevelCheckMean * 1.5;
			else
				angle = lastReal + RTlevelCheckMean;
			if (onGround && (currentTime - lastUpdate > 1.1f) && (difference(angle, lastLBY) < 35))
			{
				angles.push_back(angle);
				resolveMode.push_back(WD_Target);
			}
			else if (!onGround || (currentTime - lastUpdate < 1.1f))
			{
				angles.push_back(angle);
				resolveMode.push_back(WD_Target);
			}
		}


		//Real delta
		if (isStatic)
		{
			angle = staticMean;
			if (onGround && (currentTime - lastUpdate > 1.1f) && (difference(angle, lastLBY) < 35))
			{
				angles.push_back(angle);
				resolveMode.push_back(WD_Static);
			}
			else if (!onGround || (currentTime - lastUpdate < 1.1f))
			{
				angles.push_back(angle);
				resolveMode.push_back(WD_Static);
			}

		}
		if (isJitter)
		{
			//Needs to be right
			if (levelJitter == 2)
				angle = lastReal + levelJitterMean * 1.5;
			else
			{
				if (tick2predict % 2)
					angle = lastReal + levelJitterMean;
				else
					angle = lastReal;
			}
			if (onGround && (currentTime - lastUpdate > 1.1f) && (difference(angle, lastLBY) < 35))
			{
				angles.push_back(angle);
				resolveMode.push_back(WD_Jitter);
			}
			else if (!onGround || (currentTime - lastUpdate < 1.1f))
			{
				angles.push_back(angle);
				resolveMode.push_back(WD_Jitter);
			}
		}
		if (isSpin)
		{
			if (levelSpin == 2)
				angle = lastReal + levelCheckMean * tick2predict * 1.5;
			else
				angle = lastReal + levelCheckMean * tick2predict;
			if (onGround && (currentTime - lastUpdate > 1.1f) && (difference(angle, lastLBY) < 35))
			{
				angles.push_back(angle);
				resolveMode.push_back(WD_Spin);
			}
			else if (!onGround || (currentTime - lastUpdate < 1.1f))
			{
				angles.push_back(angle);
				resolveMode.push_back(WD_Spin);
			}
		}

		//Real vs Velocity
		if (abs(currentVelocityD) > 0)
		{
			if (RVisStatic)
			{
				angle = RVstaticMean;
				if (onGround && (currentTime - lastUpdate > 1.1f) && (difference(angle, lastLBY) < 35))
				{
					angles.push_back(angle);
					resolveMode.push_back(WD_Velocity);
				}
				else if (!onGround || (currentTime - lastUpdate < 1.1f))
				{
					angles.push_back(angle);
					resolveMode.push_back(WD_Velocity);
				}
			}
			if (RVisJitter)
			{
				if (RVlevelJitter == 2)
					angle = lastReal + RVlevelJitterMean * 1.5;
				else
					angle = lastReal + RVlevelJitterMean;
				if (onGround && (currentTime - lastUpdate > 1.1f) && (difference(angle, lastLBY) < 35))
				{
					angles.push_back(angle);
					resolveMode.push_back(WD_Velocity);
				}
				else if (!onGround || (currentTime - lastUpdate < 1.1f))
				{
					angles.push_back(angle);
					resolveMode.push_back(WD_Velocity);
				}
			}
			if (RVisSpin)
			{
				if (RVlevelSpin == 2)
					angle = lastReal + RVlevelCheckMean * 1.5;
				else
					angle = lastReal + RVlevelCheckMean;
				if (onGround && (currentTime - lastUpdate > 1.1f) && (difference(angle, lastLBY) < 35))
				{
					angles.push_back(angle);
					resolveMode.push_back(WD_Velocity);
				}
				else if (!onGround || (currentTime - lastUpdate < 1.1f))
				{
					angles.push_back(angle);
					resolveMode.push_back(WD_Velocity);
				}
			}

		}
		//Angles have been predicted
		//Let us brute force and set what we want most :)

		//Lets check if any of our algorthyms worked
		if (angles.size() > 0)
		{
			int resolveOption = shotsMissed % angles.size();
			ResolvedAngle = angles[resolveOption];
			pCPlayer->Personalize.LastResolveModeUsed = (ResolveYawModes)resolveMode[resolveOption];

		}
		else if (onGround && (currentTime - lastUpdate > 1.1f))
		{
			pCPlayer->Personalize.LastResolveModeUsed = WD_FuckIt;

			ResolvedAngle = lastLBY;

			switch ((int)shotsMissed % 9)
			{
			case 1:
				ResolvedAngle += 35;
				break;
			case 3:
				ResolvedAngle -= 35;
				break;
			case 5:
				ResolvedAngle += 20;
				break;
			case 7:
				ResolvedAngle -= 20;
				break;
			}
		}
		int resolverNumber = 0;
		resolverNumber = 30 * (shotsMissed % 13) - 180;
		if (abs(pCPlayer->Personalize.ShotsMissed) > 2 || angles.size() == 0)
		{

			if (onGround && (currentTime - lastUpdate > 1.1f))
			{
				ResolvedAngle = lastLBY;
				pCPlayer->Personalize.LastResolveModeUsed = WD_FuckIt;

				switch ((int)shotsMissed % 9)
				{

				case 1:
					ResolvedAngle += 35;
					break;
				case 3:
					ResolvedAngle -= 35;
					break;
				case 5:
					ResolvedAngle += 20;
					break;
				case 7:
					ResolvedAngle -= 20;
					break;
				}
			}
			else
			{
				if (angles.size() == 0)
				{
					pCPlayer->Personalize.LastResolveModeUsed = WD_FuckIt;

					switch ((int)shotsMissed % 12)
					{
					case 1:
						ResolvedAngle = currentTargetD + resolverNumber;
						break;
					case 2:
						ResolvedAngle = oAngles.y + resolverNumber;
						break;
					case 3:
						ResolvedAngle = currentTargetD + resolverNumber;
						break;
					case 4:
						ResolvedAngle = oAngles.y + resolverNumber;
						break;
					case 5:
						ResolvedAngle = oAngles.y + resolverNumber;
						break;
					case 6:
						ResolvedAngle = currentTargetD + resolverNumber;
						break;
					case 7:
						ResolvedAngle = oAngles.y - resolverNumber;
						break;
					case 8:
						ResolvedAngle = oAngles.y + resolverNumber;
						break;
					case 9:
						ResolvedAngle = currentTargetD + resolverNumber;
						break;
					case 10:
						ResolvedAngle = oAngles.y + resolverNumber;
						break;
					case 11:
						ResolvedAngle = oAngles.y + resolverNumber;
						break;

					}
				}
				else
				{
					if(shotsMissed % 1)
						pCPlayer->Personalize.LastResolveModeUsed = WD_FuckIt;

					switch ((int)shotsMissed % 12)
					{
					case 1:
						ResolvedAngle = currentTargetD + resolverNumber;
						break;
					case 3:
						ResolvedAngle = currentTargetD + resolverNumber;
						break;
					case 5:
						ResolvedAngle = oAngles.y + resolverNumber;
						break;
					case 7:
						ResolvedAngle = oAngles.y + resolverNumber;
						break;
					case 9:
						ResolvedAngle = currentTargetD + resolverNumber;
						break;
					case 11:
						ResolvedAngle += 35;
						break;
					case 13:
						ResolvedAngle -= 35;
						break;

					}
				}
			}
		}
		oAngles.y = ResolvedAngle;
}

//Called by NetUpdatePlayer
void LagCompensation::NewTickRecordReceived(CBaseEntity* Entity, CustomPlayer*const pCPlayer, float flNewSimulationTime)
{
	auto Records = &pCPlayer->Personalize.m_PlayerRecords;

	//Store the amount of ticks this player choked
	pCPlayer->TicksChoked = (Records->empty() || Records->front()->dormant) ? 0 : TIME_TO_TICKS(flNewSimulationTime - Records->front()->simulationtime) - 1;

	//Update velocity
	float flTimeDelta = flNewSimulationTime - Entity->GetOldSimulationTime();
	bool bForceEFNoInterp = Entity->IsParentChanging();

	if (flTimeDelta != 0.0f && !((Entity->GetEffects() & EF_NOINTERP) || bForceEFNoInterp))
	{
		Vector newVelo = (Entity->GetNetworkOrigin() - Entity->GetOldOrigin()) / flTimeDelta;
		// This code used to call SetAbsVelocity, which is a no-no since we are in networking and if
		//  in hieararchy, the parent velocity might not be set up yet.
		// On top of that GetNetworkOrigin and GetOldOrigin are local coordinates
		// So we'll just set the local vel and avoid an Assert here
		Entity->SetLocalVelocity(newVelo);
	}

	//Validate to make sure proxy is valid..
	ValidateNetVarsWithProxy(pCPlayer);

	//Allocate a new tick record
	StoredNetvars *newtick = new StoredNetvars(Entity, pCPlayer, &pCPlayer->CurrentNetvarsFromProxy);

	//Get the previous tick's fire at pelvis value so that we can still shoot the heads if we successfully resolved the player
	bool PreviousFireAtPelvis = (Records->empty() || Records->front()->dormant) ? false : Records->front()->FireAtPelvis;

	//Store the new tick record
	Records->push_front(newtick);

	//See if this tick is special (player fired, lower body updated, pitch is up, etc)
	EvaluateNewTickRecord(Entity, pCPlayer, newtick, PreviousFireAtPelvis);

#ifdef predictTicks
	//Lets log choking information for fake lag fix
	int RecordCount = Records->size();
	if (RecordCount >= 1)
	{
		StoredNetvars *CurrentNetvars = Records->front();
		recordChokedTicks(pCPlayer, CurrentNetvars->tickschoked);
	}
#endif
}

//Process the new incoming tick to see if it is special
void LagCompensation::EvaluateNewTickRecord(CBaseEntity*const Entity, CustomPlayer*const pCPlayer, StoredNetvars*const record, bool PreviousFireAtPelvis)
{
#ifdef PRINT_FUNCTION_NAMES
	AllocateConsole();
	printf("EvaluateNewTickRecord\n");
#endif
	StoredNetvars *PreviousNetvars = pCPlayer->GetPreviousRecord();
	if (PreviousNetvars)
	{
		if (PreviousNetvars->velocity.Length2D() == 0.0f && record->velocity.Length2D() != 0.0f)
		{
			VectorAngles(record->velocity, pCPlayer->DirectionWhenFirstStartedMoving);
			pCPlayer->Personalize.correctedfakewalkindex = 0;
		}
		pCPlayer->IsBreakingLagCompensation = !PreviousNetvars->dormant && (record->networkorigin - PreviousNetvars->networkorigin).LengthSqr() > (64.0f * 64.0f);
	}
	else
	{
		pCPlayer->IsBreakingLagCompensation = false;
		pCPlayer->Personalize.correctedfakewalkindex = 0;
	}

	if (!pCPlayer->Personalize.ShouldResolve())
	{
		pCPlayer->LatestRealTick = record;
		record->IsRealTick = true;
		record->FireAtPelvis = false;
		pCPlayer->Personalize.ShotsMissed = 0;
		pCPlayer->Personalize.correctedbodyyawindex = 0;
		return;
	}

	bool IsBacktrackableOrResolvable = false;
	bool IsUnresolvable = false;

	//Check to see if the player fired this tick
	CBaseCombatWeapon *pWeapon = Entity->GetWeapon();
	if (pWeapon)
	{
		//Only count a tick as a fire bullet tick if we have stored currentweapon and the weapon has not changed, and the bullet count has changed
		if (pCPlayer->CurrentWeapon && pCPlayer->CurrentWeapon == pWeapon && !pCPlayer->CurrentWeapon->IsKnife(0) && !pCPlayer->CurrentWeapon->IsGrenade(0) && pCPlayer->CurrentWeapon->GetItemDefinitionIndex() != WEAPON_C4)
		{
			//Don't count weapon changes
			int Bullets = pWeapon->GetClipOne();
			if (pCPlayer->BulletsLeft != Bullets)
			{
				//Make this record known as a firing update
				record->FiredBullet = true;

				//Don't fire at pelvis this tick
				record->FireAtPelvis = false;

				//Remember this tick as the newest firing update
				pCPlayer->LatestFiringTick = record;
				pCPlayer->BulletsLeft = Bullets;

				if (FireWhenEnemiesShootChk.Checked)
				{
					IsBacktrackableOrResolvable = true;
				}
			}
		}
		else
		{
			pCPlayer->BulletsLeft = pWeapon->GetClipOne();
		}
	}
	pCPlayer->CurrentWeapon = pWeapon;

	//The EyeAnglesUpdated proxy isn't really used anymore in here.
	//pCPlayer->EyeAnglesUpdated = false;

	//Store spinbotting value into the record
	record->bSpinbotting = pCPlayer->Personalize.bSpinbotting;

	//Check to see if the player is using a pitch that is easily hittable
	//Todo, check smaller pitch but 
	if (ClampXr(record->eyeangles.x) <= 0.0f && !record->FiredBullet && ValveResolverChk.Checked)
	{
		//Don't count holdaim packets as a pitching up tick
		//UPDATE: holdaim doesn't seem to affect it!
		//if (pCPlayer->Personalize.m_PlayerRecords.size() <= 1 || !pCPlayer->Personalize.m_PlayerRecords.at(1)->FiredBullet)
		{
			//if (!pCPlayer->LatestFiringTick || record->eyeangles != pCPlayer->LatestFiringTick->eyeangles)
			{
				//Remember this tick as the newest pitching up update
				pCPlayer->LatestPitchUpTick = record;
				//Make this record known as a pitching up tick
				record->PitchingUp = true;
				record->FireAtPelvis = false;
				IsBacktrackableOrResolvable = true;
			}
		}
	}

	//See if lower body updated this tick
	if (pCPlayer->LowerBodyUpdated)
	{
		//Check to see if we already have a lower body update tick to delta with
		if (pCPlayer->LowerBodyCanBeDeltaed || record->FiredBullet)
		{
			//Get the delta
			float lbd = ClampYr(record->lowerbodyyaw - pCPlayer->flLastLowerBodyYaw);

			//Only consider this a lower body yaw update if the delta changed or the update is new enough.
			//Servers like to send duplicate lower body updates

			float flTimeSinceLowerBodyUpdate = Interfaces::Globals->curtime - pCPlayer->TimeLowerBodyUpdated;

			if (record->FiredBullet || lbd != 0.0f || Entity->GetVelocity().Length2D() > 0.1f || flTimeSinceLowerBodyUpdate >= (0.22f + (GetNetworkLatency(FLOW_INCOMING) + GetNetworkLatency(FLOW_OUTGOING))))
			{
				//Store new lower body record with delta if possible
				auto LowerBodyRecords = &pCPlayer->Personalize.m_PlayerLowerBodyRecords;
				StoredLowerBodyYaw *PreviousLowerBodyRecord = LowerBodyRecords->size() != 0 ? LowerBodyRecords->front() : nullptr;
				StoredLowerBodyYaw *NewLowerBodyRecord = new StoredLowerBodyYaw(Entity, &pCPlayer->CurrentNetvarsFromProxy, PreviousLowerBodyRecord);
				pCPlayer->Personalize.m_PlayerLowerBodyRecords.push_front(NewLowerBodyRecord);

				//Try to predict what the player's yaw will be
				//Exclude shots fired from prediction
				if (PredictFakeAnglesChk.Checked)
				{
					if (!record->FiredBullet && PredictLowerBodyYaw(pCPlayer, Entity, lbd))
						IsBacktrackableOrResolvable = true;
					else
						IsUnresolvable = true;
				}
				else
					IsBacktrackableOrResolvable = true; //FIXME: possible logic error on this line

				//Remember this tick as the newest lower body update
				if (!record->FiredBullet)
					pCPlayer->LatestLowerBodyUpdate = record;

				//Make this record known as a lower body update
				record->LowerBodyUpdated = true;

				pCPlayer->Personalize.correctedbodyyawindex = 0;

				//Set time stamp
				pCPlayer->TimeLowerBodyUpdated = Interfaces::Globals->curtime;
			}
		}
		else
		{
			//First lower body update
			//Remember this tick as the newest lower body update
			//pCPlayer->LatestLowerBodyUpdate = record;
			//Set time stamp
			//pCPlayer->TimeLowerBodyUpdated = Interfaces::Globals->curtime;
			IsBacktrackableOrResolvable = true;
			pCPlayer->LowerBodyCanBeDeltaed = true;

			//Store first lower body record
			StoredLowerBodyYaw *NewLowerBodyRecord = new StoredLowerBodyYaw(Entity, &pCPlayer->CurrentNetvarsFromProxy, nullptr);
			pCPlayer->Personalize.m_PlayerLowerBodyRecords.push_back(NewLowerBodyRecord);
		}

		//Remember current lower body yaw
		pCPlayer->flLastLowerBodyYaw = record->lowerbodyyaw;
		pCPlayer->LowerBodyUpdated = false;
	}
	/*
	else
	{
		//Check to see if player is moving, if they are then we can backtrack to LBY
		Vector Velocity = Entity->GetVelocity();
		//if (Velocity.Length2D() > 0.1f && Velocity.z >= 0)
		if (Velocity.Length2D() > 0.1f && (pCPlayer->BaseEntity->GetFlags() & FL_ONGROUND))
		{
			//Remember this tick as the newest lower body update
			pCPlayer->LatestLowerBodyUpdate = record;
			//Make this record known as a lower body update
			record->LowerBodyUpdated = true;
			//Set time stamp
			pCPlayer->TimeLowerBodyUpdated = Interfaces::Globals->curtime;

			pCPlayer->PredictedAverageFakeAngle = record->lowerbodyyaw;

			IsBacktrackableTick = true;
		}
	}
	*/

	//If player didn't choke ticks then backtrack to this tick always
	if (pCPlayer->TicksChoked == 0 && !record->FiredBullet && (pCPlayer->Personalize.m_PlayerRecords.size() <= 1 || !pCPlayer->Personalize.m_PlayerRecords.at(1)->FiredBullet))
	{
		pCPlayer->LatestRealTick = record;
		pCPlayer->Personalize.correctedbodyyawindex = 0;
		//Make this record known as a real update
		record->IsRealTick = true;
		IsBacktrackableOrResolvable = true;
	}

	//Correct player's origin if it got phantomed
#if 0
	if (record->absorigin == vecZero || (record->absorigin - record->origin).Length() > 0.1f)
	{
		Entity->SetAbsOrigin(record->networkorigin);
	}
#endif

	if (IsBacktrackableOrResolvable)
	{
		//We found a tick that is possible to backtrack to or that we predicted the lower body yaw on
		record->FireAtPelvis = false;
		pCPlayer->Personalize.ShotsMissed = 0;
	}
	else if (pCPlayer->CloseFakeAngle)
	{
		//Player's lower body yaw is close enough to their real angles or it's been ages since an update, so let resolver fire at head and brute force it
		record->FireAtPelvis = false;
		//pCPlayer->Personalize.ShotsMissed = 0; //FIXME: deal with shots missed elsewhere so legs aren't continuously missed
	}
	else if (IsUnresolvable)
	{
		//Could not predict the player's real yaw so we need to just body aim them
		if (AimbotAimTorsoChk.Checked || AimbotAimArmsHandsChk.Checked || AimbotAimLegsFeetChk.Checked)
			record->FireAtPelvis = true;
		pCPlayer->Personalize.EstimateYawMode = ResolveYawModes::AverageLBYDelta;
	}
	else
	{
		//Use previous tick's fire at pelvis value since this tick was of no significant importance
		record->FireAtPelvis = PreviousFireAtPelvis;
	}
}

//Invalidates a specific tick range
void LagCompensation::InvalidateTicks(CustomPlayer* pCPlayer, int from, int to)
{
	std::deque<StoredNetvars*> *Records = &pCPlayer->Personalize.m_PlayerRecords;
	for (from; from <= to; from++)
	{
		StoredNetvars *record = Records->at(from);
		//Don't bother continuing the loop if we already invalidated the rest of the ticks
		if (!record->IsValidForBacktracking)
			break;
		record->IsValidForBacktracking = false;
		ClearImportantTicks(pCPlayer, record);
	}
}

//Removes excess ticks and invalidates ticks that are too old or invalid ticks
void LagCompensation::ValidateTickRecords(CustomPlayer *const pCPlayer, CBaseEntity*const Entity)
{
#ifdef PRINT_FUNCTION_NAMES
	AllocateConsole();
	printf("ValidateTickRecords\n");
#endif

	//First, validate all player tick records
	const auto Records = &pCPlayer->Personalize.m_PlayerRecords;

	//Remove excess records
	if (Records->size() > 72)
	{
		StoredNetvars* oldestrecord = Records->back();
		//If the oldest tick is an important update, then get rid of it
		ClearImportantTicks(pCPlayer, oldestrecord);
		//Free the memory used by the oldest tick record
		delete oldestrecord;
		//Delete the oldest tick record
		Records->pop_back();
	}

	const int maxindex = Records->size() - 1;
	for (int index = 0; index < maxindex; index++)
	{
		StoredNetvars *thisrecord = Records->at(index);
		//If this record is already invalidated, then just leave the loop
		if (!thisrecord->IsValidForBacktracking)
			break;

		//Check to see if this tick is still new enough to backtrack to
		if (!bTickIsValid(thisrecord->tickcount))
		{
			//Invalidate this and every tick older
			InvalidateTicks(pCPlayer, index, maxindex);
			break;
		}

		int nextrecord = index + 1; //Note: No overflow check needed
		StoredNetvars *prevrecord = Records->at(nextrecord);
		//If this record is already invalidated, then just leave the loop
		if (!prevrecord->IsValidForBacktracking)
			break;

		//Check to see if the previous tick is still new enough to backtrack to
		if (!bTickIsValid(prevrecord->tickcount))
		{
			//Invalidate the previous record and every tick older than it
			InvalidateTicks(pCPlayer, nextrecord, maxindex);
			break;
		}

		//Check to see if the player teleported too far from between this record and the one before it
		float OriginDelta = (thisrecord->origin - prevrecord->origin).LengthSqr();

		if (OriginDelta > (64.0f * 64.0f))
		{
			//The player teleported too far. Invalidate this and any prior ticks
			InvalidateTicks(pCPlayer, nextrecord, maxindex);
			break;
		}
	}

	//Then, remove excess lower body yaw records
	const auto LowerBodyRecords = &pCPlayer->Personalize.m_PlayerLowerBodyRecords;

	if (LowerBodyRecords->size() > 32)
	{
		//Retrieve the oldest tick record
		StoredLowerBodyYaw* oldestrecord = LowerBodyRecords->back();
		//Free the memory used by the oldest tick record
		delete oldestrecord;
		//Delete the oldest tick record
		LowerBodyRecords->pop_back();
	}

	//Finally, invalidate the blood record if it's too old
	if (pCPlayer->LatestBloodTick && !bTickIsValid(pCPlayer->LatestBloodTick->tickcount))
	{
		delete pCPlayer->LatestBloodTick;
		pCPlayer->LatestBloodTick = nullptr;
	}
}	

//Backtrack the eye angles and various other netvars
void LagCompensation::FSN_BacktrackPlayer(CustomPlayer *const pCPlayer, CBaseEntity*const Entity)
{
#ifdef PRINT_FUNCTION_NAMES
	AllocateConsole();
	printf("FSN_BacktrackPlayer\n");
#endif
	if (DisableAllChk.Checked)
		return;

#ifdef logicResolver
	pCPlayer->Personalize.JitterCounter++;

	if (WhaleDongTxt.iValue >= 1)//double handling but first statment wasn't working correctly
	{
		//THIS IS BROKEN ASF!!! LOWERBODY YAW ISN'T BEING GET SET, WAS TESTING AND SOMETIEMS IT SETS PLAYERS EYE ANGLES TO MORE THEN 35 FROM LBY! IT SHOULDN'T DO THIS
		//EVER!!! IF ANYONE CAN PLEASE LOOK AND HELP!
		StoredNetvars *CurrentNetVars = pCPlayer->GetCurrentRecord();
		if (CurrentNetVars)
		{
			auto LowerBodyRecords = &pCPlayer->Personalize.m_PlayerLowerBodyRecords;
			float flCurrentBodyYaw = CurrentNetVars->lowerbodyyaw;
			LogicResolver2_Log(Entity->GetEyeAngles(), QAngle(0, flCurrentBodyYaw, 0), pCPlayer->Personalize.lastLbyChcker != flCurrentBodyYaw, pCPlayer, LocalPlayer.Entity);
			pCPlayer->Personalize.lastLbyChcker = flCurrentBodyYaw;
		}
	}
#endif

	//Get the target tick to backtrack to
	StoredNetvars *backtracktick = pCPlayer->GetBestBacktrackTick();

	QAngle TargetEyeAngles; //Eye angles to backtrack to

	pCPlayer->ResolverFiredAtShootingAngles = false;

	if (backtracktick && !pCPlayer->IsBreakingLagCompensation)
	{
		pCPlayer->Personalize.LastResolveModeUsed = Backtracked;
		//Get destination eye angles to backtrack to
		if (backtracktick == pCPlayer->LatestFiringTick)
		{
			pCPlayer->Personalize.LastResolveModeUsed = BackTrackFire;
			pCPlayer->ResolverFiredAtShootingAngles = true;
			TargetEyeAngles = GetFiringTickAngles(pCPlayer, Entity, backtracktick);
		}
		else if (backtracktick == pCPlayer->LatestRealTick)
		{
			TargetEyeAngles = backtracktick->eyeangles;
			pCPlayer->Personalize.LastResolveModeUsed = BackTrackReal;
		}
		else if (backtracktick == pCPlayer->LatestPitchUpTick && ValveResolverChk.Checked)
		{
			TargetEyeAngles = backtracktick->eyeangles;
			pCPlayer->Personalize.LastResolveModeUsed = BackTrackUp;
		}
		else if (backtracktick == pCPlayer->LatestBloodTick)
		{
			TargetEyeAngles = backtracktick->eyeangles;
			pCPlayer->Personalize.LastResolveModeUsed = BackTrackHit;
		}
		else
		{			
			//Lower Body Yaw Tick
			pCPlayer->Personalize.LastResolveModeUsed = BackTrackLby;
			TargetEyeAngles = QAngle(backtracktick->eyeangles.x, backtracktick->lowerbodyyaw, backtracktick->eyeangles.z);

		}

		//Enemy player eye angles should be in 0-360 format
		if (TargetEyeAngles.y < 0.0f)
			TargetEyeAngles.y += 360.0f;

		GetIdealPitch(TargetEyeAngles.x, pCPlayer->Personalize.ShotsMissed, pCPlayer, Entity, LocalPlayer.Entity);

		FSN_RestoreNetvars(backtracktick, Entity, TargetEyeAngles, backtracktick->lowerbodyyaw);

		//Update animations and pose parameters only when update is received, since we are backtracking to this tick always
		//FIXME TODO: Update them anyway and then store in a separate buffer for use later but then use old record
		if (backtracktick->TickReceivedNetUpdate == Interfaces::Globals->tickcount)
		{
			Entity->SetAbsOrigin(backtracktick->networkorigin);

			clientanimating_t *animating = nullptr;
			int animflags;

			//Make sure game is allowed to client side animate. Probably unnecessary
			for (unsigned int i = 0; i < g_ClientSideAnimationList->count; i++)
			{
				clientanimating_t *tanimating = (clientanimating_t*)g_ClientSideAnimationList->Retrieve(i, sizeof(clientanimating_t));
				CBaseEntity *pAnimEntity = (CBaseEntity*)tanimating->pAnimating;
				if (pAnimEntity == Entity)
				{
					animating = tanimating;
					animflags = tanimating->flags;
					tanimating->flags |= FCLIENTANIM_SEQUENCE_CYCLE;
				}
			}

			//Update animations/poses
			Entity->UpdateClientSideAnimation();

#if 0
			//ignore host player
			//for (int i = 1; i < GetServerClientCount(); i++)
			{
				DWORD cl = GetServerClientEntity(2);
				if (cl && ((CBaseEntity*)cl)->IsActive() && ((CBaseEntity*)cl)->IsConnected())
				{
					CBaseEntity *pEnt = ServerClientToEntity(cl);
					if (pEnt)
					{
						float rendertime = pCPlayer->IsMoving ? 0.015625f : 1.0f;

						//printf("pEnt %#010x\nCL %#010x\n", (DWORD)pEnt, cl);
						float eyeyaw = pEnt->GetEyeAnglesServer().y;
						if (eyeyaw < 0.0f)
							eyeyaw += 360.0f;
						float goalfeet = pEnt->GetGoalFeetYawServer();
						if (goalfeet < 0.0f)
							goalfeet += 360.0f;
						float curfeet = pEnt->GetCurrentFeetYawServer();
						if (curfeet < 0.0f)
							curfeet += 360.0f;
						char fuckoff[128];
						sprintf(fuckoff, "EyeYaw Server %f\nGFY Server %f\nCFY Server %f\n", eyeyaw, goalfeet, curfeet);
				
						Interfaces::DebugOverlay->AddTextOverlay(Entity->GetOrigin(), rendertime, fuckoff);

						float cleyeyaw = backtracktick->eyeangles.y;
						float cllby = backtracktick->lowerbodyyaw;
						if (cllby < 0.0f)
							cllby += 360.0f;
						float clgoalfeet = Entity->GetGoalFeetYaw();
						float clcurfeet = Entity->GetCurrentFeetYaw();

						char fuckoff2[128];
						sprintf(fuckoff2, "EyeYaw Client %f\nGFY Client %f\nCFY Client %f\nLBY Client %f", cleyeyaw, clgoalfeet, clcurfeet, cllby);
						Interfaces::DebugOverlay->AddTextOverlay(Vector(Entity->GetOrigin().x, Entity->GetOrigin().y, Entity->GetOrigin().z - 15.0f), rendertime, fuckoff2);
						
						OutputServerYaws(eyeyaw, goalfeet, curfeet);
					}
				}
			}
#endif

#if 0
			float flOriginalBacktrackedEyeYaw = Entity->EyeAngles()->y;
			float flCorrectTheFuckingFeetYaw = ClampYr(backtracktick->lowerbodyyaw + 54.5f);
			if (flCorrectTheFuckingFeetYaw < 0)
				flCorrectTheFuckingFeetYaw += 360.0f;
			Entity->EyeAngles()->y = flCorrectTheFuckingFeetYaw;

			Entity->UpdateClientSideAnimation();

			Entity->EyeAngles()->y = flOriginalBacktrackedEyeYaw;
#endif
			//Restore anim flags
			if (animating)
				animating->flags = animflags;

			//Store the new animations
			Entity->CopyPoseParameters(backtracktick->flPoseParameter);
			Entity->CopyAnimLayers(backtracktick->AnimLayer);
		}
		else if (Entity->GetVelocity().Length2D() < 0.1f)
		{
			Entity->UpdateClientSideAnimation(); //this is shit, but the body_yaw pose parameter's fucked when backtracking so fuck it
			Entity->CopyPoseParameters(backtracktick->flPoseParameter);
			Entity->CopyAnimLayers(backtracktick->AnimLayer);
		}

#if 0
		if (backtracktick->tickschoked > 0 && backtracktick->ReceivedPoseParametersAndAnimations)
		{
			if (pCPlayer->Personalize.EstimateYawMode == ResolveYawModes::LinearFake)
			{
				if (Entity->GetVelocity().Length() == 0.0f)
				{
					//Correct the feet yaw for backtracking purposes
					CorrectFeetYaw(Entity, ClampYr(TargetEyeAngles.y), 50.0f);
				}
			}
		}
#endif

	}
	else
	{
		pCPlayer->Personalize.LastResolveModeUsed = NotBackTracked;

		//Player is not able to be backtracked
		//Use current netvars and try to extrapolate their real angles
		const auto &records = pCPlayer->Personalize.m_PlayerRecords;
		StoredNetvars* currentvars = records.front();
		TargetEyeAngles = currentvars->eyeangles;

		auto stat1 = START_PROFILING("GetIdealPitch");
		GetIdealPitch(TargetEyeAngles.x, pCPlayer->Personalize.JitterCounter, pCPlayer, Entity, LocalPlayer.Entity);
		END_PROFILING(stat1);

		auto stat = START_PROFILING("GetIdealYaw");
		GetIdealYaw(TargetEyeAngles.y, pCPlayer->Personalize.correctedindex, pCPlayer, Entity, LocalPlayer.Entity);
		END_PROFILING(stat);

		bool bForceAnimationUpdate = Entity->EyeAngles()->x != TargetEyeAngles.x || Entity->EyeAngles()->y != TargetEyeAngles.y;
		bool bReceivedUpdateThisTick = currentvars->TickReceivedNetUpdate == Interfaces::Globals->tickcount;

#if 1
		FSN_RestoreNetvars(currentvars, Entity, TargetEyeAngles, currentvars->lowerbodyyaw);
#else
		bool AnglesWereModified = TargetEyeAngles.y != currentvars->eyeangles.y || TargetEyeAngles.x != currentvars->eyeangles.x;

		if (AnglesWereModified)
		{
			FSN_RestoreNetvars(currentvars, Entity, TargetEyeAngles, TargetEyeAngles.y);
		}
		else
		{
			FSN_RestoreNetvars(currentvars, Entity, TargetEyeAngles, Entity->GetLowerBodyYaw());
		}
#endif

		#if 0
		if (ShouldResolve && pCPlayer->TicksChoked > 0 && Entity->GetVelocity().Length() == 0.0f)
		{
			if (pCPlayer->Personalize.EstimateYawMode == ResolveYawModes::LinearFake)
			{
				//Correct the feet yaw for backtracking purposes
				CorrectFeetYaw(Entity, ClampYr(TargetEyeAngles.y), 50.0f);
			}
		}
		#endif

		if (bReceivedUpdateThisTick || pCPlayer->Personalize.ShouldResolve())
		{
			//Force origin to fix phantom players
			StoredNetvars *previousvars = pCPlayer->GetPreviousRecord();
			if (!previousvars)
				Entity->SetAbsOrigin(currentvars->networkorigin);
			else
			{
				if (currentvars->networkorigin != previousvars->networkorigin || *Entity->GetAbsOrigin() != currentvars->networkorigin)
					Entity->SetAbsOrigin(currentvars->networkorigin);
			}
		}

		if (bReceivedUpdateThisTick || bForceAnimationUpdate)
		{
			//Update animations and pose parameters
			clientanimating_t *animating = nullptr;
			int animflags;

			//Make sure game is allowed to client side animate. Probably unnecessary
			for (unsigned int i = 0; i < g_ClientSideAnimationList->count; i++)
			{
				clientanimating_t *tanimating = (clientanimating_t*)g_ClientSideAnimationList->Retrieve(i, sizeof(clientanimating_t));
				CBaseEntity *pAnimEntity = (CBaseEntity*)tanimating->pAnimating;
				if (pAnimEntity == Entity)
				{
					animating = tanimating;
					animflags = tanimating->flags;
					tanimating->flags |= FCLIENTANIM_SEQUENCE_CYCLE;
					break;
				}
			}

			//Update animations/poses
			Entity->UpdateClientSideAnimation();

			//Restore anim flags
			if (animating)
				animating->flags = animflags;
		}

		//Store the new animations
		//WARNING: animations are not valid here until PostRenderStart if we do not handle animations manually
		Entity->CopyPoseParameters(currentvars->flPoseParameter);
		Entity->CopyAnimLayers(currentvars->AnimLayer);
	}
}

//Clears all lag compensation records for this player
void LagCompensation::ClearLagCompensationRecords(CustomPlayer*const pCPlayer)
{
	Personalize* pPersonalize = &pCPlayer->Personalize;
	if (!pPersonalize->m_PlayerRecords.empty())
	{
		//Free memory used by records
		for (auto record : pPersonalize->m_PlayerRecords)
			delete record;
		pPersonalize->ShotsMissed = 0;
		pPersonalize->m_PlayerRecords.clear();
		pCPlayer->LowerBodyCanBeDeltaed = false;
		pCPlayer->LatestFiringTick = nullptr;
		pCPlayer->LatestLowerBodyUpdate = nullptr;
		pCPlayer->LatestPitchUpTick = nullptr;
		pCPlayer->LatestRealTick = nullptr;
	}

	if (!pPersonalize->m_PlayerLowerBodyRecords.empty())
	{
		//Free memory used by records
		for (auto record : pPersonalize->m_PlayerLowerBodyRecords)
			delete record;
		pPersonalize->m_PlayerLowerBodyRecords.clear();
	}

	if (pCPlayer->LatestBloodTick)
	{
		delete pCPlayer->LatestBloodTick;
		pCPlayer->LatestBloodTick = nullptr;
	}

	ClearPredictedAngles(pCPlayer);
}

//Marks all lag compensation records as dormant so lag compensation doesn't think we're teleporting
void LagCompensation::MarkLagCompensationRecordsDormant(CustomPlayer*const pCPlayer)
{

	Personalize* pPersonalize = &pCPlayer->Personalize;
	if (!pPersonalize->m_PlayerRecords.empty())
	{
		for (auto record : pPersonalize->m_PlayerRecords)
		{
			//Don't continue through if we already marked records as dormant
			if (record->dormant)
				break;

			record->dormant = true;
			record->IsValidForBacktracking = false;
		}
	}
}

//Clears all blood records for this player
void LagCompensation::ClearBloodRecords(CustomPlayer*const pCPlayer)
{
	Personalize* pPersonalize = &pCPlayer->Personalize;
	if (!pPersonalize->m_BloodRecords.empty())
	{
		//Free memory used by records
		for (auto record : pPersonalize->m_BloodRecords)
			delete record;
		pPersonalize->m_BloodRecords.clear();
	}
}

//If the important tick matches the record, delete it
void LagCompensation::ClearImportantTicks(CustomPlayer *const pCPlayer, StoredNetvars *const record)
{
	if (record == pCPlayer->LatestFiringTick)
		pCPlayer->LatestFiringTick = nullptr;
	if (record == pCPlayer->LatestLowerBodyUpdate)
		pCPlayer->LatestLowerBodyUpdate = nullptr;
	if (record == pCPlayer->LatestPitchUpTick)
		pCPlayer->LatestPitchUpTick = nullptr;
	if (record == pCPlayer->LatestRealTick)
		pCPlayer->LatestRealTick = nullptr;
	if (record == pCPlayer->LatestReloadingTick)
		pCPlayer->LatestReloadingTick = nullptr;
	if (record == pCPlayer->LatestBloodTick)
		pCPlayer->LatestBloodTick = nullptr;
}

//Mark predicted angles as invalid
void LagCompensation::ClearPredictedAngles(CustomPlayer *const pCPlayer)
{
	pCPlayer->PredictedAverageFakeAngle = -999.9f;
	pCPlayer->PredictedRandomFakeAngle = -999.9f;
	pCPlayer->PredictedLinearFakeAngle = -999.9f;
	pCPlayer->PredictedStaticFakeAngle = -999.9f;
	pCPlayer->LowerBodyCanBeDeltaed = false;
}

void LagCompensation::ValidateNetVarsWithProxy(CustomPlayer *const pCPlayer)
{
	CBaseEntity *Entity = pCPlayer->BaseEntity;
	float flSimulationTime = Entity->GetSimulationTime();

	StoredNetvars *ProxyVars = &pCPlayer->CurrentNetvarsFromProxy;
	if (ProxyVars->simulationtime < flSimulationTime)
	{
		ProxyVars->simulationtime = flSimulationTime;
	}
	else if (ProxyVars->simulationtime > Entity->GetSimulationTime())
	{
		Entity->SetSimulationTime(ProxyVars->simulationtime);
	}
	if (ProxyVars->networkorigin == vecZero && Entity->GetNetworkOrigin() != vecZero)
	{
		ProxyVars->networkorigin = Entity->GetNetworkOrigin();
	}
	else if (Entity->GetNetworkOrigin() == vecZero && ProxyVars->networkorigin != vecZero)
	{
		Entity->SetNetworkOrigin(ProxyVars->networkorigin);
	}
	if (ProxyVars->origin == vecZero && Entity->GetOrigin() != vecZero)
	{
		ProxyVars->origin = Entity->GetOrigin();
	}
	else if (Entity->GetOrigin() == vecZero && ProxyVars->origin != vecZero)
	{
		Entity->SetOrigin(ProxyVars->origin);
	}
	if (ProxyVars->absorigin == vecZero && *Entity->GetAbsOrigin() != vecZero)
	{
		ProxyVars->absorigin = *Entity->GetAbsOrigin();
	}
	if (ProxyVars->eyeangles == angZero && Entity->GetEyeAngles() != angZero)
	{
		ProxyVars->eyeangles = Entity->GetEyeAngles();
	}
	else if (Entity->GetEyeAngles() == angZero && ProxyVars->eyeangles != angZero)
	{
		Entity->GetEyeAngles() = angZero;
	}
	if (ProxyVars->velocity == vecZero && Entity->GetVelocity() != vecZero)
	{
		ProxyVars->velocity = Entity->GetVelocity();
	}
}

//Update was received from the server, so update netvars
void LagCompensation::NetUpdatePlayer(CustomPlayer *const pCPlayer, CBaseEntity*const Entity)
{
	VMP_BEGINMUTILATION("NUP")
#ifdef PRINT_FUNCTION_NAMES
	AllocateConsole();
	printf("NetUpdatePlayer\n");
#endif
	const float flNewSimulationTime = Entity->GetSimulationTime();

	//See if the simulation time is valid yet or if the entity is not valid
	if ((flNewSimulationTime == 0.0f || !Entity->IsPlayer() || Entity->GetDormant() || !Entity->GetAlive()) && Entity->index <= MAX_PLAYERS)
	{
		MarkLagCompensationRecordsDormant(pCPlayer);
		ClearPredictedAngles(pCPlayer);
		pCPlayer->Personalize.correctedfakewalkindex = 0;
		return;
	}

	//Get address of player records
	auto Records = &pCPlayer->Personalize.m_PlayerRecords;

	//Store new tick record if simulation time changed or there are no records
	if (Records->empty())
	{
		ClearPredictedAngles(pCPlayer);
		NewTickRecordReceived(Entity, pCPlayer, flNewSimulationTime);
	}
	else
	{
		StoredNetvars *CurrentRecords = Records->front();
		//Also check origin for sanity
		if (flNewSimulationTime > CurrentRecords->simulationtime || Entity->GetNetworkOrigin() != CurrentRecords->networkorigin)
		{
			NewTickRecordReceived(Entity, pCPlayer, flNewSimulationTime);
		}
	}

	//Validate all the tick records. Remove excess ticks, invalidate any that are too old or those in which the player teleported too far
	ValidateTickRecords(pCPlayer, Entity);

	//Restore player netvars to the best possible tick
	FSN_BacktrackPlayer(pCPlayer, Entity);
	VMP_END
}

//Finished FRAME_NET_UPDATE_START, and NET_UPDATE_END has been called
void LagCompensation::PostNetUpdatePlayer(CustomPlayer *const pCPlayer, CBaseEntity*const Entity)
{
#ifdef PRINT_FUNCTION_NAMES
	AllocateConsole();
	printf("PostNetUpdatePlayer\n");
#endif
	if (DisableAllChk.Checked || !RemoveInterpChk.Checked)
		Entity->EnableInterpolation();
	else
		Entity->DisableInterpolation();

	//Get address of player records
	auto Records = &pCPlayer->Personalize.m_PlayerRecords;
	int RecordCount = Records->size();

	if (RecordCount)
	{
		//Store netvars that are only updated during PostDataUpdate for this player
		StoredNetvars *CurrentNetvars = Records->front();
		if (!CurrentNetvars->ReceivedPostNetUpdateVars)
		{
			CurrentNetvars->absangles = Entity->GetAbsAngles();
			CurrentNetvars->angles = Entity->GetAngleRotation();
			CurrentNetvars->absorigin = *Entity->GetAbsOrigin(); //Note: not valid from FRAME_NET_POSTDATAUPDATE_START
			CurrentNetvars->origin = Entity->GetOrigin(); //Note: not valid from FRAME_NET_POSTDATAUPDATE_START
			//CurrentNetvars->networkorigin = Entity->GetNetworkOrigin();
			CurrentNetvars->velocity = Entity->GetVelocity(); //Note: not valid from FRAME_NET_POSTDATAUPDATE_START
			CurrentNetvars->basevelocity = Entity->GetBaseVelocity();

			if (CurrentNetvars->velocity.Length() > 0.1f)
			{
				//Clear predicted angles
				pCPlayer->PredictedLinearFakeAngle = -999.9f;
				pCPlayer->PredictedRandomFakeAngle = -999.9f;
				pCPlayer->PredictedStaticFakeAngle = -999.9f;
				pCPlayer->PredictedAverageFakeAngle = -999.9f;

				pCPlayer->IsMoving = true;
			}
			else
			{
				pCPlayer->IsMoving = false;
			}

			//IsBreakingLagCompensation is now handled in EvaluateNewTickRecord
			//StoredNetvars *PreviousNetvars = pCPlayer->GetPreviousRecord();
			//pCPlayer->IsBreakingLagCompensation = PreviousNetvars && !PreviousNetvars->dormant && (CurrentNetvars->networkorigin - PreviousNetvars->networkorigin).LengthSqr() > (64.0f * 64.0f);

			CurrentNetvars->ReceivedPostNetUpdateVars = true;
		}
	}
}

void LagCompensation::PreRenderStart()
{
#ifdef PRINT_FUNCTION_NAMES
	AllocateConsole();
	printf("PreRenderStart\n");
#endif
	if (LocalPlayer.Entity)
	{
		//Need to iterate the whole thing since rendering is on a different thread than networking..
		int MaxEntities = 64;// Interfaces::ClientEntList->GetHighestEntityIndex();
		int NumPlayers = GetClientCount();
		int CountedPlayers = 0;

		if (NumPlayers)
		{
			for (int i = 0; i <= MaxEntities; i++)
			{
				CBaseEntity* Entity = Interfaces::ClientEntList->GetClientEntity(i);

				if (Entity && Entity->IsPlayer())
				{
					if (Entity != LocalPlayer.Entity && !Entity->GetDormant())
					{
						if (!DisableAllChk.Checked && RemoveInterpChk.Checked)
							Entity->DisableInterpolation();
						else
							Entity->EnableInterpolation();

						//Entity->FrameAdvance(0.0f);
					}

					if (++CountedPlayers == NumPlayers)
						break;
				}
			}
		}

		if (!DisableAllChk.Checked)
		{
			//Stop the game from updating player animations and pose parameters
			for (unsigned int i = 0; i < g_ClientSideAnimationList->count; i++)
			{
				clientanimating_t *animating = (clientanimating_t*)g_ClientSideAnimationList->Retrieve(i, sizeof(clientanimating_t));
				CBaseEntity *Entity = (CBaseEntity*)animating->pAnimating;
				if (Entity->IsPlayer() && Entity != LocalPlayer.Entity && !Entity->GetDormant() && Entity->GetAlive())
				{
					CustomPlayer *pCPlayer = &AllPlayers[Entity->index];
					if (pCPlayer->Personalize.ShouldResolve())
					{
						unsigned int flags = animating->flags;
						pCPlayer->ClientSideAnimationFlags = flags;
						pCPlayer->HadClientAnimSequenceCycle = (flags & FCLIENTANIM_SEQUENCE_CYCLE);
						if (pCPlayer->HadClientAnimSequenceCycle)
						{
							StoredNetvars *CurrentVars = pCPlayer->GetCurrentRecord();
							if (pCPlayer->IsBreakingLagCompensation && Interfaces::Globals->tickcount != CurrentVars->TickReceivedNetUpdate)
							{
								Entity->UpdateClientSideAnimation();
								//Store the new animations
								Entity->CopyPoseParameters(CurrentVars->flPoseParameter);
								Entity->CopyAnimLayers(CurrentVars->AnimLayer);
							}
							animating->flags &= ~FCLIENTANIM_SEQUENCE_CYCLE;
						}
					}
				}
			}
		}
	}
	printf("PreRenderStart Finished\n");
}

//Stores player pose parameters and animations for this tick since CreateMove happens before these are updated
void LagCompensation::PostRenderStart()
{
#ifdef PRINT_FUNCTION_NAMES
	AllocateConsole();
	printf("PostRenderStart\n");
#endif
	if (LocalPlayer.Entity && !DisableAllChk.Checked)
	{
		//No need to copy animations from here anymore as we handle them ourselves now
#if 0
		int MaxEntities = 64;// Interfaces::ClientEntList->GetHighestEntityIndex();
		int NumPlayers = GetClientCount();
		int CountedPlayers = 0;

		if (NumPlayers)
		{
			for (int i = 0; i <= MaxEntities; i++)
			{
				CBaseEntity* Entity = Interfaces::ClientEntList->GetClientEntity(i);

				if (Entity && Entity->IsPlayer())
				{
					CustomPlayer* pCPlayer = &AllPlayers[i];

					if (pCPlayer->Active && !pCPlayer->IsLocalPlayer && !Entity->GetDormant() && Entity->GetAlive() && pCPlayer->Personalize.m_PlayerRecords.size() != 0)
					{
						StoredNetvars *CurrentNetvars = pCPlayer->Personalize.m_PlayerRecords.front();


						if (!CurrentNetvars->ReceivedPoseParametersAndAnimations)
						{
							//Store poses for the current tick
							Entity->CopyPoseParameters(CurrentNetvars->flPoseParameter);

							//Store animations for the current tick
							Entity->CopyAnimLayers(CurrentNetvars->AnimLayer);

							CurrentNetvars->GoalFeetYaw = Entity->GetGoalFeetYaw();
							CurrentNetvars->CurrentFeetYaw = Entity->GetCurrentFeetYaw();
#ifdef _DEBUG
#ifdef SHOW_FEET_YAW
							OutputFeetYawInformation(pCPlayer, Entity);
#endif
#endif
							CurrentNetvars->ReceivedPoseParametersAndAnimations = true;
						}
					}

					if (++CountedPlayers == NumPlayers)
						break;
				}
			}
		}
#endif
		//Restore original animation flags after rendering
		for (unsigned int i = 0; i < g_ClientSideAnimationList->count; i++)
		{
			clientanimating_t *animating = (clientanimating_t*)g_ClientSideAnimationList->Retrieve(i, sizeof(clientanimating_t));
			CBaseEntity *Entity = (CBaseEntity*)animating->pAnimating;
			if (Entity->IsPlayer() && Entity != LocalPlayer.Entity && !Entity->GetDormant() && Entity->GetAlive())
			{
				CustomPlayer *pCPlayer = &AllPlayers[Entity->index];
				if (pCPlayer->HadClientAnimSequenceCycle && pCPlayer->Personalize.ShouldResolve())
				{
					animating->flags |= FCLIENTANIM_SEQUENCE_CYCLE;
				}
			}
		}
	}
}

//Gets the ideal angles for backtracking to a tick a player shot
QAngle LagCompensation::GetFiringTickAngles(CustomPlayer *const pCPlayer, CBaseEntity*const Entity, StoredNetvars*const  vars)
{
	QAngle TargetEyeAngles = vars->eyeangles;

	if (pCPlayer->Personalize.ShouldResolve() && FiringCorrectionsChk.Checked)
	{
		if (vars->tickschoked != 0)
		{
			StoredNetvars *TickBeforeFiring = nullptr;
			bool FoundFiringTick = false;
			for (auto tick : pCPlayer->Personalize.m_PlayerRecords)
			{
				if (FoundFiringTick)
				{
					TickBeforeFiring = tick;
					break;
				}
				else if (tick == vars)
				{
					FoundFiringTick = true;
				}
			}

			if (TickBeforeFiring)
			{
				if (pCPlayer->Personalize.correctedresolvewhenshootingindex % 2 == 1)
				{
					TargetEyeAngles.x = TickBeforeFiring->eyeangles.x;
					//if (!vars->LowerBodyUpdated)
						TargetEyeAngles.y = TickBeforeFiring->eyeangles.y;
					/*else
					{
						//TargetEyeAngles.y = vars->lowerbodyyaw;
					}
					*/
				}
				else
				{
					TargetEyeAngles.x = vars->eyeangles.x;
					//if (!vars->LowerBodyUpdated)
						TargetEyeAngles.y = Entity->GetGoalFeetYaw();// TickBeforeFiring->lowerbodyyaw;
					/*else
					{
						TargetEyeAngles.y = vars->lowerbodyyaw;
					}
					*/
				}
			}
#if 0
			if (TargetEyeAngles.x > 80.0f)
			{
				QAngle angletous = CalcAngle(Entity->GetEyePosition(), LocalPlayer.Entity->GetEyePosition());
				TargetEyeAngles.x = angletous.x;
				//TargetEyeAngles.y = angletous.y - 45.0f;
			}
#endif

#if 0
			if (pCPlayer->LatestFiringTick->tickschoked > 0)
			{
				//Extrapolate yaw
				//FIXME: THIS IS FUCKED, WE NEED TO STORE PREDICTED ANGLES FOR EACH TICK RECORDS AND USE THOSE, NOT THE CURRENT TICK'S PREDICTED ONES!!
				GetIdealYaw(TargetEyeAngles.y, pCPlayer->Personalize.correctedindex, pCPlayer, Entity, LocalPlayer.Entity);
			}
#endif
		}

		if (TargetEyeAngles.y < 0.0f)
			TargetEyeAngles.y += 360.0f;
	}

	return TargetEyeAngles;

#if 0
	if (pCPlayer->Personalize.ShouldResolve() && FiringCorrectionsChk.Checked && pCPlayer->Personalize.correctedresolvewhenshootingindex > 0)
	{
		auto Records = &pCPlayer->Personalize.m_PlayerRecords;
		const int NumRecords = Records->size();
		if (NumRecords < 2)
		{
			return TargetEyeAngles;
		}
		
		int index = 0;
		for (auto& Record : *Records)
		{
			if (Record == pCPlayer->LatestFiringTick)
				break;
			index++;
		}

		if (++index <= (NumRecords - 1))
		{
			//Use Previous tick's pitch
			auto& TickBeforeFiring = Records->at(index);
			TargetEyeAngles.x = TickBeforeFiring->eyeangles.x;
		}

		//Extrapolate yaw
		GetIdealYaw(TargetEyeAngles.y, pCPlayer->Personalize.correctedindex, pCPlayer, Entity, LocalPlayer.Entity);

		if (TargetEyeAngles.y < 0.0f)
			TargetEyeAngles.y += 360.0f;
	}

	return TargetEyeAngles;
#endif
}

//Called from CreateMove to backtrack a player to the best possible tick, or compensates for fake lag
bool LagCompensation::CM_BacktrackPlayer(CustomPlayer *const pCPlayer, CBaseEntity*const pLocalEntity, CBaseEntity*const Entity)
{
#ifdef PRINT_FUNCTION_NAMES
	AllocateConsole();
	printf("CM_BacktrackPlayer\n");
#endif
	//Check to see if we have a valid tick to backtrack to
	StoredNetvars *BestBacktrackTick = pCPlayer->GetBestBacktrackTick();

	if (BestBacktrackTick && BestBacktrackTick->IsValidForBacktracking && !pCPlayer->IsBreakingLagCompensation)
	{
		//Only backtrack to the best tick if the pose parameters and animations have been saved
		if (BestBacktrackTick->ReceivedPoseParametersAndAnimations)
		{
			//Restore Animations and Pose Parameters
			CM_RestoreAnimations(BestBacktrackTick, Entity);
		}

		//Restore Netvars
		CM_RestoreNetvars(BestBacktrackTick, Entity, &BestBacktrackTick->networkorigin);

		if (!BestBacktrackTick->ResolvedFromBloodSpawning && !BestBacktrackTick->IsRealTick && BestBacktrackTick->velocity.Length() == 0.0f)
		{
			switch (pCPlayer->Personalize.correctedbodyyawindex)
			{
				//case 0 uses current pose parameter for the record, so is already set
				case 1:
					Entity->SetPoseParameterScaled(11, 0.0f);
					break;
				case 2:
					Entity->SetPoseParameterScaled(11, -0.5f);
					break;
				case 3:
					Entity->SetPoseParameterScaled(11, 0.5f);
			}
		}

		pCPlayer->BacktrackTick = BestBacktrackTick->tickcount;
		pCPlayer->Backtracked = true;
		pCPlayer->NetvarsModified = true;
		return true;
	}

	//Couldn't backtrack the player, so try to extrapolate positions
	pCPlayer->BacktrackTick = 0;
	pCPlayer->Backtracked = false;
	if (pCPlayer->Personalize.ShouldResolve())
		CompensateLag(pCPlayer);
	return false;
}

//Simulates player movement for x amount of ticks
void LagCompensation::EnginePredictPlayer(CBaseEntity* Entity, int ticks, bool CalledFromCreateMove, bool WasJumping, bool Ducking, bool ForwardVelocity, CustomPlayer*const pCPlayer)
{
	if (ForwardVelocity)
	{
		QAngle VelocityAngles;
		VectorAngles(Entity->GetVelocity(), VelocityAngles);
		VelocityAngles.Clamp();
		Entity->SetEyeAngles(VelocityAngles);
	}

	for (int i = 0; i < ticks; i++)
	{
		QAngle EyeAngles = Entity->GetEyeAngles();
		EyeAngles.Clamp();
		QAngle VelocityAngles;
		VectorAngles(Entity->GetVelocity(), VelocityAngles);
		float yawdelta = ClampYr(EyeAngles.y - VelocityAngles.y);
		CUserCmd newcmd;
		CMoveData newdata;
		memset(&newdata, 0, sizeof(CMoveData));
		memset(&newcmd, 0, sizeof(CUserCmd));
		newcmd.viewangles = Entity->GetEyeAngles();
		newcmd.viewangles.Clamp();
		newcmd.tick_count = CurrentUserCmd.cmd->tick_count;

		if (Ducking)
			newcmd.buttons = IN_DUCK;

		if ((WasJumping && Entity->GetFlags() & FL_ONGROUND) || Entity->IsInDuckJump())
			newcmd.buttons |= IN_JUMP;

		if (*(DWORD*)((DWORD)Entity + 0x389D))
			newcmd.buttons |= IN_SPEED;

		newcmd.command_number = CurrentUserCmd.cmd->command_number;
		newcmd.hasbeenpredicted = false;

		if (Entity->GetVelocity().Length() != 0.0f)
		{
			if (yawdelta < 0.0f)
				yawdelta += 360.0f;
			float yawdeltaabs = fabsf(yawdelta);

			if (yawdelta > 359.5f || yawdelta < 0.5f)
			{
				newcmd.buttons |= IN_FORWARD;
				newcmd.forwardmove = 450.0f;
				newcmd.sidemove = 0.0f;
			}
			else if (yawdelta > 179.5f && yawdelta < 180.5f)
			{
				newcmd.buttons |= IN_BACK;
				newcmd.forwardmove = -450.0f;
				newcmd.sidemove = 0.0f;
			}
			else if (yawdelta > 89.5f && yawdelta < 90.5f)
			{
				newcmd.buttons |= IN_MOVERIGHT;
				newcmd.forwardmove = 0.0f;
				newcmd.sidemove = 450.0f;
			}
			else if (yawdelta > 269.5f && yawdelta < 270.5f)
			{
				newcmd.buttons |= IN_MOVELEFT;
				newcmd.forwardmove = 0.0f;
				newcmd.sidemove = -450.0f;
			}
			else if (yawdelta > 0.0f && yawdelta < 90.0f)
			{
				newcmd.buttons |= IN_FORWARD;
				newcmd.buttons |= IN_MOVERIGHT;
				newcmd.forwardmove = 450.0f;
				newcmd.sidemove = 450.0f;
			}
			else if (yawdelta > 90.0f && yawdelta < 180.0f)
			{
				newcmd.buttons |= IN_BACK;
				newcmd.buttons |= IN_MOVERIGHT;
				newcmd.forwardmove = -450.0f;
				newcmd.sidemove = 450.0f;
			}
			else if (yawdelta > 180.0f && yawdelta < 270.0f)
			{
				newcmd.buttons |= IN_BACK;
				newcmd.buttons |= IN_MOVELEFT;
				newcmd.forwardmove = -450.0f;
				newcmd.sidemove = -450.0f;
			}
			else
			{
				//yawdelta > 270.0f && yawdelta > 0.0f
				newcmd.buttons |= IN_FORWARD;
				newcmd.buttons |= IN_MOVELEFT;
				newcmd.forwardmove = 450.0f;
				newcmd.sidemove = -450.0f;
			}
		}
		pMoveHelperServer = (CMoveHelperServer*)ReadInt(ReadInt(pMoveHelperServerPP));
		pMoveHelperServer->SetHost((CBasePlayer*)Entity);
		DWORD pOriginalPlayer = *(DWORD*)m_pPredictionPlayer;
		*(DWORD*)m_pPredictionPlayer = (DWORD)Entity;
		int flags = Entity->GetFlags();
		UINT rand = MD5_PseudoRandom(CurrentUserCmd.cmd->command_number) & 0x7fffffff;
		UINT originalrandomseed = *(UINT*)m_pPredictionRandomSeed;
		*(UINT*)m_pPredictionRandomSeed = rand;
		float frametime = Interfaces::Globals->frametime;
		float curtime = Interfaces::Globals->curtime;
		float tickinterval = Interfaces::Globals->interval_per_tick;
		Interfaces::Globals->frametime = tickinterval;
		int tickbase = Entity->GetTickBase();
		Interfaces::Globals->curtime = tickbase * tickinterval;
		CUserCmd *currentcommand = GetCurrentCommand(Entity);
		SetCurrentCommand(Entity, &newcmd);

		//Interfaces::GameMovement->StartTrackPredictionErrors((CBasePlayer*)Entity);

		Interfaces::Prediction->SetupMove((C_BasePlayer*)Entity, &newcmd, pMoveHelperServer, &newdata);
		Interfaces::GameMovement->ProcessMovement((CBasePlayer*)Entity, &newdata);
		//Interfaces::GameMovement->FullWalkMove();
		//Interfaces::Prediction->RunCommand((C_BasePlayer*)LocalPlayer.Entity, &newcmd, pMoveHelperServer);
		Interfaces::Prediction->FinishMove((C_BasePlayer*)Entity, &newcmd, &newdata);

		//Interfaces::GameMovement->FinishTrackPredictionErrors((CBasePlayer*)Entity);
		SetCurrentCommand(Entity, currentcommand);
		Interfaces::Globals->curtime = curtime;
		Interfaces::Globals->frametime = frametime;
		Entity->SetFlags(flags);
		*(DWORD*)m_pPredictionPlayer = pOriginalPlayer;
		pMoveHelperServer->SetHost(!CalledFromCreateMove ? nullptr : EnginePredictChk.Checked ? LocalPlayer.Entity : nullptr);
		*(UINT*)m_pPredictionRandomSeed = originalrandomseed;
	}
}

//Aimware Simulate player movement for fake lag. Don't allow players to be teleported into walls
void LagCompensation::SimulateMovement(CSimulationData& data, bool in_air) {
	if (!(data.m_nFlags & FL_ONGROUND))
		data.velocity.z -= (Interfaces::Globals->interval_per_tick * sv_gravity->GetFloat());
	else if (in_air)
		data.velocity.z = sqrt(91200.f);


	Vector mins = data.m_pEntity->GetMins();
	Vector maxs = data.m_pEntity->GetMaxs();

	Vector src = data.networkorigin;
	Vector end = src + (data.velocity * Interfaces::Globals->interval_per_tick);

	Ray_t ray;
	ray.Init(src, end, mins, maxs);

	trace_t trace;
	CTraceFilter filter;
	filter.m_icollisionGroup = COLLISION_GROUP_NONE;
	filter.pSkip = (IHandleEntity*)data.m_pEntity;
	Interfaces::EngineTrace->TraceRay(ray, CONTENTS_SOLID, &filter, &trace);

	if (trace.fraction != 1.f)
	{
		for (int i = 0; i < 2; ++i)
		{
			data.velocity -= trace.plane.normal * data.velocity.Dot(trace.plane.normal);

			float dot = data.velocity.Dot(trace.plane.normal);
			if (dot < 0.f)
			{
				data.velocity.x -= dot * trace.plane.normal.x;
				data.velocity.y -= dot * trace.plane.normal.y;
				data.velocity.z -= dot * trace.plane.normal.z;
			}

			end = trace.endpos + (data.velocity * (Interfaces::Globals->interval_per_tick * (1.f - trace.fraction)));
			ray.Init(trace.endpos, end, mins, maxs);
			Interfaces::EngineTrace->TraceRay(ray, CONTENTS_SOLID, &filter, &trace);

			if (trace.fraction == 1.f)
				break;
		}
	}

	data.networkorigin = trace.endpos;
	end = trace.endpos;
	end.z -= 2.f;

	ray.Init(data.networkorigin, end, mins, maxs);
	Interfaces::EngineTrace->TraceRay(ray, CONTENTS_SOLID, &filter, &trace);

	data.m_nFlags &= ~FL_ONGROUND;

	if (trace.fraction != 1.f && trace.plane.normal.z > 0.7f)
		data.m_nFlags |= FL_ONGROUND;
}

//MOVE ME
#if 0
inline float NormalizeFloat(float angle) {
	auto revolutions = angle / 360.f;
	if (angle > 180.f || angle < -180.f) {
		revolutions = round(abs(revolutions));

		if (angle < 0.f)
			angle = (angle + 360.f * revolutions);

		else
			angle = (angle - 360.f * revolutions);

		return angle;
	}

	return angle;
}
#endif

//Please make no attempt that I don't know csgo anywhere near as well as many people
//Nor do I understand how you are setting and obtaining values so this is probably wrong
//However, working off the real physics of life and maths and logic it should very 
//much work correctly

//So this is very simple, calculate new velocity for every chocked tick from static acceleration
//Add that updated velocity to netowrked origin for every chocked tick
//Will work for any case where linear acceleration is used, so that is always on the Z
//X and Y it will be more common then linear velocity

//Known issues, if player is falling and land on the ground within their chooked ticks it will fuck up
//Player may be extrapolated into walls and into the ground
//Doesn't take into account your ping
#include <iostream>

void LagCompensation::JakesSimpleMathLagComp(CustomPlayer*const pCPlayer, int ticksChoked, StoredNetvars* CurrentTick, StoredNetvars *PreviousTick)
{
	//accuracy check

#ifdef _DEBUG
	AllocateConsole();
#endif

	//I'm assuming that this is only being done when lag comp has been broken and extrapolation is actually needed
	//Probably bad assumption
	//velocity > (64.0f * 64.0f)

	//How much it accelerates per tick, possibly wrong logic. not sure
	//Vector Acceleration = ((PreviousTick->velocity - CurrentTick->velocity) / ticksChoked) / Interfaces::Globals->interval_per_tick;

	//If entity is in air apply acceleration due to gravity
	//if (!CurrentTick->flags & FL_ONGROUND)
	//	Acceleration.z = -sv_gravity->GetFloat();

	//Acceleration is now units/tick
	Vector Velocity = CurrentTick->velocity;
	Vector CurrentPos = CurrentTick->networkorigin;

	//Accuracy checks

#ifdef _DEBUG
	if (pCPlayer->pred_LastPos == vecZero)
	{
		pCPlayer->pred_LastPos = CurrentTick->networkorigin;
		pCPlayer->pred_HowClose = CurrentTick->networkorigin;
	}

	//if (ticksChoked <= 1)
	//{
	//	static Vector Average = Vector(0, 0, 0);
	//	howClose.x = abs(CurrentTick->networkorigin.x - lastPos.x);
	//	howClose.y = abs(CurrentTick->networkorigin.y - lastPos.y);
	//	howClose.z = abs(CurrentTick->networkorigin.z - lastPos.z);

	//	Average += howClose;
	//	static int times = 0;
	//	times++;
	//	std::cout << "apart: " << howClose.x << " " << howClose.y << " " << howClose.z << std::endl;

	//	std::cout << "Aware Avg: " << Average.x / times << " " << Average.y / times << " " << Average.z / times << std::endl;

	//}
#endif

	INetChannelInfo *nci = Interfaces::Engine->GetNetChannelInfo();
	//static int counter = 1;
	//double additionvalue = abs(sin(counter / 30))+1;
	//counter++;
	CBaseEntity *Entity = pCPlayer->BaseEntity;
	//std::cout << additionvalue << std::endl;
	CSimulationData data;
	data.networkorigin = CurrentPos;
	data.velocity = Velocity;
	data.m_nFlags = CurrentTick->flags;
	data.m_pEntity = Entity;

#if 0
	for (int i = 0; i < ticksChoked; i++)
	{
		//Updates the players velocity for acceleration
		if (i > 0)//Don't update acceleration on first tick as it will be over predicting
		{
		//	data.velocity.x += Acceleration.x;
		//	data.velocity.y += Acceleration.y;
		}
		//Update player position 
		bool InAir = !(CurrentTick->flags & FL_ONGROUND);
		SimulateMovement(data, InAir);
	}
#else
	float latency = nci->GetAvgLatency(FLOW_INCOMING) + nci->GetAvgLatency(FLOW_OUTGOING);

	float TimeUntilNextFrame = 0.0f;// Interfaces::Globals->absoluteframetime * 0.5f;
	
	//Add 1 because CreateMove is 1 tick behind
    //FIXME: need to take into account psilent/fakelag packets!!!!!!!!!!! For now, just assume we choked ticksChoked is the amount of ticks the firing packet will be held back
	float TimeItWillBeWhenPacketReachesServer = TICKS_TO_TIME(CurrentUserCmd.cmd->tick_count + TIME_TO_TICKS(latency + TimeUntilNextFrame) + 1 + ticksChoked);

	//Time we reach the server - time the player was simulated on the server , clamped to sv_maxunlag (1.0 seconds)
	float deltaTime = fminf(TimeItWillBeWhenPacketReachesServer - CurrentTick->simulationtime, 1.0f);

	int deltaTicks = TIME_TO_TICKS(deltaTime);

	//float simulationTimeDelta = CurrentTick->simulationtime - PreviousTick->simulationtime;
	//int simulationTicks = clamp(TIME_TO_TICKS(simulationTimeDelta), 1, 15);

	//How many choked ticks to simulate.
	int ticks = CurrentTick->tickschoked;
#ifdef predictTicks
	int CurrentChockedTics = TIME_TO_TICKS(abs(CurrentTick->simulationtime-PreviousTick->simulationtime));
	if (CurrentChockedTics > 15)
		CurrentChockedTics = 15;
	// Number of ticks chocked since last tick was sent
	//not sure how to get or best way to get, I could make a counter but 
	//Think you store it somewhere
	//ticks = predictedChokedTicks(pCPlayer, CurrentChockedTics);
	//if (ticks > 1000)
	//	ticks = CurrentTick->tickschoked;
#endif
	int ChokedTicks = clamp(ticks + 1, 1, 15);

	int v20 = deltaTicks - ChokedTicks;
#ifdef acceleration
	Vector vecAcceleration = ((CurrentTick->velocity - PreviousTick->velocity) / TIME_TO_TICKS(CurrentTick->simulationtime - PreviousTick->simulationtime))/ Interfaces::Globals->interval_per_tick;
#endif
	int predicted = 0;
	while (v20 >= 0)
	{
		for (int i = 0; i < ChokedTicks; i++)
		{
			predicted++;
			//pCPlayer->LagCompensatedThisTick = true;
			bool InAir = !(CurrentTick->flags & FL_ONGROUND);
			SimulateMovement(data, InAir);
#ifdef acceleration
			//data.velocity.x += vecAcceleration.x * Interfaces::Globals->interval_per_tick;
			//data.velocity.y += vecAcceleration.y * Interfaces::Globals->interval_per_tick;
#endif
		}
		v20 -= ChokedTicks;
	}
//	AllocateConsole();
	//std::cout << predicted << std::endl;
#endif

	//Now set updated position
	//pCPlayer->pred_LastPos = CurrentPos;

#ifdef _DEBUG
	//Entity->DrawHitboxes(ColorRGBA(0, 0, 255, 255), 0.1f);
#endif

	if (Entity->GetOrigin() != data.networkorigin && data.networkorigin != vecZero)
	{
		Entity->SetAbsOrigin(data.networkorigin);
		Entity->SetFlags(data.m_nFlags);
	}
	//if (DrawResolveModeChk.Checked)
	//	Entity->DrawHitboxes(ColorRGBA(0, 0, 255, 100), 0.1f);

#ifdef _DEBUG
	//Entity->DrawHitboxes(ColorRGBA(0, 255, 0, 255), 0.1f);
#endif
}; 

void LagCompensation::JakesSimpleMathLagCompZ(CustomPlayer*const pCPlayer, int ticksChoked, StoredNetvars *CurrentTick, StoredNetvars* PreviousTick)
{
#ifdef _DEBUG
	AllocateConsole();
#endif

	auto Records = &pCPlayer->Personalize.m_PlayerRecords;

	Vector Velocity = CurrentTick->velocity;
	Vector CurrentPos = CurrentTick->networkorigin;
	
	//Accuracy checks
	if (pCPlayer->pred_LastPos == vecZero)
	{
		pCPlayer->pred_LastPos = CurrentTick->networkorigin;
		pCPlayer->pred_LastConfirmedValue = CurrentPos;
		pCPlayer->pred_LastPredictedVelocity = vecZero;
	}

#ifdef _DEBUG
	if (ticksChoked <= 1)
	{
		int difference = (CurrentPos.z - pCPlayer->pred_LastPos.z);
		std::cout << difference << " " << CurrentPos.z - pCPlayer->pred_LastConfirmedValue.z << " " << pCPlayer->pred_LastPredictedVelocity.z << std::endl;
	}
#endif


	INetChannelInfo *nci = Interfaces::Engine->GetNetChannelInfo();
	float ping = nci->GetAvgLatency(FLOW_OUTGOING) + nci->GetAvgLatency(FLOW_INCOMING);
	ticksChoked += TIME_TO_TICKS(ping);
	CBaseEntity *Entity = pCPlayer->BaseEntity;
	CSimulationData data;
	data.networkorigin = CurrentPos; 
	data.velocity = Velocity;
	data.m_nFlags = CurrentTick->flags;
	data.m_pEntity = Entity;
	for (int i = 0; i < ticksChoked; i++)
	{
		if (i > 0)
		{
			Velocity.z -= sv_gravity->GetFloat() * Interfaces::Globals->interval_per_tick;
		}
		//float maxVel = 3500;
		//if (Velocity.z < -maxVel)
		//	Velocity.z = -maxVel;
		//if (Velocity.z > maxVel)
		//	Velocity.z = maxVel;
		CurrentPos.z += Velocity.z * Interfaces::Globals->interval_per_tick;
	}
	pCPlayer->pred_LastPredictedVelocity.z = Velocity.z * Interfaces::Globals->interval_per_tick;
	CM_RestoreNetvars(CurrentTick, Entity, &CurrentPos);
	pCPlayer->pred_LastPos = CurrentPos;
};


//Attempt to compensate for fake lag
void LagCompensation::CompensateLag(CustomPlayer*const pCPlayer)
{
	//Dylan's lag compensation attempt
	VMP_BEGINMUTILATION("CMPLAG")

	auto Records = &pCPlayer->Personalize.m_PlayerRecords;
	int RecordCount = Records->size();
	CBaseEntity *Entity = pCPlayer->BaseEntity;
	if (RecordCount <= 1 || Entity->GetVelocity().Length() == 0.0f)
	{
		//pCPlayer->BacktrackTick = Entity->GetSimulationTime() + GetLerpTime();
		pCPlayer->NetvarsModified = false;
#if 1
		ResolveYawModes mode = pCPlayer->Personalize.EstimateYawMode;
		if (mode != ResolveYawModes::CloseFake && mode != AtTarget && mode != InverseAtTarget)
		{
			switch (pCPlayer->Personalize.correctedbodyyawindex)
			{
				//case 0 uses current pose parameter for the record, so is already set
			case 1:
				Entity->SetPoseParameterScaled(11, 0.0f);
				pCPlayer->NetvarsModified = true;
				break;
			case 2:
				Entity->SetPoseParameterScaled(11, -1.0f);
				pCPlayer->NetvarsModified = true;
				break;
			case 3:
				Entity->SetPoseParameterScaled(11, 1.0f);
				pCPlayer->NetvarsModified = true;
				break;
			}
		}
#endif
		return;
	}
#if 0
	else if (RecordCount == 1 || Entity->GetVelocity().Length() == 0.0f)
	{
		//auto CurrentTick = Records->front();
		//if (CurrentTick->ReceivedPoseParametersAndAnimations)
		//	CM_RestoreAnimations(CurrentTick, Entity);
		
		//CM_RestoreNetvars(CurrentTick, Entity, &CurrentTick->networkorigin);
		//pCPlayer->BacktrackTick = CurrentTick->tickcount;
		//pCPlayer->NetvarsModified = true;
		//pCPlayer->Backtracked = true;
		return;
	}
#endif

	auto CurrentTick = Records->front();
	auto PreviousTick = Records->at(1);

	if (CurrentTick->ReceivedPoseParametersAndAnimations)
		CM_RestoreAnimations(CurrentTick, Entity);
	
	CM_RestoreNetvars(CurrentTick, Entity, &CurrentTick->networkorigin);

	pCPlayer->BacktrackTick = CurrentTick->tickcount;

	pCPlayer->NetvarsModified = true;

	if ((CurrentTick->networkorigin - PreviousTick->networkorigin).LengthSqr() > (64.0f * 64.0f)
		&& !PreviousTick->dormant)
	{
#define DYLANS_LAG_COMPENSATION
#ifdef DYLANS_LAG_COMPENSATION
		//INetChannelInfo *nci = Interfaces::Engine->GetNetChannelInfo();

		//int TicksSinceUpdate = pCPlayer->TicksChoked;//(Interfaces::Globals->tickcount - CurrentTick->TickReceivedNetUpdate); //TIME_TO_TICKS(Interfaces::Globals->curtime - LocalPlayer.Entity->GetSimulationTime()) + TIME_TO_TICKS(GetLerpTime());
		JakesSimpleMathLagComp(pCPlayer, 1, CurrentTick, PreviousTick);

#if 0
		int v20 = deltaTicks - simulationTicks;
		bool Jumping = CurrentTick->velocity.z > PreviousTick->velocity.z; // CurrentTick->jumptimemsecs != 0;
		bool Ducking = CurrentTick->duckamount > PreviousTick->duckamount;
		int runtimes = 0;
		if (v20 >= 0)
		{
			pCPlayer->LagCompensatedThisTick = true;
			QAngle OriginalEyeAngles = Entity->GetEyeAngles();
			while (v20 >= 0)
			{
				int i = simulationTicks;
				while (i)
				{

					//Ticks was 1 before I changed it.
					EnginePredictPlayer(Entity, 1, true, Jumping, Ducking, true, pCPlayer);
					i--;
					runtimes++;
				}
				v20 -= simulationTicks;
			}
			Entity->SetEyeAngles(OriginalEyeAngles);
#ifdef _DEBUG
			Entity->DrawHitboxes(ColorRGBA(255, 0, 0, 255), 0.1f);
		}
		std::cout << runtimes << std::endl;

		int v20 = deltaTicks - simulationTicks;
		if (v20 >= 0)
		{
			pCPlayer->LagCompensatedThisTick = true;
			for (int i = 0; i < simulationTicks; i++)
			{
				EnginePredictPlayer(Entity, 1, true);
			}
		}
		Entity->DrawHitboxes(ColorRGBA(255, 0, 0, 255), 0.1f);
#endif
#endif
#else
		//Aimware code. completely broken..
		Vector originalOrigin = CurrentTick->networkorigin;

		CSimulationData data;
		data.networkorigin = *Entity->GetAbsOrigin(); //CurrentTick->networkorigin;
		data.velocity = CurrentTick->velocity;
		data.m_nFlags = CurrentTick->flags;
		data.m_pEntity = Entity;

		INetChannelInfo *nci = Interfaces::Engine->GetNetChannelInfo();

		float tickInterval = 1.0 / Interfaces::Globals->interval_per_tick;

		float deltaTime = ((floorf(((nci->GetAvgLatency(FLOW_INCOMING) + nci->GetAvgLatency(FLOW_OUTGOING)) * tickInterval) + 0.5)
			+ CurrentUserCmd.cmd->tick_count
			+ 1)
			* Interfaces::Globals->interval_per_tick)
			- CurrentTick->simulationtime;

		if (deltaTime > 1.0f)
			deltaTime = 1.0f;

		float simulationTimeDelta = CurrentTick->simulationtime - PreviousTick->simulationtime;

		int simulationTicks = clamp(floorf((simulationTimeDelta * tickInterval) + 0.5f), 1, 15);

		float velocityDegree = RAD2DEG(atan2(CurrentTick->velocity.x, CurrentTick->velocity.y));
		int deltaTicks = floorf((tickInterval * deltaTime) + 0.5);
		float velocityAngle = velocityDegree - RAD2DEG(atan2(PreviousTick->velocity.x, PreviousTick->velocity.y));
		int times = 1;
		if (velocityAngle <= 180.0)
		{
			if (velocityAngle < -180.0)
				velocityAngle = velocityAngle + 360.0;
		}
		else
		{
			velocityAngle = velocityAngle - 360.0;
		}
		int v20 = deltaTicks - simulationTicks;
		float v21 = velocityAngle / simulationTimeDelta;
		float velocityLength2D = sqrtf((data.velocity.y * data.velocity.y) + (data.velocity.x * data.velocity.x)); //data.velocity.Length2D();

		if (v20 < 0)
		{
			CurrentTick->networkorigin = originalOrigin;
		}
		else
		{
			//pCPlayer->LagCompensatedThisTick = true;

			do
			{
				if (simulationTicks > 0)
				{
					int v22 = simulationTicks;
					do
					{
						float extrapolatedMovement = velocityDegree + (Interfaces::Globals->interval_per_tick * v21);
						data.velocity.x = cosf(DEG2RAD(extrapolatedMovement)) * velocityLength2D;
						data.velocity.y = sinf(DEG2RAD(extrapolatedMovement)) * velocityLength2D;
						bool InAir = !(CurrentTick->flags & FL_ONGROUND);
						SimulateMovement(data, InAir);
						times++;
						velocityDegree = extrapolatedMovement;
						--v22;
					} while (v22);
				}
				v20 -= simulationTicks;
			} while (v20 >= 0);
			//CurrentTick->networkorigin = data.networkorigin;
		}
		std::cout << times <<" hell " <<  std::endl;
		CM_RestoreNetvars(CurrentTick, Entity, &data.networkorigin);
		Entity->DrawHitboxes(ColorRGBA(255, 0, 255, 255), 0.1f);
#endif

	}
	VMP_END
}

//Called from CreateMove to set up backtracking for all clients. If couldn't backtrack, then compensate for lag
void LagCompensation::CM_BacktrackPlayers()
{
#ifdef PRINT_FUNCTION_NAMES
	AllocateConsole();
	printf("CM_BacktrackPlayers\n");
#endif
	VMP_BEGINMUTILATION("CMBTP")
	int MaxEntities = 64;// Interfaces::ClientEntList->GetHighestEntityIndex();
	int NumPlayers = GetClientCount();
	int CountedPlayers = 0;

	if (NumPlayers)
	{
		for (int i = 0; i <= MaxEntities; i++)
		{
			CBaseEntity* pPlayer = Interfaces::ClientEntList->GetClientEntity(i);

			if (pPlayer && pPlayer->IsPlayer())
			{
				CustomPlayer* pCPlayer = &AllPlayers[pPlayer->index];

				//Only backtrack resolved players
				if (pPlayer != LocalPlayer.Entity && pCPlayer->BaseEntity && !pPlayer->GetDormant() && pPlayer->GetAlive() && !pCPlayer->IsSpectating)
				{
					//Always store current netvars because animations and interp could have changed their properties
					//pCPlayer->LagCompensatedThisTick = false;
					pCPlayer->TempNetVars = StoredNetvars(pPlayer, pCPlayer, nullptr);
					StoredNetvars *BestBacktrackTick = pCPlayer->GetBestBacktrackTick();
					if (BestBacktrackTick && BestBacktrackTick == pCPlayer->LatestBloodTick)
						pCPlayer->TempNetVars.ResolvedFromBloodSpawning = true;
					CM_BacktrackPlayer(pCPlayer, LocalPlayer.Entity, pPlayer);
				}
				else
				{
					pCPlayer->NetvarsModified = false;
					pCPlayer->Backtracked = false;
					pCPlayer->BacktrackTick = 0;
				}

				if (++CountedPlayers == NumPlayers)
					break;
			}
		}
	}
	VMP_END
}

//Called from CreateMove to restore backtracked players back to current values
void LagCompensation::CM_RestorePlayers()
{
#ifdef PRINT_FUNCTION_NAMES
	AllocateConsole();
	printf("CM_RestorePlayers\n");
#endif
	VMP_BEGINMUTILATION("CMRESP")
	int MaxEntities = 64;// Interfaces::ClientEntList->GetHighestEntityIndex();
	int NumPlayers = GetClientCount();
	int CountedPlayers = 0;

	if (NumPlayers)
	{
		for (int i = 0; i <= MaxEntities; i++)
		{
			CBaseEntity* pPlayer = Interfaces::ClientEntList->GetClientEntity(i);

			if (pPlayer && pPlayer->IsPlayer())
			{
				if (pPlayer != LocalPlayer.Entity)
				{
					CustomPlayer* pCPlayer = &AllPlayers[pPlayer->index];

					if (pCPlayer->BaseEntity && /*pCPlayer->Personalize.ShouldResolve() &&*/ pCPlayer->NetvarsModified)
					{
						CM_RestoreAnimations(&pCPlayer->TempNetVars, pPlayer);
						CM_RestoreNetvars(&pCPlayer->TempNetVars, pPlayer, &pCPlayer->TempNetVars.origin);
						if (pCPlayer->LatestBloodTick != nullptr && pCPlayer->GetBestBacktrackTick() == pCPlayer->LatestBloodTick)
						{
							pCPlayer->BaseEntity->SetAbsAngles(pCPlayer->TempNetVars.absangles);
						}
						pPlayer->SetGoalFeetYaw(pCPlayer->TempNetVars.GoalFeetYaw);
						pPlayer->SetCurrentFeetYaw(pCPlayer->TempNetVars.CurrentFeetYaw);
						//pPlayer->SetupBones(nullptr, MAXSTUDIOBONES, BONE_USED_BY_ANYTHING, Interfaces::Globals->curtime);
						pCPlayer->BacktrackTick = 0;
						pCPlayer->NetvarsModified = false;
						//pCPlayer->LagCompensatedThisTick = false;
					}
				}

				if (++CountedPlayers == NumPlayers)
					break;
			}
		}
	}
	VMP_END
}

//Called from CreateMove to get ideal tick to set cmd->tickcount
void LagCompensation::AdjustTickCountForCmd(CBaseEntity *const pPlayerFiredAt, bool bLocalPlayerIsFiring)
{
#ifdef PRINT_FUNCTION_NAMES
	AllocateConsole();
	printf("AdjustTickCountForCmd\n");
#endif
	VMP_BEGINMUTILATION("ATCFC")
	//int lerpTicks = RemoveInterpChk.Checked ? TIME_TO_TICKS(cl_interp_ratio->GetFloat() / cl_updaterate->GetFloat()) : 0;
	//pPlayerFiredAt is the player we are aimbotting or triggerbotting and we are definitely firing a bullet this tick
	if (pPlayerFiredAt)
	{
		CustomPlayer *pCPlayer = &AllPlayers[ReadInt((uintptr_t)&pPlayerFiredAt->index)];

#ifdef ONLY_SHOOT_WHEN_BLOOD_SPAWNS
		//for debugging blood resolver
		if (!pCPlayer->GetBestBacktrackTick() || pCPlayer->GetBestBacktrackTick() != pCPlayer->LatestBloodTick)
		{
			if (!GetAsyncKeyState(VK_LBUTTON))
				CurrentUserCmd.cmd->buttons &= ~IN_ATTACK;
		}
#endif
		int backtracktick = pCPlayer->BacktrackTick;
		if (BacktrackExploitChk.Checked)
			backtracktick -= 1;

		if (bTickIsValid(backtracktick))
		{
		//	int tick = pCPlayer->BacktrackTick;
		//	int tickssinceupdate = Interfaces::Globals->tickcount - pCPlayer->TickReceivedLowerBodyUpdate;
		//	if (tickssinceupdate > 0 && pCPlayer->GetBestBacktrackTick() == pCPlayer->LatestLowerBodyUpdate && bTickIsValid(tick + tickssinceupdate))
		//		tick += tickssinceupdate;
		//	WriteInt((uintptr_t)&CurrentUserCmd.cmd->tick_count, tick);
			
			WriteInt((uintptr_t)&CurrentUserCmd.cmd->tick_count, backtracktick);
		}
		else
		{
			int targettick;
			if (RemoveInterpChk.Checked)
				targettick = TIME_TO_TICKS(pCPlayer->CurrentNetvarsFromProxy.simulationtime) + TIME_TO_TICKS(GetLerpTime());
			else
				targettick = CurrentUserCmd.cmd->tick_count;
			WriteInt((uintptr_t)&CurrentUserCmd.cmd->tick_count, targettick);
		}
		//Store hit percentage so resolver won't count really inaccurate shots as misses
		LastShotHitPercentage = CurrentHitPercentage;
	}
	else
	{
		if (bLocalPlayerIsFiring)
		{
			Vector vecDir;
			Vector LocalEyePos = LocalPlayer.Entity->GetEyePosition(); //FIXME: correct this eye position for simtime?
			QAngle EyeAngles = LocalPlayer.Entity->GetEyeAngles();
			AngleVectors(EyeAngles, &vecDir);
			VectorNormalizeFast(vecDir);
			Vector EndPos = LocalEyePos + (vecDir * 8192.0f);
			trace_t tr;
			UTIL_TraceLine(LocalEyePos, EndPos, MASK_SHOT, LocalPlayer.Entity, &tr);
			CTraceFilterPlayersOnlyNoWorld filter;
			filter.AllowTeammates = true;
			filter.pSkip = (IHandleEntity*)LocalPlayer.Entity;
			filter.m_icollisionGroup = COLLISION_GROUP_NONE;
			UTIL_ClipTraceToPlayers(LocalEyePos, EndPos + vecDir * 40.0f, 0x4600400B, (ITraceFilter*)&filter, &tr);
			if (MTargetting.IsPlayerAValidTarget(tr.m_pEnt))
			{
				CBaseCombatWeapon* weapon = LocalPlayer.Entity->GetWeapon();
				if (gTriggerbot.WeaponCanFire(weapon))
				{
					//Store hit percentage so resolver won't count really inaccurate shots as misses
					BulletWillHit(weapon, buttons, tr.m_pEnt, EyeAngles, LocalEyePos, &EndPos, MTargetting.CURRENT_HITCHANCE_FLAGS);
					LastShotHitPercentage = CurrentHitPercentage;
				}
				int index = ReadInt((uintptr_t)&tr.m_pEnt->index);
				CustomPlayer* pCPlayer = &AllPlayers[index];
				if (bTickIsValid(pCPlayer->BacktrackTick))
				{
					WriteInt((uintptr_t)&CurrentUserCmd.cmd->tick_count, pCPlayer->BacktrackTick);
				}
				else
				{
					int targettick;
					if (RemoveInterpChk.Checked)
						targettick = TIME_TO_TICKS(pCPlayer->CurrentNetvarsFromProxy.simulationtime) + TIME_TO_TICKS(GetLerpTime());
					else
						targettick = CurrentUserCmd.cmd->tick_count;
					WriteInt((uintptr_t)&CurrentUserCmd.cmd->tick_count, targettick);
				}
			}
			else
			{
				CBaseEntity *pPlayerClosestToCrosshair = MTargetting.GetPlayerClosestToCrosshair(20.0f);
				if (pPlayerClosestToCrosshair)
				{
					CustomPlayer* pCPlayer = &AllPlayers[pPlayerClosestToCrosshair->index];
					if (bTickIsValid(pCPlayer->BacktrackTick))
					{
						WriteInt((uintptr_t)&CurrentUserCmd.cmd->tick_count, pCPlayer->BacktrackTick);
					}
					else
					{
						int targettick;
						if (RemoveInterpChk.Checked)
							targettick = TIME_TO_TICKS(pCPlayer->CurrentNetvarsFromProxy.simulationtime) + TIME_TO_TICKS(GetLerpTime());
						else
							targettick = CurrentUserCmd.cmd->tick_count;
						WriteInt((uintptr_t)&CurrentUserCmd.cmd->tick_count, targettick);
					}
				}
				else
				{
					int targettick = CurrentUserCmd.cmd->tick_count;
					if (RemoveInterpChk.Checked)
					{
						targettick += TIME_TO_TICKS(GetLerpTime());
					}
					WriteInt((uintptr_t)&CurrentUserCmd.cmd->tick_count, targettick);
				}
			}
		}
	}
	VMP_END
}






//Sets the ideal yaw for this player
//FIXME: WE NEED TO STORE PREDICTED ANGLES FOR EACH TICK RECORD AND USE THOSE, NOT THE CURRENT TICK'S PREDICTED ONES!!!
//THE CURRENT SETUP WON'T WORK FOR GETTING PREDICTED ANGLES FOR OLD TICKS DURING BACKTRACKING
void LagCompensation::GetIdealYaw(float& yaw, int correctedindex, CustomPlayer* pCPlayer, CBaseEntity* Entity, CBaseEntity* pLocalEntity)
{
#ifdef PRINT_FUNCTION_NAMES
	AllocateConsole();
	printf("GetIdealYaw\n");
#endif
	VMP_BEGINMUTILATION("GIY")
#ifdef NO_IDEAL_YAW
	pCPlayer->Personalize.LastResolveModeUsed = ResolveYawModes::NoYaw;
	return;
#endif

	if (!pLocalEntity)
	{
		pCPlayer->Personalize.LastResolveModeUsed = ResolveYawModes::NoYaw;
		return;
	}

	Personalize *pPersonalize = &pCPlayer->Personalize;

	int NumberOfRecords = pPersonalize->m_PlayerRecords.size();

	if (NumberOfRecords == 0)
	{
		pCPlayer->Personalize.LastResolveModeUsed = ResolveYawModes::NoYaw;
		return;
	}

#ifdef logicResolver
	if (WhaleDongTxt.iValue >= 1 && WhaleDongTxt.iValue != 0)
	{
		pCPlayer->Personalize.LastResolveModeUsed = WD;
		QAngle yawang = QAngle(0, yaw, 0);
		LogicResolver2_Predict(yawang, false, pCPlayer, pLocalEntity, 0);
		yaw = yawang.y;
		ClampY(yaw);
		if (yaw < 0.0f)
			yaw += 360.0f;

		return;
	}
#endif

	//Try to resolve those legit antiaimers that you don't know about
	if (!pPersonalize->ShouldResolve())
	{
		if (NumberOfRecords >= 2)
		{
			auto Records = &pPersonalize->m_PlayerRecords;
			//Check to see if last two ticks had choked packets
			if (Records->at(0)->tickschoked && Records->at(1)->tickschoked)
			{
				if (pCPlayer->IsMoving)
				{
					yaw = ResolveStaticFake(pCPlayer);
				}
				else
				{
					float TimeSinceLBYUpdate = (Interfaces::Globals->curtime - pCPlayer->TimeLowerBodyUpdated) - GetNetworkLatency(FLOW_OUTGOING);
					StoredNetvars *CurrentVars = pPersonalize->m_PlayerRecords.front();
					float delta = ClampYr(ClampYr(CurrentVars->eyeangles.y) - CurrentVars->lowerbodyyaw);
					if (fabsf(delta) > 35.0f && TimeSinceLBYUpdate > 0.4f)
						yaw = ResolveStaticFake(pCPlayer);
				}
			}
		}
		return;
	}

	ResolveYawModes mode = pPersonalize->ResolveYawMode;

	switch (mode)
	{
		//Manual selected resolve modes
	case ResolveYawModes::NoYaw:
		return;
	case ResolveYawModes::FakeSpins:
		yaw = ResolveFakeSpins(pCPlayer);
		return;
	case ResolveYawModes::LinearFake:
		yaw = ResolveLinearFake(pCPlayer);
		return;
	case ResolveYawModes::RandomFake:
		yaw = ResolveRandomFake(pCPlayer);
		return;
	case ResolveYawModes::CloseFake:
		pCPlayer->CloseFakeAngle = true;
		yaw = ResolveCloseFakeAngles(pCPlayer);
		return;
	case ResolveYawModes::BloodBodyRealDelta:
		yaw = ResolveBloodBodyRealDelta(pCPlayer);
		return;
	case ResolveYawModes::BloodEyeRealDelta:
		yaw = ResolveBloodEyeRealDelta(pCPlayer);
		return;
	case ResolveYawModes::BloodReal:
		yaw = ResolveBloodReal(pCPlayer);
		return;
	case ResolveYawModes::AtTarget:
		yaw = ResolveAtTargetFake(pCPlayer);
		return;
	case ResolveYawModes::InverseAtTarget:
		yaw = ResolveInverseAtTargetFake(pCPlayer);
		return;
	case ResolveYawModes::AverageLBYDelta:
		yaw = ResolveAverageLBYDelta(pCPlayer);
		return;
	case ResolveYawModes::StaticFakeDynamic:
		yaw = ResolveStaticFakeDynamic(pCPlayer);
		return;
	case ResolveYawModes::FakeWalk:
		yaw = ResolveFakeWalk(pCPlayer);
		return;
	case ResolveYawModes::StaticFake:
		yaw = ResolveStaticFake(pCPlayer);
		return;
	case ResolveYawModes::Inverse:
		yaw += 180.0f;
		pCPlayer->Personalize.LastResolveModeUsed = ResolveYawModes::Inverse;
		break;
	case ResolveYawModes::Back:
		yaw = CalcAngle(Entity->GetEyePosition(), pLocalEntity->GetEyePosition()).y - 180.0f;
		pCPlayer->Personalize.LastResolveModeUsed = ResolveYawModes::Back;
		break;
	case ResolveYawModes::Left:
		yaw += 90.0f;
		pCPlayer->Personalize.LastResolveModeUsed = ResolveYawModes::Left;
		break;
	case ResolveYawModes::Right:
		yaw -= 90.0f;
		pCPlayer->Personalize.LastResolveModeUsed = ResolveYawModes::Right;
		break;
	case ResolveYawModes::CustomStaticYaw:
		yaw = ResolveYawTxt.flValue;
		pCPlayer->Personalize.LastResolveModeUsed = ResolveYawModes::CustomStaticYaw;
		break;
	case ResolveYawModes::CustomAdditiveYaw:
		yaw += ResolveYawTxt.flValue;
		pCPlayer->Personalize.LastResolveModeUsed = ResolveYawModes::CustomAdditiveYaw;
		break;
	//Automatic resolve
	case ResolveYawModes::AutomaticYaw:
	{
		if (pCPlayer->TicksChoked == 0 && !pCPlayer->ResolverFiredAtShootingAngles)
		{
			pCPlayer->Personalize.LastResolveModeUsed = ResolveYawModes::NoYaw;
			return;
		}

		const Vector velocity = Entity->GetVelocity();
		float speed = velocity.Length2D();
		bool Moving = speed > 0.1f;
		int OnGround = Entity->GetFlags() & FL_ONGROUND;

		if (!pCPlayer->IsBreakingLagCompensation)
		{
			if (Moving && OnGround)
			{
				//fake walk
				if (speed > 12.0f && speed < 18.0f)
				{
					yaw = ResolveFakeWalk(pCPlayer);
				}
				else
				{
					yaw = ResolveStaticFake(pCPlayer);
				}
				return;
			}
			else if (pCPlayer->CurrentWeapon && pCPlayer->CurrentWeapon->IsGun() && pCPlayer->BulletsLeft == 0)
			{
				yaw = ResolveStaticFake(pCPlayer);
				return;
			}
		}

		if (PredictFakeAnglesChk.Checked)
		{
			//Automatic Resolver

			//bool poseclose = fabsf(Entity->GetPoseParameterUnscaled(11)) < 0.1f;
			//pCPlayer->CloseFakeAngle = Entity->GetVelocity().Length() == 0.0f && ((fabsf(Entity->GetPoseParameterUnscaled(11)) < 0.5f /*|| fabsf(pCPlayer->CurrentNetVars.eyeangles.y - pCPlayer->CurrentNetVars.lowerbodyyaw) < 3.0f*/) || (ReadFloat((uintptr_t)&Interfaces::Globals->curtime) - pCPlayer->TimeLowerBodyUpdated) > 1.55f);

			//float EyeBodyDelta = fabsf(pCPlayer->CurrentNetvarsFromProxy.eyeangles.y - pCPlayer->CurrentNetvarsFromProxy.lowerbodyyaw);
			//if (EyeBodyDelta <= 15.0f)
			//	return; //todo: miss counter for this!!

#if 1
			if (pCPlayer->IsBreakingLagCompensation || pCPlayer->IsMoving)
				pCPlayer->CloseFakeAngle = false;
			else
			{
				//Check to see if the enemy's lower body yaw is looking 180 degrees at us or 180 degrees away from us
				//If they are, then assume they are using either a static yaw or an Anti-LBY Antiaim
				//TODO: detect to see if lower body yaw has a history of spinning to rule out spinbots

				//TODO: FIX SHOTS FIRED DETECTION FOR THIS!
#ifdef FIXED
				float YawFromUsToEnemy = CalcAngle(LocalPlayer.Entity->GetEyePosition(), Entity->GetEyePosition()).y;
				float DeltaYaw = fabsf(ClampYr(YawFromUsToEnemy - pCPlayer->CurrentNetvarsFromProxy.lowerbodyyaw));
				pCPlayer->CloseFakeAngle = false;
									//FIXME BUGBUG: WasInverseFakingWhileMoving is not the same kind of inverse check! Check PredictLowerBodyYaw to see what I mean
				if (DeltaYaw < 5.0f /*|| (pPersonalize->WasInverseFakingWhileMoving && pPersonalize->ShotsMissed < 2)*/)
				{
					pPersonalize->EstimateYawMode = ResolveYawModes::InverseAtTarget;
				}
				else if (DeltaYaw > 175.0f)
				{
					pPersonalize->EstimateYawMode = ResolveYawModes::AtTarget;
				}
				else
#endif
				{
					//If player's lower body yaw has not updated in > 1.1 seconds then assume they are using an Anti-LBY Antiaim
					float TimeSinceLBYUpdate = (Interfaces::Globals->curtime - pCPlayer->TimeLowerBodyUpdated) - GetNetworkLatency(FLOW_OUTGOING);
					pCPlayer->CloseFakeAngle = TimeSinceLBYUpdate >= 1.1f;
				}
			}
#endif

			//Get predicted yaw mode
			ResolveYawModes mode = pPersonalize->EstimateYawMode;

			if (Moving)
			{
				if (OnGround)
				{
					mode = ResolveYawModes::StaticFake;
				}
				else
				{
					if (velocity.z < 0.0f)
						mode = pPersonalize->EstimateYawModeInAir;
					else
						mode = ResolveYawModes::StaticFake;
				}
			}

			if (pCPlayer->CloseFakeAngle)
			{
				//Resolve for 'Anti-Resolve Yaw'
				yaw = ResolveCloseFakeAngles(pCPlayer);
				return;
			}
			else
			{
				switch (mode)
				{
				default:
				case ResolveYawModes::NoYaw:
					return;
				case ResolveYawModes::FakeSpins:
					yaw = ResolveFakeSpins(pCPlayer);
					return;
				case ResolveYawModes::LinearFake:
					yaw = ResolveLinearFake(pCPlayer);
					return;
				case ResolveYawModes::RandomFake:
					yaw = ResolveRandomFake(pCPlayer);
					return;
				case ResolveYawModes::AtTarget:
					yaw = ResolveAtTargetFake(pCPlayer);
					return;
				case ResolveYawModes::InverseAtTarget:
					yaw = ResolveInverseAtTargetFake(pCPlayer);
					return;
				case ResolveYawModes::AverageLBYDelta:
					yaw = ResolveAverageLBYDelta(pCPlayer);
					return;
				case ResolveYawModes::StaticFakeDynamic:
					yaw = ResolveStaticFakeDynamic(pCPlayer);
					return;
				case ResolveYawModes::FakeWalk:
					yaw = ResolveFakeWalk(pCPlayer);
					return;
				case ResolveYawModes::StaticFake:
					yaw = ResolveStaticFake(pCPlayer);
					return;
				case ResolveYawModes::BloodReal:
					//FIXME: check time since it was set!
					yaw = ResolveBloodReal(pCPlayer);
					return;
				case ResolveYawModes::BloodBodyRealDelta:
					yaw = ResolveBloodBodyRealDelta(pCPlayer);
					return;
				case ResolveYawModes::BloodEyeRealDelta:
					yaw = ResolveBloodEyeRealDelta(pCPlayer);
					return;
				}
			}
		}
		else
		{
			//Shots missed based resolver.
			//TODO: Could use cleanup
			Vector screen;
#ifdef _DEBUG
			bool Draw = false;// EXTERNAL_WINDOW && /*DrawEyeAnglesChk.Checked &&*/ WorldToScreenCapped(Entity->GetBonePosition(HITBOX_CHEST, Entity->GetSimulationTime()), screen);
#endif
			//MAX_RESOLVER_MISSES is the max
			switch (correctedindex)
			{
			case 0:
				//Corrects non-fake angles
#ifdef _DEBUG
				if (Draw)
					DrawStringUnencrypted("Estimate Mode: No Yaw", Vector2D(screen.x - 200, screen.y + 25), ColorRGBA(255, 0, 0, 255), pFont);
#endif
				pCPlayer->Personalize.LastResolveModeUsed = ResolveYawModes::NoYaw;
				break;
			case 1:
			case 2:
				if (pPersonalize->isCorrected)
					pPersonalize->correctedindex = 1;
			case 9:
			case 10:
				if (pPersonalize->isCorrected && correctedindex == 10)
					pPersonalize->correctedindex = 9;
				//Corrects slow spin with fakeangles
				yaw = ResolveFakeSpins(pCPlayer);
#ifdef _DEBUG
				if (Draw)
					DrawStringUnencrypted("Estimate Mode: Spin Fake", Vector2D(screen.x - 200, screen.y + 25), ColorRGBA(255, 0, 0, 255), pFont);
#endif
				return;
			case 3:
			case 4:
				if (pPersonalize->isCorrected)
					pPersonalize->correctedindex = 3;
			case 11:
			case 12:
				if (pPersonalize->isCorrected && correctedindex == 12)
					pPersonalize->correctedindex = 11;
				//Corrects linear fakeangles
				yaw = ResolveLinearFake(pCPlayer);
#ifdef _DEBUG
				if (Draw)
					DrawStringUnencrypted("Estimate mode: Linear Fake", Vector2D(screen.x - 200, screen.y + 25), ColorRGBA(255, 0, 0, 255), pFont);
#endif
				return;
			case 5:
			case 6:
				if (pPersonalize->isCorrected)
					pPersonalize->correctedindex = 5;
			case 13:
			case 14:
			case 15:
			case 16:
				if (pPersonalize->isCorrected && correctedindex == 16)
					pPersonalize->correctedindex = 15;
				//Corrects random fake angles (ballerina) most of the time
				yaw = ResolveRandomFake(pCPlayer);
#ifdef _DEBUG
				if (Draw)
					DrawStringUnencrypted("Estimate mode: Random Fake", Vector2D(screen.x - 200, screen.y + 25), ColorRGBA(255, 0, 0, 255), pFont);
#endif
				return;
			case 7:
			case 8:
				if (pPersonalize->isCorrected && correctedindex == 8)
					pPersonalize->correctedindex = 7;
				//Corrects static fakeangles
				yaw = ResolveStaticFake(pCPlayer);
#ifdef _DEBUG
				if (Draw)
					DrawStringUnencrypted("Estimate mode: Static Fake", Vector2D(screen.x - 200, screen.y + 25), ColorRGBA(255, 0, 0, 255), pFont);
#endif
				return;
			case 17:
			case 18:
				if (pPersonalize->isCorrected && correctedindex == 18)
					pPersonalize->correctedindex = 17;
				//Corrects static dynamic fakeangles
				yaw = ResolveStaticFakeDynamic(pCPlayer);
#ifdef _DEBUG
				if (Draw)
					DrawStringUnencrypted("Estimate mode: Static Fake Dynamic", Vector2D(screen.x - 200, screen.y + 25), ColorRGBA(255, 0, 0, 255), pFont);
#endif
				return;
			}

		}
		break;
	}
	}
	ClampY(yaw);
	if (yaw < 0.0f)
		yaw += 360.0f;
	VMP_END
}

//Sets the ideal pitch for this player
void LagCompensation::GetIdealPitch(float& pitch, int correctedindex, CustomPlayer* pCPlayer, CBaseEntity* Entity, CBaseEntity* pLocalEntity)
{
	Personalize *pPersonalize = &pCPlayer->Personalize;
	ResolvePitchModes mode = pPersonalize->ResolvePitchMode;

	switch (mode)
	{
	case ResolvePitchModes::NoPitch:
		return;
	case ResolvePitchModes::AutomaticPitch:

		if (!ValveResolverChk.Checked)
		{
			if (pitch < -179.f) pitch += 360.f;
			else if (pitch > 90.0 || pitch < -90.0) pitch = 89.f;
			else if (pitch > 89.0 && pitch < 91.0) pitch -= 90.f;
			else if (pitch > 179.0 && pitch < 181.0) pitch -= 180;
			else if (pitch > -179.0 && pitch < -181.0) pitch += 180;
			else if (fabs(pitch) == 0) pitch = std::copysign(89.0f, pitch);

			switch (correctedindex % 12)
			{
			case 10:
				pitch = -89.0f;
				break;
			case 11:
				pitch = 89.0f;
				break;

			}
		}
		break;
	case ResolvePitchModes::Up:
		pitch = -89.0f;
		break;
	case ResolvePitchModes::Down:
		pitch = 89.0f;
		break;
	case ResolvePitchModes::CustomStaticPitch:
		pitch = ResolvePitchTxt.flValue;
		break;
	case ResolvePitchModes::CustomAdditivePitch:
		pitch += ResolvePitchTxt.flValue;
		break;
	}
	ClampX(pitch);
	if (pitch < 0.0f)
		pitch += 360.0f;
}

float LagCompensation::ResolveCloseFakeAngles(CustomPlayer* pCPlayer)
{
	VMP_BEGINMUTILATION("RCFA")
	float yaw;
	StoredNetvars *CurrentNetVars = &pCPlayer->CurrentNetvarsFromProxy;

	//MAX_NEAR_BODY_YAW_RESOLVER_MISSES is the max
	switch (pCPlayer->Personalize.correctedclosefakeindex)
	{
		default:
		case 0:
			yaw = CurrentNetVars->lowerbodyyaw;
			break;
		case 1:
			yaw = CurrentNetVars->lowerbodyyaw + 17.5f;
			break;
		case 2:
			yaw = CurrentNetVars->lowerbodyyaw - 17.5f;
			break;
		case 3:
			yaw = CurrentNetVars->lowerbodyyaw + 35.0f;
			break;
		case 4:
			yaw = CurrentNetVars->lowerbodyyaw - 35.0f;
			break;
		case 5:
			return ResolveStaticFakeDynamic(pCPlayer);
	}

	ClampY(yaw);

	if (yaw < 0.0f)
		yaw += 360.0f;

	pCPlayer->Personalize.LastResolveModeUsed = ResolveYawModes::CloseFake;

	return yaw;
VMP_END
}

//Also works very well for static fake angles
float LagCompensation::ResolveFakeSpins(CustomPlayer* pCPlayer)
{
	VMP_BEGINMUTILATION("RFS")
	float yaw;
	StoredNetvars *CurrentNetVars = &pCPlayer->CurrentNetvarsFromProxy;

	yaw = ClampYr(ClampYr(CurrentNetVars->eyeangles.y) - pCPlayer->LowerBodyDelta);
	if (yaw < 0.0f)
		yaw += 360.0f;

	pCPlayer->Personalize.LastResolveModeUsed = ResolveYawModes::FakeSpins;

	return yaw;
	VMP_END
}

float LagCompensation::ResolveLinearFake(CustomPlayer* pCPlayer)
{
	VMP_BEGINMUTILATION("RLF")
	float yaw;
	StoredNetvars *CurrentNetVars = &pCPlayer->CurrentNetvarsFromProxy; //pCPlayer->Personalize.m_PlayerRecords.front();

	yaw = ClampYr(CurrentNetVars->lowerbodyyaw + pCPlayer->LowerBodyDelta);// +(pCPlayer->LowerBodyDelta * 0.5f);

	if (yaw < 0.0f)
		yaw += 360.0f;

	pCPlayer->Personalize.LastResolveModeUsed = ResolveYawModes::LinearFake;

	return yaw;
	VMP_END
}

float LagCompensation::ResolveRandomFake(CustomPlayer* pCPlayer)
{
	VMP_BEGINMUTILATION("RRF")
	float yaw;
	StoredNetvars *CurrentNetVars = &pCPlayer->CurrentNetvarsFromProxy;

	yaw = ClampYr(CurrentNetVars->eyeangles.y + (pCPlayer->LowerBodyEyeDelta * 0.5f)); //taps ballerina most of the time

	if (yaw < 0.0f)
		yaw += 360.0f;

	pCPlayer->Personalize.LastResolveModeUsed = ResolveYawModes::RandomFake;

	return yaw;
	VMP_END
}

float LagCompensation::ResolveAtTargetFake(CustomPlayer* pCPlayer)
{
	VMP_BEGINMUTILATION("RATF")
	float yaw;
	float dt;
	StoredNetvars *CurrentNetVars = pCPlayer->GetCurrentRecord();

	QAngle AngleToUs = CalcAngle(CurrentNetVars->networkorigin, LocalPlayer.Entity->GetNetworkOrigin());

	if (pCPlayer->Personalize.ShotsMissed < 2)
	{
		yaw = AngleToUs.y;
	}
	else if (pCPlayer->Personalize.ShotsMissed % 2 == 0)
	{
		yaw = ClampYr(AngleToUs.y + 20.0f);

		//FIXME: this still isn't right
		dt = ClampYr(yaw - CurrentNetVars->lowerbodyyaw);
		if (fabsf(dt) > 35.0f)
		{
			yaw = ClampYr(CurrentNetVars->lowerbodyyaw + 20.0f);
		}
	}
	else
	{
		yaw = ClampYr(AngleToUs.y - 20.0f);

		//FIXME: this still isn't right
		dt = ClampYr(yaw - CurrentNetVars->lowerbodyyaw);
		if (fabsf(dt) > 35.0f)
		{
			yaw = ClampYr(CurrentNetVars->lowerbodyyaw - 20.0f);
		}
	}
	if (yaw < 0.0f)
		yaw += 360.0f;
	pCPlayer->Personalize.LastResolveModeUsed = ResolveYawModes::AtTarget;
	return yaw;
	VMP_END
}

float LagCompensation::ResolveInverseAtTargetFake(CustomPlayer* pCPlayer)
{
	VMP_BEGINMUTILATION("RIATF")
	float yaw;
	float dt;
	StoredNetvars *CurrentNetVars = pCPlayer->GetCurrentRecord();

	QAngle AngleFromUsToTarget = CalcAngle(LocalPlayer.Entity->GetOrigin(), CurrentNetVars->origin);

	if (pCPlayer->Personalize.ShotsMissed < 1)
	{
		yaw = AngleFromUsToTarget.y;
	}
	else if (pCPlayer->Personalize.ShotsMissed % 2 == 0)
	{
		yaw = ClampYr(AngleFromUsToTarget.y + 20.0f);

		//FIXME: this still isn't right
		dt = ClampYr(yaw - CurrentNetVars->lowerbodyyaw);
		if (fabsf(dt) > 35.0f)
		{
			yaw = ClampYr(CurrentNetVars->lowerbodyyaw + 20.0f);
		}
	}
	else
	{
		yaw = ClampYr(AngleFromUsToTarget.y - 20.0f);

		//FIXME: this still isn't right
		dt = ClampYr(yaw - CurrentNetVars->lowerbodyyaw);
		if (fabsf(dt) > 35.0f)
		{
			yaw = ClampYr(CurrentNetVars->lowerbodyyaw - 20.0f);
		}
	}

	if (yaw < 0.0f)
		yaw += 360.0f;
	pCPlayer->Personalize.LastResolveModeUsed = ResolveYawModes::InverseAtTarget;
	return yaw;
	VMP_END
}

float LagCompensation::ResolveStaticFake(CustomPlayer* pCPlayer)
{
	VMP_BEGINMUTILATION("RSF")
	float yaw;
	StoredNetvars *CurrentNetVars = &pCPlayer->CurrentNetvarsFromProxy;

	yaw = CurrentNetVars->lowerbodyyaw;

	if (yaw < 0.0f)
		yaw += 360.0f;

	pCPlayer->Personalize.LastResolveModeUsed = ResolveYawModes::StaticFake;

	return yaw;
	VMP_END
}

float LagCompensation::ResolveStaticFakeDynamic(CustomPlayer* pCPlayer)
{
	VMP_BEGINMUTILATION("RSFD")
	float yaw;
	StoredNetvars *CurrentNetVars = pCPlayer->Personalize.m_PlayerRecords.front();
	//StoredLowerBodyYaw *CurrentLowerBodyVars = pCPlayer->Personalize.m_PlayerLowerBodyRecords.front();

	yaw = ClampYr(CurrentNetVars->eyeangles.y + pCPlayer->LowerBodyEyeDelta);

	if (yaw < 0.0f)
		yaw += 360.0f;

	pCPlayer->Personalize.LastResolveModeUsed = ResolveYawModes::StaticFakeDynamic;

	return yaw;
	VMP_END
}

float LagCompensation::ResolveAverageLBYDelta(CustomPlayer* pCPlayer)
{
	VMP_BEGINMUTILATION("RALBYD")
	float yaw;
	StoredNetvars *CurrentNetVars = pCPlayer->Personalize.m_PlayerRecords.front();
	//StoredLowerBodyYaw *CurrentLowerBodyVars = pCPlayer->Personalize.m_PlayerLowerBodyRecords.front();

	yaw = pCPlayer->PredictedAverageFakeAngle + (45.0f * pCPlayer->Personalize.ShotsMissed);

	if (yaw < 0.0f)
		yaw += 360.0f;

	pCPlayer->Personalize.LastResolveModeUsed = ResolveYawModes::AverageLBYDelta;

	return yaw;
	VMP_END
}

float LagCompensation::ResolveBloodReal(CustomPlayer* pCPlayer)
{
	VMP_BEGINMUTILATION("RBR")
	float yaw;

	yaw = pCPlayer->LastRealYaw;

	pCPlayer->Personalize.LastResolveModeUsed = ResolveYawModes::BloodReal;
	
	return yaw;
	VMP_END
}

float LagCompensation::ResolveBloodBodyRealDelta(CustomPlayer* pCPlayer)
{
	VMP_BEGINMUTILATION("RBBRD")
	float yaw;
	StoredNetvars *CurrentNetVars = pCPlayer->Personalize.m_PlayerRecords.front();

	yaw = ClampYr(CurrentNetVars->lowerbodyyaw + pCPlayer->LastBodyRealDelta);

	if (yaw < 0.0f)
		yaw += 360.0f;

	pCPlayer->Personalize.LastResolveModeUsed = ResolveYawModes::BloodBodyRealDelta;

	return yaw;
	VMP_END
}

float LagCompensation::ResolveBloodEyeRealDelta(CustomPlayer* pCPlayer)
{
	VMP_BEGINMUTILATION("RBERD")
	float yaw;
	StoredNetvars *CurrentNetVars = pCPlayer->Personalize.m_PlayerRecords.front();

	yaw = ClampYr(CurrentNetVars->eyeangles.y + pCPlayer->LastEyeRealDelta);

	if (yaw < 0.0f)
		yaw += 360.0f;

	pCPlayer->Personalize.LastResolveModeUsed = ResolveYawModes::BloodEyeRealDelta;

	return yaw;
	VMP_END
}

float LagCompensation::ResolveFakeWalk(CustomPlayer* pCPlayer)
{
	VMP_BEGINMUTILATION("RBERD")
	float yaw;
	int correctedinex = pCPlayer->Personalize.correctedfakewalkindex;

	if (correctedinex < 2)
	{
		yaw = ClampYr(pCPlayer->DirectionWhenFirstStartedMoving.y + 180.0f);
	}
	else if (correctedinex < 4)
	{
		QAngle dir;
		VectorAngles(pCPlayer->CurrentNetvarsFromProxy.velocity, dir);
		yaw = ClampYr(dir.y + 180.0f);
	}
	else if (correctedinex < 6)
	{
		yaw = pCPlayer->DirectionWhenFirstStartedMoving.y;
	}
	else
	{
		QAngle dir;
		VectorAngles(pCPlayer->CurrentNetvarsFromProxy.velocity, dir);
		yaw = dir.y;
	}

	if (yaw < 0.0f)
		yaw += 360.0f;

	pCPlayer->Personalize.LastResolveModeUsed = ResolveYawModes::FakeWalk;
	return yaw;
	VMP_END
}

void LagCompensation::CorrectFeetYaw(CBaseEntity* const Entity, float flClampedYaw, float flCorrect)
{
	VMP_BEGINMUTILATION("CFY")
	float newyaw = flClampedYaw;

	if (newyaw < 0.0f)
	{
		newyaw += flCorrect;
	}
	else if (newyaw > 0.0f)
	{
		newyaw -= flCorrect;
	}
#ifdef _DEBUG
	else
	{
		//FIXME
		//DebugBreak();
	}
#endif

	while (newyaw > 360.0f)
		newyaw -= 360.0f;

	while (newyaw < -360.0f)
		newyaw += 360.0f;

	//Feet yaw must always be 0-360 range
	if (newyaw < 0.0f)
		newyaw += 360.0f;

	Entity->SetCurrentFeetYaw(newyaw);
	Entity->SetGoalFeetYaw(newyaw);
	VMP_END
}


//Attempts to predict real player's yaw from blood received
bool LagCompensation::PredictRealYawFromBlood(CustomPlayer *const pCPlayer, CBaseEntity*const Entity, float yaw)
{
	VMP_BEGINMUTILATION("PRYFB")
	auto stat = START_PROFILING("PredictRealYawFromBlood");
	//FIXME: CHECK FOR INVALID DELTAS!!
	bool bCorrectPrediction = false;
	Personalize *pPersonalize = &pCPlayer->Personalize;
	StoredNetvars *CurrentVars = &pCPlayer->CurrentNetvarsFromProxy;
	//auto BloodRecords = &pPersonalize->m_BloodRecords;
	
	float BodyRealDelta = ClampYr(CurrentVars->lowerbodyyaw - yaw);
	float EyeRealDelta = ClampYr(CurrentVars->eyeangles.y - yaw);
	float RealDelta = ClampYr(yaw - pCPlayer->LastRealYaw);
	if (fabsf(RealDelta < 10.0f))
	{
		pPersonalize->EstimateYawMode = ResolveYawModes::BloodReal;
		bCorrectPrediction = true;
	}
	if (fabsf(BodyRealDelta - pCPlayer->LastBodyRealDelta) < 10.0f)
	{
		pPersonalize->EstimateYawMode = ResolveYawModes::BloodBodyRealDelta;
		bCorrectPrediction = true;
	}
	if (fabsf(EyeRealDelta - pCPlayer->LastEyeRealDelta) < 10.0f)
	{
		pPersonalize->EstimateYawMode = ResolveYawModes::BloodEyeRealDelta;
		bCorrectPrediction = true;
	}
	pCPlayer->LastBodyRealDelta = BodyRealDelta;
	pCPlayer->LastEyeRealDelta = EyeRealDelta;
	pCPlayer->LastRealYaw = yaw;

	END_PROFILING(stat);
	return bCorrectPrediction;
	VMP_END
}

//char *baimbloodresolveheadposstr = new char[26]{ 56, 21, 30, 3, 90, 59, 19, 23, 90, 40, 31, 9, 21, 22, 12, 31, 30, 90, 50, 31, 27, 30, 42, 21, 9, 0 }; /*Body Aim Resolved HeadPos*/

void LagCompensation::OnPlayerBloodSpawned(CBaseEntity* const Entity, Vector* const origin, Vector* const vecDir)
{
	if (BloodResolverChk.Checked)
	{
		CustomPlayer* pCPlayer = &AllPlayers[Entity->index];
		Personalize *pPersonalize = &pCPlayer->Personalize;
		if (pCPlayer->BaseEntity && !pCPlayer->IsLocalPlayer && pPersonalize->ShouldResolve())
		{
			//OnPlayerBloodSpawned seems to happen after the game events always, so no need for absolute value
			float flSimulationTime = pCPlayer->CurrentNetvarsFromProxy.simulationtime;
			if (flSimulationTime - pPersonalize->flSimulationTimePlayerWasShot < 0.5f)
			{
				pPersonalize->flSimulationTimeBloodWasSpawned = flSimulationTime;
				StoredNetvars tmpvars = StoredNetvars(Entity, pCPlayer, nullptr);
				tmpvars.absorigin = *Entity->GetAbsOrigin();
				tmpvars.absangles = Entity->GetAbsAngles();
				QAngle oRenderAngles = Entity->GetRenderAngles();
				int IdealHitGroup = pPersonalize->iHitGroupPlayerWasShotOnServer;

				const Vector tracestart = *origin + (*vecDir * 64.0f);
				const Vector traceexclude = *origin + (*vecDir * 1.0f); //don't count as valid if trace still hits slightly outside of the origin to get a more accurate angle
				CTraceFilterTargetSpecificEntity filter;
				filter.m_icollisionGroup = COLLISION_GROUP_NONE;
				filter.pTarget = Entity;
				Ray_t ray;
				trace_t tr;
				StoredNetvars *ResolvedTick = nullptr;

				if (pPersonalize->m_PlayerRecords.size() == 0)
					return;

				for (auto &tick : pPersonalize->m_PlayerRecords)
				{
					if (tick->simulationtime != pPersonalize->flSimulationTimePlayerWasShot)
						continue;
					ResolvedTick = tick;
					break;
				}

				if (!ResolvedTick)
					return;

				CM_RestoreAnimations(ResolvedTick, Entity);
				if (Entity->GetOrigin() != ResolvedTick->networkorigin)
				{
					//Entity->SetOrigin(ResolvedTick->networkorigin);
					//Entity->InvalidatePhysicsRecursive(POSITION_CHANGED);
					Entity->SetAbsOrigin(ResolvedTick->networkorigin);
				}

				QAngle neweyeangles = QAngle(ResolvedTick->eyeangles.x, tmpvars.absangles.y, ResolvedTick->eyeangles.z); //ResolvedTick->eyeangles;
				QAngle firsteyeanglesfound;
				float firstbodyyawfound;
				bool FoundRealHead = false; //Real head should be the exact spot the head is
				bool FoundGeneralHead = false; //General head is an area where the head will most likely generally be. It's not an exact spot
				//Entity->DrawHitboxes(ColorRGBA(255, 255, 255, 255), 0.6f);
				matrix3x4_t tmpmatrix[MAXSTUDIOBONES];
				float originaleyeyaw = Entity->EyeAngles()->y;


				for (float anginc = 0.0f; anginc <= 1080.0f; anginc += 4.0f)
				{
					float body_yaw = anginc <= 360.0f ? 0.0f : anginc <= 720.0f ? -1.0f : 1.0f;
					Entity->SetPoseParameterScaled(11, body_yaw);

					Entity->InvalidateBoneCache();
					//Entity->SetRenderAngles(neweyeangles);
					//Entity->SetEyeAngles(neweyeangles);
					//Entity->EyeAngles()->y = neweyeangles.y;
					//Entity->UpdateClientSideAnimation();
					Entity->SetAbsAngles(QAngle(tmpvars.absangles.x, neweyeangles.y, tmpvars.absangles.z));
					
					Entity->SetupBones(tmpmatrix, MAXSTUDIOBONES, BONE_USED_BY_ANYTHING, /*Entity->GetSimulationTime()*/ Interfaces::Globals->curtime);

#ifndef USE_TRACERAY
					mstudiohitboxset_t *set = Interfaces::ModelInfoClient->GetStudioModel(Entity->GetModel())->pHitboxSet(Entity->GetHitboxSet());
					std::vector<CSphere>m_cSpheres;
					std::vector<COBB>m_cOBBs;

					for (int i = 0; i < set->numhitboxes; i++)
					{
						mstudiobbox_t *pbox = set->pHitbox(i);
						if (pbox->group == IdealHitGroup)
						{
							if (pbox->radius != -1.0f)
							{
								Vector vMin, vMax;
								VectorTransformZ(pbox->bbmin, tmpmatrix[pbox->bone], vMin);
								VectorTransformZ(pbox->bbmax, tmpmatrix[pbox->bone], vMax);
								SetupCapsule(vMin, vMax, pbox->radius, pbox->group, m_cSpheres);
							}
							else
							{
								m_cOBBs.push_back(COBB(pbox->bbmin, pbox->bbmax, &tmpmatrix[pbox->bone], pbox->group));
							}
						}
					}
#endif

					//Entity->DrawHitboxesFromCache(ColorRGBA(255, 0, 0, 255), 0.6f, tmpmatrix);

					ray.Init(tracestart, *origin);

#ifndef USE_TRACERAY
					TRACE_HITBOX(Entity, ray, tr, m_cSpheres, m_cOBBs);
#else
					Interfaces::EngineTrace->TraceRay(ray, MASK_SHOT, &filter, &tr);
#endif
					if (tr.m_pEnt == Entity && tr.hitgroup == IdealHitGroup)
					{
						//Entity->DrawHitboxesFromCache(ColorRGBA(255, 0, 0, 255), 1.6f, tmpmatrix);
						if (!FoundGeneralHead)
						{
							firstbodyyawfound = body_yaw;
							firsteyeanglesfound = neweyeangles;
							FoundGeneralHead = true;
						}
						//Make sure if we trace slightly out of the impact origin, we don't still hit the same thing, otherwise the angle isn't correct
						ray.Init(tracestart, traceexclude);
#ifndef USE_TRACERAY
						TRACE_HITBOX(Entity, ray, tr, m_cSpheres, m_cOBBs);
#else
						Interfaces::EngineTrace->TraceRay(ray, MASK_SHOT, &filter, &tr);
#endif
						if (!tr.m_pEnt || tr.hitgroup != IdealHitGroup)
						{
							FoundRealHead = true;
							break;
						}
					}
					neweyeangles.y = ClampYr(neweyeangles.y + 1.0f);
					if (neweyeangles.y < 0.0f)
						neweyeangles.y += 360.0f;
				}

				//Entity->DrawHitboxesFromCache(ColorRGBA(255, 0, 0, 255), 0.6f, tmpmatrix);

				if (!FoundRealHead && FoundGeneralHead)
				{
					neweyeangles = firsteyeanglesfound;
				}

				if (FoundRealHead || FoundGeneralHead)
				{
					if (!FoundRealHead)
					{
						Entity->SetPoseParameter(11, firstbodyyawfound);
						Entity->SetAbsAngles(QAngle(tmpvars.absangles.x, firsteyeanglesfound.y, tmpvars.absangles.z));
					}

					if (DrawBloodResolveChk.Checked)
					{
						Vector HeadPos = Entity->GetBonePositionCachedOnly(HITBOX_HEAD, Interfaces::Globals->curtime, tmpmatrix);
						//DecStr(baimbloodresolveheadposstr, 25);
						//Interfaces::DebugOverlay->AddTextOverlay(Vector(HeadPos.x, HeadPos.y, HeadPos.z + 1.0f), 1.0f, baimbloodresolveheadposstr);
						//EncStr(baimbloodresolveheadposstr, 25);
						Interfaces::DebugOverlay->AddBoxOverlay(HeadPos, Vector(-2, -2, -2), Vector(2, 2, 2), QAngle(0, 0, 0), 0, 255, 0, 255, 1.0f);
					}

					//INetChannelInfo *nci = Interfaces::Engine->GetNetChannelInfo();
					//float Latency = nci ? (nci->GetAvgLatency(FLOW_OUTGOING) + nci->GetAvgLatency(FLOW_INCOMING)) : 0.0f;

					if (BacktrackToBloodChk.Checked)
					{
						float cursimulation = flSimulationTime + GetLerpTime();
						StoredNetvars *NewTick = new StoredNetvars(Entity, pCPlayer, false);
						NewTick->absangles = Entity->GetAbsAngles();
						NewTick->absorigin = *Entity->GetAbsOrigin();
						//NewTick->tickcount = TIME_TO_TICKS(cursimulation - Latency); //ResolvedTick->tickcount;
						//if (!bTickIsValid(NewTick->tickcount))
						NewTick->tickcount = TIME_TO_TICKS(cursimulation);
						NewTick->ResolvedFromBloodSpawning = true;
						NewTick->ReceivedPoseParametersAndAnimations = true;
						NewTick->FireAtPelvis = false;
						pCPlayer->LatestBloodTick = NewTick;
					}

					pPersonalize->angEyeAnglesResolvedFromBlood = neweyeangles;

					//Store new blood record
					BloodRecord *nBloodRecord = new BloodRecord(Entity, &pCPlayer->CurrentNetvarsFromProxy, neweyeangles.y);
					auto BloodRecords = &pPersonalize->m_BloodRecords;
					BloodRecords->push_front(nBloodRecord);

					//Don't store too many blood records
					if (BloodRecords->size() > 8)
					{	
						delete BloodRecords->back(); //Free memory used by the record
						BloodRecords->pop_back(); //Delete the record
					}

					if (PredictFakeAnglesChk.Checked)
					{
						PredictRealYawFromBlood(pCPlayer, Entity, neweyeangles.y);
					}
				}

				//Restore pre-modified values
				Entity->InvalidateBoneCache();
				//Entity->SetRenderAngles(oRenderAngles);
				Entity->EyeAngles()->y = originaleyeyaw;
				if (Entity->GetAbsAngles() != tmpvars.absangles)
					Entity->SetAbsAngles(tmpvars.absangles);
				CM_RestoreAnimations(&tmpvars, Entity);
				CM_RestoreNetvars(&tmpvars, Entity, &tmpvars.absorigin);
				Entity->SetGoalFeetYaw(tmpvars.GoalFeetYaw);
				Entity->SetCurrentFeetYaw(tmpvars.CurrentFeetYaw);
				Entity->SetEyeAngles(tmpvars.eyeangles);
				LocalPlayer.Entity->SetupBones(tmpmatrix, MAXSTUDIOBONES, BONE_USED_BY_ANYTHING, Interfaces::Globals->curtime);
			}
		}
	}
}

#ifdef _DEBUG
#include <fstream>
//#define DEBUG_FILE
#ifdef FEET_YAW_DEBUG_FILE
std::ofstream feetyawdebugfile;
#endif

void LagCompensation::OutputFeetYawInformation(CustomPlayer* const pCPlayer, CBaseEntity* const Entity)
{
	static float sCurrentFeetYaw[34];
	static float sGoalFeetYaw[34];
	static float sLowerBodyYaw[34];

	AllocateConsole();

	float CurrentFeetYaw = Entity->GetCurrentFeetYaw();
	float GoalFeetYaw = Entity->GetGoalFeetYaw();
	float LowerBodyYaw = pCPlayer->CurrentNetvarsFromProxy.lowerbodyyaw;
	int index = pCPlayer->Index;

	//If the current does match the saved values then print the changes
	if (CurrentFeetYaw != sCurrentFeetYaw[index]
		|| GoalFeetYaw != sGoalFeetYaw[index]
		|| LowerBodyYaw != sLowerBodyYaw[index])
	{
		wchar_t PlayerName[32];
		GetPlayerName(GetRadar(), index, PlayerName);

		char debuginfo[1024];
		sprintf(debuginfo, "%S : CFY %.3f : GFY %.3f : LBY %.3f : LMB %i\n", PlayerName, ClampYr(CurrentFeetYaw), ClampYr(GoalFeetYaw), LowerBodyYaw, (BOOL)(GetAsyncKeyState(VK_LBUTTON) & (1 << 16)) > 0);

		printf(debuginfo);

#ifdef FEET_YAW_DEBUG_FILE
		if (!feetyawdebugfile.is_open())
		{
			feetyawdebugfile.open("G:\\debug_ft.txt", std::ios::out);
		}

		if (feetyawdebugfile.is_open())
		{
			feetyawdebugfile << debuginfo << "\n";
		}
#endif

		//Store current values
		sCurrentFeetYaw[index] = CurrentFeetYaw;
		sGoalFeetYaw[index] = GoalFeetYaw;
		sLowerBodyYaw[index] = LowerBodyYaw;
	}
}


#ifdef LOWERBODY_YAW_DEBUG_FILE
std::ofstream lowerbodyyawdebugfile;
#endif

void LagCompensation::OutputLowerBodyInformation(CustomPlayer* const pCPlayer, CBaseEntity* const Entity)
{
	if (!AllocedConsole)
	{
		AllocateConsole();
	}

	wchar_t PlayerName[32];
	GetPlayerName(GetRadar(), pCPlayer->Index, PlayerName);

	char debuginfo[1024];
	sprintf(debuginfo, "%S : LBY %.3f\n", PlayerName, pCPlayer->CurrentNetvarsFromProxy.lowerbodyyaw);

	printf(debuginfo);

#ifdef LOWERBODY_YAW_DEBUG_FILE
	if (!lowerbodyyawdebugfile.is_open())
	{
		lowerbodyyawdebugfile.open("G:\\debug_lb.txt", std::ios::out);
	}

	if (lowerbodyyawdebugfile.is_open())
	{
		lowerbodyyawdebugfile << debuginfo << "\n";
	}
#endif
}

std::ofstream serveryawsfile;
void LagCompensation::OutputServerYaws(float eyeyaw, float goalfeet, float curfeet)
{
	if (!serveryawsfile.is_open())
	{
		serveryawsfile.open("G:\\debug_sy.txt", std::ios::out);
	}

	if (serveryawsfile.is_open())
	{
		serveryawsfile << "EyeYaw " << eyeyaw << " GFY " << goalfeet << " CFY " << curfeet << "\n";
		serveryawsfile.flush();
	}
}

#endif


static std::deque<CIncomingSequence>sequences;
static int lastincomingsequencenumber;

//call in createmove 
void LagCompensation::UpdateIncomingSequences()
{
	DWORD ClientState = (DWORD)ReadInt(pClientState);
	if (ClientState)
	{
		INetChannel *netchan = (INetChannel*)ReadInt((uintptr_t)(ClientState + 0x9C));

		if (netchan)
		{
			if (LocalPlayer.Real_m_nInSequencenumber == 0)
			{
				LocalPlayer.Real_m_nInSequencenumber = netchan->m_nInSequenceNr;
				lastincomingsequencenumber = netchan->m_nInSequenceNr;
				//LocalPlayer.ModifiedSequenceNr = false;
			}

			if (netchan->m_nInSequenceNr > lastincomingsequencenumber)
			{
				//sequences.push_front(netchan->m_nInSequenceNr);
				lastincomingsequencenumber = netchan->m_nInSequenceNr;

				sequences.push_front(CIncomingSequence(netchan->m_nInReliableState, netchan->m_nOutReliableState, netchan->m_nInSequenceNr, Interfaces::Globals->realtime));
			}

			if (sequences.size() > 2048)
				sequences.pop_back();
		}
	}
}

void LagCompensation::ClearIncomingSequences()
{
	LocalPlayer.Real_m_nInSequencenumber = 0.0f;
	sequences.clear();
}

//Adds latency to the local player
void LagCompensation::AddLatencyToNetchan(INetChannel *netchan, float Latency)
{
	for (auto& seq : sequences)
	{
		if (Interfaces::Globals->realtime - seq.curtime >= Latency)
		{
			netchan->m_nInReliableState = seq.inreliablestate;
			//netchan->m_nOutReliableState = seq.outreliablestate;
			netchan->m_nInSequenceNr = seq.sequencenr;
			break;
		}
	}
}