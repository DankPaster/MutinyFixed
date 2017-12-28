#include "NetVars.h"
#include "CreateMove.h"
#include "LocalPlayer.h"
#include "player_lagcompensation.h"
#include "INetchannelInfo.h"

//NETVAR HOOKS ARE AT THE BOTTOM

void OnReceivedPitch(CustomPlayer *pCPlayer, float pitch)
{
	Personalize *pPersonalize = &pCPlayer->Personalize;
	if (pPersonalize->flLastPitch == 0.0f)
	{
		pPersonalize->flLastPitch = pitch;
	}
	else
	{
		float changeddegrees = pitch - pPersonalize->flLastPitch;
		pPersonalize->flLastPitch = pitch;
		if (changeddegrees > 180.0f)
		{
			changeddegrees -= 360.0f;
		}
		else if (changeddegrees < -180.0f)
		{
			changeddegrees += 360.0f;
		}
		pPersonalize->flLastPitchChange = changeddegrees;
	}
}


DWORD oAimProxyX = NULL;
void AntiAntiAimProxyX(const CRecvProxyData *pData, void *ent, void *pOut)
{
	if (oAimProxyX)
	{
		((void(*)(const CRecvProxyData *, void *, void *))oAimProxyX)(pData, ent, pOut);
	}

	if (!pOut)
		return;

	float val = ReadFloat((uintptr_t)&pData->m_Value.m_Float);

	CBaseEntity* pent = (CBaseEntity*)ent;
	if (pent && pent->IsPlayer())
	{
		if (pent == LocalPlayer.Entity)
		{
			LocalPlayer.EyeAnglesOnServer.x = ClampXr(val);
		}
		else
		{
			int index = ReadInt((uintptr_t)&pent->index);
			CustomPlayer *pCAPlayer = &AllPlayers[index];
			const float SimulationTime = pent->GetSimulationTime();
			if (SimulationTime != pCAPlayer->SimulationTimeReceivedNewEyeAngleX || pCAPlayer->CurrentNetvarsFromProxy.eyeangles.x != val || (val != 0.0f && pCAPlayer->CurrentNetvarsFromProxy.eyeangles.x == 0.0f))
			{
				OnReceivedPitch(pCAPlayer, val);
				pCAPlayer->CurrentNetvarsFromProxy.eyeangles.x = val;
				pCAPlayer->CurrentNetvarsFromProxy.eyeyaw = val;
				//	if (val == 0.0f)
				//		DebugBreak();
				//pCAPlayer->DataUpdated = true;
				//pCAPlayer->EyeAnglesUpdated = true;
				pCAPlayer->SimulationTimeReceivedNewEyeAngleX = SimulationTime;
			}
		}
	}
	//WriteFloat((uintptr_t)pOut, val);
}

void OnReceivedYaw(CustomPlayer *pCPlayer, float yaw)
{
	//Check to see if player is doing a linear spinbot
	Personalize *pPersonalize = &pCPlayer->Personalize;
	if (pPersonalize->flLastYaw == 0.0f)
	{
		pPersonalize->flLastYaw = yaw;
		pPersonalize->bSpinbotting = false;
	}
	else
	{
		float changeddegrees = yaw - pPersonalize->flLastYaw;
		if (changeddegrees != 0.0f)
		{
			pPersonalize->flLastYaw = yaw;
			if (changeddegrees > 180.0f)
			{
				changeddegrees -= 360.0f;
			}
			else if (changeddegrees < -180.0f)
			{
				changeddegrees += 360.0f;
			}

			if (pPersonalize->flLastYawChange == 0.0f || changeddegrees == 0.0f)
			{
				pPersonalize->bSpinbotting = false;
			}
			else if (((changeddegrees > 0 && pPersonalize->flLastYawChange > 0) || (changeddegrees < 0 && pPersonalize->flLastYawChange < 0)) && fabsf(changeddegrees) >= 15.0f && fabsf(changeddegrees - pPersonalize->flLastYawChange) <= 10.0f)
			{
				pPersonalize->bSpinbotting = true;
			}
			else
			{
				pPersonalize->bSpinbotting = false;
			}
			pPersonalize->flLastYawChange = changeddegrees;
			pPersonalize->bEyeYawUpdated = true;
		}
	}
}

