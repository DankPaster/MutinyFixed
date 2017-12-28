#include "LocalPlayer.h"
#include "Interfaces.h"

MyPlayer LocalPlayer;

CMoveHelperServer* pMoveHelperServer;

void MyPlayer::DrawUserCmdInfo(CUserCmd* cmd)
{
	if (EXTERNAL_WINDOW)
	{
		//pSprite->Begin(D3DXSPRITE_ALPHABLEND | D3DXSPRITE_DO_NOT_ADDREF_TEXTURE);
		static Vector previousvelocity = Entity->GetVelocity();
		static float previousforwardmove = cmd->forwardmove;
		static float previoussidemove = cmd->sidemove;
		static float previousupmove = cmd->upmove;
		char test[1024];
		QAngle dir;
		Vector Velocity = Entity->GetVelocity();
		VectorAngles(Velocity, dir);
		sprintf(test, "Eye Angles: %.2f %.2f %.2f\nVelocity VectorAngle: %.2f %.2f %.2f\nVelocity: %.2f %.2f %.2f\nPrevious Velocity: %.2f %.2f %.2f\nForward Move: %.2f\nPrevious Forward Move: %.2f\nSide Move: %.2f\nPrevious Side Move: %.2f\nUp Move: %.2f\nPrevious Up Move: %.2f",
			Entity->GetEyeAngles().x, Entity->GetEyeAngles().y, Entity->GetEyeAngles().z, dir.x, dir.y, dir.z, Velocity.x, Velocity.y, Velocity.z,
			previousvelocity.x, previousvelocity.y, previousvelocity.z, cmd->forwardmove, previousforwardmove, cmd->sidemove, previoussidemove, cmd->upmove, previousupmove);
		previousvelocity = Velocity;
		previousforwardmove = cmd->forwardmove;
		previoussidemove = cmd->sidemove;
		DrawStringUnencrypted(test, Vector2D(128, 128), ColorRGBA(0, 255, 0, 255), pFont);

		QAngle EyeAngles = Entity->GetEyeAngles();
		EyeAngles.Clamp();
		QAngle VelocityAngles;
		VectorAngles(Velocity, VelocityAngles);
		float yawdelta = ClampYr(EyeAngles.y - VelocityAngles.y);
		CUserCmd newcmd;
		if (Velocity.Length() == 0.0f)
		{
			newcmd.forwardmove = 0.0f;
			newcmd.sidemove = 0.0f;
		}
		else
		{
			if (yawdelta < 0.0f)
				yawdelta += 360.0f;
			float yawdeltaabs = fabsf(yawdelta);

			if (yawdelta > 359.5f || yawdelta < 0.5f)
			{
				newcmd.forwardmove = 450.0f;
				newcmd.sidemove = 0.0f;
			}
			else if (yawdelta > 179.5f && yawdelta < 180.5f)
			{
				newcmd.forwardmove = -450.0f;
				newcmd.sidemove = 0.0f;
			}
			else if (yawdelta > 89.5f && yawdelta < 90.5f)
			{
				newcmd.forwardmove = 0.0f;
				newcmd.sidemove = 450.0f;
			}
			else if (yawdelta > 269.5f && yawdelta < 270.5f)
			{
				newcmd.forwardmove = 0.0f;
				newcmd.sidemove = -450.0f;
			}
			else if (yawdelta > 0.0f && yawdelta < 90.0f)
			{
				newcmd.forwardmove = 450.0f;
				newcmd.sidemove = 450.0f;
			}
			else if (yawdelta > 90.0f && yawdelta < 180.0f)
			{
				newcmd.forwardmove = -450.0f;
				newcmd.sidemove = 450.0f;
			}
			else if (yawdelta > 180.0f && yawdelta < 270.0f)
			{
				newcmd.forwardmove = -450.0f;
				newcmd.sidemove = -450.0f;
			}
			else
			{
				//yawdelta > 270.0f && yawdelta > 0.0f
				newcmd.forwardmove = 450.0f;
				newcmd.sidemove = -450.0f;
			}
		}
		char test2[256];
		sprintf(test2, "Rebuilt Forward Move: %.2f\nRebuilt Side Move: %.2f\n%f", newcmd.forwardmove, newcmd.sidemove, LocalPlayer.Entity->GetLowerBodyYaw());
		DrawStringUnencrypted(test2, Vector2D(400, 128), ColorRGBA(255, 0, 0, 255), pFont);
		//pSprite->End();
	}
}

