#include "FixMove.h"
#include "Overlay.h"
#include "Aimbot.h" //normalizeangle
#include "player_lagcompensation.h"
#include "Targetting.h"
#include "LocalPlayer.h"
#include "Triggerbot.h"
#include "VMProtectDefs.h"

extern int LastTargetIndex;
extern double difference(double first, double second);

void ClassicalAA(QAngle &views)
{
	if (abs(LocalPlayer.Entity->GetVelocity().Length2D()) < 25 && (LocalPlayer.Entity->GetFlags() & FL_ONGROUND))
	{
		QAngle dif180 = views;
		dif180.y += 180;
		(dif180.Clamp());
		if (abs(difference(LocalPlayer.LowerBodyYaw, dif180.y)) > 20)
		{
			views.y += 180;
		}
		else
		{
			static bool jitteryaw = false;
			if (jitteryaw)
				views.y -= 146;
			else
				views.y += 146;

			jitteryaw = !jitteryaw;
		}
	}
	else if (LocalPlayer.Entity->GetFlags() & FL_ONGROUND)
	{
		static bool jitteryaw = false;
		if (jitteryaw)
			views.y -= 155;
		else
			views.y += 155;

		jitteryaw = !jitteryaw;

	}
	else
	{
		views.y += 180;

	}
}
void Classical90AA(QAngle& views)
{
	if (abs(LocalPlayer.Entity->GetVelocity().Length2D()) < 25 && (LocalPlayer.Entity->GetFlags() & FL_ONGROUND))
	{
		QAngle dif180 = views;
		dif180.y += 90;
		dif180.Clamp();
		if (abs(difference(LocalPlayer.LowerBodyYaw, dif180.y)) > 20)
		{
			views.y += 90;
		}
		else
		{
			views.y += 120;
		}
	}
	else if (LocalPlayer.Entity->GetFlags() & FL_ONGROUND)
	{
		views.y += 90;

	}
	else
	{
		views.y += 90;

	}
}

QAngle LastChoke = QAngle(0, 0, 0);
QAngle LastSend = QAngle(0, 0, 0);
void Classical180AA(QAngle& views)
{
	bool onGround = LocalPlayer.Entity->GetFlags() & FL_ONGROUND;
	static bool shotswitch = true;
	static bool first = true;
	static bool jitter = false;
	float speed = LocalPlayer.Entity->GetVelocity().Length2D();
	if (buttons & IN_ATTACK)
	{
		if (first)
		{
			first = first;
			shotswitch = !shotswitch;
		}
	}
	else
		first = first;
	static int swap = 1;
	
	if (CurrentUserCmd.IsFakeAngle())
	{
		if (onGround)
		{
			if (jitter)
				views.y += swap* 32;
			else
				views.y -= swap * rand() % 58 + 3;
			jitter = !jitter;
		}
		else
		{
			if (shotswitch)
				if (jitter)
					views.y += 74 * speed / 100;
				else
					views.y -= 74 * speed / 200;
			else
				if (jitter)
					views.y += 150;
				else
					views.y -= 150;

		}

	}
	else
	{
		if (onGround)
		{
			if (speed < 1.0f)
			{
				static bool didrun = false;
				float minValue = 0.27f;
				float maxValue = 0.35f;//need these values small as possible
				if (abs(LocalPlayer.NextLowerBodyUpdateTime - Interfaces::Globals->curtime) < minValue)
				{

					if ((int)LocalPlayer.LowerBodyYaw == (int)(views.y + 180))
						views.y += swap * 90;
					else
						views.y += 180;
				}
				else if ((int)LocalPlayer.LowerBodyYaw == (views.y + 180))
				{
					views.y += swap * 90;

				}
				else if (LocalPlayer.NextLowerBodyUpdateTime - Interfaces::Globals->curtime > maxValue)
				{
					didrun = true;
					views.y += 180;
					if(abs(difference(views.y, LocalPlayer.LowerBodyYaw)) < 20)
						views.y += swap * 90;

				}
				else
				{
					if(didrun)
						first = true;
					didrun = false;
					if (first)
						swap *= -1;
					first = false;

					views.y += swap * (90);
				}

			}
			else
			{
				jitter = !jitter;
				if (jitter)
					views.y += 160;
				else
					views.y -= 160;

			}
		}
		else
		{
			jitter = !jitter;
			views.y += 180;

		}
	}
	/*float additionval = LastChoke.y;
	if (!CurrentUserCmd.bSendPacket)
		additionval = LastSend.y;

	if (abs(LocalPlayer.Entity->GetVelocity().Length2D()) < 25 && (LocalPlayer.Entity->GetFlags() & FL_ONGROUND))
	{
		QAngle dif180 = views;
		dif180.y = additionval + 180;
		dif180.Clamp();
		if (abs(difference(LocalPlayer.LowerBodyYaw, dif180.y)) > 20)
		{
			views.y = additionval + 180;
		}
		else
		{
			static bool jitteryaw = false;
			if (jitteryaw)
				views.y = additionval + 146;
			else
				views.y = additionval - 146;

			jitteryaw = !jitteryaw;
		}
	}
	else if (LocalPlayer.Entity->GetFlags() & FL_ONGROUND)
	{
		static bool jitteryaw = false;
		if (jitteryaw)
			views.y = additionval - 130;
		else
			views.y += 150;

		jitteryaw = !jitteryaw;

	}
	else
	{
		static bool jitteryaw = false;
		if (jitteryaw)
			views.y = additionval + 180;
		else
			views.y += 180;

		jitteryaw = !jitteryaw;


	}
	if (!CurrentUserCmd.bSendPacket)
		LastChoke = views;
	else
		LastSend = views;*/

}
void InverseSwitchAA(QAngle& views)
{
	bool onGround = LocalPlayer.Entity->GetFlags() & FL_ONGROUND;
	static bool shotswitch = true;
	static bool first = true;
	float speed = LocalPlayer.Entity->GetVelocity().Length2D();
	if (buttons & IN_ATTACK)
	{
		if (first)
		{
			first = false;
			shotswitch = !shotswitch;
		}
	}
	else
		first = true;

	if (CurrentUserCmd.IsFakeAngle())
	{
		if (speed < 5)
		{
			if (onGround)
				views.y += 90;
			else if (shotswitch)
				views.y += 90;
			else
				views.y -= 90;
		}
		else
		{
			if (shotswitch)
				views.y += 74 * speed / 100;
			else
				views.y -= 74 * speed / 200;
		}
	}
	else
	{
		if (onGround)
		{
			if (speed < 5.0f)
			{
				static int switched = 0;
				static float currentLby = LocalPlayer.LowerBodyYaw;
				if (currentLby != LocalPlayer.LowerBodyYaw && !switched)
				{
					views.y += 90;
					switched = 2;
					currentLby = LocalPlayer.LowerBodyYaw;
				}
				else
				{
					views.y -= 90;
					if (switched > 0)
						switched--;
				}
			}
			else
				views.y += 180;
		}
		else
		{
			if (shotswitch)
				views.y -= 140;
			else
				views.y += 140;

			views.y += rand() % 40 - 20;
		}
	}
	/*
	if (buttons & IN_ATTACK)
	{
		views.y += 90;
	}
	else
		views.y += 180;
	*/
}
void AutisticYossAA(QAngle& views)
{
	int addvalue = rand() % 40 - 20;
	static bool left = true;
	static bool first = true;
	static int counter = 1;
	if (buttons & IN_ATTACK)
	{
		if (first)
			left = !left;
		first = false;
		if (counter % 30 == 0)
			left = !left;
	}
	else
		first = true;
	if (counter % 64 == 0)
		left = !left;

	counter++;


	if (left)
		views.y -= 90 + addvalue;
	else
		views.y += 90 + addvalue;
}
void Do_AntiAimsPitch(QAngle& TargetAAAngles, bool& LegitAAed, DWORD* FramePointer)
{
	CurrentUserCmd.SetIsFakeAngle(CurrentUserCmd.bSendPacket);

	switch (AntiAimPitchDrop.index)
	{
#ifdef JAKES
	case PITCH_NONE://No AA
		break;
	case PITCH_EMOTION: //Emotion
		TargetAAAngles.x = 87.32f;
		break;
	case PITCH_UP: //Up
		TargetAAAngles.x = -87.32f;
		break;
	case PITCH_DOWN://Down
		TargetAAAngles.x = 173;
		break;
	case PITCH_FAKEDOWN: //Fake Down
		TargetAAAngles.x = 180;
		break;
	case PITCH_FAKEDOWN2: //Fake Down
		TargetAAAngles.x = 1169;
		break;
	case PITCH_FAKEUP://Fake Up
		TargetAAAngles.x = 1000;
		break;
#else
	case PITCH_NONE://No AA
		break;
	case PITCH_UP:
		TargetAAAngles.x = -89.0f;
		break;
	case PITCH_DOWN:
		TargetAAAngles.x = 89.0f;
		break;
	case PITCH_JITTERDOWN:
		TargetAAAngles.x = SORandomFloat(68.0f, 89.0f);
		break;
	case PITCH_JITTERUPDOWN:
		TargetAAAngles.x = SORandomFloat(-89.0f, 89.0f);
		break;
	case PITCH_LISP1:
		TargetAAAngles.x = 1000.0f;
		break;
	case PITCH_LISP2:
		TargetAAAngles.x = 1080.0f;
		break;
	case PITCH_TDOWN://Down
		TargetAAAngles.x = 173;
		break;
	case PITCH_FAKEDOWN: //Fake Down
		TargetAAAngles.x = 180;

#endif
	}
	VMP_END

}
void Do_AntiAims(QAngle& TargetAAAngles, bool& LegitAAed, DWORD* FramePointer)
{
	VMP_BEGINMUTILATION("DOAA")
	int AntiAimIndex;

	CurrentUserCmd.SetIsFakeAngle(CurrentUserCmd.bSendPacket);

	if (CurrentUserCmd.IsFakeAngle())
	{
		if (AntiAimFakeDrop.index == ANTIAIM_DEFAULT)
			AntiAimIndex = AntiAimRealDrop.index;
		else if (AntiAimFakeDrop.index > 1)
			AntiAimIndex = AntiAimFakeDrop.index - 1;
		else
			AntiAimIndex = AntiAims::ANTIAIM_NONE;
	}
	else
	{
		AntiAimIndex = AntiAimRealDrop.index;
	}

	switch (AntiAimIndex)
	{
	case ANTIAIM_NONE:
		break;
	case ANTIAIM_SPIN:
		FixMove_Spinbot(TargetAAAngles, LocalPlayer.LastEyeAngles, LocalPlayer.CurrentEyeAngles, FramePointer);
		break;
	case ANTIAIM_DOWNBACK:
		FixMove_AntiAim(TargetAAAngles, LocalPlayer.LastEyeAngles, LocalPlayer.CurrentEyeAngles, LastTargetIndex == INVALID_PLAYER ? nullptr : AllPlayers[LastTargetIndex].BaseEntity, FramePointer);
		break;
	case ANTIAIM_SLOWSPINFAKE:
		FixMove_Spinbot_Unhittable(TargetAAAngles, LocalPlayer.LastEyeAngles, LocalPlayer.CurrentEyeAngles, FramePointer);
		break;
	case ANTIAIM_BALLERINAFAKE:
		FixMove_AntiAimMemester(TargetAAAngles, LocalPlayer.LastEyeAngles, LocalPlayer.CurrentEyeAngles, FramePointer);
		break;
	case ANTIAIM_SIDEWAYS:
		FixMove_SideAntiAim(TargetAAAngles, LocalPlayer.LastEyeAngles, LocalPlayer.CurrentEyeAngles, LastTargetIndex == INVALID_PLAYER ? nullptr : AllPlayers[LastTargetIndex].BaseEntity, FramePointer);
		break;
	case ANTIAIM_SKATE:
		FixMove_AntiAimSkate(TargetAAAngles, LocalPlayer.LastEyeAngles, LocalPlayer.CurrentEyeAngles, FramePointer);
		break;
	case ANTIAIM_LEGIT:
		LegitAAed = FixMove_LegitAntiAim(TargetAAAngles, buttons, LocalPlayer.LastEyeAngles, LocalPlayer.CurrentEyeAngles, FramePointer, LocalPlayer.CurrentWeapon);
		break;
	case ANTIAIM_EXPERIMENTAL:
		FixMove_Experimental(LastTargetIndex == INVALID_PLAYER ? nullptr : AllPlayers[LastTargetIndex].BaseEntity, buttons, TargetAAAngles, LocalPlayer.LastEyeAngles, LocalPlayer.CurrentEyeAngles, FramePointer);
		break;
	case ANTIAIM_EXPERIMENTAL2:
		FixMove_Experimental2(LastTargetIndex == INVALID_PLAYER ? nullptr : AllPlayers[LastTargetIndex].BaseEntity, buttons, TargetAAAngles, LocalPlayer.LastEyeAngles, LocalPlayer.CurrentEyeAngles, FramePointer);
		break;
	case ANTIAIM_EXPERIMENTALFORWARDS:
		FixMove_ExperimentalForwards(LastTargetIndex == INVALID_PLAYER ? nullptr : AllPlayers[LastTargetIndex].BaseEntity, buttons, TargetAAAngles, LocalPlayer.LastEyeAngles, LocalPlayer.CurrentEyeAngles, FramePointer);
		break;
	case ANTIAIM_EXPERIMENTALFORWARDS_INVERTED:
		FixMove_ExperimentalForwards_Inverted(LastTargetIndex == INVALID_PLAYER ? nullptr : AllPlayers[LastTargetIndex].BaseEntity, buttons, TargetAAAngles, LocalPlayer.LastEyeAngles, LocalPlayer.CurrentEyeAngles, FramePointer);
		break;
	case ANTIAIM_TESTING:
		FixMove_Testing(TargetAAAngles, LocalPlayer.LastEyeAngles, LocalPlayer.CurrentEyeAngles, LastTargetIndex == INVALID_PLAYER ? nullptr : AllPlayers[LastTargetIndex].BaseEntity, FramePointer);
		break;
	case ANTIAIM_TESTING2:
		FixMove_Testing2(TargetAAAngles, LocalPlayer.LastEyeAngles, LocalPlayer.CurrentEyeAngles, LastTargetIndex == INVALID_PLAYER ? nullptr : AllPlayers[LastTargetIndex].BaseEntity, FramePointer);
		break;
	case ANTIAIM_CLASSICAL:
		TargetAAAngles = LocalPlayer.CurrentEyeAngles;
		ClassicalAA(TargetAAAngles);
		break;
	case ANTIAIM_CLASSICAL180:
		TargetAAAngles = LocalPlayer.CurrentEyeAngles;
		Classical180AA(TargetAAAngles);
		break;
	case ANTIAIM_CLASSICAL90:
		TargetAAAngles = LocalPlayer.CurrentEyeAngles;
		Classical90AA(TargetAAAngles);
		break;
	case ANTIAIM_INVERSE_SWITCH:
		TargetAAAngles = LocalPlayer.CurrentEyeAngles;
		InverseSwitchAA(TargetAAAngles);
		break;
	case ANTIAIM_SIDEWAYS_JITTER:
		TargetAAAngles = LocalPlayer.CurrentEyeAngles;
		AutisticYossAA(TargetAAAngles);
		break;
	}

#if 0
	if (AntiAimJitterChk.Checked)
	{
		//If jitter button is checked letsjitter all our anit aims :) 
		TargetAAAngles.y += SORandomFloat(-1 * SpinDegPerTickTxt.flValue, SpinDegPerTickTxt.flValue);
	}
#endif

	VMP_END
}