DWORD oAimProxyY = NULL;
void AntiAntiAimProxyY(const CRecvProxyData *pData, void *ent, void *pOut)
{
	if (oAimProxyY)
	{
		((void(*)(const CRecvProxyData *, void *, void *))oAimProxyY)(pData, ent, pOut);
	}

	if (!pOut)
		return;

	float val = ReadFloat((uintptr_t)&pData->m_Value.m_Float);

	CBaseEntity* pent = (CBaseEntity*)ent;
	if (pent && pent->IsPlayer())
	{
		if (pent == LocalPlayer.Entity)
		{
			LocalPlayer.EyeAnglesOnServer.y = ClampYr(val);
		}
		else
		{
			int index = ReadInt((uintptr_t)&pent->index);
			CustomPlayer *pCAPlayer = &AllPlayers[index];
			const float SimulationTime = pent->GetSimulationTime();
			if (SimulationTime != pCAPlayer->SimulationTimeReceivedNewEyeAngleY || pCAPlayer->CurrentNetvarsFromProxy.eyeangles.y != val || (val != 0.0f && pCAPlayer->CurrentNetvarsFromProxy.eyeyaw == 0.0f))
			{
				//Eye angle update
				//ClampY(val);
				OnReceivedYaw(pCAPlayer, val);
				pCAPlayer->CurrentNetvarsFromProxy.eyeangles.y = val;
				pCAPlayer->CurrentNetvarsFromProxy.eyeyaw = val;
				//pCAPlayer->DataUpdated = true;
				//pCAPlayer->EyeAnglesUpdated = true;
				pCAPlayer->SimulationTimeReceivedNewEyeAngleY = SimulationTime;
			}
		}
	}

	//WriteFloat((uintptr_t)pOut, val);
}

DWORD oAimProxyZ = NULL;
void AntiAntiAimProxyZ(const CRecvProxyData *pData, void *ent, void *pOut)
{
	if (oAimProxyY)
	{
		((void(*)(const CRecvProxyData *, void *, void *))oAimProxyZ)(pData, ent, pOut);
	}

	if (!pOut)
		return;

	float val = ReadFloat((uintptr_t)&pData->m_Value.m_Float);

	CBaseEntity* pent = (CBaseEntity*)ent;
	if (pent && pent->IsPlayer() && pent != LocalPlayer.Entity)
	{
		int index = ReadInt((uintptr_t)&pent->index);
		CustomPlayer *pCAPlayer = &AllPlayers[index];
		pCAPlayer->CurrentNetvarsFromProxy.eyeangles.z = val;
		//Eye angle update
		//pCAPlayer->DataUpdated = true;
	}

	//WriteFloat((uintptr_t)pOut, val);
}

DWORD oLowerBodyProxy = NULL;
void ReceivedLowerBodyYawProxy(const CRecvProxyData *pData, void *ent, void *pOut)
{
	if (oLowerBodyProxy)
	{
		((void(*)(const CRecvProxyData *, void *, void *))oLowerBodyProxy)(pData, ent, pOut);
	}

	CBaseEntity* pent = (CBaseEntity*)ent;
	if (pent && pOut)
	{
		if (pent == LocalPlayer.Entity)
		{
			LocalPlayer.LowerBodyYaw = ReadFloat((uintptr_t)pOut);
			int tickcount = gLagCompensation.GetEstimatedServerTickCount(gLagCompensation.GetNetworkLatency(FLOW_OUTGOING));
			LocalPlayer.LowerBodyYawUpdateTick = tickcount;
			float curtime = ReadFloat((uintptr_t)&Interfaces::Globals->curtime);
			LocalPlayer.NextLowerBodyUpdateTime = curtime;

			if (LocalPlayer.Entity->GetVelocity().Length() == 0.0f)
				LocalPlayer.NextLowerBodyUpdateTime += 1.1f;

			LocalPlayer.LowerBodyYawUpdated = true;
		}
		else if (pent->IsPlayer())
		{
			int index = ReadInt((uintptr_t)&pent->index);
			CustomPlayer *pCAPlayer = &AllPlayers[index];
			float yaw = ReadFloat((uintptr_t)pOut);
			//if (yaw != pCAPlayer->CurrentNetVars.lowerbodyyaw)
			{
				pCAPlayer->TickReceivedLowerBodyUpdate = Interfaces::Globals->tickcount;
				pCAPlayer->CurrentNetvarsFromProxy.lowerbodyyaw = yaw;
				pCAPlayer->LowerBodyUpdated = true;
				//pCAPlayer->DataUpdated = true;
#ifdef _DEBUG
				//#define SHOW_LOWER_BODY
#ifdef SHOW_LOWER_BODY
				gLagCompensation.OutputLowerBodyInformation(pCAPlayer, pent);
#endif
#endif
			}
		}
	}
}

//Attempts to restore (64 tick servers) Simulation time to the exact value it was on the server before it got compressed
float ExtractLostPrecisionForSimulationTime(float val) {
	char Str1[50];
	char Str2[50];
	// convert float to string
	int n = sprintf(Str1, "%f", val);
	// find the index of the decimal point in the string
	int pointLoc = strchr(Str1, '.') - Str1;
	// remove leading zeroes from the end of the string (Very fast, 5 iterations at max)
	int c = n - 1;
	for (; c > pointLoc + 1 && Str1[c] == '0' && Str1[c - 1] == '0'; c--) {
		Str1[c] = 0;
	}
	// remove the decimal point from the string
	memcpy(Str2, Str1, pointLoc);
	memcpy(Str2 + pointLoc, Str1 + pointLoc + 1, n - (pointLoc + 1) - ((n - 1) - c) + 1);
	// convert the string to an int
	long long NewLL = atoll(Str2);
	// round the last digit to the nearest multiple of 25
	long long num25s = round((double)NewLL / 25.0);
	long long nigNew = num25s * 25;
	// convert the newly rounded int in nigNew to a string
	n = sprintf(Str1, "%lld", nigNew);
	// add the decimal point back into the int string
	Str2[pointLoc] = '.';
	// convert the int string back to a floating point string
	memcpy(Str2, Str1, pointLoc);
	memcpy(Str2 + pointLoc + 1, Str1 + pointLoc, n - pointLoc + 1);
	// store the result in nig ( Due the the limitations of floating point format, I was unable to see a difference for some values =/,
	// step through the code in the debugger and you will see how the string version of the float is perfectly rounded, but when it
	// is converted back to a floating point value, it loses that rounding >:( )
	return atof(Str2);
}