void MyPlayer::BeginEnginePrediction(CUserCmd* cmd)
{
	pMoveHelperServer = (CMoveHelperServer*)ReadInt(ReadInt(pMoveHelperServerPP));
	if (!pMoveHelperServer)
	{
		THROW_ERROR(ERR_INVALID_MOVEHELPER_POINTER);
		exit(EXIT_SUCCESS);
	}

	//pMoveHelperServer->SetHost((CBasePlayer*)Entity); //FIXME: THIS SEEMS RETARDED TO RUN ON CLIENT!
	originalpredictionplayer = *(DWORD*)m_pPredictionPlayer;
	*(DWORD*)m_pPredictionPlayer = (DWORD)Entity;
	originalrandomseed = ReadInt(m_pPredictionRandomSeed);
	WriteInt(m_pPredictionRandomSeed, MD5_PseudoRandom(cmd->command_number) & 0x7fffffff);
	originalflags = Entity->GetFlags();
	originalframetime = ReadFloat((uintptr_t)&Interfaces::Globals->frametime);
	originalcurtime = ReadFloat((uintptr_t)&Interfaces::Globals->curtime);
	float interval_per_tick = ReadFloat((uintptr_t)&Interfaces::Globals->interval_per_tick);
	WriteFloat((uintptr_t)&Interfaces::Globals->frametime, interval_per_tick);
	WriteFloat((uintptr_t)&Interfaces::Globals->curtime, Entity->GetTickBase() * interval_per_tick);
	originalcurrentcommand = GetCurrentCommand(Entity);
	SetCurrentCommand(Entity, cmd);

	//Interfaces::GameMovement->StartTrackPredictionErrors((CBasePlayer*)Entity);
	memset(&NewMoveData, 0, sizeof(CMoveData));
	Interfaces::Prediction->SetupMove((C_BasePlayer*)Entity, cmd, pMoveHelperServer, &NewMoveData);
	Interfaces::GameMovement->ProcessMovement((CBasePlayer*)Entity, &NewMoveData);
	Interfaces::Prediction->FinishMove((C_BasePlayer*)Entity, cmd, &NewMoveData);
}

void MyPlayer::FinishEnginePrediction(CUserCmd* cmd)
{
	//Interfaces::GameMovement->FinishTrackPredictionErrors((CBasePlayer*)Entity);
	SetCurrentCommand(Entity, originalcurrentcommand);
	WriteFloat((uintptr_t)&Interfaces::Globals->curtime, originalcurtime);
	WriteFloat((uintptr_t)&Interfaces::Globals->frametime, originalframetime);
	Entity->SetFlags(originalflags);
	//pMoveHelperServer->SetHost(nullptr); //FIXME: THIS SEEMS RETARDED TO RUN ON CLIENT!
	*(DWORD*)m_pPredictionPlayer = originalpredictionplayer;
	WriteInt(m_pPredictionRandomSeed, originalrandomseed);
}

#include "FixMove.h"
void MyPlayer::SetRenderAngles(CUserCmd* cmd, bool StrafeModifiedAngles, bool DontSetViewAngles, void* FramePointer, bool RanAntiAim)
{
	QAngle EyeAnglesToRender;

	if (DontSetViewAngles || RanAntiAim || StrafeModifiedAngles)
	{
		EyeAnglesToRender = CurrentEyeAngles - AimPunchSubtracted;
	}
	else
	{
		EyeAnglesToRender = cmd->viewangles;
	}

	if (NoVisualRecoilChk.Checked)
		EyeAnglesToRender += AimPunchSubtracted;

#if 0
	if (!SilentAimChk.Checked && !DontSetViewAngles)
	{
		if (NoAntiAimsAreOn(true))
		{
			if (StrafeModifiedAngles)
				EyeAnglesToRender = CurrentEyeAngles - AimPunchSubtracted;
			else
				EyeAnglesToRender = cmd->viewangles;// -StrafDif;
		}
		else if (AntiAimRealDrop.index == AntiAims::ANTIAIM_LEGIT)
		{
			if (ReadByte((ReadInt((uintptr_t)FramePointer) - 0x1C)) == 0)
				EyeAnglesToRender = CurrentEyeAngles - AimPunchSubtracted;
			else
				EyeAnglesToRender = cmd->viewangles;//  -StrafDif;
		}
		else
			EyeAnglesToRender = CurrentEyeAngles - AimPunchSubtracted;

		if (NoVisualRecoilChk.Checked)
		{
			EyeAnglesToRender += AimPunchSubtracted; //FIXME
		}
	}
	else
	{
		EyeAnglesToRender = CurrentEyeAngles;
		if (!NoVisualRecoilChk.Checked)
			EyeAnglesToRender -= AimPunchSubtracted;
	}
#endif

	EyeAnglesToRender.Clamp();
	Interfaces::Engine->SetViewAngles(EyeAnglesToRender);
}