void RunGrenadeKnifeAA(bool& Ignore)
{
	if (LocalPlayer.CurrentWeapon)
	{
		int WeaponID = LocalPlayer.CurrentWeapon->GetItemDefinitionIndex();
		if (LocalPlayer.CurrentWeapon->IsGrenade(WeaponID))
		{
			BOOLEAN bRedraw = ReadInt((uintptr_t)LocalPlayer.CurrentWeapon + m_bRedraw);
			if (!bRedraw)
			{
				BOOLEAN bPinPulled = ReadInt((uintptr_t)LocalPlayer.CurrentWeapon + m_bPinPulled);
				if (bPinPulled || fabsf(Interfaces::Globals->curtime - ReadFloat((uintptr_t)LocalPlayer.CurrentWeapon + m_fThrowTime)) > 0.1f)
					Ignore = false;
			}
		}
		else if (LocalPlayer.CurrentWeapon->IsKnife(WeaponID) && !LocalPlayer.EnemyIsWithinKnifeRange)
		{
			static float flLastKnifeAttack = 0.0f;
			bool bKnifing = (buttons & IN_ATTACK) || (buttons & IN_ATTACK2);
			if (!bKnifing)
			{
				if (ReadFloat((uintptr_t)&Interfaces::Globals->curtime) - flLastKnifeAttack > 0.2f)
					Ignore = false;
			}
			else
			{
				if (gTriggerbot.WeaponCanFire(LocalPlayer.CurrentWeapon))
				{
					flLastKnifeAttack = ReadFloat((uintptr_t)&Interfaces::Globals->curtime);
				}
				else if (ReadFloat((uintptr_t)&Interfaces::Globals->curtime) - flLastKnifeAttack > 0.2f)
				{
					Ignore = false;
				}
			}
		}
	}
}

void MoveFix(QAngle angles, QAngle OriginalAngs, Vector Move)
{
	float Speed = sqrt(Move.x * Move.x + Move.y * Move.y);
	QAngle DotObject;
	VectorAngles3D(Move, DotObject);

	float Yaw = DEG2RAD(angles.y - OriginalAngs.y + DotObject.y);

	CurrentUserCmd.cmd->forwardmove = cos(Yaw) * Speed;
	CurrentUserCmd.cmd->sidemove = sin(Yaw) * Speed;

	if (abs(angles.x) > 90.f)
		CurrentUserCmd.cmd->forwardmove = -CurrentUserCmd.cmd->forwardmove;
}
float PickRandomAngle(float numangles, ...)
{
	va_list list;
	va_start(list, numangles);
	float ret;
	float *angles = new float[numangles];
	for (int i = 0; i < numangles; i++)
	{
		angles[i] = (float)va_arg(list, double);
	}
	va_end(list);
	ret = angles[rand() % (int)numangles];

	delete[]angles;
	return ret;
}