DWORD oReceivedSimulationTime = NULL;
void ReceivedSimulationTime(const CRecvProxyData *pData, void *ent, void *pOut)
{
	if (oReceivedSimulationTime)
	{
		((void(*)(const CRecvProxyData *, void *, void *))oReceivedSimulationTime)(pData, ent, pOut);
	}

	CBaseEntity* pent = (CBaseEntity*)ent;
	if (pOut && pent && pent->IsPlayer() && pent != LocalPlayer.Entity)
	{
		int index = ReadInt((uintptr_t)&pent->index);
		CustomPlayer *pCAPlayer = &AllPlayers[index];
		float time = ReadFloat((uintptr_t)pOut); //ExtractLostPrecisionForSimulationTime(ReadFloat((uintptr_t)pOut));
		//if (time != pCAPlayer->CurrentNetvarsFromProxy.simulationtime)
		{
			//pCAPlayer->m_flOldSimulationTime = pCAPlayer->CurrentNetvarsFromProxy.simulationtime;
			if (Interfaces::Globals->interval_per_tick == (1.0f / 64.0f))
				time = ExtractLostPrecisionForSimulationTime(time);
			pCAPlayer->CurrentNetvarsFromProxy.simulationtime = time;
			//pCAPlayer->DataUpdated = true;
		}
	}
}

DWORD oAimPunchProxy = NULL;
void ReceivedAimPunch(const CRecvProxyData *pData, void *ent, void *pOut)
{
	if (oAimPunchProxy)
	{
		((void(*)(const CRecvProxyData *, void *, void *))oAimPunchProxy)(pData, ent, pOut);
	}

	int adr = (int)&oAimPunchProxy;

	CBaseEntity* pent = (CBaseEntity*)((DWORD)ent - m_Local);
	if (pent && pent != LocalPlayer.Entity && pent->IsPlayer())
	{
		int index = ReadInt((uintptr_t)&pent->index);
		CustomPlayer *pCPlayer = &AllPlayers[index];
		//ReadAngle((uintptr_t)pOut, pCPlayer->CurrentNetVars.aimpunch);
		//pCPlayer->DataUpdated = true;
	}
}

DWORD oViewOffsetProxyX = NULL;
void ReceivedViewOffsetX(const CRecvProxyData *pData, void *ent, void *pOut)
{
	if (oViewOffsetProxyX)
	{
		((void(*)(const CRecvProxyData *, void *, void *))oViewOffsetProxyX)(pData, ent, pOut);
	}

	CBaseEntity* pent = (CBaseEntity*)((DWORD)ent);
	if (pent && pent != LocalPlayer.Entity && pent->IsPlayer())
	{
		int index = ReadInt((uintptr_t)&pent->index);
		CustomPlayer *pCPlayer = &AllPlayers[index];
		pCPlayer->CurrentNetvarsFromProxy.viewoffset.x = ReadFloat((uintptr_t)pOut);
		//pCPlayer->DataUpdated = true;
	}
}

DWORD oViewOffsetProxyY = NULL;
void ReceivedViewOffsetY(const CRecvProxyData *pData, void *ent, void *pOut)
{
	if (oViewOffsetProxyY)
	{
		((void(*)(const CRecvProxyData *, void *, void *))oViewOffsetProxyY)(pData, ent, pOut);
	}

	CBaseEntity* pent = (CBaseEntity*)((DWORD)ent);
	if (pent && pent != LocalPlayer.Entity && pent->IsPlayer())
	{
		int index = ReadInt((uintptr_t)&pent->index);
		CustomPlayer *pCPlayer = &AllPlayers[index];
		pCPlayer->CurrentNetvarsFromProxy.viewoffset.y = ReadFloat((uintptr_t)pOut);
		//pCPlayer->DataUpdated = true;
	}
}

DWORD oViewOffsetProxyZ = NULL;
void ReceivedViewOffsetZ(const CRecvProxyData *pData, void *ent, void *pOut)
{
	if (oViewOffsetProxyZ)
	{
		((void(*)(const CRecvProxyData *, void *, void *))oViewOffsetProxyZ)(pData, ent, pOut);
	}

	CBaseEntity* pent = (CBaseEntity*)((DWORD)ent);
	if (pent && pent->IsPlayer() && pent != LocalPlayer.Entity)
	{
		int index = ReadInt((uintptr_t)&pent->index);
		CustomPlayer *pCPlayer = &AllPlayers[index];
		pCPlayer->CurrentNetvarsFromProxy.viewoffset.z = ReadFloat((uintptr_t)pOut);
		//pCPlayer->DataUpdated = true;
	}
}


