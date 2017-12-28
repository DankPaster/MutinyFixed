#include "CreateMove.h"
#include "CSGO_HX.h"
#include "Overlay.h"
#include "BunnyHop.h"
#include "BaseCombatWeapon.h"
#include "PlayerList.h"
#include "Aimbot.h"
#include "AutoWall.h"
#include "RunCommand.h"
#include "Events.h"
#include "NoRecoil.h"
#include "Netchan.h"
#include "player_lagcompensation.h"
#include "Aimbot.h"
#include "OverrideView.h"
#include "GrenadePrediction.h"
FrameStageNotifyFn oFrameStageNotify;

void OnReceivedPitch(CustomPlayer *pCPlayer, float pitch)
{
	if (!pCPlayer->Dormant)
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
			if (changeddegrees > 180.0)
			{
				changeddegrees -= 360.0;
			}
			else if (changeddegrees < -180.0)
			{
				changeddegrees += 360.0;
			}
			pPersonalize->flLastPitchChange = changeddegrees;
		}
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
	if (pent && pent->IsPlayer() && pent != LocalPlayer)
	{
		int index = ReadInt((uintptr_t)&pent->index);
		if (index <= MAX_PLAYERS)
		{
			CustomPlayer *pCAPlayer = &AllPlayers[index];
			pCAPlayer->CurrentNetVars.eyeangles[0] = val;
			OnReceivedPitch(pCAPlayer, val);
			pCAPlayer->DataUpdated = true;
		}
	}
	WriteFloat((uintptr_t)pOut, val);
}