void FixMove_Spinbot(QAngle& angles, QAngle LastAngle, QAngle RealViewAngles, DWORD* FramePointer)
{
	Vector viewForward, viewRight, viewUp;
	angles.y = LastAngle.y - SpinDegPerTickTxt.flValue;
	angles.ClampY();

	if (AntiAimJitterChk.Checked)
	{
		angles.y += SORandomFloat(-40.0f, 40.0f);

		if (CurrentUserCmd.IsRealAngle())
		{
			while (fabsf(angles.y - LastAngle.y) < 35.0f || fabsf(angles.y - LocalPlayer.LowerBodyYaw) < 35.0f)
			{
				angles.y += SORandomFloat(-55.0f, 55.0f);
			}
		}
	}

	angles.ClampY();
}

void FixMove_Spinbot_Unhittable(QAngle& angles, QAngle LastAngle, QAngle RealViewAngles, DWORD* FramePointer)
{
	float m_flServerTime = LocalPlayer.Entity->GetTickBase() * ReadFloat((uintptr_t)&Interfaces::Globals->interval_per_tick);
	angles.y = (float)(fmod((m_flServerTime * 1.5f) * 360.0f, 360.0f));

	if (CurrentUserCmd.IsRealAngle())
	{
		angles.y += 110.0f;
		angles.ClampY();
	}

	if (AntiAimJitterChk.Checked)
	{
		angles.y += SORandomFloat(-40.0f, 40.0f);

		if (CurrentUserCmd.IsRealAngle())
		{
			while (fabsf(angles.y - LocalPlayer.LowerBodyYaw) < 35.0f)
			{
				angles.y += SORandomFloat(-55.0f, 55.0f);
			}
		}
	}

	angles.ClampY();
}

void pyCacheAA(QAngle RealViewAngles, QAngle& angles, DWORD* FramePointer) {
	static bool flip2;
	if (CurrentUserCmd.IsFakeAngle()) {
		flip2 = !flip2;
		if (flip2) {
			angles.y += SORandomFloat(40.0f, 135.0f);
		}
		else {
			angles.y -= SORandomFloat(40.0f, 135.0f);
			angles.y -= 90.f;
		}
	}
	else {
		angles.y += 180.0f;

		if (LocalPlayer.Entity->GetVelocity().Length2D() < 0.1) //standind still
			angles.y += SORandomFloat(0.0f, 60.0f) - 30;
	}

}

void FixMove_Experimental(CBaseEntity* AimTarget, int buttons, QAngle& angles, QAngle LastAngle, QAngle RealViewAngles, DWORD* FramePointer)
{
	static int jitter = 0;

	if (!AimTarget)
	{
		AimTarget = MTargetting.GetPlayerClosestToCrosshair(AimbotFOVTxt.flValue);
	}


	//if (GetEstimatedServerTickCount(GetNetworkLatency()) > LocalPlayerNextLowerBodyUpdateTick)
	{
		//void* animstate = LocalPlayer.Entity->GetPlayerAnimState();
		//float m_flCurrentFeetYaw = ReadFloat((uintptr_t)animstate + 0x84);
		Vector LocalVelocity = LocalPlayer.Entity->GetVelocity();
		float LocalSpeed = LocalVelocity.Length();
		bool lagging = Interfaces::Globals->absoluteframetime >= Interfaces::Globals->interval_per_tick;

		if (CurrentUserCmd.IsRealAngle())
		{
			//bool Strafing = ( == 0.0f &&  != 0.0f);
			if (AimTarget && /*!Strafing &&*/ LocalSpeed > 0.1f)
			{
				QAngle angletotarget = CalcAngle(LocalPlayer.Entity->GetEyePosition(), AimTarget->GetEyePosition());
				angletotarget.y += 180.0f;

				if (AntiAimJitterChk.Checked)
				{
					if (jitter < 100)
					{
						angletotarget.y -= 5.0f;
						jitter++;
					}
					else
					{
						angletotarget.y += 5.0f;
						jitter = 0;
					}
				}
				angletotarget.ClampY();
				angles.y = angletotarget.y;
			}
#if 0
			else if (Strafing)
			{
				VectorAngles(LocalVelocity, angles);
				angles.y += 180.0f;
				angles.z = 0.0f;
			}
#endif
			else
			{
				if (LocalPlayer.NextLowerBodyUpdateTime - ReadFloat((uintptr_t)&Interfaces::Globals->curtime) > 0.3f)
				{
					if (AimTarget)
					{
						QAngle angletous = CalcAngle(AimTarget->GetEyePosition(), LocalPlayer.Entity->GetEyePosition());
						angles.y = angletous.y + 90.0f;
					}
					else
					{
						angles.y = RealViewAngles.y + 100.0f;
					}
					if (AntiAimJitterChk.Checked)
					{
						angles.y += SORandomFloat(-15.0f, 15.0f);
					}
				}
				else
				{
					Vector TargetEyePos;
					Vector LocalEyePos;
					if (AimTarget)
					{
						TargetEyePos = AimTarget->GetEyePosition();
						LocalEyePos = LocalPlayer.Entity->GetEyePosition();
						QAngle fakeangles = angles;
						fakeangles.y = LocalPlayer.LowerBodyYaw;
						Vector vecForward, vecRight, vecUp;
						AngleVectors(fakeangles, &vecForward, &vecRight, &vecUp);

						Vector EnemyDir = LocalEyePos - TargetEyePos;

						float side = DotProduct(vecRight, EnemyDir);

						Vector ourheadpos = LocalPlayer.Entity->GetEyePosition();;
						QAngle angletous = CalcAngle(AimTarget->GetEyePosition(), ourheadpos);

						float FOV = LocalPlayer.LowerBodyYaw - angletous.y;
						ClampY(FOV);
						FOV = fabsf(FOV);

						if (side < 0)
						{
							//Enemy is on the right side of the direction of lower body yaw
							if (FOV > 35.0f)
							{
								angles.y = LocalPlayer.LowerBodyYaw + FOV;
							}
							else
							{
								angles.y = LocalPlayer.LowerBodyYaw + 34.8f;
							}
						}
						else
						{
							//Enemy is on the left side of the direction of lower body yaw
							if (FOV > 35.0f)
							{
								angles.y = LocalPlayer.LowerBodyYaw - FOV;
							}
							else
							{
								angles.y = LocalPlayer.LowerBodyYaw - 34.8f;
							}
						}


						}
					else
					{
						if (jitter < 1)
						{
							angles.y = LocalPlayer.LowerBodyYaw + 34.8f;
							jitter++;
						}
						else
						{
							angles.y = LocalPlayer.LowerBodyYaw - 34.8f;
							jitter = 0;
						}
					}
					}
				}
			}
		else
		{
			//fake angle

			if (LocalPlayer.Entity->GetVelocity().Length() > 0.0f)
			{
				angles.y = LocalPlayer.LowerBodyYaw + 115.0f;
			}
			else
			{
				angles.y = RealViewAngles.y + 180.0f;

				if (AimTarget)
				{
					if (AimTarget)
					{
						//Vector ourheadpos = LocalPlayer.Entity->GetEyePosition();;
						//QAngle angletous = CalcAngle(AimTarget->GetEyePosition(), ourheadpos);
						//angles.y = angletous.y + 90.0f;
						static bool add = true;
						static float ramp = 140.0f;
						if (add)
						{
							ramp += 35.0f;
							if (ramp > 220.0f)
								add = !add;
						}
						else
						{
							ramp -= 35.0f;
							if (ramp < 140.0f)
								add = !add;
						}
						angles.y = LocalPlayer.LastRealEyeAngles.y + ramp;
					}
				}
			}
		}
		}
	/*
	else
	{
		if (sendpacket < 2)
		{
			angles.y = LocalPlayer.LowerBodyYaw + 180.0f;;
			CurrentUserCmd.BsendPacket = 0; //SendPacket = false
			sendpacket++;
		}
		else
		{
			angles.y = LocalPlayer.LowerBodyYaw;
			sendpacket = 0;
		}
	}
	*/

	angles.ClampY();
	}