DWORD oDuckAmountProxy = NULL;
void ReceivedDuckAmount(const CRecvProxyData *pData, void *ent, void *pOut)
{
	if (oDuckAmountProxy)
	{
		((void(*)(const CRecvProxyData *, void *, void *))oDuckAmountProxy)(pData, ent, pOut);
	}

	if (pOut)
	{
		CBaseEntity* pent = (CBaseEntity*)((DWORD)ent);
		if (pent && pent->IsPlayer() && pent != LocalPlayer.Entity)
		{
			int index = ReadInt((uintptr_t)&pent->index);
			CustomPlayer *pCAPlayer = &AllPlayers[index];
			pCAPlayer->CurrentNetvarsFromProxy.duckamount = ReadFloat((uintptr_t)pOut);
			//pCAPlayer->DataUpdated = true;
		}
	}
}

DWORD oDuckSpeedProxy = NULL;
void ReceivedDuckSpeed(const CRecvProxyData *pData, void *ent, void *pOut)
{
	if (oDuckSpeedProxy)
	{
		((void(*)(const CRecvProxyData *, void *, void *))oDuckSpeedProxy)(pData, ent, pOut);
	}

	if (pOut)
	{
		CBaseEntity* pent = (CBaseEntity*)((DWORD)ent);
		if (pent && pent->IsPlayer() && pent != LocalPlayer.Entity)
		{
			int index = ReadInt((uintptr_t)&pent->index);
			CustomPlayer *pCAPlayer = &AllPlayers[index];
			pCAPlayer->CurrentNetvarsFromProxy.duckspeed = ReadFloat((uintptr_t)pOut);
			//pCAPlayer->DataUpdated = true;
		}
	}
}

DWORD oVelocityModifierProxy = NULL;
void ReceivedVelocityModifier(const CRecvProxyData *pData, void *ent, void *pOut)
{
	if (oVelocityModifierProxy)
	{
		((void(*)(const CRecvProxyData *, void *, void *))oVelocityModifierProxy)(pData, ent, pOut);
	}

	if (pOut)
	{
		CBaseEntity *pent = (CBaseEntity*)ent;
		if (pent && pent->IsPlayer() && pent != LocalPlayer.Entity)
		{
			int index = ReadInt((uintptr_t)&pent->index);
			CustomPlayer *pCAPlayer = &AllPlayers[index];
			pCAPlayer->CurrentNetvarsFromProxy.velocitymodifier = ReadFloat((uintptr_t)pOut);
			//pCAPlayer->DataUpdated = true;
		}
	}
}

DWORD oMinsProxy = NULL;
void ReceivedMins(const CRecvProxyData *pData, void *ent, void *pOut)
{
	if (oMinsProxy)
	{
		((void(*)(const CRecvProxyData *, void *, void *))oMinsProxy)(pData, ent, pOut);
	}

	if (pOut)
	{
		CBaseEntity* pent = (CBaseEntity*)((DWORD)ent - m_Collision);
		if (pent && pent->IsPlayer() && pent != LocalPlayer.Entity)
		{
			int index = ReadInt((uintptr_t)&pent->index);
			CustomPlayer *pCAPlayer = &AllPlayers[index];
			ReadVector((uintptr_t)pOut, pCAPlayer->CurrentNetvarsFromProxy.mins);
			//pCAPlayer->DataUpdated = true;
		}
	}
}

DWORD oMaxsProxy = NULL;
void ReceivedMaxs(const CRecvProxyData *pData, void *ent, void *pOut)
{
	if (oMaxsProxy)
	{
		((void(*)(const CRecvProxyData *, void *, void *))oMaxsProxy)(pData, ent, pOut);
	}

	if (pOut)
	{
		CBaseEntity* pent = (CBaseEntity*)((DWORD)ent - m_Collision);
		if (pent && pent->IsPlayer() && pent != LocalPlayer.Entity)
		{
			int index = ReadInt((uintptr_t)&pent->index);
			CustomPlayer *pCAPlayer = &AllPlayers[index];
			ReadVector((uintptr_t)pOut, pCAPlayer->CurrentNetvarsFromProxy.maxs);
			//pCAPlayer->DataUpdated = true;
		}
	}
}

DWORD oFlagsProxy = NULL;
void ReceivedFlags(const CRecvProxyData *pData, void *ent, void *pOut)
{
	if (oFlagsProxy)
	{
		((void(*)(const CRecvProxyData *, void *, void *))oFlagsProxy)(pData, ent, pOut);
	}

	if (pOut)
	{
		CBaseEntity *pent = (CBaseEntity*)ent;
		if (pent && pent->IsPlayer() && pent != LocalPlayer.Entity)
		{
			int index = ReadInt((uintptr_t)&pent->index);
			CustomPlayer *pCAPlayer = &AllPlayers[index];
			pCAPlayer->CurrentNetvarsFromProxy.flags = ReadInt((uintptr_t)pOut);
			//pCAPlayer->DataUpdated = true;
		}
	}
}

