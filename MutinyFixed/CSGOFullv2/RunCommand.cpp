#include "RunCommand.h"
RunCommandFn oRunCommand;
int thsptr;
float flArrayCorrect[128][4];
int g_pTickcount[150];
PredVariables PredictedVars[150];
int lastpredictedcommandnr = 0;
int lastcommandnr = 0;

void RestorePredictedVariables(CBaseEntity* pEntity, bool ShouldRestoreOrigin, PredVariables* vars)
{
	if (pEntity->GetTickBase() == vars->TickBase)
	{
		QAngle punch = vars->aimpunch;
		QAngle curpunch = pEntity->GetPunch();
		QAngle delta = punch - curpunch;
		if (fabsf(delta.x) < 0.5f && fabsf(delta.y) < 0.5f && fabsf(delta.z) < 0.5f)
		{
			pEntity->SetPunch(punch);
		}

		Vector punchvel = vars->aimpunchvel;
		Vector curpunchvel = pEntity->GetPunchVel();
		Vector veldelta = punchvel - curpunchvel;
		if (fabsf(veldelta.x) < 0.5f && fabsf(veldelta.y) < 0.5f && fabsf(veldelta.z) < 0.5f)
		{
			pEntity->SetPunchVel(punchvel);
		}

		Vector viewoffset = vars->viewoffset;
		Vector curviewoffset = pEntity->GetViewOffset();
		Vector offsetdelta = viewoffset - curviewoffset;
		if (fabsf(offsetdelta.x) < 0.5f && fabsf(offsetdelta.y) < 0.5f && fabsf(offsetdelta.z) < 0.5f)
		{
			pEntity->SetViewOffset(viewoffset);
		}

		if (!ShouldRestoreOrigin)
		{
			pEntity->SetOrigin(vars->origin);
			QAngle *v17 = (QAngle *)(*(int(**)(void))(ReadInt((uintptr_t)pEntity) + 40))(); //DYLAN FIXME
			WriteVector((uintptr_t)v17, vars->origin);
		}
	}
}

void __stdcall RunCmd(void* thisptr, CBaseEntity *pEntity, CUserCmd* pUserCmd, void* moveHelper)
{
#if 0
#if 0
	static bool printed = false;
	
	{
		printed = true;
		int adr = (int)((int)Interfaces::Prediction + 8);
		int adr2 = (int)(int)&Interfaces::Prediction->m_bInPrediction;
		char txt[128];
		sprintf(txt, "%02X", adr);
		MessageBoxA(NULL, txt, "test", MB_OK);
	}
#endif
	if (pUserCmd->tick_count != -1)
	{
		int command_number = pUserCmd->command_number; // pUserCmd[1].command_number;
		//int tick = g_pTickcount[command_number % 150];
		RestorePredictedVariables(pEntity, 1, &PredictedVars[command_number % 150]); //0
		oRunCommand(thisptr, pEntity, pUserCmd, moveHelper);
		PredVariables *nextcmd = &PredictedVars[(command_number + 1) % 150];
		nextcmd->TickBase = pEntity->GetTickBase();
		nextcmd->aimpunch = pEntity->GetPunch();
		nextcmd->aimpunchvel = pEntity->GetPunchVel();
		nextcmd->viewoffset = pEntity->GetViewOffset();
		nextcmd->origin = pEntity->GetOrigin();
		//lastpredictedcommandnr = command_number;
	}
#if 0
	oRunCommand(thisptr, pEntity, pUserCmd, moveHelper);
#if 0
	if (pEntity == LocalPlayer.Entity)
	{
		Fixed_LocalAngRotation = pEntity->GetAngleRotation();
		Fixed_LocalAnimTime = pEntity->GetAnimTime();
		Fixed_LocalBaseVelocity = pEntity->GetBaseVelocity();
		Fixed_LocalClientSideAnimation = pEntity->GetClientSideAnimation();
		Fixed_LocalCycle = pEntity->GetCycle();
		Fixed_LocalDuckAmount = pEntity->GetDuckAmount();
		Fixed_LocalDuckSpeed = pEntity->GetDuckSpeed();
		Fixed_LocalDuckTimeMsecs = pEntity->GetDuckTimeMsecs();
		Fixed_LocalEyePos = pEntity->GetEyePosition();
		Fixed_LocalFallVelocity = pEntity->GetFallVelocity();
		Fixed_LocalFlags = pEntity->GetFlags();
		Fixed_LocalGroundEntity = pEntity->GetGroundEntity();
		Fixed_LocalHitboxSet = pEntity->GetHitboxSet();
		Fixed_LocalInDuckJump = pEntity->IsInDuckJump();
		Fixed_LocalJumpTimeMsecs = pEntity->GetJumpTimeMsecs();
		Fixed_LocalLaggedMovement = pEntity->GetLaggedMovement();
		Fixed_LocalLocalDucked = pEntity->IsDucked();
		Fixed_LocalLocalDucking = pEntity->IsDucking();
		Fixed_LocalMaxs = pEntity->GetMaxs();
		Fixed_LocalMins = pEntity->GetMins();
		Fixed_LocalModelIndex = pEntity->GetModelIndex();
		Fixed_LocalMoveType = pEntity->GetMoveType();
		Fixed_LocalNextAttack = pEntity->GetNextAttack();
		Fixed_LocalNextThinkTick = pEntity->GetNextThinkTick();
		Fixed_LocalOrigin = pEntity->GetOrigin();
		Fixed_LocalPlaybackRate = pEntity->GetPlaybackRate();
		Fixed_LocalPoseParameter = pEntity->GetPoseParameter();
		Fixed_LocalSequence = pEntity->GetSequence();
		Fixed_LocalSimulationTime = pEntity->GetSimulationTime();
		Fixed_LocalTickBase = pEntity->GetTickBase();
		Fixed_LocalVelocity = pEntity->GetVelocity();
		Fixed_LocalVelocityModifier = pEntity->GetVelocityModifier();
		Fixed_LocalViewOffset = pEntity->GetViewOffset();
		Fixed_LocalEyeAngles = pEntity->GetEyeAngles();
		Fixed_LocalAimPunch = pEntity->GetPunch();
		Fixed_LocalViewPunch = pEntity->GetViewPunch();
		Fixed_LocalAimPunchVel = pEntity->GetPunchVel();
	}
#endif
	int nTickBase = pEntity->GetTickBase();
	int iStorage = 9 * nTickBase % 128;
	flArrayCorrect[iStorage][0] = nTickBase;

	//at this point punchangle isnt compressed and we can retrieve it then just predict it when we call processmovement inside createmove

	QAngle qPunchangle = pEntity->GetPunch();
	flArrayCorrect[iStorage][1] = qPunchangle.x;
	flArrayCorrect[iStorage][2] = qPunchangle.y;
	flArrayCorrect[iStorage][3] = qPunchangle.z;
#endif
#endif
}

__declspec (naked) void __stdcall Hooks::RunCommand(CBaseEntity *pEntity, CUserCmd* pUserCmd, void* moveHelper)
{
	__asm
	{
		push[esp + 12]
		push[esp + 12]
		push[esp + 12]
		push ecx
		call RunCmd
		retn 12
	}
}