void FixMove_ExperimentalForwards(CBaseEntity* AimTarget, int buttons, QAngle& angles, QAngle LastAngle, QAngle RealViewAngles, DWORD* FramePointer)
{
	static int jitter = 0;

	if (!AimTarget)
	{
		AimTarget = MTargetting.GetPlayerClosestToCrosshair(AimbotFOVTxt.flValue);
	}

	Vector LocalVelocity = LocalPlayer.Entity->GetVelocity();
	float LocalSpeed = LocalVelocity.Length();
	bool lagging = Interfaces::Globals->absoluteframetime >= Interfaces::Globals->interval_per_tick;

	if (CurrentUserCmd.IsRealAngle())
	{
		//real angle
		//bool Strafing = ( == 0.0f &&  != 0.0f);
		if (AimTarget && /*!Strafing &&*/ LocalSpeed > 0.1f)
		{
			QAngle angletotarget = CalcAngle(LocalPlayer.Entity->GetEyePosition(), AimTarget->GetEyePosition());
			angletotarget.y += 180.0f;

			if (AntiAimJitterChk.Checked)
			{
				if (jitter < 100)
				{
					angletotarget.y -= 5.0f;
					jitter++;
				}
				else
				{
					angletotarget.y += 5.0f;
					jitter = 0;
				}
			}
			angletotarget.ClampY();
			angles.y = angletotarget.y;
		}
		else if (LocalSpeed > 0.1f)
		{
			VectorAngles(LocalVelocity, angles);
			angles.y += 180.0f;
			angles.z = 0.0f;
		}
		else
		{
			if (LocalPlayer.NextLowerBodyUpdateTime - ReadFloat((uintptr_t)&Interfaces::Globals->curtime) > 0.3f)
			{
				if (AimTarget)
				{
					QAngle angletous = CalcAngle(AimTarget->GetEyePosition(), LocalPlayer.Entity->GetEyePosition());
					angles.y = angletous.y + 180.0f + PickRandomAngle(2, -6.0, 6.0);// PickRandomAngle(4, -90.0, 90.0, 180.0);
				}
				else
				{
					angles.y = RealViewAngles.y + 180.0f;
				}
				if (AntiAimJitterChk.Checked)
				{
					angles.y += SORandomFloat(-10.0f, 10.0f);
				}
			}
			else
			{
				Vector TargetEyePos;
				Vector LocalEyePos;
				if (AimTarget)
				{
					//TargetEyePos = AimTarget->GetEyePosition();
					LocalEyePos = LocalPlayer.Entity->GetEyePosition();

					angles = CalcAngle(LocalEyePos, AimTarget->GetNetworkOrigin());
					angles.y += PickRandomAngle(2, -34.6, 34.6);
					angles.z = 0.0f;
				}
				else
				{
					if (jitter < 1)
					{
						angles.y = RealViewAngles.y + 34.8f;
						jitter++;
					}
					else
					{
						angles.y = RealViewAngles.y - 34.8f;
						jitter = 0;
					}
				}
			}
		}
	}
	else
	{
		//fake angle
		if (AimTarget)
		{
			Vector TargetEyePos = AimTarget->GetEyePosition();
			Vector LocalEyePos = LocalPlayer.Entity->GetEyePosition();

			angles = CalcAngle(LocalEyePos, AimTarget->GetNetworkOrigin());
			angles.z = 0.0f;
		}
		else
		{
			angles.y = RealViewAngles.y;
		}
	}

	angles.ClampY();
}

void FixMove_ExperimentalForwards_Inverted(CBaseEntity* AimTarget, int buttons, QAngle& angles, QAngle LastAngle, QAngle RealViewAngles, DWORD* FramePointer)
{
	static int jitter = 0;

	if (!AimTarget)
	{
		AimTarget = MTargetting.GetPlayerClosestToCrosshair(AimbotFOVTxt.flValue);
	}

	Vector LocalVelocity = LocalPlayer.Entity->GetVelocity();
	float LocalSpeed = LocalVelocity.Length();
	bool lagging = Interfaces::Globals->absoluteframetime >= Interfaces::Globals->interval_per_tick;
	int maxchoked = lagging || (Interfaces::Globals->curtime - gTriggerbot.PS_LastShotTime < 0.1f) ? 1 : LocalSpeed > 0.0f ? 5 : 2;

	if (CurrentUserCmd.IsRealAngle())
	{
		//real angle
		if (AimTarget)
		{
			Vector TargetEyePos = AimTarget->GetEyePosition();
			Vector LocalEyePos = LocalPlayer.Entity->GetEyePosition();

			angles = CalcAngle(LocalEyePos, TargetEyePos);
			angles.z = 0.0f;
		}
		else
		{
			angles.y = RealViewAngles.y;
		}
	}
	else
	{
		//fake angle
		if (AimTarget && /*!Strafing &&*/ LocalSpeed > 0.1f)
		{
			QAngle angletotarget = CalcAngle(LocalPlayer.Entity->GetEyePosition(), AimTarget->GetEyePosition());
			angletotarget.y += 180.0f;

			if (AntiAimJitterChk.Checked)
			{
				if (jitter < 100)
				{
					angletotarget.y -= 5.0f;
					jitter++;
				}
				else
				{
					angletotarget.y += 5.0f;
					jitter = 0;
				}
			}
			angletotarget.ClampY();
			angles.y = angletotarget.y;
		}
		else if (LocalSpeed > 0.1f)
		{
			VectorAngles(LocalVelocity, angles);
			angles.y += 180.0f;
			angles.z = 0.0f;
		}
		else
		{
			if (LocalPlayer.NextLowerBodyUpdateTime - ReadFloat((uintptr_t)&Interfaces::Globals->curtime) > 0.3f)
			{
				if (AimTarget)
				{
					QAngle angletous = CalcAngle(AimTarget->GetEyePosition(), LocalPlayer.Entity->GetEyePosition());
					angles.y = angletous.y + 180.0f + PickRandomAngle(2, -6.0, 6.0);// PickRandomAngle(4, -90.0, 90.0, 180.0);
				}
				else
				{
					angles.y = RealViewAngles.y + 180.0f;
				}
				if (AntiAimJitterChk.Checked)
				{
					angles.y += SORandomFloat(-10.0f, 10.0f);
				}
			}
			else
			{
				Vector TargetEyePos;
				Vector LocalEyePos;
				if (AimTarget)
				{
					TargetEyePos = AimTarget->GetEyePosition();
					LocalEyePos = LocalPlayer.Entity->GetEyePosition();

					angles = CalcAngle(LocalEyePos, TargetEyePos);
					angles.y += PickRandomAngle(2, -34.8, 34.8);
					angles.z = 0.0f;
				}
				else
				{
					if (jitter < 1)
					{
						angles.y = RealViewAngles.y + 34.8f;
						jitter++;
					}
					else
					{
						angles.y = RealViewAngles.y - 34.8f;
						jitter = 0;
					}
				}
			}
		}
	}

	angles.ClampY();	
}