DWORD oGroundEntityProxy = NULL;
void ReceivedGroundEntity(const CRecvProxyData *pData, void *ent, void *pOut)
{
	if (oGroundEntityProxy)
	{
		((void(*)(const CRecvProxyData *, void *, void *))oGroundEntityProxy)(pData, ent, pOut);
	}

	if (pOut)
	{
		CBaseEntity *pent = (CBaseEntity*)ent;
		if (pent && pent->IsPlayer() && pent != LocalPlayer.Entity)
		{
			int index = ReadInt((uintptr_t)&pent->index);
			CustomPlayer *pCAPlayer = &AllPlayers[index];
			pCAPlayer->CurrentNetvarsFromProxy.groundentity = (CBaseEntity*)ReadInt((uintptr_t)pOut);
			//pCAPlayer->DataUpdated = true;
		}
	}
}


DWORD oFallVelocityProxy = NULL;
void ReceivedFallVelocity(const CRecvProxyData *pData, void *ent, void *pOut)
{
	if (oFallVelocityProxy)
	{
		((void(*)(const CRecvProxyData *, void *, void *))oFallVelocityProxy)(pData, ent, pOut);
	}

	if (pOut)
	{
		CBaseEntity* pent = (CBaseEntity*)((DWORD)ent - m_Local);
		if (pent && pent->IsPlayer() && pent != LocalPlayer.Entity)
		{
			int index = ReadInt((uintptr_t)&pent->index);
			CustomPlayer *pCAPlayer = &AllPlayers[index];
			pCAPlayer->CurrentNetvarsFromProxy.fallvelocity = ReadFloat((uintptr_t)pOut);
			//pCAPlayer->DataUpdated = true;
		}
	}
}

DWORD oBaseVelocityProxy = NULL;
void ReceivedBaseVelocity(const CRecvProxyData *pData, void *ent, void *pOut)
{
	if (oBaseVelocityProxy)
	{
		((void(*)(const CRecvProxyData *, void *, void *))oBaseVelocityProxy)(pData, ent, pOut);
	}

	if (pOut)
	{
		CBaseEntity *pent = (CBaseEntity*)ent;
		if (pent && pent->IsPlayer() && pent != LocalPlayer.Entity)
		{
			int index = ReadInt((uintptr_t)&pent->index);
			CustomPlayer *pCAPlayer = &AllPlayers[index];
			ReadVector((uintptr_t)pOut, pCAPlayer->CurrentNetvarsFromProxy.basevelocity);
			//pCAPlayer->DataUpdated = true;
		}
	}
}

DWORD oObserverModeProxy = NULL;
void ReceivedObserverMode(const CRecvProxyData *pData, void *ent, void *pOut)
{
	if (oObserverModeProxy)
	{
		((void(*)(const CRecvProxyData *, void *, void *))oObserverModeProxy)(pData, ent, pOut);
	}

	if (pOut)
	{
		CBaseEntity *pent = (CBaseEntity*)ent;
		if (pent && pent == LocalPlayer.Entity)
		{
			LocalPlayer.OriginalObserverMode = ReadInt((uintptr_t)pOut);
		}
	}
}

DWORD oEffectsProxy = NULL;
void ReceivedEffects(const CRecvProxyData *pData, void *ent, void *pOut)
{
	if (oEffectsProxy)
	{
		((void(*)(const CRecvProxyData *, void *, void *))oEffectsProxy)(pData, ent, pOut);
	}

	if (pOut && RemoveInterpChk.Checked)
	{
		CBaseEntity *pent = (CBaseEntity*)ent;
		if (pent && pent != LocalPlayer.Entity && pent->IsPlayer())
		{
			//pent->SetEffects(*(int*)pOut | EF_NOINTERP);
		}
	}
}

DWORD oValveDSProxy = NULL;
void ReceivedValveDS(const CRecvProxyData *pData, void *ent, void *pOut)
{
	if (oValveDSProxy)
	{
		((void(*)(const CRecvProxyData *, void *, void *))oValveDSProxy)(pData, ent, pOut);
	}

	LocalPlayer.bIsValveDS = ReadByte((uintptr_t)pOut);
}


//////////////////////////
//                      //   
//     HOOK NETVARS     //
//                      //
//////////////////////////

