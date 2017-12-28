#include "VTHook.h"
#include "LocalPlayer.h"

//Server lag compensation hook

#ifdef _DEBUG
FrameUpdatePostEntityThinkFn oLagCompFrameUpdatePostEntityThink;

float FrameUpdatePostEntityThinkSimTimes[65]{ 0 };

void __fastcall Hooks::LagComp_FrameUpdatePostEntityThink(void* thisptr)
{
	return;
	return;
	AllocateConsole();
	for (int i = 0; i < GetServerClientCount(); i++)
	{
		DWORD cl = GetServerClientEntity(i);
		if (cl && ((CBaseEntity*)cl)->IsActive() && ((CBaseEntity*)cl)->IsConnected() && !((CBaseEntity*)cl)->IsFakeClient())
		{
			CBaseEntity *pEnt = ServerClientToEntity(cl);
			if (pEnt)
			{	
				int index = pEnt->entindexServer();
				if (index != 3)
					continue;

				CUserCmd *lastusercmd = (CUserCmd*)((DWORD)pEnt + 0x0F54);
				float flSimulationTime = pEnt->GetSimulationTimeServer();
				if (flSimulationTime > FrameUpdatePostEntityThinkSimTimes[index])
				{
					int tickschoked = TIME_TO_TICKS(flSimulationTime - FrameUpdatePostEntityThinkSimTimes[index]) - 1;

					tickschoked++;

					FrameUpdatePostEntityThinkSimTimes[index] = flSimulationTime;

					BOOLEAN bAlive = pEnt->GetAliveServer();
					QAngle absangles = pEnt->GetAbsAnglesServer();
					Vector absorigin = pEnt->GetAbsOriginServer();
					float body_yaw = pEnt->GetPoseParameterUnScaledServer(11);
					float body_pitch = pEnt->GetPoseParameterUnScaledServer(12);
					float goalfeetyaw = pEnt->GetGoalFeetYawServer();
					float curfeetyaw = pEnt->GetCurrentFeetYawServer();

					printf("Server Index: %i\nAlive: %i\nTickCount: %i\nCommandNr: %i\nTicksChoked: %i\nSimulationTime: %f\nAbsAngles: %f %f %f\nAbsOrigin: %f %f %f\nBody_Yaw: %f\nBody_Pitch: %f\nGoalFeetYaw: %f\nCurFeetYaw: %f\n\n", index, (int)bAlive, lastusercmd->tick_count, lastusercmd->command_number, tickschoked, flSimulationTime, absangles.x, absangles.y, absangles.z, absorigin.x, absorigin.y, absorigin.z, body_yaw, body_pitch, goalfeetyaw, curfeetyaw);
				}
			}
		}
	}

	oLagCompFrameUpdatePostEntityThink(thisptr);
}

#endif