void FixMove_Experimental2(CBaseEntity* AimTarget, int buttons, QAngle& angles, QAngle LastAngle, QAngle RealViewAngles, DWORD* FramePointer)
{
	static int jitter = 0;

	if (!AimTarget)
	{
		AimTarget = MTargetting.GetPlayerClosestToCrosshair(AimbotFOVTxt.flValue);
	}


	//if (GetEstimatedServerTickCount(GetNetworkLatency()) > LocalPlayerNextLowerBodyUpdateTick)
	{
		//void* animstate = LocalPlayer.Entity->GetPlayerAnimState();
		//float m_flCurrentFeetYaw = ReadFloat((uintptr_t)animstate + 0x84);

		if (CurrentUserCmd.IsRealAngle())
		{
			if (AimTarget && LocalPlayer.Entity->GetVelocity().Length() != 0.0f)
			{
				QAngle angletotarget = CalcAngle(LocalPlayer.Entity->GetEyePosition(), AimTarget->GetEyePosition());
				angletotarget.y += 180.0f;

				if (AntiAimJitterChk.Checked)
				{
					if (jitter < 100)
					{
						angletotarget.y -= 5.0f;
						jitter++;
					}
					else
					{
						angletotarget.y += 5.0f;
						jitter = 0;
					}
				}
				angletotarget.ClampY();
				angles.y = angletotarget.y;
			}
			else
			{
				if (LocalPlayer.NextLowerBodyUpdateTime - ReadFloat((uintptr_t)&Interfaces::Globals->curtime) > 0.3f)
				{
					if (AimTarget)
					{
						QAngle angletous = CalcAngle(AimTarget->GetEyePosition(), LocalPlayer.Entity->GetEyePosition());
						angles.y = angletous.y + 90.0f;
					}
					else
					{
						angles.y = RealViewAngles.y + 100.0f;
					}
					if (AntiAimJitterChk.Checked)
					{
						angles.y += SORandomFloat(-15.0f, 15.0f);
					}
				}
				else
				{
					Vector TargetEyePos;
					Vector LocalEyePos;
					if (AimTarget)
					{
						//Vector t;

						TargetEyePos = AimTarget->GetEyePosition();
						LocalEyePos = LocalPlayer.Entity->GetEyePosition();

						QAngle OriginalRenderAngles = LocalPlayer.Entity->GetRenderAngles();
						QAngle OriginalAbsAngles = LocalPlayer.Entity->GetAbsAngles();

						Vector oureyepos = LocalPlayer.Entity->GetEyePosition();
						Vector enemyeyepos = AimTarget->GetEyePosition();

						//	WorldToScreen(enemyeyepos, t);
						//	DrawBox(Vector2D(t.x, t.y), 10, 10, 2, 0, 255, 0, 255);

						angles.ClampY();

						QAngle testangles = QAngle(89.0f, LocalPlayer.LowerBodyYaw, 0.0f);

						LocalPlayer.Entity->SetRenderAngles(testangles);
						LocalPlayer.Entity->SetEyeAngles(testangles);
						LocalPlayer.Entity->InvalidatePhysicsRecursive(ANGLES_CHANGED);
						LocalPlayer.Entity->InvalidateBoneCache();
						LocalPlayer.Entity->UpdateClientSideAnimation();
						LocalPlayer.Entity->SetupBones(nullptr, MAXSTUDIOBONES, BONE_USED_BY_ANYTHING, Interfaces::Globals->curtime);

						Vector lowerbodyheadpos = LocalPlayer.Entity->GetBonePosition(HITBOX_HEAD, /*LocalPlayer.Entity->GetSimulationTime()*/Interfaces::Globals->curtime, true, false);
						QAngle angletous = CalcAngle(enemyeyepos, lowerbodyheadpos);
						Vector vForward, vRight, vUp;
						AngleVectors(angletous, &vForward, &vRight, &vUp);
						VectorNormalizeFast(vForward);
						Vector TraceEndPos = lowerbodyheadpos + vForward * 40.0f;
						//Vector offset = vRight * 2.0f;
						//Vector TraceEndPosRight = (lowerbodyheadpos + offset) + (vForward * 40.0f);
						//Vector TraceEndPosLeft = (lowerbodyheadpos - offset) + (vForward * 40.0f);


						//Vector t2, t3;
						//WorldToScreen(enemyeyepos, t2);
						//WorldToScreen(TraceEndPos, t3);
						//DrawLine(Vector2D(t2.x, t2.y), Vector2D(t3.x, t3.y), 255, 255, 0, 255);
						//WorldToScreen(lowerbodyheadpos, t);
						//DrawBox(Vector2D(t.x, t.y), 10, 10, 2, 255, 0, 0, 255);

						QAngle targetabsangles = OriginalAbsAngles;

						testangles.y = LocalPlayer.LowerBodyYaw + 34.8f;
						testangles.ClampY();
						targetabsangles.y = testangles.y;
						targetabsangles.ClampY();
						LocalPlayer.Entity->SetRenderAngles(testangles);
						LocalPlayer.Entity->SetEyeAngles(testangles);
						LocalPlayer.Entity->InvalidatePhysicsRecursive(ANGLES_CHANGED);
						LocalPlayer.Entity->InvalidateBoneCache();
						LocalPlayer.Entity->UpdateClientSideAnimation();
						LocalPlayer.Entity->SetAbsAngles(targetabsangles);
						LocalPlayer.Entity->SetupBones(nullptr, MAXSTUDIOBONES, BONE_USED_BY_ANYTHING, Interfaces::Globals->curtime);

						//LocalPlayer.Entity->DrawHitboxes(ColorRGBA(255, 0, 0, 255), 0.1f);

						Ray_t ray;
						ray.Init(enemyeyepos, TraceEndPos);
						CTraceFilterPlayersOnlyNoWorld filter;
						filter.AllowTeammates = true;
						filter.pSkip = (IHandleEntity*)AimTarget;
						filter.m_icollisionGroup = COLLISION_GROUP_NONE;
						trace_t tr;
						Interfaces::EngineTrace->TraceRay(ray, MASK_SHOT, &filter, &tr);
						//UTIL_ClipTraceToPlayers(enemyeyepos, TraceEndPos + vForward * 40.0f, MASK_SHOT, &filter, &tr);

						//WorldToScreen(tr.endpos, t);
						//DrawBox(Vector2D(t.x, t.y), 10, 10, 2, 0, 0, 255, 255);

#if 0
						if (tr.m_pEnt != LocalPlayer.Entity)
						{
							ray.Init(enemyeyepos, TraceEndPosLeft);
							Interfaces::EngineTrace->TraceRay(ray, MASK_SHOT, &filter, &tr);
							if (tr.m_pEnt != LocalPlayer.Entity)
							{
								ray.Init(enemyeyepos, TraceEndPosRight);
								Interfaces::EngineTrace->TraceRay(ray, MASK_SHOT, &filter, &tr);
					}
				}
#endif

						bool add34hithead = false;
						if (tr.m_pEnt == LocalPlayer.Entity)
						{
							add34hithead = tr.hitgroup == HITGROUP_HEAD;
							testangles.y = LocalPlayer.LowerBodyYaw - 34.8f;
							testangles.ClampY();
							targetabsangles.y = testangles.y;
							targetabsangles.ClampY();
							LocalPlayer.Entity->SetRenderAngles(testangles);
							LocalPlayer.Entity->SetEyeAngles(testangles);
							LocalPlayer.Entity->InvalidatePhysicsRecursive(ANGLES_CHANGED);
							LocalPlayer.Entity->InvalidateBoneCache();
							LocalPlayer.Entity->UpdateClientSideAnimation();
							LocalPlayer.Entity->SetAbsAngles(targetabsangles);
							LocalPlayer.Entity->SetupBones(nullptr, MAXSTUDIOBONES, BONE_USED_BY_ANYTHING, Interfaces::Globals->curtime);

							//LocalPlayer.Entity->DrawHitboxes(ColorRGBA(0, 255, 0, 255), 0.1f);

							ray.Init(enemyeyepos, TraceEndPos);
							Interfaces::EngineTrace->TraceRay(ray, MASK_SHOT, &filter, &tr);
							//UTIL_ClipTraceToPlayers(enemyeyepos, TraceEndPos + vForward * 40.0f, MASK_SHOT, &filter, &tr);

							//WorldToScreen(tr.endpos, t);
							//DrawBox(Vector2D(t.x, t.y), 10, 10, 2, 255, 255, 0, 255);

#if 0
							if (tr.m_pEnt != LocalPlayer.Entity)
							{
								ray.Init(enemyeyepos, TraceEndPosLeft);
								Interfaces::EngineTrace->TraceRay(ray, MASK_SHOT, &filter, &tr);
								if (tr.m_pEnt != LocalPlayer.Entity)
								{
									ray.Init(enemyeyepos, TraceEndPosRight);
									Interfaces::EngineTrace->TraceRay(ray, MASK_SHOT, &filter, &tr);
						}
			}
#endif

							if (tr.m_pEnt == LocalPlayer.Entity)
							{
								bool sub32hithead = tr.hitgroup == HITGROUP_HEAD;
								bool add = false;
								float yawnotfoundhead = 0.0f;
								bool foundanglethatisnthead = false;
								if (sub32hithead && !add34hithead)
								{
									add = true;
									testangles.y = LocalPlayer.LowerBodyYaw + 34.8f;
									yawnotfoundhead = testangles.y;
									foundanglethatisnthead = true;
								}
								float total = 0.0f;
								while (tr.m_pEnt == LocalPlayer.Entity)
								{
									testangles.y += add ? 17.6f : -17.6f;
									testangles.ClampY();
									total += 17.6f;
									targetabsangles.y = testangles.y;
									targetabsangles.ClampY();
									LocalPlayer.Entity->SetRenderAngles(testangles);
									LocalPlayer.Entity->SetEyeAngles(testangles);

									LocalPlayer.Entity->InvalidatePhysicsRecursive(ANGLES_CHANGED);
									LocalPlayer.Entity->InvalidateBoneCache();
									LocalPlayer.Entity->UpdateClientSideAnimation();
									LocalPlayer.Entity->SetAbsAngles(targetabsangles);
									LocalPlayer.Entity->SetupBones(nullptr, MAXSTUDIOBONES, BONE_USED_BY_ANYTHING, Interfaces::Globals->curtime);

									ray.Init(enemyeyepos, TraceEndPos);
									Interfaces::EngineTrace->TraceRay(ray, MASK_SHOT, &filter, &tr);


#if 0
									if (tr.m_pEnt != LocalPlayer.Entity)
									{
										ray.Init(enemyeyepos, TraceEndPosLeft);
										Interfaces::EngineTrace->TraceRay(ray, MASK_SHOT, &filter, &tr);
										if (tr.m_pEnt != LocalPlayer.Entity)
										{
											ray.Init(enemyeyepos, TraceEndPosRight);
											Interfaces::EngineTrace->TraceRay(ray, MASK_SHOT, &filter, &tr);
								}
							}
#endif

									//UTIL_ClipTraceToPlayers(enemyeyepos, TraceEndPos + vForward * 40.0f, MASK_SHOT, &filter, &tr);
									if (!foundanglethatisnthead && tr.hitgroup != HITGROUP_HEAD)
									{
										yawnotfoundhead = testangles.y;
										foundanglethatisnthead = true;
									}
									if ((total > 360.0f && foundanglethatisnthead) || (!foundanglethatisnthead && total > 720.0f))
										break;
		}

								if (tr.m_pEnt == LocalPlayer.Entity && foundanglethatisnthead)
								{
									testangles.y = yawnotfoundhead;
								}
	}
}
						LocalPlayer.Entity->SetRenderAngles(OriginalRenderAngles);
						LocalPlayer.Entity->SetEyeAngles(OriginalRenderAngles);
						LocalPlayer.Entity->InvalidatePhysicsRecursive(ANGLES_CHANGED);
						LocalPlayer.Entity->InvalidateBoneCache();
						LocalPlayer.Entity->UpdateClientSideAnimation();
						LocalPlayer.Entity->SetAbsAngles(OriginalAbsAngles);
						LocalPlayer.Entity->SetupBones(nullptr, MAXSTUDIOBONES, BONE_USED_BY_ANYTHING, Interfaces::Globals->curtime);
						angles = testangles;
}
					else
					{
						if (jitter < 1)
						{
							angles.y = LocalPlayer.LowerBodyYaw + 34.8f;
							jitter++;
						}
						else
						{
							angles.y = LocalPlayer.LowerBodyYaw - 34.8f;
							jitter = 0;
						}
					}
				}
			}
		}
		else
		{
			//fake angle

			if (LocalPlayer.Entity->GetVelocity().Length() > 0.1f)
			{
				angles.y = LocalPlayer.LowerBodyYaw + 90.0f;
			}
			else
			{
				angles.y = RealViewAngles.y + 180.0f;

				if (AimTarget)
				{
					//Vector ourheadpos = LocalPlayer.Entity->GetEyePosition();;
					//QAngle angletous = CalcAngle(AimTarget->GetEyePosition(), ourheadpos);
					//angles.y = angletous.y + 90.0f;
					static bool add = true;
					static float ramp = 140.0f;
					if (add)
					{
						ramp += 35.0f;
						if (ramp > 220.0f)
							add = !add;
					}
					else
					{
						ramp -= 35.0f;
						if (ramp < 140.0f)
							add = !add;
					}
					angles.y = LocalPlayer.LastRealEyeAngles.y + ramp;
				}
			}
		}
	}
	/*
	else
	{
	if (sendpacket < 2)
	{
	angles.y = LocalPlayer.LowerBodyYaw + 180.0f;;
	CurrentUserCmd.BsendPacket = 0; //SendPacket = false
	sendpacket++;
	}
	else
	{
	angles.y = LocalPlayer.LowerBodyYaw;
	sendpacket = 0;
	}
	}
	*/

	angles.ClampY();

}