bool HookedEyeAnglesRecv = false;
char *csplayerstr = new char[12]{ 62, 46, 37, 57, 41, 42, 22, 27, 3, 31, 8, 0 }; /*DT_CSPlayer*/
char *eyeang0str = new char[18]{ 23, 37, 27, 20, 29, 63, 3, 31, 59, 20, 29, 22, 31, 9, 33, 74, 39, 0 }; /*m_angEyeAngles[0]*/
char *eyeang1str = new char[18]{ 23, 37, 27, 20, 29, 63, 3, 31, 59, 20, 29, 22, 31, 9, 33, 75, 39, 0 }; /*m_angEyeAngles[1]*/
char *eyeang2str = new char[18]{ 23, 37, 27, 20, 29, 63, 3, 31, 59, 20, 29, 22, 31, 9, 33, 72, 39, 0 }; /*m_angEyeAngles[2]*/
char *lowerbodyyawstr = new char[23]{ 23, 37, 28, 22, 54, 21, 13, 31, 8, 56, 21, 30, 3, 35, 27, 13, 46, 27, 8, 29, 31, 14, 0 }; /*m_flLowerBodyYawTarget*/
char *velocitystr = new char[14]{ 23, 37, 12, 31, 25, 44, 31, 22, 21, 25, 19, 14, 3, 0 }; /*m_vecVelocity*/
char *baseentitystr = new char[14]{ 62, 46, 37, 56, 27, 9, 31, 63, 20, 14, 19, 14, 3, 0 }; /*DT_BaseEntity*/
char *dtbaseanimatingstr = new char[17]{ 62, 46, 37, 56, 27, 9, 31, 59, 20, 19, 23, 27, 14, 19, 20, 29, 0 }; /*DT_BaseAnimating*/
char *simulationtimestr = new char[19]{ 23, 37, 28, 22, 41, 19, 23, 15, 22, 27, 14, 19, 21, 20, 46, 19, 23, 31, 0 }; /*m_flSimulationTime*/
char *baseplayerstr = new char[14]{ 62, 46, 37, 56, 27, 9, 31, 42, 22, 27, 3, 31, 8, 0 }; /*DT_BasePlayer*/
char *aimpunchstr = new char[16]{ 23, 37, 27, 19, 23, 42, 15, 20, 25, 18, 59, 20, 29, 22, 31, 0 }; /*m_aimPunchAngle*/
char *viewoffset0str = new char[19]{ 23, 37, 12, 31, 25, 44, 19, 31, 13, 53, 28, 28, 9, 31, 14, 33, 74, 39, 0 }; /*m_vecViewOffset[0]*/
char *viewoffset1str = new char[19]{ 23, 37, 12, 31, 25, 44, 19, 31, 13, 53, 28, 28, 9, 31, 14, 33, 75, 39, 0 }; /*m_vecViewOffset[1]*/
char *viewoffset2str = new char[19]{ 23, 37, 12, 31, 25, 44, 19, 31, 13, 53, 28, 28, 9, 31, 14, 33, 72, 39, 0 }; /*m_vecViewOffset[2]*/
char *duckamountstr = new char[15]{ 23, 37, 28, 22, 62, 15, 25, 17, 59, 23, 21, 15, 20, 14, 0 }; /*m_flDuckAmount*/
char *duckspeedstr = new char[14]{ 23, 37, 28, 22, 62, 15, 25, 17, 41, 10, 31, 31, 30, 0 }; /*m_flDuckSpeed*/
char *velocitymodifierstr = new char[21]{ 23, 37, 28, 22, 44, 31, 22, 21, 25, 19, 14, 3, 55, 21, 30, 19, 28, 19, 31, 8, 0 }; /*m_flVelocityModifier*/
char *minsstr = new char[10]{ 23, 37, 12, 31, 25, 55, 19, 20, 9, 0 }; /*m_vecMins*/
char *maxsstr = new char[10]{ 23, 37, 12, 31, 25, 55, 27, 2, 9, 0 }; /*m_vecMaxs*/
char *flagsstr = new char[9]{ 23, 37, 28, 60, 22, 27, 29, 9, 0 }; /*m_fFlags*/
char *groundentitystr = new char[16]{ 23, 37, 18, 61, 8, 21, 15, 20, 30, 63, 20, 14, 19, 14, 3, 0 }; /*m_hGroundEntity*/
char *fallvelstr = new char[17]{ 23, 37, 28, 22, 60, 27, 22, 22, 44, 31, 22, 21, 25, 19, 14, 3, 0 }; /*m_flFallVelocity*/
char *basevelstr = new char[18]{ 23, 37, 12, 31, 25, 56, 27, 9, 31, 44, 31, 22, 21, 25, 19, 14, 3, 0 }; /*m_vecBaseVelocity*/
char *observermodestr = new char[16]{ 23, 37, 19, 53, 24, 9, 31, 8, 12, 31, 8, 55, 21, 30, 31, 0 }; /*m_iObserverMode*/
char *effectsstr = new char[11]{ 23, 37, 28, 63, 28, 28, 31, 25, 14, 9, 0 }; /*m_fEffects*/
char *valvedsstr = new char[13]{ 23, 37, 24, 51, 9, 44, 27, 22, 12, 31, 62, 41, 0 }; /*m_bIsValveDS*/
char *gamerulesproxystr = new char[20]{ 62, 46, 37, 57, 41, 61, 27, 23, 31, 40, 15, 22, 31, 9, 42, 8, 21, 2, 3, 0 }; /*DT_CSGameRulesProxy*/