void OnReceivedYaw(CustomPlayer *pCPlayer, float yaw)
{
	if (!pCPlayer->Dormant)
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
			pPersonalize->flLastYaw = yaw;
			if (changeddegrees > 180.0)
			{
				changeddegrees -= 360.0;
			}
			else if (changeddegrees < -180.0)
			{
				changeddegrees += 360.0;
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
	if (pent && pent->IsPlayer() && pent != LocalPlayer)
	{
		int index = ReadInt((uintptr_t)&pent->index);
		if (index <= MAX_PLAYERS)
		{
			CustomPlayer *pCAPlayer = &AllPlayers[index];
			//Eye angle update
			//ClampY(val);
			OnReceivedYaw(pCAPlayer, val);
			pCAPlayer->CurrentNetVars.eyeangles[1] = val;
			pCAPlayer->DataUpdated = true;
		}
	}

	WriteFloat((uintptr_t)pOut, val);
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
	if (pent && pent->IsPlayer() && pent != LocalPlayer)
	{
		int index = ReadInt((uintptr_t)&pent->index);
		if (index <= MAX_PLAYERS)
		{
			CustomPlayer *pCAPlayer = &AllPlayers[index];
			//Eye angle update
			pCAPlayer->CurrentNetVars.eyeangles[2] = val;
			pCAPlayer->DataUpdated = true;
		}
	}

	WriteFloat((uintptr_t)pOut, val);
}

DWORD oLowerBodyProxy = NULL;
extern float LocalPlayerLowerBodyYaw;
extern bool LocalPlayerLowerBodyYawUpdated;
void ReceivedLowerBodyYawProxy(const CRecvProxyData *pData, void *ent, void *pOut)
{
	if (oLowerBodyProxy)
	{
		((void(*)(const CRecvProxyData *, void *, void *))oLowerBodyProxy)(pData, ent, pOut);
	}

	CBaseEntity* pent = (CBaseEntity*)ent;
	if (pent && pOut)
	{
		if (pent == LocalPlayer)
		{
			LocalPlayerLowerBodyYaw = ReadFloat((uintptr_t)pOut);
			LocalPlayerLowerBodyYawUpdated = true;
		}
		else if (pent->IsPlayer())
		{
			int index = ReadInt((uintptr_t)&pent->index);
			if (index <= MAX_PLAYERS)
			{
				CustomPlayer *pCAPlayer = &AllPlayers[index];
				pCAPlayer->CurrentNetVars.lowerbodyyaw = ReadFloat((uintptr_t)pOut);
				pCAPlayer->LowerBodyUpdated = true;
				pCAPlayer->DataUpdated = true;
			}
		}
	}
}

DWORD oOriginProxy = NULL;
void ReceivedOrigin(const CRecvProxyData *pData, void *ent, void *pOut)
{
	if (oOriginProxy)
	{
		((void(*)(const CRecvProxyData *, void *, void *))oOriginProxy)(pData, ent, pOut);
	}

	//Vector val;
	//ReadVector((uintptr_t)&pData->m_Value.m_Vector, val);

	CBaseEntity* pent = (CBaseEntity*)ent;
	if (pent && pent->IsPlayer() && pent != LocalPlayer)
	{
		int index = ReadInt((uintptr_t)&pent->index);
		if (index <= MAX_PLAYERS)
		{
			CustomPlayer *pCAPlayer = &AllPlayers[index];
			pCAPlayer->m_vecOldOrigin = pCAPlayer->CurrentNetVars.networkorigin;
			ReadVector((uintptr_t)pOut, pCAPlayer->CurrentNetVars.origin);
			ReadVector((uintptr_t)pOut, pCAPlayer->CurrentNetVars.networkorigin);
			pCAPlayer->DataUpdated = true;
		}
	}
}

DWORD oReceivedSimulationTime = NULL;
void ReceivedSimulationTime(const CRecvProxyData *pData, void *ent, void *pOut)
{
	if (oReceivedSimulationTime)
	{
		((void(*)(const CRecvProxyData *, void *, void *))oReceivedSimulationTime)(pData, ent, pOut);
	}

	CBaseEntity* pent = (CBaseEntity*)ent;
	if (pOut && pent && pent->IsPlayer() && pent != LocalPlayer)
	{
		int index = ReadInt((uintptr_t)&pent->index);
		if (index <= MAX_PLAYERS)
		{
			CustomPlayer *pCAPlayer = &AllPlayers[index];
			pCAPlayer->m_flOldSimulationTime = pCAPlayer->CurrentNetVars.simulationtime;
			pCAPlayer->CurrentNetVars.simulationtime = ReadFloat((uintptr_t)pOut);
			pCAPlayer->TickReceivedSimulationTime = ReadInt((uintptr_t)&Interfaces::Globals->tickcount);
			pCAPlayer->DataUpdated = true;
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

	CBaseEntity* pent = (CBaseEntity*)((DWORD)ent - m_Local);
 	if (pent && pent != LocalPlayer && pent->IsPlayer())
	{
		int index = ReadInt((uintptr_t)&pent->index);
		if (index <= MAX_PLAYERS)
		{
			CustomPlayer *pCPlayer = &AllPlayers[index];
			ReadAngle((uintptr_t)pOut, pCPlayer->CurrentNetVars.aimpunch);
			pCPlayer->DataUpdated = true;
		}
	}
}


DWORD oAimPunchVelProxy = NULL;
void ReceivedAimPunchVel(const CRecvProxyData *pData, void *ent, void *pOut)
{
	if (oAimPunchVelProxy)
	{
		((void(*)(const CRecvProxyData *, void *, void *))oAimPunchVelProxy)(pData, ent, pOut);
	}

	CBaseEntity* pent = (CBaseEntity*)((DWORD)ent - m_Local);
	if (pent && pent->IsPlayer())
	{
		int index = ReadInt((uintptr_t)&pent->index);
		if (index <= MAX_PLAYERS)
		{
			CustomPlayer *pCPlayer = &AllPlayers[index];
			pCPlayer->DataUpdated = true;
		}
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
	if (pent && pent->IsPlayer())
	{
		int index = ReadInt((uintptr_t)&pent->index);
		if (index <= MAX_PLAYERS)
		{
			CustomPlayer *pCPlayer = &AllPlayers[index];
			pCPlayer->CurrentNetVars.viewoffset[0] = ReadFloat((uintptr_t)pOut);
			pCPlayer->DataUpdated = true;
		}
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
	if (pent && pent->IsPlayer())
	{
		int index = ReadInt((uintptr_t)&pent->index);
		if (index <= MAX_PLAYERS)
		{
			CustomPlayer *pCPlayer = &AllPlayers[index];
			pCPlayer->CurrentNetVars.viewoffset[1] = ReadFloat((uintptr_t)pOut);
			pCPlayer->DataUpdated = true;
		}
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
	if (pent && pent->IsPlayer() && pent != LocalPlayer)
	{
		int index = ReadInt((uintptr_t)&pent->index);
		if (index <= MAX_PLAYERS)
		{
			CustomPlayer *pCPlayer = &AllPlayers[index];
			pCPlayer->CurrentNetVars.viewoffset[2] = ReadFloat((uintptr_t)pOut);
			pCPlayer->DataUpdated = true;
		}
	}
}

DWORD oVelocityProxy = NULL;
void ReceivedVelocity(const CRecvProxyData *pData, void *ent, void *pOut)
{
	if (oVelocityProxy)
	{
		((void(*)(const CRecvProxyData *, void *, void *))oVelocityProxy)(pData, ent, pOut);
	}

	if (pOut)
	{
		CBaseEntity* pent = (CBaseEntity*)((DWORD)ent - m_Local);
		if (pent && pent->IsPlayer() && pent != LocalPlayer)
		{
			int index = ReadInt((uintptr_t)&pent->index);
			if (index <= MAX_PLAYERS)
			{
				CustomPlayer *pCAPlayer = &AllPlayers[index];
				ReadVector((uintptr_t)pOut, pCAPlayer->CurrentVelocity);
			}
		}
	}
}

DWORD oPoseParameterProxy = NULL;
void ReceivedPoseParameter(const CRecvProxyData *pData, void *ent, void *pOut)
{
	if (oPoseParameterProxy)
	{
		((void(*)(const CRecvProxyData *, void *, void *))oPoseParameterProxy)(pData, ent, pOut);
	}

	if (pOut)
	{
		CBaseEntity *pent = (CBaseEntity*)ent;
		if (pent && pent->IsPlayer() && pent != LocalPlayer)
		{
			int index = ReadInt((uintptr_t)&pent->index);
			if (index <= MAX_PLAYERS)
			{
				CustomPlayer *pCAPlayer = &AllPlayers[index];
				DWORD poseindex = ((DWORD)pOut - ((DWORD)pent + m_flPoseParameter)) * (1 / sizeof(float));
				pCAPlayer->CurrentNetVars.flPoseParameter[poseindex] = ReadFloat((uintptr_t)pOut);
				pCAPlayer->DataUpdated = true;
			}
		}
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
		if (pent && pent->IsPlayer() && pent != LocalPlayer)
		{
			int index = ReadInt((uintptr_t)&pent->index);
			if (index <= MAX_PLAYERS)
			{
				CustomPlayer *pCAPlayer = &AllPlayers[index];
				pCAPlayer->CurrentNetVars.duckamount = ReadFloat((uintptr_t)pOut);
				pCAPlayer->DataUpdated = true;
			}
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
		if (pent && pent->IsPlayer() && pent != LocalPlayer)
		{
			int index = ReadInt((uintptr_t)&pent->index);
			if (index <= MAX_PLAYERS)
			{
				CustomPlayer *pCAPlayer = &AllPlayers[index];
				pCAPlayer->CurrentNetVars.duckspeed = ReadFloat((uintptr_t)pOut);
				pCAPlayer->DataUpdated = true;
			}
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
		if (pent && pent->IsPlayer() && pent != LocalPlayer)
		{
			int index = ReadInt((uintptr_t)&pent->index);
			if (index <= MAX_PLAYERS)
			{
				CustomPlayer *pCAPlayer = &AllPlayers[index];
				pCAPlayer->CurrentNetVars.velocitymodifier = ReadFloat((uintptr_t)pOut);
				pCAPlayer->DataUpdated = true;
			}
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
		if (pent && pent->IsPlayer() && pent != LocalPlayer)
		{
			int index = ReadInt((uintptr_t)&pent->index);
			if (index <= MAX_PLAYERS)
			{
				CustomPlayer *pCAPlayer = &AllPlayers[index];
				ReadVector((uintptr_t)pOut, pCAPlayer->CurrentNetVars.mins);
				pCAPlayer->DataUpdated = true;
			}
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
		if (pent && pent->IsPlayer() && pent != LocalPlayer)
		{
			int index = ReadInt((uintptr_t)&pent->index);
			if (index <= MAX_PLAYERS)
			{
				CustomPlayer *pCAPlayer = &AllPlayers[index];
				ReadVector((uintptr_t)pOut, pCAPlayer->CurrentNetVars.maxs);
				pCAPlayer->DataUpdated = true;
			}
		}
	}
}

DWORD oAnimTimeProxy = NULL;
void ReceivedAnimTime(const CRecvProxyData *pData, void *ent, void *pOut)
{
	if (oAnimTimeProxy)
	{
		((void(*)(const CRecvProxyData *, void *, void *))oAnimTimeProxy)(pData, ent, pOut);
	}

	if (pOut)
	{
		CBaseEntity *pent = (CBaseEntity*)ent;
		if (pent && pent->IsPlayer() && pent != LocalPlayer)
		{
			int index = ReadInt((uintptr_t)&pent->index);
			if (index <= MAX_PLAYERS)
			{
				CustomPlayer *pCAPlayer = &AllPlayers[index];
				pCAPlayer->CurrentNetVars.animtime = ReadFloat((uintptr_t)pOut);
				pCAPlayer->DataUpdated = true;
			}
		}
	}
}

DWORD oHitboxSetProxy = NULL;
void ReceivedHitboxSet(const CRecvProxyData *pData, void *ent, void *pOut)
{
	if (oHitboxSetProxy)
	{
		((void(*)(const CRecvProxyData *, void *, void *))oHitboxSetProxy)(pData, ent, pOut);
	}

	if (pOut)
	{
		CBaseEntity *pent = (CBaseEntity*)ent;
		if (pent && pent->IsPlayer() && pent != LocalPlayer)
		{
			int index = ReadInt((uintptr_t)&pent->index);
			if (index <= MAX_PLAYERS)
			{
				CustomPlayer *pCAPlayer = &AllPlayers[index];
				pCAPlayer->DataUpdated = true;
			}
		}
	}
}

DWORD oClientSideAnimationProxy = NULL;
void ReceivedClientSideAnimation(const CRecvProxyData *pData, void *ent, void *pOut)
{
	if (oClientSideAnimationProxy)
	{
		((void(*)(const CRecvProxyData *, void *, void *))oClientSideAnimationProxy)(pData, ent, pOut);
	}

	if (pOut)
	{
		CBaseEntity *pent = (CBaseEntity*)ent;
		if (pent && pent->IsPlayer() && pent != LocalPlayer)
		{
			int index = ReadInt((uintptr_t)&pent->index);
			if (index <= MAX_PLAYERS)
			{
				CustomPlayer *pCAPlayer = &AllPlayers[index];
				pCAPlayer->DataUpdated = true;
			}
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
		if (pent && pent->IsPlayer() && pent != LocalPlayer)
		{
			int index = ReadInt((uintptr_t)&pent->index);
			if (index <= MAX_PLAYERS)
			{
				CustomPlayer *pCAPlayer = &AllPlayers[index];
				pCAPlayer->CurrentNetVars.flags = ReadInt((uintptr_t)pOut);
				pCAPlayer->DataUpdated = true;
			}
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
		if (pent && pent->IsPlayer() && pent != LocalPlayer)
		{
			int index = ReadInt((uintptr_t)&pent->index);
			if (index <= MAX_PLAYERS)
			{
				CustomPlayer *pCAPlayer = &AllPlayers[index];
				pCAPlayer->CurrentNetVars.groundentity = (CBaseEntity*)ReadInt((uintptr_t)pOut);
				pCAPlayer->DataUpdated = true;
			}
		}
	}
}

DWORD oHealthProxy = NULL;
void ReceivedHealth(const CRecvProxyData *pData, void *ent, void *pOut)
{
	if (oHealthProxy)
	{
		((void(*)(const CRecvProxyData *, void *, void *))oHealthProxy)(pData, ent, pOut);
	}

	if (pOut)
	{
		CBaseEntity *pent = (CBaseEntity*)ent;
		if (pent && pent->IsPlayer() && pent != LocalPlayer)
		{
			int index = ReadInt((uintptr_t)&pent->index);
			if (index <= MAX_PLAYERS)
			{
				CustomPlayer *pCAPlayer = &AllPlayers[index];
				pCAPlayer->Health = ReadInt((uintptr_t)&pOut);
				pCAPlayer->DataUpdated = true;
			}
		}
	}
}

DWORD oAmmoProxy = NULL;
void ReceivedAmmo(const CRecvProxyData *pData, void *ent, void *pOut)
{
	if (oAmmoProxy)
	{
		((void(*)(const CRecvProxyData *, void *, void *))oAmmoProxy)(pData, ent, pOut);
	}

	if (pOut)
	{
		CBaseEntity *pent = (CBaseEntity*)ent;
		if (pent && pent->IsPlayer() && pent != LocalPlayer)
		{
			int index = ReadInt((uintptr_t)&pent->index);
			if (index <= MAX_PLAYERS)
			{
				CustomPlayer *pCAPlayer = &AllPlayers[index];
				pCAPlayer->DataUpdated = true;
			}
		}
	}
}

DWORD oWeaponProxy = NULL;
void ReceivedWeapon(const CRecvProxyData *pData, void *ent, void *pOut)
{
	if (oWeaponProxy)
	{
		((void(*)(const CRecvProxyData *, void *, void *))oWeaponProxy)(pData, ent, pOut);
	}

	if (pOut)
	{
		CBaseEntity *pent = (CBaseEntity*)ent;
		if (pent && pent->IsPlayer() && pent != LocalPlayer)
		{
			int index = ReadInt((uintptr_t)&pent->index);
			if (index <= MAX_PLAYERS)
			{
				CustomPlayer *pCAPlayer = &AllPlayers[index];
				pCAPlayer->DataUpdated = true;
			}
		}
	}
}

DWORD oSequenceProxy = NULL;
void ReceivedSequence(const CRecvProxyData *pData, void *ent, void *pOut)
{
	if (oSequenceProxy)
	{
		((void(*)(const CRecvProxyData *, void *, void *))oSequenceProxy)(pData, ent, pOut);
	}

	if (pOut)
	{
		CBaseEntity *pent = (CBaseEntity*)ent;
		if (pent && pent->IsPlayer() && pent != LocalPlayer)
		{
			int index = ReadInt((uintptr_t)&pent->index);
			if (index <= MAX_PLAYERS)
			{
				CustomPlayer *pCAPlayer = &AllPlayers[index];
				pCAPlayer->CurrentNetVars.sequence = ReadInt((uintptr_t)pOut);
				pCAPlayer->DataUpdated = true;
			}
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
		if (pent && pent->IsPlayer() && pent != LocalPlayer)
		{
			int index = ReadInt((uintptr_t)&pent->index);
			if (index <= MAX_PLAYERS)
			{
				CustomPlayer *pCAPlayer = &AllPlayers[index];
				pCAPlayer->CurrentNetVars.fallvelocity = ReadFloat((uintptr_t)pOut);
				pCAPlayer->DataUpdated = true;
			}
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
		if (pent && pent->IsPlayer() && pent != LocalPlayer)
		{
			int index = ReadInt((uintptr_t)&pent->index);
			if (index <= MAX_PLAYERS)
			{
				CustomPlayer *pCAPlayer = &AllPlayers[index];
				ReadVector((uintptr_t)pOut, pCAPlayer->CurrentNetVars.basevelocity);
				pCAPlayer->DataUpdated = true;
			}
		}
	}
}


static bool HookedEyeAnglesRecv = false;
char *csplayerstr = new char[12]{ 62, 46, 37, 57, 41, 42, 22, 27, 3, 31, 8, 0 }; /*DT_CSPlayer*/
char *eyeang0str = new char[18]{ 23, 37, 27, 20, 29, 63, 3, 31, 59, 20, 29, 22, 31, 9, 33, 74, 39, 0 }; /*m_angEyeAngles[0]*/
char *eyeang1str = new char[18]{ 23, 37, 27, 20, 29, 63, 3, 31, 59, 20, 29, 22, 31, 9, 33, 75, 39, 0 }; /*m_angEyeAngles[1]*/
char *eyeang2str = new char[18]{ 23, 37, 27, 20, 29, 63, 3, 31, 59, 20, 29, 22, 31, 9, 33, 72, 39, 0 }; /*m_angEyeAngles[2]*/
char *lowerbodyyawstr = new char[23]{ 23, 37, 28, 22, 54, 21, 13, 31, 8, 56, 21, 30, 3, 35, 27, 13, 46, 27, 8, 29, 31, 14, 0 }; /*m_flLowerBodyYawTarget*/
char *velocitystr = new char[14]{ 23, 37, 12, 31, 25, 44, 31, 22, 21, 25, 19, 14, 3, 0 }; /*m_vecVelocity*/
char *baseentitystr = new char[14]{ 62, 46, 37, 56, 27, 9, 31, 63, 20, 14, 19, 14, 3, 0 }; /*DT_BaseEntity*/
char *vecoriginstr = new char[12]{ 23, 37, 12, 31, 25, 53, 8, 19, 29, 19, 20, 0 }; /*m_vecOrigin*/
char *dtbaseanimatingstr = new char[17]{ 62, 46, 37, 56, 27, 9, 31, 59, 20, 19, 23, 27, 14, 19, 20, 29, 0 }; /*DT_BaseAnimating*/
char *simulationtimestr = new char[19]{ 23, 37, 28, 22, 41, 19, 23, 15, 22, 27, 14, 19, 21, 20, 46, 19, 23, 31, 0 }; /*m_flSimulationTime*/
char *baseplayerstr = new char[14]{ 62, 46, 37, 56, 27, 9, 31, 42, 22, 27, 3, 31, 8, 0 }; /*DT_BasePlayer*/
char *aimpunchstr = new char[16]{ 23, 37, 27, 19, 23, 42, 15, 20, 25, 18, 59, 20, 29, 22, 31, 0 }; /*m_aimPunchAngle*/
char *aimpunchvelstr = new char[19]{ 23, 37, 27, 19, 23, 42, 15, 20, 25, 18, 59, 20, 29, 22, 31, 44, 31, 22, 0 }; /*m_aimPunchAngleVel*/
char *viewoffset0str = new char[19]{ 23, 37, 12, 31, 25, 44, 19, 31, 13, 53, 28, 28, 9, 31, 14, 33, 74, 39, 0 }; /*m_vecViewOffset[0]*/
char *viewoffset1str = new char[19]{ 23, 37, 12, 31, 25, 44, 19, 31, 13, 53, 28, 28, 9, 31, 14, 33, 75, 39, 0 }; /*m_vecViewOffset[1]*/
char *viewoffset2str = new char[19]{ 23, 37, 12, 31, 25, 44, 19, 31, 13, 53, 28, 28, 9, 31, 14, 33, 72, 39, 0 }; /*m_vecViewOffset[2]*/
char *poseparameterstr = new char[18]{ 23, 37, 28, 22, 42, 21, 9, 31, 42, 27, 8, 27, 23, 31, 14, 31, 8, 0 }; /*m_flPoseParameter*/
char *duckamountstr = new char[15]{ 23, 37, 28, 22, 62, 15, 25, 17, 59, 23, 21, 15, 20, 14, 0 }; /*m_flDuckAmount*/
char *duckspeedstr = new char[14]{ 23, 37, 28, 22, 62, 15, 25, 17, 41, 10, 31, 31, 30, 0 }; /*m_flDuckSpeed*/
char *velocitymodifierstr = new char[21]{ 23, 37, 28, 22, 44, 31, 22, 21, 25, 19, 14, 3, 55, 21, 30, 19, 28, 19, 31, 8, 0 }; /*m_flVelocityModifier*/
char *minsstr = new char[10]{ 23, 37, 12, 31, 25, 55, 19, 20, 9, 0 }; /*m_vecMins*/
char *maxsstr = new char[10]{ 23, 37, 12, 31, 25, 55, 27, 2, 9, 0 }; /*m_vecMaxs*/
char *animtimestr = new char[13]{ 23, 37, 28, 22, 59, 20, 19, 23, 46, 19, 23, 31, 0 }; /*m_flAnimTime*/
char *hitboxsetstr = new char[13]{ 23, 37, 20, 50, 19, 14, 24, 21, 2, 41, 31, 14, 0 }; /*m_nHitboxSet*/
char *clientsideanimationstr = new char[23]{ 23, 37, 24, 57, 22, 19, 31, 20, 14, 41, 19, 30, 31, 59, 20, 19, 23, 27, 14, 19, 21, 20, 0 }; /*m_bClientSideAnimation*/
char *flagsstr = new char[9]{ 23, 37, 28, 60, 22, 27, 29, 9, 0 }; /*m_fFlags*/
char *groundentitystr = new char[16]{ 23, 37, 18, 61, 8, 21, 15, 20, 30, 63, 20, 14, 19, 14, 3, 0 }; /*m_hGroundEntity*/
char *healthstrfsn = new char[10]{ 23, 37, 19, 50, 31, 27, 22, 14, 18, 0 }; /*m_iHealth*/
char *ammostr = new char[8]{ 23, 37, 19, 59, 23, 23, 21, 0 }; /*m_iAmmo*/
char *activeweaponstr = new char[16]{ 23, 37, 18, 59, 25, 14, 19, 12, 31, 45, 31, 27, 10, 21, 20, 0 }; /*m_hActiveWeapon*/
char *fallvelstr = new char[17]{ 23, 37, 28, 22, 60, 27, 22, 22, 44, 31, 22, 21, 25, 19, 14, 3, 0 }; /*m_flFallVelocity*/
char *basevelstr = new char[18]{ 23, 37, 12, 31, 25, 56, 27, 9, 31, 44, 31, 22, 21, 25, 19, 14, 3, 0 }; /*m_vecBaseVelocity*/


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

	DecStr(csplayerstr);
	DecStr(eyeang0str);
	DecStr(eyeang1str);
	DecStr(lowerbodyyawstr);
	DecStr(velocitystr);
	DecStr(baseentitystr);
	DecStr(vecoriginstr);
	DecStr(dtbaseanimatingstr);
	DecStr(simulationtimestr);
	DecStr(baseplayerstr);
	DecStr(aimpunchstr);
	DecStr(aimpunchvelstr);
	DecStr(viewoffset0str);
	DecStr(viewoffset1str);
	DecStr(viewoffset2str);
	DecStr(poseparameterstr);
	DecStr(duckamountstr);
	DecStr(duckspeedstr);
	DecStr(velocitymodifierstr);
	DecStr(minsstr);
	DecStr(maxsstr);
	DecStr(animtimestr);
	DecStr(hitboxsetstr);
	DecStr(clientsideanimationstr);
	DecStr(flagsstr);
	DecStr(groundentitystr);
	DecStr(healthstrfsn);
	DecStr(ammostr);
	DecStr(activeweaponstr);
	DecStr(fallvelstr);
	DecStr(basevelstr);
	
	RecvProp* prop = nullptr;

	Get_Prop(csplayerstr, aimpunchstr, &prop);
	if (prop)
	{
		oAimPunchProxy = (DWORD)prop->m_ProxyFn;
		prop->m_ProxyFn = ReceivedAimPunch;
	}
	prop = nullptr;
	Get_Prop(csplayerstr, aimpunchvelstr, &prop);
	if (prop)
	{
		oAimPunchVelProxy = (DWORD)prop->m_ProxyFn;
		prop->m_ProxyFn = ReceivedAimPunchVel;
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
	Get_Prop(csplayerstr, vecoriginstr, &prop);
	if (prop)
	{
		oOriginProxy = (DWORD)prop->m_ProxyFn;
		prop->m_ProxyFn = ReceivedOrigin;
	}

	prop = nullptr;
	Get_Prop(csplayerstr, poseparameterstr, &prop);
	if (prop)
	{
		oPoseParameterProxy = (DWORD)prop->m_ProxyFn;
		prop->m_ProxyFn = ReceivedPoseParameter;
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
	Get_Prop(csplayerstr, animtimestr, &prop);
	if (prop)
	{
		oAnimTimeProxy = (DWORD)prop->m_ProxyFn;
		prop->m_ProxyFn = ReceivedAnimTime;
	}

	prop = nullptr;
	Get_Prop(csplayerstr, hitboxsetstr, &prop);
	if (prop)
	{
		oHitboxSetProxy = (DWORD)prop->m_ProxyFn;
		prop->m_ProxyFn = ReceivedHitboxSet;
	}

	prop = nullptr;
	Get_Prop(csplayerstr, clientsideanimationstr, &prop);
	if (prop)
	{
		oClientSideAnimationProxy = (DWORD)prop->m_ProxyFn;
		prop->m_ProxyFn = ReceivedClientSideAnimation;
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
	Get_Prop(csplayerstr, healthstrfsn, &prop);
	if (prop)
	{
		oHealthProxy = (DWORD)prop->m_ProxyFn;
		prop->m_ProxyFn = ReceivedHealth;
	}

	prop = nullptr;
	Get_Prop(csplayerstr, ammostr, &prop);
	if (prop)
	{
		oAmmoProxy = (DWORD)prop->m_ProxyFn;
		prop->m_ProxyFn = ReceivedAmmo;
	}

	prop = nullptr;
	Get_Prop(csplayerstr, activeweaponstr, &prop);
	if (prop)
	{
		oWeaponProxy = (DWORD)prop->m_ProxyFn;
		prop->m_ProxyFn = ReceivedWeapon;
	}

	//prop = nullptr;
	//Get_Prop(csplayerstr, "m_nSequence", &prop);
	//if (prop)
	{
	//	oSequenceProxy = (DWORD)prop->m_ProxyFn;
	//	prop->m_ProxyFn = ReceivedSequence;
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

	EncStr(csplayerstr);
	EncStr(eyeang0str);
	EncStr(eyeang1str);
	EncStr(lowerbodyyawstr);
	EncStr(velocitystr);
	EncStr(baseentitystr);
	EncStr(vecoriginstr);
	EncStr(dtbaseanimatingstr);
	EncStr(simulationtimestr);
	EncStr(baseplayerstr);
	EncStr(aimpunchstr);
	EncStr(aimpunchvelstr);
	EncStr(viewoffset0str);
	EncStr(viewoffset1str);
	EncStr(viewoffset2str);
	EncStr(poseparameterstr);
	EncStr(duckamountstr);
	EncStr(duckspeedstr);
	EncStr(velocitymodifierstr);
	EncStr(minsstr);
	EncStr(maxsstr);
	EncStr(animtimestr);
	EncStr(hitboxsetstr);
	EncStr(clientsideanimationstr);
	EncStr(flagsstr);
	EncStr(groundentitystr);
	EncStr(healthstrfsn);
	EncStr(ammostr);
	EncStr(activeweaponstr);
	EncStr(fallvelstr);
	EncStr(basevelstr);
	HookedEyeAnglesRecv = true;
}

void GetIdealYaw(float& yaw, int correctedindex, CustomPlayer* pCPlayer, CBaseEntity* Entity, CBaseEntity* pLocalEntity)
{
	Personalize *pPersonalize = &pCPlayer->Personalize;
	if (pPersonalize->ResolveYaw)
	{
		if (pPersonalize->AutomaticYaw)
		{
			if (pPersonalize->ResolveValve)
			{
				if (GetAsyncKeyState(VK_CAPITAL) & (1 << 16))
				{
					yaw = pCPlayer->CurrentNetVars.lowerbodyyaw;
				}
				else
				{
					float flLowerBodyYaw = pCPlayer->CurrentNetVars.lowerbodyyaw;
					float eyeyaw = pCPlayer->CurrentNetVars.eyeangles.y;
					ClampY(eyeyaw);
					float delta = flLowerBodyYaw - eyeyaw;
					ClampY(delta);
					if (fabsf(delta) > 35.0f)
						yaw = flLowerBodyYaw;
				}

				float CurYaw = Entity->GetLowerBodyYaw();
				if (pCPlayer->LastLowerBodyYaw != CurYaw || yaw != CurYaw)
				{
					yaw = CurYaw;
					pCPlayer->LastLowerBodyYaw = CurYaw;
				}
			}
			else
			{
				//MAX_RESOLVER_MISSES
				switch (correctedindex)
				{
				case 0:
					break; //Do nothing
				case 1:
					yaw += 180.f;
					break;
				case 2:
					yaw += 90.0f;
					break;
				case 3:
					yaw -= 90.0f;
					break;
				case 4:
					yaw = CalcAngle(Entity->GetEyePosition(), pLocalEntity->GetEyePosition()).y - 180.0f;
					break;
				case 5:
					yaw = (CalcAngle(Entity->GetEyePosition(), pLocalEntity->GetEyePosition()).y - 180.0f) - 45.0f;
					break;
				case 6:
					yaw = (CalcAngle(Entity->GetEyePosition(), pLocalEntity->GetEyePosition()).y - 180.0f) + 45.0f;
					break;
				case 7:
					yaw = (CalcAngle(Entity->GetEyePosition(), pLocalEntity->GetEyePosition()).y - 180.0f) - 90.0f;
					break;
				case 8:
					yaw = (CalcAngle(Entity->GetEyePosition(), pLocalEntity->GetEyePosition()).y - 180.0f) + 90.0f;
					break;
				}
			}
		}
		else if (pPersonalize->InverseYaw)
			yaw += 180.0f;
		else if (pPersonalize->YawLeft)
			yaw += 90.0f;
		else if (pPersonalize->YawRight)
			yaw -= 90.0f;
		else if (pPersonalize->YawForceBack)
		{
			yaw = CalcAngle(Entity->GetEyePosition(), pLocalEntity->GetEyePosition()).y - 180.0f;
		}
	}
	ClampY(yaw);
	if (yaw < 0.0f)
		yaw += 360.0f;
}

void GetIdealPitch(float& pitch, int correctedindex, CustomPlayer* pCPlayer, CBaseEntity* Entity, CBaseEntity* pLocalEntity)
{
	Personalize *pPersonalize = &pCPlayer->Personalize;
	if (pPersonalize->ResolvePitch)
	{
		if (pPersonalize->AutomaticPitch)
		{
			//MAX_RESOLVER_MISSES
#if 0
			if (pPersonalize->ResolveValve)
			{
				switch (correctedindex)
				{
				case 0:
				case 1:
					break;
				case 2:
				case 3:
					pitch = 89.0f;
					break;
				case 4:
				case 5:
				case 6:
					break;
				case 7:
				case 8:
					pitch = 89.0f;
				}
			}
			else
			{
				//Todo
			}
#endif
		}
		else
		{
			if (pPersonalize->PitchDown)
				pitch = 89.0f;
			else if (pPersonalize->PitchUp)
				pitch = -89.0f;
			else if (pPersonalize->StaticPitch)
				pitch = pPersonalize->flPitch;
			else
				pitch += pPersonalize->flPitch;
		}
		ClampX(pitch);
		if (pitch < 0.0f)
			pitch += 360.0f;
	}
}

bool AllowCustomCode = false;

void __stdcall Hooks::FrameStageNotify(ClientFrameStage_t stage)
{
	bool SetPunches = false;
	bool SetViewAngles = false;
	QAngle OldAimPunch;
	QAngle OldViewPunch;
	QAngle OldViewAngles;
	CBasePlayer* pLocalEntity = Interfaces::ClientEntList->GetClientEntity(Interfaces::Engine->GetLocalPlayer());
	bool InGame = Interfaces::Engine->IsInGame() && pLocalEntity && pLocalEntity->IsConnected() && pLocalEntity->IsActive();
	
	static bool ResetEyeAngles = false;
	if (InGame)
	{
		switch (stage)
		{
		case FRAME_START:
			AllowCustomCode = true;
			DirectXBeginFrame();
			pLocalEntity = Interfaces::ClientEntList->GetClientEntity(Interfaces::Engine->GetLocalPlayer());
			if (pLocalEntity)
			{
				if (ResolverChk.Checked)
				{
					BacktrackPlayersOnNewFrame(pLocalEntity);
				}
			}
		case FRAME_RENDER_START:
			if (AllowCustomCode)
			{
				if (RunningFrame)
				{
					if (MENU_IS_OPEN)
					{
						DrawMenu();
					}
					else if (!DisableAllChk.Checked)
					{
						TEAMS LocalTeam = (TEAMS)LocalPlayer->GetTeam();
						GetStreamedPlayers_And_RunPerPlayerHacks(LocalTeam, true);
					}
				}
				if (!DisableAllChk.Checked)
				{
					if (!HookedEyeAnglesRecv)
					{
						HookEyeAnglesProxy();
					}
					if (pLocalEntity && !pLocalEntity->GetDormant() && pLocalEntity->GetAlive())
					{

						if (NoVisualRecoilChk.Checked)
						{
							CBaseCombatWeapon *pWeapon = pLocalEntity->GetWeapon();
							if (pWeapon)
							{
								int ID = pWeapon->GetItemDefinitionIndex();
								if (NoRecoilPistolsShotgunsSnipersChk.Checked || (!pWeapon->IsPistol(ID) && !pWeapon->IsShotgun(ID) && !pWeapon->IsSniper(false, ID)))
								{
									//No Visual Recoil
									OldAimPunch = pLocalEntity->GetPunch();
									OldViewPunch = pLocalEntity->GetViewPunch();
									WriteAngle((DWORD)pLocalEntity + m_aimPunchAngle, QAngle(0, 0, 0));
									WriteAngle((DWORD)pLocalEntity + m_viewPunchAngle, QAngle(0, 0, 0));
									SetPunches = true;
								}
							}
						}
						else if (NoRecoilChk.Checked && !SilentAimChk.Checked)
						{
							CBaseCombatWeapon *pWeapon = pLocalEntity->GetWeapon();
							if (IsAllowedToNoRecoil(pWeapon))
							{
								//Show the RCS
								OldAimPunch = pLocalEntity->GetPunch();
								WriteAngle((DWORD)pLocalEntity + m_aimPunchAngle, -OldAimPunch);
								SetViewAngles = true;
							}
						}
						if (DrawRecoilCrosshairChk.Checked)
						{

	#if 0
							Vector EndPos;
							AngleVectors(pLocalEntity->GetEyeAngles(), &EndPos);
							VectorNormalizeFast(EndPos);
							Vector StartPos = pLocalEntity->GetEyePosition();
							EndPos = StartPos + EndPos * 4096.0f;
							trace_t tr;
							UTIL_TraceLine(StartPos, EndPos, MASK_SHOT, pLocalEntity, &tr);
							WorldToScreenCapped(tr.endpos, EndPos);
							RecoilCrosshairScreen = { EndPos.x, EndPos.y };
	#else
							QAngle punch = pLocalEntity->GetPunch() * 2.0f;
							int dy = (int)(CenterOfScreen.y / LocalFOV);
							int dx = (int)(CenterOfScreen.x / LocalFOV);
							int recoilx = (int)(dx * punch.y * 0.5f);
							int recoily;
							if (NoRecoilChk.Checked)
							{
								recoily = (int)(dy * punch.x * 0.8f);
								RecoilCrosshairScreen.x = CenterOfScreen.x + recoilx;
								RecoilCrosshairScreen.y = CenterOfScreen.y - recoily;
							}
							else
							{
								recoily = (int)(dy * punch.x);
								RecoilCrosshairScreen.x = CenterOfScreen.x - recoilx;
								RecoilCrosshairScreen.y = CenterOfScreen.y + recoily;
							}
	#endif
						}
					}
				}
			}
			break;
		case FRAME_RENDER_END:
			if (AllowCustomCode)
			{
				if (ResolverChk.Checked)
				{
					pLocalEntity = Interfaces::ClientEntList->GetClientEntity(Interfaces::Engine->GetLocalPlayer());
					if (pLocalEntity)
					{
						FinishLagCompensation(pLocalEntity);
					}
				}
				if (!DisableAllChk.Checked && GrenadeTrajectoryChk.Checked && RunningFrame && LocalPlayer && LocalPlayer->GetAlive())
				{
					PaintGrenadeTrajectory();
				}
				DirectXEndFrame();
				AllowCustomCode = false;
			}
			break;
		case FRAME_NET_UPDATE_POSTDATAUPDATE_START:
			if (!DisableAllChk.Checked)
			{
				pLocalEntity = Interfaces::ClientEntList->GetClientEntity(Interfaces::Engine->GetLocalPlayer());
				if (pLocalEntity)
				{
					if (NoFlashChk.Checked)
					{
						pLocalEntity->SetFlashDuration(0.0f);
					}

					float flFallVelocity = pLocalEntity->GetFallVelocity();
					if (flFallVelocity > 0.03124f && flFallVelocity < 0.03126f)
						pLocalEntity->SetFallVelocity(0.0f);

					Vector baseVel = pLocalEntity->GetBaseVelocity();
					bool updated = false;
					if (baseVel.x > -0.000999f && baseVel.x < -0.0009f)
					{
						baseVel.x = 0.0f;
						updated = true;
					}

					if (baseVel.y > -0.000999f && baseVel.y < -0.0009f)
					{
						baseVel.y = 0.0f;
						updated = true;
					}

					if (baseVel.z > -0.000999f && baseVel.z < -0.0009f)
					{
						baseVel.z = 0.0f;
						updated = true;
					}

					if (updated)
						pLocalEntity->SetBaseVelocity(baseVel);

					if (ResolverChk.Checked)
					{
						for (int i = 0; i < MAX_PLAYERS; i++)
						{
							CBaseEntity* Entity = Interfaces::ClientEntList->GetClientEntity(i);
							if (Entity && Entity != pLocalEntity /*&& !Entity->IsFakeClient()*/ && Entity->IsPlayer() && Entity->IsActive() && IsPlayerAValidTarget(pLocalEntity->GetTeam(), Entity) && Entity->GetTeam() != TEAM_NONE)
							{
								//Setup angles for editing
								int iEnt = ReadInt((uintptr_t)&Entity->index); //enemy index
								CustomPlayer *pCPlayer = &AllPlayers[iEnt]; //enemy
								Personalize *pPersonalize = &pCPlayer->Personalize; //enemy resolver settings

								//First store updated netvars
								BeginLagCompensation(pCPlayer, Entity);
						
								if (pPersonalize->ResolvePitch || pPersonalize->ResolveYaw)
								{
									//Now try to teleport players back in time to a known real position
									if (!BacktrackPlayer(pCPlayer, pLocalEntity, Entity))
									{
										QAngle angles = Entity->GetEyeAngles();
										int correctedindex = pPersonalize->isCorrected ? pPersonalize->correctedindex : pPersonalize->miss;
										GetIdealPitch(angles.x, correctedindex, pCPlayer, Entity, pLocalEntity);
										GetIdealYaw(angles.y, correctedindex, pCPlayer, Entity, pLocalEntity);
										Entity->SetEyeAngles(angles);
									}
								}
								else
								{
									pCPlayer->Backtracked = false;
									pCPlayer->FireAtPelvis = false;
								}
							}
						}
					}
				}
			}
			break;
		case FRAME_NET_UPDATE_POSTDATAUPDATE_END:
			if (!DisableAllChk.Checked)
			{
				for (int i = 0; i < MAX_PLAYERS; i++)
				{
					CBaseEntity* Entity = Interfaces::ClientEntList->GetClientEntity(i);
					if (Entity && Entity != pLocalEntity /*&& !Entity->IsFakeClient()*/ && Entity->IsPlayer() && Entity->IsActive() && IsPlayerAValidTarget(pLocalEntity->GetTeam(), Entity) && Entity->GetTeam() != TEAM_NONE)
					{
						int iEnt = ReadInt((uintptr_t)&Entity->index); //enemy index
						CustomPlayer *pCPlayer = &AllPlayers[iEnt]; //enemy
						pCPlayer->networkorigin = Entity->GetNetworkOrigin();
						pCPlayer->origin = Entity->GetOrigin();
#if 0
						if (pCPlayer->Backtracked)
						{
							Entity->SetNetworkOrigin(pCPlayer->LastUpdatedNetVars.origin);
							Entity->SetOrigin(pCPlayer->LastUpdatedNetVars.origin);
							Entity->SetAbsOrigin(pCPlayer->LastUpdatedNetVars.origin);
							//Entity->SetAbsOrigin(pCPlayer->LastUpdatedNetVars.origin);
						}
#endif
					}
				}
			}
			break;
		}
	}
	oFrameStageNotify(stage);
	if (SetPunches)
	{
		WriteAngle((DWORD)pLocalEntity + m_aimPunchAngle, OldAimPunch);
		WriteAngle((DWORD)pLocalEntity + m_viewPunchAngle, OldViewPunch);
	}
	else if (SetViewAngles)
	{
		WriteAngle((DWORD)pLocalEntity + m_aimPunchAngle, OldAimPunch);
	}
}