void FixMove_AntiAim(QAngle &angles, QAngle LastAngle, QAngle RealViewAngles, CBaseEntity* AimTarget, DWORD* FramePointer)
{
	QAngle TargetAngles = RealViewAngles;

	if (AimTarget)
	{
		Vector LocalEyePos = LocalPlayer.Entity->GetEyePosition();
		Vector TargetEyePos = AimTarget->GetEyePosition();
		TargetAngles = CalcAngle(LocalEyePos, TargetEyePos);
		TargetAngles.x = 89.0f;
		TargetAngles.y += 165.0f;
		TargetAngles.z = 0.0f;

		if (CurrentUserCmd.IsFakeAngle())
		{
			TargetAngles.y += PickRandomAngle(2, -34.5, 34.5);
		}
	}
	else
	{
		TargetAngles.x = 89.0f;
		TargetAngles.y += 180.0f;
	}

	TargetAngles.Clamp();

	QAngle oldangles = RealViewAngles;
	// Now we need to make each viewangle apparent with each Vector using AngleVectors
	Vector viewForward, viewRight, viewUp;
	AngleVectors(oldangles, &viewForward, &viewRight, &viewUp);

	angles = LastAngle;
	QAngle delta_angle;

	if (RageAimStepChk.Checked || LegitAimStepChk.Checked)
	{
		delta_angle = TargetAngles - angles;
		NormalizeAngles(delta_angle);

		if (delta_angle.y < -SpinDegPerTickTxt.flValue)
			angles.y -= SpinDegPerTickTxt.flValue;
		else if (delta_angle.y > SpinDegPerTickTxt.flValue)
			angles.y += SpinDegPerTickTxt.flValue;
		else
			angles.y = TargetAngles.y;
	}
	else
	{
		angles.y = RealViewAngles.y + 180.0f;
	}

	angles.ClampY();
}

void FixMove_AntiAimSkate(QAngle& angles, QAngle LastAngle, QAngle RealViewAngles, DWORD* FramePointer)
{
	angles = RealViewAngles;
#if 1
	if (CurrentUserCmd.IsRealAngle())
	{
		angles.y += 145.0f;
	}
#else
	int choked = CurrentUserCmd.iCmdsChoked;
	if (choked < 2)
		angles.y += 180.0f;
	else if (choked < 4)
		angles.y += 145.0f;
#endif

	angles.ClampY();
}

void FixMove_AntiAimMemester(QAngle& angles, QAngle LastAngle, QAngle RealViewAngles, DWORD* FramePointer)
{
	static float TargetXDegrees = 89.0f;
	static float TargetYDegrees = 91.0f;
	static float LastTargetYDegrees = 91.0f;
	static float TimeSpentAtTargetDegrees = 0.0f;
	static float TimeToWaitAtTargetDegrees = 0.01f;
	QAngle TargetAngles = RealViewAngles;

	TargetAngles.x = TargetXDegrees;
	TargetAngles.y += TargetYDegrees;

	TargetAngles.Clamp();

	angles = LastAngle;
	QAngle delta_angle;

	//bool AimStepEnabled = (RageAimStepChk.Checked || LegitAimStepChk.Checked);

	//if (AimStepEnabled)
	{
		delta_angle = TargetAngles - angles;
		NormalizeAngles(delta_angle);

		if (delta_angle.y < -SpinDegPerTickTxt.flValue)
		{
			TimeSpentAtTargetDegrees = 0.0f;
			angles.y -= SpinDegPerTickTxt.flValue;
		}
		else if (delta_angle.y > SpinDegPerTickTxt.flValue)
		{
			TimeSpentAtTargetDegrees = 0.0f;
			angles.y += SpinDegPerTickTxt.flValue;
		}
		else
		{
			TimeSpentAtTargetDegrees += ReadFloat((uintptr_t)&Interfaces::Globals->frametime);
			angles.y = TargetAngles.y;
			if (TimeSpentAtTargetDegrees >= TimeToWaitAtTargetDegrees)
			{
				if (TargetYDegrees == 180.0f)
				{
					TimeToWaitAtTargetDegrees = SORandomFloat(0.0f, 0.001f);
					if (LastTargetYDegrees == 90.0f)
					{
						TargetYDegrees = 269.0f;
					}
					else
					{
						TargetYDegrees = 91.0f;
					}
				}
				else
				{
					if (LastTargetYDegrees == 91.0f)
					{
						LastTargetYDegrees = TargetYDegrees;
						TargetYDegrees = 269.0f;
					}
					else
					{
						LastTargetYDegrees = TargetYDegrees;
						TargetYDegrees = 91.0f;
					}
				}
				TimeToWaitAtTargetDegrees = SORandomFloat(0.00021f, 0.00852f);
				TimeSpentAtTargetDegrees = 0.0f;
				TargetXDegrees = TargetXDegrees == 89.0f ? 78.2398f : 89.0f;
			}
		}
	}
	//else
	{
	//	angles.y = TargetAngles.y;
	}

	if (AimbotSpeedTxt.flValue == AimbotSpeedTxt.dbMax)
	{
		if (CurrentUserCmd.IsFakeAngle())
		{
			angles.y += PickRandomAngle(5, -95.0f, 95.0f, 110.0f, -110.0f, 50.0f);
		}
	}

	angles.ClampY();	
}