int Get_Prop(RecvTable *recvTable, const char *propName, RecvProp **prop)
{
	int extraOffset = 0;
	for (int i = 0; i < recvTable->m_nProps; ++i)
	{
		RecvProp *recvProp = &recvTable->m_pProps[i];
		RecvTable *child = recvProp->m_pDataTable;

		if (child && (ReadInt((uintptr_t)&child->m_nProps) > 0))
		{
			int tmp = Get_Prop(child, propName, prop);
			if (tmp)
				extraOffset += (ReadInt((uintptr_t)&recvProp->m_Offset) + tmp);
		}

		if (stricmp(recvProp->m_pVarName, propName))
			continue;

		if (prop)
			*prop = recvProp;

		return (ReadInt((uintptr_t)&recvProp->m_Offset) + extraOffset);
	}

	return extraOffset;
}

int Get_Prop(const char *tableName, const char *propName, RecvProp **prop)
{
	ClientClass *pClass = Interfaces::Client->GetAllClasses();
	while (pClass) {
		const char *pszName = pClass->m_pRecvTable->m_pNetTableName;
		if (!strcmp(pszName, tableName))
		{
			RecvTable *recvTable = pClass->m_pRecvTable;
			if (!recvTable)
				return 0;

			int offset = Get_Prop(recvTable, propName, prop);
			if (!offset)
				return 0;
			return offset;
		}
		pClass = (ClientClass*)ReadInt((uintptr_t)&pClass->m_pNext);
	}
	return 0;
}


void HookEyeAnglesProxy()
{
	//ClientClass *pClass = Interfaces::Client->GetAllClasses();

	DecStr(csplayerstr, 11);
	DecStr(eyeang0str, 17);
	DecStr(eyeang1str, 17);
	DecStr(lowerbodyyawstr, 22);
	DecStr(velocitystr, 13);
	DecStr(baseentitystr, 13);
	DecStr(dtbaseanimatingstr, 16);
	DecStr(simulationtimestr, 18);
	DecStr(baseplayerstr, 13);
	DecStr(aimpunchstr, 15);
	DecStr(viewoffset0str, 18);
	DecStr(viewoffset1str, 18);
	DecStr(viewoffset2str, 18);
	DecStr(duckamountstr, 14);
	DecStr(duckspeedstr, 13);
	DecStr(velocitymodifierstr, 20);
	DecStr(minsstr, 9);
	DecStr(maxsstr, 9);
	DecStr(flagsstr, 8);
	DecStr(groundentitystr, 15);
	DecStr(fallvelstr, 16);
	DecStr(basevelstr, 17);
	DecStr(observermodestr, 15);
	DecStr(effectsstr, 10);
	DecStr(gamerulesproxystr, 19);
	DecStr(valvedsstr, 12);

	RecvProp* prop = nullptr;

	Get_Prop(csplayerstr, aimpunchstr, &prop);
	if (prop)
	{
		oAimPunchProxy = (DWORD)prop->m_ProxyFn;
		prop->m_ProxyFn = ReceivedAimPunch;
	}

	prop = nullptr;
	Get_Prop(csplayerstr, simulationtimestr, &prop);
	if (prop)
	{
		oReceivedSimulationTime = (DWORD)prop->m_ProxyFn;
		prop->m_ProxyFn = ReceivedSimulationTime;
	}

	prop = nullptr;
	Get_Prop(csplayerstr, eyeang0str, &prop);
	if (prop)
	{
		oAimProxyX = (DWORD)prop->m_ProxyFn;
		prop->m_ProxyFn = AntiAntiAimProxyX;
	}

	prop = nullptr;
	Get_Prop(csplayerstr, eyeang1str, &prop);
	if (prop)
	{
		oAimProxyY = (DWORD)prop->m_ProxyFn;
		prop->m_ProxyFn = AntiAntiAimProxyY;
	}

	prop = nullptr;
	Get_Prop(csplayerstr, eyeang2str, &prop);
	if (prop)
	{
		oAimProxyZ = (DWORD)prop->m_ProxyFn;
		prop->m_ProxyFn = AntiAntiAimProxyZ;
	}

	prop = nullptr;
	Get_Prop(csplayerstr, lowerbodyyawstr, &prop);
	if (prop)
	{
		oLowerBodyProxy = (DWORD)prop->m_ProxyFn;
		prop->m_ProxyFn = ReceivedLowerBodyYawProxy;
	}

	prop = nullptr;
	Get_Prop(csplayerstr, duckamountstr, &prop);
	if (prop)
	{
		oDuckAmountProxy = (DWORD)prop->m_ProxyFn;
		prop->m_ProxyFn = ReceivedDuckAmount;
	}

	prop = nullptr;
	Get_Prop(csplayerstr, duckspeedstr, &prop);
	if (prop)
	{
		oDuckSpeedProxy = (DWORD)prop->m_ProxyFn;
		prop->m_ProxyFn = ReceivedDuckSpeed;
	}

	prop = nullptr;
	Get_Prop(csplayerstr, velocitymodifierstr, &prop);
	if (prop)
	{
		oVelocityModifierProxy = (DWORD)prop->m_ProxyFn;
		prop->m_ProxyFn = ReceivedVelocityModifier;
	}

	prop = nullptr;
	Get_Prop(csplayerstr, minsstr, &prop);
	if (prop)
	{
		oMinsProxy = (DWORD)prop->m_ProxyFn;
		prop->m_ProxyFn = ReceivedMins;
	}

	prop = nullptr;
	Get_Prop(csplayerstr, maxsstr, &prop);
	if (prop)
	{
		oMaxsProxy = (DWORD)prop->m_ProxyFn;
		prop->m_ProxyFn = ReceivedMaxs;
	}

	prop = nullptr;
	Get_Prop(csplayerstr, flagsstr, &prop);
	if (prop)
	{
		oFlagsProxy = (DWORD)prop->m_ProxyFn;
		prop->m_ProxyFn = ReceivedFlags;
	}

	prop = nullptr;
	Get_Prop(csplayerstr, groundentitystr, &prop);
	if (prop)
	{
		oGroundEntityProxy = (DWORD)prop->m_ProxyFn;
		prop->m_ProxyFn = ReceivedGroundEntity;
	}

	prop = nullptr;
	Get_Prop(csplayerstr, fallvelstr, &prop);
	if (prop)
	{
		oFallVelocityProxy = (DWORD)prop->m_ProxyFn;
		prop->m_ProxyFn = ReceivedFallVelocity;
	}

	prop = nullptr;
	Get_Prop(csplayerstr, basevelstr, &prop);
	if (prop)
	{
		oBaseVelocityProxy = (DWORD)prop->m_ProxyFn;
		prop->m_ProxyFn = ReceivedBaseVelocity;
	}

	prop = nullptr;
	Get_Prop(csplayerstr, observermodestr, &prop);
	if (prop)
	{
		oObserverModeProxy = (DWORD)prop->m_ProxyFn;
		prop->m_ProxyFn = ReceivedObserverMode;
	}

	prop = nullptr;
	Get_Prop(csplayerstr, viewoffset0str, &prop);
	if (prop)
	{
		oViewOffsetProxyX = (DWORD)prop->m_ProxyFn;
		prop->m_ProxyFn = ReceivedViewOffsetX;
	}

	prop = nullptr;
	Get_Prop(csplayerstr, viewoffset1str, &prop);
	if (prop)
	{
		oViewOffsetProxyY = (DWORD)prop->m_ProxyFn;
		prop->m_ProxyFn = ReceivedViewOffsetY;
	}

	prop = nullptr;
	Get_Prop(csplayerstr, viewoffset2str, &prop);
	if (prop)
	{
		oViewOffsetProxyZ = (DWORD)prop->m_ProxyFn;
		prop->m_ProxyFn = ReceivedViewOffsetZ;
	}

	prop = nullptr;
	Get_Prop(baseentitystr, effectsstr, &prop);
	if (prop)
	{
		oEffectsProxy = (DWORD)prop->m_ProxyFn;
		prop->m_ProxyFn = ReceivedEffects;
	}

#if 0
	prop = nullptr;
	Get_Prop(gamerulesproxystr, valvedsstr, &prop);
	if (prop)
	{
		oValveDSProxy = (DWORD)prop->m_ProxyFn;
		prop->m_ProxyFn = ReceivedValveDS;
	}
#endif

	EncStr(csplayerstr, 11);
	EncStr(eyeang0str, 17);
	EncStr(eyeang1str, 17);
	EncStr(lowerbodyyawstr, 22);
	EncStr(velocitystr, 13);
	EncStr(baseentitystr, 13);
	EncStr(dtbaseanimatingstr, 16);
	EncStr(simulationtimestr, 18);
	EncStr(baseplayerstr, 13);
	EncStr(aimpunchstr, 15);
	EncStr(viewoffset0str, 18);
	EncStr(viewoffset1str, 18);
	EncStr(viewoffset2str, 18);
	EncStr(duckamountstr, 14);
	EncStr(duckspeedstr, 13);
	EncStr(velocitymodifierstr, 20);
	EncStr(minsstr, 9);
	EncStr(maxsstr, 9);
	EncStr(flagsstr, 8);
	EncStr(groundentitystr, 15);
	EncStr(fallvelstr, 16);
	EncStr(basevelstr, 17);
	EncStr(observermodestr, 15);
	EncStr(effectsstr, 10);
	EncStr(gamerulesproxystr, 19);
	EncStr(valvedsstr, 12);

	delete[] csplayerstr;
	delete[] eyeang0str;
	delete[] eyeang1str;
	delete[] lowerbodyyawstr;
	delete[] velocitystr;
	delete[] baseentitystr;
	delete[] dtbaseanimatingstr;
	delete[] simulationtimestr;
	delete[] baseplayerstr;
	delete[] aimpunchstr;
	delete[] viewoffset0str;
	delete[] viewoffset1str;
	delete[] viewoffset2str;
	delete[] duckamountstr;
	delete[] duckspeedstr;
	delete[] velocitymodifierstr;
	delete[] minsstr;
	delete[] maxsstr;
	delete[] flagsstr;
	delete[] groundentitystr;
	delete[] fallvelstr;
	delete[] basevelstr;
	delete[] observermodestr;
	delete[] effectsstr;
	delete[] gamerulesproxystr;
	delete[] valvedsstr;

	HookedEyeAnglesRecv = true;
}