#include "BaseCombatWeapon.h"
#include "INetchannelInfo.h"
bool FixMove_LegitAntiAim(QAngle &angles, int buttons, QAngle LastAngle, QAngle RealViewAngles, DWORD* FramePointer, CBaseCombatWeapon* weapon)
{
	static float AvgFrameTime = 0.0f;
	static std::deque<float>FrameTimes;
	FrameTimes.push_front(Interfaces::Globals->absoluteframetime);
	if (FrameTimes.size() > 3)
		FrameTimes.pop_back();

	float TotalTime = 0.0f;
	for (auto ft : FrameTimes)
		TotalTime += ft;

	AvgFrameTime = TotalTime / FrameTimes.size();

	float MinimumFrameTime = Interfaces::Globals->interval_per_tick - (Interfaces::Globals->interval_per_tick * 0.2f); //frametime must be 20% faster than tick interval
	if (Interfaces::Globals->absoluteframetime > MinimumFrameTime || AvgFrameTime > MinimumFrameTime)
		return false;

	INetChannelInfo *nci = Interfaces::Engine->GetNetChannelInfo();
	if (nci)
	{
		if (nci->GetAvgLoss(FLOW_INCOMING) > 0 || nci->GetAvgLoss(FLOW_OUTGOING) > 0)
			return false;
	}

	//if ((buttons & IN_ATTACK) || (buttons & IN_ATTACK2))
	//	return false;

	if (weapon)
	{
		if (weapon->GetItemDefinitionIndex() == WEAPON_GLOCK && *(int*)((DWORD)weapon + m_iBurstShotsRemaining) > 0)
			return false;

		if (((buttons & IN_ATTACK) || (buttons & IN_ATTACK2)))
		{
			float tickinterval = ReadFloat((uintptr_t)&Interfaces::Globals->interval_per_tick);
			float m_flServerTime = (LocalPlayer.Entity->GetTickBase() * tickinterval);
			float flMaxTime = TICKS_TO_TIME(10);
			if (fabsf(m_flServerTime - weapon->GetNextPrimaryAttack()) < flMaxTime || fabsf(m_flServerTime - weapon->GetNextSecondaryAttack()) < flMaxTime)
			{
				return false;
			}
		}
	}

	if (CurrentUserCmd.IsRealAngle())
	{
		if (CurrentUserCmd.cmd->forwardmove == 0.0f && CurrentUserCmd.cmd->sidemove == 0.0f)
		{
			if ((LocalPlayer.Entity->GetFlags() & FL_DUCKING) && !(CurrentUserCmd.cmd->buttons & IN_USE))
				angles.y += 90.0f;
			else
				angles.y += 180;//145.0f;
		}
		else
		{
			if (CurrentUserCmd.cmd->forwardmove == 0.0f && CurrentUserCmd.cmd->sidemove != 0.0f)
			{
				if (CurrentUserCmd.cmd->sidemove < 0.0f)
					angles.y -= 90.0f;
				else
					angles.y += 90.0f;
			}
			else if (CurrentUserCmd.cmd->forwardmove != 0.0f && CurrentUserCmd.cmd->sidemove == 0.0f)
			{
				if (LocalPlayer.Entity->GetFlags() & FL_DUCKING)
					angles.y += 90.0f;
				else
				{
					angles.y += 75.0f;
				}
			}
			else
			{
				//forwardmove != 0 && sidemove != 0
				if (LocalPlayer.Entity->GetFlags() & FL_DUCKING)
					angles.y += 90.0f;
				else
					angles.y -= 45.0f;
			}
#if 0
			if (forwardmove != 0.0f && sidemove != 0.0f)
			{
				angles.y -= 45.0f;
			}
			else if (forwardmove != 0.0f)
			{
				angles.y += 75.0f;
			}
			else
			{
				angles.y += 80.0f;
			}
#endif
		}
		// += 25.0f;
	}

	angles.Clamp();
	
	return CurrentUserCmd.IsRealAngle();
}

enum ManualAA : int
{
	LEFT = 0,
	NONE,
	RIGHT,
	BACK
};

bool FixMove_ManualAntiAim(QAngle &angles, QAngle RealViewAngles, DWORD* FramePointer)
{
	CurrentUserCmd.SetIsFakeAngle(CurrentUserCmd.bSendPacket);

	static ManualAA AntiAimMode = NONE;

	int Key = VK_LEFT;
	for (int i = 0; i < 4; i++)
	{
		if (GetAsyncKeyState(Key) & (1 << 16))
		{
			AntiAimMode = (ManualAA)i;
			if (i == NONE)
				return false;
			break;
		}
		Key++;
	}

	if (AntiAimMode == NONE)
		return false;

	bool Fake = CurrentUserCmd.IsFakeAngle();

	switch (AntiAimMode)
	{
	case LEFT:
		if (Fake)
		{
			angles.y -= 90.0f;
			break;
		}
		angles.y += 90.0f;
		break;
	case RIGHT:
		if (Fake)
		{
			angles.y += 90.0f;
			break;
		}
		angles.y -= 90.0f;
		break;
	case BACK:
		if (Fake)
		{
			angles.y += 90.0f;
			break;
		}
		angles.y += 180.0f;
		break;
	}

	angles.Clamp();

	return true;
}

void FixMove_SideAntiAim(QAngle &angles, QAngle LastAngle, QAngle RealViewAngles, CBaseEntity* AimTarget, DWORD* FramePointer)
{
	bool LocalPlayerIsMoving = (LocalPlayer.Entity->GetVelocity().Length() > 100.0f);
	bool StaticFakeAngle = true;
	static int scount2 = 0;

	if (NumStreamedEnemies != 0)
	{
		for (int i = 0; i < NumStreamedPlayers; i++)
		{
			CustomPlayer* pCPlayer = StreamedPlayers[i];
			if (pCPlayer->IsTarget && pCPlayer->Personalize.ShouldResolve())
			{
				StaticFakeAngle = false;
				break;
			}
		}
	}

	if (StaticFakeAngle)
	{
		if (AimTarget)
			angles = CalcAngle(LocalPlayer.Entity->GetEyePosition(), AimTarget->GetEyePosition()); //Clamped in function
		else
			angles = RealViewAngles;


		if (CurrentUserCmd.IsRealAngle())
			angles.y += 90.0f;
		else
			angles.y -= 90.0f;
	}
	else
	{
		if (CurrentUserCmd.IsRealAngle())
		{
			angles.y = /*RealViewAngles.y + */LocalPlayer.LowerBodyYaw + SORandomFloat(65.0f, 90.0f); //90.0f
			ClampY(angles.y);
			float dt = angles.y - LocalPlayer.LowerBodyYaw;
			ClampY(dt);
			if (fabsf(dt) < 50.0f)
			{
				angles.y += 50.0f;
			}
		}
		else
		{
			if (scount2 == 0)
			{
				scount2++;
				angles.y = RealViewAngles.y + 100.0f;
			}
			else
			{
				scount2 = 0;
				angles.y = RealViewAngles.y - 100.0f;
			}
		}
	}

	angles.Clamp();
}

void FixMove_Testing(QAngle &angles, QAngle LastAngle, QAngle RealViewAngles, CBaseEntity* AimTarget, DWORD* FramePointer)
{
	if (CurrentUserCmd.IsRealAngle())
		angles.y = LocalPlayer.LowerBodyYaw + 180.0f;
	else
	{
		if (fabsf(angles.y - LocalPlayer.LowerBodyYaw) < 10.0f)
			angles.y += SORandomFloat(10.0f, 35.0f);
	}

	angles.Clamp();
}

void FixMove_Testing2(QAngle &angles, QAngle LastAngle, QAngle RealViewAngles, CBaseEntity* AimTarget, DWORD* FramePointer)
{
	if (CurrentUserCmd.IsRealAngle())
	{
		angles.y = SORandomFloat(-180.0f, 180.0f);
		if (fabsf(angles.y - LocalPlayer.LowerBodyYaw) < 10.0f)
			angles.y += SORandomFloat(10.0f, 35.0f);
	}

	angles.Clamp();
}

#if 0
void FixMove_SideAntiAim(QAngle &angles, QAngle LastAngle, QAngle RealViewAngles, CBaseEntity* AimTarget, DWORD* FramePointer)
{
	bool LocalPlayerIsMoving = (LocalPlayer.Entity->GetVelocity().Length() > 100.0f);
	float forwardmove = ReadFloat((intptr_t)&);
	float sidemove = ReadFloat((intptr_t)&);
	float upmove = ReadFloat((intptr_t)&);

	static int scount = 0;
	static int scount2 = 0;
	bool StaticFakeAngle = true;
	if (NumStreamedEnemies != 0)
	{
		for (int i = 0; i < NumStreamedPlayers; i++)
		{
			CustomPlayer* pCPlayer = StreamedPlayers[i];
			if (pCPlayer->IsTarget && (pCPlayer->Personalize.ResolvePitch || pCPlayer->Personalize.ResolveYaw))
			{
				StaticFakeAngle = false;
				break;
			}
		}
	}

	int chokelimit;

	if ((ReadInt((uintptr_t)&CurrentUserCmd.cmd->buttons) & IN_ATTACK2))
		chokelimit = 1;
	else if (LocalPlayerIsMoving)
	{
		if (ReadFloat((uintptr_t)&Interfaces::Globals->absoluteframetime) > ReadFloat((uintptr_t)&Interfaces::Globals->interval_per_tick))
			chokelimit = 1;
		else
			chokelimit = 1;
	}
	else
	{
		chokelimit = 1;
	}

	if (StaticFakeAngle)
	{
		if (AimTarget)
			angles = CalcAngle(LocalPlayer.Entity->GetEyePosition(), AimTarget->GetEyePosition()); //Clamped in function
		else
			angles = RealViewAngles;

		if (scount < chokelimit)
		{
			CurrentUserCmd.BsendPacket = 0; //SendPacket = false
			angles.y += 90.0f;
			scount++;
		}
		else
		{
			angles.y -= 90.0f;
			scount = 0;
		}
	}
	else
	{
		static bool swtch = false;
		bool onground = (LocalPlayer.Entity->GetFlags() & FL_ONGROUND) ? 1 : 0;
		static bool fake = false;
		if (swtch)
		{
			if (!onground)
			{
				if (fake)
				{

					static int jitter = 5;
					if (jitter > 10)
						jitter = -10;
					else
						jitter += 5;

					int forviews = 0;
					static int counter = 0;
					static int stage = 0;
					int switchval = stage % 4;
					switch (switchval)
					{
					case 0: forviews = 45; break;
					case 1: forviews = 135; break;
					case 2: forviews = -45; break;
					case 3: forviews = -135; break;
					}

					forviews += jitter;
					counter++;
					if (counter % 50 == 1)
						stage++;
					if (buttons & IN_ATTACK)
						counter++;
					angles.y += forviews;
				}
				else
				{

					static int jitter = 5;
					if (jitter > 10)
						jitter = -10;
					else
						jitter += 5;

					if (buttons & IN_ATTACK)
					{
						angles.y += 180 + jitter * 2;
					}
					else
					{
						float additionval = LastChoke.y;
						if (!Hacks.SendPacket)
							additionval = LastSend.y;


						if (additionval < 0)
							views.y += 90 + jitter;
						else
							views.y -= 90 + jitter;

					}

				}

			}
			else
			{
				if (fake)
				{
					views.y -= 90 + jitterval;
				}
				else
				{


					if (Hacks.CurrentCmd->buttons & IN_ATTACK)
					{
						views.y += 90;
					}
					else
						views.y += 180;


				}

			}
		}

		else
		{


			if (!onground)
			{
				if (fake)
				{

					int addvalue = rand() % 40 - 20;
					static bool left = true;
					static bool first = true;
					static int counter = 1;
					if (Hacks.CurrentCmd->buttons & IN_ATTACK)
					{
						if (first)
							left = !left;
						first = false;
						if (counter % 30 == 0)
							left = !left;
					}
					else
						first = true;
					if (counter % 64 == 0)
						left = !left;

					counter++;


					if (left)
						views.y -= 90 + addvalue;
					else
						views.y += 90 + addvalue;
				}
				else
				{

					views.y += 180;

				}

			}
			else
			{
				if (fake)
				{
					views.y -= 90 + jitterval;
				}
				else
				{


					if (Hacks.CurrentCmd->buttons & IN_ATTACK)
					{
						views.y += 90;
					}
					else
						views.y += 180;


				}

			}




		}
		if (ondeath)
			Lisp(views.y);








	}

	angles.Clamp();

	QAngle oldangles = RealViewAngles;
	old = 0.0f;
	// Now we need to make each viewangle apparent with each Vector using AngleVectors
	Vector viewForward, viewRight, viewUp;
	AngleVectors(oldangles, &viewForward, &viewRight, &viewUp);

	old = ;
	 = 0.0f;

	//Now that we have our modified angles, the engine isn't going to cooperate with our new movements so we need to fix the angles and rotation like so:
	Vector aimUp, aimForward, aimRight;
	AngleVectors(angles, &aimForward, &aimRight, &aimUp);

	 = old;

	//Now we can normalize our current angles so they play nicely with the new ones when we do our dot products
	VectorNormalizeFast(viewForward);
	VectorNormalizeFast(viewRight);
	VectorNormalizeFast(viewUp);

	VectorNormalizeFast(aimForward);
	VectorNormalizeFast(aimRight);
	VectorNormalizeFast(aimUp);

	float newforward, newright;
	newforward = DotProduct(viewForward * forwardmove, aimForward) + DotProduct(viewRight * sidemove, aimForward) + DotProduct(viewUp * upmove, aimForward);
	newright = DotProduct(viewForward * forwardmove, aimRight) + DotProduct(viewRight * sidemove, aimRight) + DotProduct(viewUp * upmove, aimRight);
	//Now we do our dot products to fix the previous movement factors we stored with the applied antiaim/angle manipulation:
	WriteFloat((uintptr_t)&, clamp(newforward, -450.0f, 450.0f));
	WriteFloat((uintptr_t)&, clamp(newright, -450.0f, 450.0f));
}
#endif


bool EdgeAntiAim(QAngle &angles, QAngle *RealViewAngles, float flWall, float flCornor, DWORD* FramePointer)
{
	CurrentUserCmd.SetIsFakeAngle(CurrentUserCmd.bSendPacket);

	//I really hate your edge anti aim will rewrite when I get onto desktop,... It works some times but all and all is bad... 
	//Nothing personal ofc Xoxoxox
	QAngle prevangles = angles;
	CTraceFilterNoPlayers filter;
	filter.pSkip = (IHandleEntity*)LocalPlayer.Entity;
	filter.m_icollisionGroup = COLLISION_GROUP_NONE;

	Vector EyePosition = LocalPlayer.Entity->GetEyePosition();
	static bool faked = false;
	bool Edged = false;
	bool FoundCorner = false;

	for (int y = 0; y < 360; y++)
	{
		QAngle tmp(10.0f, angles.y, 0.0f);
		tmp.y += y;
		ClampY(tmp.y);

		Vector forward;
		AngleVectors(tmp, &forward);

		VectorNormalizeFast(forward);

		float length = ((16.0f + 3.0f) + ((16.0f + 3.0f) * sin(DEG2RAD(10.0f)))) + 7.0f;
		forward *= length;

		Ray_t ray;
		trace_t traceData;

		ray.Init(EyePosition, (EyePosition + forward));
		Interfaces::EngineTrace->TraceRay(ray, 0x200400B, (CTraceFilter*)&filter, &traceData);

		if (traceData.fraction != 1.0f)
		{
			QAngle newangles;
			Vector Negate = traceData.plane.normal;

			Negate *= -1;
			VectorAngles(Negate, newangles);

			tmp.y = newangles.y;

			ClampY(tmp.y);

			trace_t leftTrace, rightTrace;

			Vector left, right;
			AngleVectors(tmp + QAngle(0.0f, 30.0f, 0.0f), &left);
			AngleVectors(tmp - QAngle(0.0f, 30.0f, 0.0f), &right);

			left *= (length + (length * sin(DEG2RAD(30.0f))));
			right *= (length + (length * sin(DEG2RAD(30.0f))));

			ray.Init(EyePosition, (EyePosition + left));
			Interfaces::EngineTrace->TraceRay(ray, 0x200400B, (CTraceFilter*)&filter, &leftTrace);

			ray.Init(EyePosition, (EyePosition + right));
			Interfaces::EngineTrace->TraceRay(ray, 0x200400B, (CTraceFilter*)&filter, &rightTrace);

			if ((leftTrace.fraction == 1.0f)
				&& (rightTrace.fraction != 1.0f))
			{
				tmp.y -= flCornor;
				// LEFT
				FoundCorner = true;
				Edged = true;
			}
			else if ((leftTrace.fraction != 1.0f)
				&& (rightTrace.fraction == 1.0f))
			{
				tmp.y += flCornor;
				// RIGHT
				FoundCorner = true;
				Edged = true;
			}
			//Edged = true;
			angles.y = tmp.y;
			angles.y -= flWall;
		}
	}

	if (FoundCorner)
	{
		angles.Clamp();
	}
	else
	{
		angles = prevangles;
	}
	return Edged;
}

void FixMove_SilentAim(QAngle angles, QAngle RealViewAngles)
{
	//Correct movement for autoaim so we dont move towards the target.
	//First, store your current angles and movement factors before doing any kind of anti-aim/spinbot/jitterbot/memebot
	float forwardmove = ReadFloat((uintptr_t)&CurrentUserCmd.cmd->forwardmove);
	float sidemove = ReadFloat((uintptr_t)&CurrentUserCmd.cmd->sidemove);
	float upmove = ReadFloat((uintptr_t)&CurrentUserCmd.cmd->upmove);

	QAngle oldangles = RealViewAngles;
	oldangles.x = 0.0f;
	// Now we need to make each viewangle apparent with each Vector using AngleVectors
	Vector viewForward, viewRight, viewUp;
	AngleVectors(oldangles, &viewForward, &viewRight, &viewUp);

	oldangles.x = angles.x;
	angles.x = 0.0f;

	//Now that we have our modified angles, the engine isn't going to cooperate with our new movements so we need to fix the angles and rotation like so:
	Vector aimUp, aimForward, aimRight;
	AngleVectors(angles, &aimForward, &aimRight, &aimUp);

	angles.x = oldangles.x;

	//Now we can normalize our current angles so they play nicely with the new ones when we do our dot products
	VectorNormalizeFast(viewForward);
	VectorNormalizeFast(viewRight);
	VectorNormalizeFast(viewUp);

	VectorNormalizeFast(aimForward);
	VectorNormalizeFast(aimRight);
	VectorNormalizeFast(aimUp);

	float newforward, newright;
	newforward = DotProduct(viewForward * forwardmove, aimForward) + DotProduct(viewRight * sidemove, aimForward) + DotProduct(viewUp * upmove, aimForward);
	newright = DotProduct(viewForward * forwardmove, aimRight) + DotProduct(viewRight * sidemove, aimRight) + DotProduct(viewUp * upmove, aimRight);
	WriteFloat((uintptr_t)&CurrentUserCmd.cmd->forwardmove, clamp(newforward, -450.0f, 450.0f));
	WriteFloat((uintptr_t)&CurrentUserCmd.cmd->sidemove, clamp(newright, -450.0f, 450.0f));
}
