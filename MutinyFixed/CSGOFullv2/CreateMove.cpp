#include "CreateMove.h"
#include "CSGO_HX.h"
#include "Overlay.h"
#include "FakeLag.h"
#include "Aimbot.h"
#include "NoRecoil.h"
#include "pSilent.h"
#include "AutoWall.h"
#include "BunnyHop.h"
#include "FixMove.h"
#define NO_SPREAD
#ifdef NO_SPREAD
#include "NoSpread.h"
#endif
#include "KnifeBot.h"
#include "Triggerbot.h"
#include "ConVar.h"
#include "Netchan.h"
#include "VTHook.h"
#include "RunCommand.h"
#include "Events.h"
#include "player_lagcompensation.h"
#include "GrenadePrediction.h"
#include "AutoGrief.h"
#include "ThirdPerson.h"
#include "LocalPlayer.h"
#include <iostream>
#include "NameChanger.h"
#include "VMProtectDefs.h"
#include "SetClanTag.h"
#include "ESP.h"
#include "LegitBot.h"

#ifdef LICENSED
extern float ValidLicense2;
#endif

CreateMoveFn oCreateMove;
bool IsReadyForFiring = false;
ModifiableUserCmd CurrentUserCmd;
float TimeSinceWeaponChange = 0.0f;
class C_CSPlayer;
bool WaitForRePress = false; //Wait until we let go of LMB and click again
bool SetLastAngleFirstTime = false;
int buttons;
//SpoofedConvar *cl_interp_spoofed = nullptr;
//SpoofedConvar *cl_interp_ratio_spoofed = nullptr;
//SpoofedConvar *cl_extrapolate_spoofed = nullptr;
//ConVar* cl_extrapolate = nullptr;
//MinspecCvar *cl_interp;
//MinspecCvar *cl_interp_ratio;
//ConVar *cl_interp;
ConVar* m_pitch = nullptr;
ConVar* m_yaw = nullptr;
ConVar* sensitivity = nullptr;
ConVar *engine_no_focus_sleep = nullptr;

char *mpitchstr = new char[8]{ 23, 37, 10, 19, 14, 25, 18, 0 }; /*m_pitch*/
char *myawstr = new char[6]{ 23, 37, 3, 27, 13, 0 }; /*m_yaw*/
char *sensstr = new char[12]{ 9, 31, 20, 9, 19, 14, 19, 12, 19, 14, 3, 0 }; /*sensitivity*/
char *nofocusstr = new char[22]{ 31, 20, 29, 19, 20, 31, 37, 20, 21, 37, 28, 21, 25, 15, 9, 37, 9, 22, 31, 31, 10, 0 }; /*engine_no_focus_sleep*/

																														//char *cl_interpstr = new char[10]{ 25, 22, 37, 19, 20, 14, 31, 8, 10, 0 }; /*cl_interp*/
																														//char *cl_interpolatestr = new char[15]{ 25, 22, 37, 19, 20, 14, 31, 8, 10, 21, 22, 27, 14, 31, 0 }; /*cl_interpolate*/
																														//char *cl_extrapolatestr = new char[15]{ 25, 22, 37, 31, 2, 14, 8, 27, 10, 21, 22, 27, 14, 31, 0 }; /*cl_extrapolate*/
																														//char *n_cl_interp_ratiostr = new char[18]{ 20, 37, 25, 22, 37, 19, 20, 14, 31, 8, 10, 37, 8, 27, 14, 19, 21, 0 }; /*n_cl_interp_ratio*/
int choked = 0;

void Get_CreateMove_ConVars()
{
#if 0
	DecStr(cl_interp_ratiostr);
	DecStr(n_cl_interp_ratiostr);
	DecStr(cl_extrapolatestr);
	cl_interp = new MinspecCvar("cl_interp", "test_r", 0.0f);
	cl_interp_ratio = new MinspecCvar("cl_interp_ratio", "test_", 0.0f); //new MinspecCvar(cl_interp_ratiostr, n_cl_interp_ratiostr, 0.0f);
	cl_extrapolate = Interfaces::Cvar->FindVar(cl_extrapolatestr);
	cl_extrapolate_spoofed = new SpoofedConvar(cl_extrapolate);
	EncStr(cl_interp_ratiostr);
	EncStr(n_cl_interp_ratiostr);
	EncStr(cl_extrapolatestr);
#endif
	DecStr(mpitchstr, 7);
	m_pitch = Interfaces::Cvar->FindVar(mpitchstr);
	EncStr(mpitchstr, 7);
	DecStr(myawstr, 5);
	m_yaw = Interfaces::Cvar->FindVar(myawstr);
	EncStr(myawstr, 5);
	DecStr(sensstr, 11);
	sensitivity = Interfaces::Cvar->FindVar(sensstr);
	EncStr(sensstr, 11);
	DecStr(nofocusstr, 21);
	engine_no_focus_sleep = Interfaces::Cvar->FindVar(nofocusstr);
	EncStr(nofocusstr, 21);
	engine_no_focus_sleep->SetValue(0.0f);
	gLagCompensation.GetLagCompensationConVars();
}

char *FreeEntityBaselinesstr = new char[59]{ 79, 79, 90, 90, 66, 56, 90, 90, 63, 57, 90, 90, 66, 73, 90, 90, 63, 57, 90, 90, 74, 66, 90, 90, 79, 73, 90, 90, 79, 76, 90, 90, 79, 77, 90, 90, 66, 62, 90, 90, 56, 67, 90, 90, 63, 57, 90, 90, 74, 73, 90, 90, 74, 74, 90, 90, 74, 74, 0 }; /*55  8B  EC  83  EC  08  53  56  57  8D  B9  EC  03  00  00*/
using FreeEntityBaselinesFn = void(__thiscall *)(void*);
FreeEntityBaselinesFn FreeEntityBaselines = nullptr;
int choke = 0;

Vector2D RecoilCrosshairScreen = Vector2D(0.0f, 0.0f);
int iDamageGivenToTarget = 0;
int iLastHitgroupHit = 0;
int iLastHealthTarget = 0;
CBaseEntity* LastPlayerDamageGiven = nullptr;
int iHitsToTarget = 0;
float flTimeDamageWasGiven = 0.0f;

void WriteEyeAngles(CUserCmd *cmd)
{
	VMP_BEGINMUTILATION("WEA")
	auto stat = START_PROFILING("WriteEyeAngles");
	LocalPlayer.TargetEyeAngles.ClampY();

	if (AntiAimPitchDrop.index < AntiAimPitches::PITCH_LISP1)
	{
		LocalPlayer.TargetEyeAngles.ClampX();
		float pitch = m_pitch->GetFloat();
		float yaw = m_yaw->GetFloat();

		if (!FaceItChk.Checked)
		{
			QAngle delta = LocalPlayer.TargetEyeAngles - LocalPlayer.LastEyeAngles;
			delta.Clamp();
			short mousedx = (short)((LocalPlayer.LastEyeAngles.y - delta.y) / yaw);
			short mousedy = (short)((LocalPlayer.LastEyeAngles.x - delta.x) / pitch);
			mousedx = clamp(mousedx, SHRT_MIN, SHRT_MAX);
			mousedy = clamp(mousedy, SHRT_MIN, SHRT_MAX);
			WriteShort((intptr_t)&CurrentUserCmd.cmd->mousedx, mousedx);
			WriteShort((intptr_t)&CurrentUserCmd.cmd->mousedy, mousedy);
		}
		else
		{
			float mousesensitivity = sensitivity->GetFloat();

			Vector AimPixels = AnglesToPixels(LocalPlayer.LastEyeAngles, LocalPlayer.TargetEyeAngles, mousesensitivity, pitch, yaw);

			AimPixels.x = round(AimPixels.x / mousesensitivity) * mousesensitivity;
			AimPixels.y = round(AimPixels.y / mousesensitivity) * mousesensitivity;
			QAngle delta_angle = PixelsDeltaToAnglesDelta(AimPixels, mousesensitivity, pitch, yaw);

			LocalPlayer.TargetEyeAngles = LocalPlayer.LastEyeAngles + delta_angle;
			LocalPlayer.TargetEyeAngles.Clamp();

			Vector Pixels = AnglesToPixels(LocalPlayer.LastEyeAngles, LocalPlayer.TargetEyeAngles, mousesensitivity, pitch, yaw);

			WriteShort((uintptr_t)&cmd->mousedx, Pixels.x);
			WriteShort((uintptr_t)&cmd->mousedy, Pixels.y);
		}

		if (!isnan(LocalPlayer.TargetEyeAngles.x) && !isnan(LocalPlayer.TargetEyeAngles.y) && !isnan(LocalPlayer.TargetEyeAngles.z))
		{
			WriteAngle((uintptr_t)&cmd->viewangles, LocalPlayer.TargetEyeAngles);
		}
		else
		{
			THROW_ERROR(ERR_INVALID_VIEWANGLES);
			exit(EXIT_SUCCESS);
		}
		//End of faceit aimbot detection prevention
	}
	else
	{
		WriteAngle((uintptr_t)&cmd->viewangles, LocalPlayer.TargetEyeAngles);
	}
	END_PROFILING(stat);
	VMP_END
}

//Returns true if we should exit
bool PreCreateMove(float flInputSampleTime, CUserCmd* cmd, bool& returnvalue)
{
	auto stat = START_PROFILING("PreCreateMove");
	if (!LocalPlayer.Entity)
	{
		ClearAllPlayers();
		returnvalue = oCreateMove(flInputSampleTime, cmd); //true
		END_PROFILING(stat);
		return true;
	}

	DWORD ClientState = (DWORD)ReadInt(pClientState);
	//if (!ClientState || Interfaces::Engine->IsPlayingDemo() || IsHLTV(ClientState) /*|| IsPaused(ClientState)*/)
	if (!ClientState || Interfaces::Engine->IsPlayingDemo())
	{
		ClearAllPlayers();
		returnvalue = oCreateMove(flInputSampleTime, cmd); //true
		END_PROFILING(stat);
		return true;
	}

	if (!Interfaces::Engine->IsInGame() || !Interfaces::Engine->IsConnected())
	{
		SetLastAngleFirstTime = false;
		LastTargetIndex = INVALID_PLAYER;
		if (!m_pitch || !m_yaw || !sensitivity)
		{
			THROW_ERROR(ERR_CANT_FIND_M_PITCH_M_YAW_SENSITIVITY);
			exit(EXIT_SUCCESS);
		}
		ClearAllPlayers();
		returnvalue = oCreateMove(flInputSampleTime, cmd); //false
		END_PROFILING(stat);
		return true;
	}

	if (ReadInt((uintptr_t)&cmd->command_number) == 0)
	{
		returnvalue = oCreateMove(flInputSampleTime, cmd); //true
		END_PROFILING(stat);
		return true;
	}
	END_PROFILING(stat);
	return false;
}

bool __stdcall Hooks::CreateMove(float flInputSampleTime, CUserCmd* cmd)
{
#ifdef PRINT_FUNCTION_NAMES
	AllocateConsole();
	printf("BeginCreateMove\n");
#endif
#ifdef _DEBUG
	//Dump class ID's in debug mode
	static ClassIDDumper* dumper = new ClassIDDumper;
	if (!dumper->HasDumpedClassIDs())
		dumper->DumpClassIDs();
#endif

	LocalPlayer.Get(&LocalPlayer);
	LocalPlayer.IsAlive = LocalPlayer.Entity->GetAlive() && LocalPlayer.Entity->GetHealth() > 0;

	gLagCompensation.UpdateIncomingSequences();

	bool oValue;
	if (PreCreateMove(flInputSampleTime, cmd, oValue))
	{
		return oValue;
	}

	auto stat = START_PROFILING("CreateMove");

	Interfaces::MDLCache->EndLock();

	if (!sensitivity)
	{
		Get_CreateMove_ConVars();
	}

	CurrentUserCmd.Reset(cmd);

	DWORD* FramePointer;
	__asm mov FramePointer, ebp

	Interfaces::Engine->SetViewAngles(cmd->viewangles);

#ifndef LICENSED
#ifndef MUTINY
	RunNameChanger();
#endif
#endif

	LocalPlayer.AimPunchSubtracted = angZero;
	LocalPlayer.AimbotAimPunchSubtracted = angZero;
	gTriggerbot.SetAllowedToFire(false);

	if (!DisableAllChk.Checked)
	{
		if (!LegitBotChk.Checked && AimbotChk.Checked && (GetAsyncKeyState(pSilentKeyTxt.iValue) & (1 << 16)))
		{
			//Requested feature: hold a separate key to enable pSilent temporarily. Todo: put this in some struct or something cleaner
			gpSilent.PressingpSilentKey = TRUE;
			gpSilent.Push_pSilent();
		}
		else
			gpSilent.PressingpSilentKey = FALSE;

		int flags = LocalPlayer.Entity->GetFlags();

		LocalPlayer.UpdateCurrentWeapon();

		//Prediction
		bool bPredict = false;

#ifdef _DEBUG
		LocalPlayer.DrawUserCmdInfo(cmd);
#endif

		if (EnginePredictChk.Checked && LocalPlayer.IsAlive)
		{
			//Engine Prediction
			bPredict = true;
			LocalPlayer.BeginEnginePrediction(cmd);
		}

		ReadAngle((uintptr_t)&cmd->viewangles, LocalPlayer.CurrentEyeAngles);
		LocalPlayer.TargetEyeAngles = LocalPlayer.CurrentEyeAngles;

		if (!SetLastAngleFirstTime)
		{
			SetLastAngleFirstTime = true;
			LocalPlayer.LastEyeAngles = LocalPlayer.CurrentEyeAngles;
		}

		buttons = ReadInt((uintptr_t)&cmd->buttons);
		FakeLag(buttons, (uintptr_t)FramePointer);

		int RealButtons = buttons; //Actual buttons player is pressing from engine
		bool RanAntiAim = false; //Did we run anti-aim this tick
		bool DontSetViewAngles = true;
		bool LegitAAed = false; //Shit bool for now
		bool AimingAtTarget = false; //Is the aimbot aiming directly at the target
		bool StrafeModifiedAngles = false; //Did autostrafe modify player's view angles
		bool AimbotModifiedAngles = false; //Did the aimbot modify player's view angles
		bool RanHitChanceThisTick = false; //Did we run hitchance during this tick?
		QAngle StrafeAngles = LocalPlayer.CurrentEyeAngles; //Eye angles that autostrafe set
		int PressingAimbotKey = (GetAsyncKeyState(AimbotKeyTxt.iValue) & (1 << 16));
		int PressingTriggerbotKey = (GetAsyncKeyState(TriggerbotKeyTxt.iValue) & (1 << 16));
		

		//If CSGO is the active window..
		//HWND wnd = GetForegroundWindow();
		//if (wnd == tWnd || wnd == hWnd || Time - TimeSinceWeaponChange < RECOIL_GRACE_PERIOD)
		{
			float BestDamage = 0.0f; //Best damage we can give to the target, currently useless

			LocalPlayer.PressingJumpButton = (buttons & IN_JUMP);

			if (LocalPlayer.IsAlive)
			{
				RunBunnyHopping(cmd, flags, StrafeModifiedAngles, StrafeAngles);
			}
#if 0
			QAngle OriginalAngs = cmd->viewangles;
			Vector Move = Vector(cmd->forwardmove, cmd->sidemove,
				cmd->upmove);
#endif

			TEAMS LocalTeam = (TEAMS)LocalPlayer.Entity->GetTeam();

			gLagCompensation.CM_BacktrackPlayers();

			if (AimESPChk.Checked)
				RunAimESP();

			//AIMBOT


			if (AimbotChk.Checked)
			{
				if (LegitBotChk.Checked)
				{
					//NEW LEGIT BOT
					CUserCmd tmpcmd = *cmd;
					AimingAtTarget = false;
					aimbot.Think(LocalPlayer.Entity, LocalPlayer.CurrentWeapon, LocalPlayer.CurrentWeapon ? LocalPlayer.CurrentWeapon->GetCSWpnData() : nullptr, &tmpcmd, AimbotModifiedAngles);
					if (AutoStopMovementChk.Checked && gTriggerbot.WeaponCanFire(LocalPlayer.CurrentWeapon))
					{
						int id = LocalPlayer.CurrentWeapon->GetItemDefinitionIndex();
						if (!LocalPlayer.CurrentWeapon->IsKnife(id) && id != WEAPON_C4 && !LocalPlayer.CurrentWeapon->IsGrenade(id))
							LocalPlayer.StopPlayerMovement(cmd, buttons);
					}
					if (LocalPlayer.CurrentWeapon && AutoScopeSnipersChk.Checked && GetZoomLevel(LocalPlayer.Entity) == 0 && LocalPlayer.CurrentWeapon->IsSniper(true, NULL))
					{
						AimbotModifiedAngles = false;
						WriteAngle((uintptr_t)&cmd->viewangles, LocalPlayer.CurrentEyeAngles);
						buttons |= IN_ATTACK2;
					}
					else
					{
						if (AimbotModifiedAngles)
						{
							DontSetViewAngles = false;
							LocalPlayer.TargetEyeAngles = tmpcmd.viewangles;
						}
					}
				}
				else
				{
					//OLD LEGIT BOT AND RAGE BOT
					static bool AlreadyResetValues = false;
					if (LocalPlayer.IsAlive && (!AimbotHoldKeyChk.Checked || PressingAimbotKey || gpSilent.PressingpSilentKey))
					{
						//Pressing aimbot key or auto aiming
						AlreadyResetValues = false;

						AimingAtTarget = Aimbot(LocalPlayer.TargetEyeAngles, FramePointer, buttons, AimbotModifiedAngles, LocalPlayer.LastEyeAngles, LocalPlayer.CurrentWeapon, PressingTriggerbotKey, RanHitChanceThisTick);

						if (AimbotModifiedAngles)
						{

							if (AutoStopMovementChk.Checked && gTriggerbot.WeaponCanFire(LocalPlayer.CurrentWeapon))
							{
								int id = LocalPlayer.CurrentWeapon->GetItemDefinitionIndex();
								if (!LocalPlayer.CurrentWeapon->IsKnife(id) && id != WEAPON_C4 && !LocalPlayer.CurrentWeapon->IsGrenade(id))
									LocalPlayer.StopPlayerMovement(cmd, buttons);
							}
							if (LocalPlayer.CurrentWeapon && AutoScopeSnipersChk.Checked && GetZoomLevel(LocalPlayer.Entity) == 0 && LocalPlayer.CurrentWeapon->IsSniper(true, NULL))
							{
								AimbotModifiedAngles = false;
								WriteAngle((uintptr_t)&cmd->viewangles, LocalPlayer.CurrentEyeAngles);
								buttons |= IN_ATTACK2;
							}
							else
							{
								DontSetViewAngles = false;
							}
						}
					}
					else
					{
						//Not aimlocking, reset values
						if (!AlreadyResetValues)
						{
							//Avoid calling expensive function SetHitboxGroup every frame
							WaitForRePress = false;
							LastBoneCheckIndex = 0;
							LastBoneChecked = BONES_TO_CHECK[0];
							LastCheckedIndex = 0; //Reset tick based counters
							IndexOfClosest = INVALID_PLAYER;
							LastTargetIndex = INVALID_PLAYER; //reset master 'current target'
							MTargetting.SetHitboxGroup();
							MTargetting.SetHitChanceFlags();
							AlreadyResetValues = true;
						}
					}
				}
			}

			DecayPlayerIsShot();

			//NO RECOIL
			if (!LegitBotChk.Checked && LocalPlayer.IsAlive && !AimbotModifiedAngles && IsAllowedToNoRecoil(LocalPlayer.CurrentWeapon))
			{
				RemoveRecoil(LocalPlayer.TargetEyeAngles);

#ifdef SPARK_SCRIPT_TESTING
				SparkDebugNoRecoil();
#endif
				//if (NoAntiAimsAreOn(false))
				//	DontSetViewAngles = false;
			}

			//TRIGGER BOT

			if (LocalPlayer.CurrentWeapon && LocalPlayer.IsAlive && (Time >= gTriggerbot.NextTimeTriggerBot || LocalPlayer.WeaponVars.IsRevolver))
			{
				if (LocalPlayer.WeaponVars.IsRevolver)
					gTriggerbot.AutoCockRevolver(buttons, LocalPlayer.CurrentWeapon);

				if (PressingTriggerbotKey || AutoFireChk.Checked)
				{
					//If we are locked onto a target and are aiming directly at them
					if (AimingAtTarget)
					{
						//Aimbot says to fire, so fire when ready!
						if (TriggerbotWhileAimbottingChk.Checked)
						{
							//DO NOT FIRE IF SOMETHING TELLS US NOT TO!
							if (gTriggerbot.IsAllowedToFire())
							{
								CBaseEntity* pTargetPlayer = AllPlayers[LastTargetIndex].BaseEntity;
								//Fire at the target!
								gTriggerbot.RunTriggerbot(pTargetPlayer, LocalPlayer.CurrentWeapon, &LocalPlayer.CurrentEyeAngles, LocalPlayer.TargetEyeAngles, flags, BestDamage, DontSetViewAngles, FramePointer, RanHitChanceThisTick);
							}
						}
					}
					else
					{
						//manual triggerbot or autofire when not locked on
						//if (NoAntiAimsAreOn(false) && (TriggerbotWhileAimbottingChk.Checked || !AimbotModifiedAngles)) //FIXME: This could be avoided but do we want to allow manual triggerbotting with AA/Spins or wait for aimbot to say shoot?
							gTriggerbot.RunTriggerbot(nullptr, LocalPlayer.CurrentWeapon, &LocalPlayer.CurrentEyeAngles, LocalPlayer.TargetEyeAngles, flags, BestDamage, DontSetViewAngles, FramePointer, RanHitChanceThisTick);
					}
				}
			}

			//END OF TRIGGER BOT

			if (AimbotModifiedAngles)
			{
				if (!(buttons & IN_ATTACK))
				{
					if (SilentAimChk.Checked || pSilentChk.Checked)
					{
						//if (AimbotSpeedTxt.flValue == AimbotSpeedTxt.dbMax || (!PressingTriggerbotKey && !AutoFireChk.Checked)) //Only do this if aimbot is set to max speed
							//We didn't fire during this aimlock, so set our angles back to what they were before.
						DontSetViewAngles = true;
					}
				}
			}

			MyWeapon *pCWeapon = &LocalPlayer.WeaponVars;

			//Always don't spinbot/antiaim under these conditions
			bool ForceIgnore = ((buttons & IN_USE) && AntiAimRealDrop.index != AntiAims::ANTIAIM_LEGIT) || /*(flags & FL_INWATER) ||*/ LocalPlayer.Entity->GetMoveType() == MOVETYPE_LADDER || LocalPlayer.Entity->GetMoveType() == MOVETYPE_NOCLIP;

			//Custom conditions
			bool WeaponIsFiring = (buttons & IN_ATTACK) && gTriggerbot.WeaponCanFire(LocalPlayer.CurrentWeapon) && (!pCWeapon->IsRevolver || gTriggerbot.RevolverWillFireThisTick(LocalPlayer.CurrentWeapon));
			if (WeaponIsFiring && !pCWeapon->IsRevolver && LocalPlayer.CurrentWeapon->GetCSWpnData() && LocalPlayer.CurrentWeapon->GetCSWpnData()->bFullAuto == 0 && LocalPlayer.Entity->GetShotsFired() > 0)
				WeaponIsFiring = false; //disallow pistols from disabling aa when holding down LMB

			bool Ignore = ForceIgnore || WeaponIsFiring
				|| ((buttons & IN_ATTACK2) && (LocalPlayer.CurrentWeapon && (!pCWeapon->IsSniper || pCWeapon->IsRevolver)))
				|| (LocalPlayer.CurrentWeapon && (pCWeapon->IsKnife || pCWeapon->IsGrenade) || pCWeapon->IsC4);

			//Grenade/Knife antiaim checks

			CurrentUserCmd.SetIsFakeAngle(CurrentUserCmd.bSendPacket);

			if (!ForceIgnore && Ignore)
				RunGrenadeKnifeAA(Ignore);

			//If we are not overriding angles then do antiaim
			if (LocalPlayer.IsAlive)
			{
				if (!Ignore && DontSetViewAngles && (!NoAntiAimsAreOn(true) || EdgeAntiAimChk.Checked || ManualAntiAimChk.Checked)  /*(!(buttons & IN_ATTACK) && !(buttons & IN_ATTACK2) && !(buttons & IN_USE) && !AimbotModifiedAngles)*/)
				{
					RanAntiAim = true;
					DontSetViewAngles = false;
					QAngle TargetAAAngles = LocalPlayer.TargetEyeAngles;

					if (ManualAntiAimChk.Checked && FixMove_ManualAntiAim(TargetAAAngles, LocalPlayer.CurrentEyeAngles, FramePointer))
					{
						//do nothing
						LocalPlayer.RanManualAntiAim = true;
						Do_AntiAimsPitch(TargetAAAngles, LegitAAed, FramePointer);

					}
					else if (EdgeAntiAimChk.Checked && EdgeAntiAim(TargetAAAngles, &LocalPlayer.CurrentEyeAngles, 360.0f, 65.0f, FramePointer))
					{
						//do nothing
						LocalPlayer.RanEdgeAntiAim = true;
						Do_AntiAimsPitch(TargetAAAngles, LegitAAed, FramePointer);

					}
					else
					{
						TargetAAAngles = LocalPlayer.TargetEyeAngles;

						Do_AntiAims(TargetAAAngles, LegitAAed, FramePointer);
						Do_AntiAimsPitch(TargetAAAngles, LegitAAed, FramePointer);

						//FIXME: MOVE ME
						LocalPlayer.RanEdgeAntiAim = false;
						LocalPlayer.RanManualAntiAim = false;
					}

					//Now aim step to the target angles if needed
					if ((RageAimStepChk.Checked || LegitAimStepChk.Checked) && AimbotSpeedTxt.flValue < 999.0f)
						MTargetting.RageAimStep(LocalPlayer.TargetEyeAngles, TargetAAAngles, LocalPlayer.TargetEyeAngles, SpinDegPerTickTxt.flValue);
					else
						LocalPlayer.TargetEyeAngles = TargetAAAngles;

					if (IsAllowedToNoRecoil(LocalPlayer.CurrentWeapon))
						RemoveRecoil(LocalPlayer.TargetEyeAngles);

					LocalPlayer.TargetEyeAngles.Clamp();

					if (StrafeModifiedAngles)
						MoveFix(LocalPlayer.TargetEyeAngles, StrafeAngles, Vector(cmd->forwardmove, cmd->sidemove, cmd->upmove));
					else
						MoveFix(LocalPlayer.TargetEyeAngles, LocalPlayer.CurrentEyeAngles, Vector(cmd->forwardmove, cmd->sidemove, cmd->upmove));

				}
				else if (!pSilentChk.Checked)
				{
					//FIXME: MOVE ME
					LocalPlayer.RanEdgeAntiAim = false;
					LocalPlayer.RanManualAntiAim = false;

					//FIXME:

					if (!AimbotModifiedAngles && !StrafeModifiedAngles)
					{
						bool AimStep = true;
						if (!NoAntiAimsAreOn(false) && ((buttons & IN_ATTACK) || (buttons & IN_ATTACK2)))
							AimStep = false;

						if (AimStep)
						{
							//AimStep back towards the real crosshair position
							if (RageAimStepChk.Checked || LegitAimStepChk.Checked)
							{
								if (StrafeModifiedAngles)
									LocalPlayer.TargetEyeAngles = StrafeAngles;

								if (AntiAimRealDrop.index != ANTIAIM_LEGIT)
									MTargetting.AimStepBackToCrosshair(DontSetViewAngles);
							}
						}
					}

					if ((SilentAimChk.Checked && !DontSetViewAngles))
					{
						//if(StrafeModifiedAngles)
						MoveFix(LocalPlayer.TargetEyeAngles, LocalPlayer.CurrentEyeAngles, Vector(cmd->forwardmove, cmd->sidemove, cmd->upmove));
						//else
						//	FixMove_SilentAim(StrafeAngles, LocalPlayer.CurrentEyeAngles);

					}
				}
				else
				{
					//FIXME: MOVEME
					LocalPlayer.RanEdgeAntiAim = false;
					LocalPlayer.RanManualAntiAim = false;
				}

				WriteByte((ReadInt((uintptr_t)FramePointer) - 0x1C), CurrentUserCmd.bSendPacket);

			} 
			else
			{
				//FIXME: MOVEME
				LocalPlayer.RanEdgeAntiAim = false;
				LocalPlayer.RanManualAntiAim = false;
			}
		}//Foreground window only above

		if (DontSetViewAngles)
		{
			//Set target angles back to the current angles since they should not be changed
			if (StrafeModifiedAngles)
				LocalPlayer.TargetEyeAngles = StrafeAngles;
			else
				LocalPlayer.TargetEyeAngles = LocalPlayer.CurrentEyeAngles;

			if (!LegitBotChk.Checked && IsAllowedToNoRecoil(LocalPlayer.CurrentWeapon))
				RemoveRecoil(LocalPlayer.TargetEyeAngles);
		}

		if (LocalPlayer.IsAlive && AutoGriefChk.Checked && NoAntiAimsAreOn(false))
			AutoGrief(buttons, LocalPlayer.TargetEyeAngles, LocalPlayer.CurrentEyeAngles, PressingAimbotKey);

		//Write the target eye angles to cmd

		if (ValveResolverChk.Checked)
			cmd->viewangles.Clamp();

		WriteEyeAngles(cmd);

		if (LegitAAed)
		{
			LocalPlayer.LastEyeAngles = LocalPlayer.CurrentEyeAngles - LocalPlayer.AimPunchSubtracted;
			LocalPlayer.LastEyeAngles.Clamp();
		}
		else
			LocalPlayer.LastEyeAngles = LocalPlayer.TargetEyeAngles;


		if (NoAntiAimsAreOn(true))
		{
			LocalPlayer.LastRealEyeAngles = LocalPlayer.TargetEyeAngles;
			LocalPlayer.LastFakeEyeAngles = LocalPlayer.TargetEyeAngles;
		}
		else
		{
			if (CurrentUserCmd.IsRealAngle()) //SendPacket = false
				LocalPlayer.LastRealEyeAngles = LocalPlayer.TargetEyeAngles;
			else
				LocalPlayer.LastFakeEyeAngles = LocalPlayer.TargetEyeAngles;
		}

		if (buttons != RealButtons)
			WriteInt((uintptr_t)&cmd->buttons, buttons); //Only modify if changed..

		if (SilentAimChk.Checked || pSilentChk.Checked)
			DontSetViewAngles = true;

		LocalPlayer.SetRenderAngles(cmd, StrafeModifiedAngles, DontSetViewAngles, FramePointer, RanAntiAim);

		if (gpSilent.PressingpSilentKey)
		{
			//Restore saved settings after pressing the dedicated pSilent key
			gpSilent.Pop_pSilent();
		}

		if (bPredict)
			LocalPlayer.FinishEnginePrediction(cmd);

		if (LocalPlayer.IsAlive)
		{
			gLagCompensation.AdjustTickCountForCmd(gTriggerbot.TriggerbotTargetEntity, (buttons & IN_ATTACK));
			/*static CBaseEntity * lastTarget = gTriggerbot.TriggerbotTargetEntity;
			if (lastTarget != gTriggerbot.TriggerbotTargetEntity && gTriggerbot.TriggerbotTargetEntity)
			{
				if (gTriggerbot.TriggerbotTargetEntity->GetHealth() > 0)
					lastTarget = gTriggerbot.TriggerbotTargetEntity;
			}
			if (DrawHitboxesChk.Checked && lastTarget)
			{
				if (lastTarget->GetHealth() > 0)
					lastTarget->DrawHitboxes(ColorRGBA(0, 255, 40, 250), !EXTERNAL_WINDOW ? ReadFloat((uintptr_t)&Interfaces::Globals->absoluteframetime) * 2.0f : 0.001f);
			}*/
		}


#ifdef _DEBUG
		if (gTriggerbot.TriggerbotTargetEntity)
		{
			AllocateConsole();
			Vector origin = gTriggerbot.TriggerbotTargetEntity->GetOrigin();
			printf("Predicted Origin: %.1f %.1f %.1f\n", origin.x, origin.y, origin.z);
		}
#endif

		gTriggerbot.TriggerbotTargetEntity = nullptr; //Reset triggerbot entity for the next cmd

		if (buttons & IN_SCORE)
			ServerRankRevealAll();

		if (GrenadeTrajectoryChk.Checked)
			Tick(buttons);

#ifdef LICENSED
		if (ValidLicense2 != 1)
		{
			THROW_ERROR(ERR_UNTRUSTED_LICENSE);
			exit(EXIT_SUCCESS);
			//WriteFloat((uintptr_t)&CurrentUserCmd.viewangles.z, 666.0f); //Get this fucker an untrusted ban if they try to bypass licensing and fail at doing so
		}
#endif
//#define ONLY_SHOOT_DURING_LAG_COMPENSATION 1
#ifdef _DEBUG
#ifdef ONLY_SHOOT_DURING_LAG_COMPENSATION
		if (LastTargetIndex && !AllPlayers[LastTargetIndex].IsBreakingLagCompensation)
		{
			CurrentUserCmd.cmd->buttons &= ~IN_ATTACK;
		}
#endif

		if (LastTargetIndex != INVALID_PLAYER)
		{
			if (CurrentUserCmd.cmd->buttons & IN_ATTACK)// && LocalPlayer.WeaponVars.IsRevolver && gTriggerbot.RevolverWillFireThisTick(LocalPlayer.CurrentWeapon))
			{
				Interfaces::DebugOverlay->ClearDeadOverlays();
				CustomPlayer *pCPlayer = &AllPlayers[LastTargetIndex];

				//Draw the hitboxes the hack is using
				pCPlayer->BaseEntity->DrawHitboxes(ColorRGBA(255, 255, 0, 200), 4.5f);

				//Draw the spot the aimbot shot at
				Interfaces::DebugOverlay->AddBoxOverlay(LocalPlayer.LastAimbotBonePosition, Vector(-2, -2, -2), Vector(1, 1, 1), cmd->viewangles, 0, 255, 255, 255, 0.5f);

				//Draw debug information
				//char debugstr[128];
				//sprintf(debugstr, "CFY: %.1f\nGFY: %.1f\nLBY: %.1f\nGFY/LBY Delta: %.1f", LocalPlayer.LastAimbotCurrentFeetYawClamped, LocalPlayer.LastAimbotGoalFeetYawClamped, LocalPlayer.LastAimbotLowerBodyYaw, ClampYr(LocalPlayer.LastAimbotGoalFeetYawClamped - LocalPlayer.LastAimbotLowerBodyYaw));
				//Interfaces::DebugOverlay->AddTextOverlay(Vector(LocalPlayer.LastAimbotBonePosition.x + 15.0f, LocalPlayer.LastAimbotBonePosition.y + 15.0f, LocalPlayer.LastAimbotBonePosition.z - 15.0f), 1.5f, debugstr);

				printf("Client Fired EyeAngles: %f %f %f\n\n", cmd->viewangles.x, cmd->viewangles.y, cmd->viewangles.z);
			}
		}
#endif
		
		

		gLagCompensation.CM_RestorePlayers();

		static bool LastCmdShot = false;
		bool bFiring = (cmd->buttons & IN_ATTACK);
		if (!LocalPlayer.bSpecialFiring)
		{
			LocalPlayer.bSpecialFiring = bFiring;
		}

#ifdef EXPLOIT_TEST
		bool exploit = false;

		//DWORD ClientState = *(DWORD*)pClientState;
		//*(DWORD*)(ClientState + 0x174) = -1; // Set m_nDeltaTick to - 1

		if (exploit)
		{
			if (!bFiring)
			{
				if (LastCmdShot)
				{
					cmd->viewangles.x = 89.0f;
					WriteByte((ReadInt((uintptr_t)FramePointer) - 0x1C), 1);
				}
				else
				{
					cmd->tick_count = INT_MAX;
					cmd->viewangles.x = -89.0f;
				}
			}
			//else
			{
				//WriteByte((ReadInt((uintptr_t)FramePointer) - 0x1C), 1);
			}
		}
		else
		{
			LocalPlayer.CheckAirStuck(cmd);
		}
#else
		LocalPlayer.CheckAirStuck(cmd);
#endif

		LastCmdShot = bFiring;

		if (ValveResolverChk.Checked)
			cmd->viewangles.Clamp();

#if 1
		if (BacktrackExploitChk.Checked)
		{
			if (!HNetchan)
			{
				DWORD ClientState = *(DWORD*)pClientState;
				if (ClientState)
				{
					DWORD NetChannel = *(DWORD*)(*(DWORD*)pClientState + 0x9C);
					if (NetChannel)
					{
						HNetchan = new VTHook((DWORD**)NetChannel);
						oSendDatagram = (SendDatagramFn)HNetchan->HookFunction((DWORD)Hooks::SendDatagram, 48);
					}
				}
			}
		}
#if 0
		else
		{
			if (HNetchan)
			{
				DWORD ClientState = *(DWORD*)pClientState;
				if (ClientState)
				{
					DWORD NetChannel = *(DWORD*)(*(DWORD*)pClientState + 0x9C);
					if (NetChannel)
					{
						LocalPlayer.Real_m_nInSequencenumber = 0.0f;
						delete HNetchan;
						HNetchan = nullptr;
					}
					else
					{
						LocalPlayer.Real_m_nInSequencenumber = 0.0f;
						HNetchan->ClearClassBase();
						delete HNetchan;
						HNetchan = nullptr;
					}
				}
				else
				{
					LocalPlayer.Real_m_nInSequencenumber = 0.0f;
					HNetchan->ClearClassBase();
					delete HNetchan;
					HNetchan = nullptr;
				}
			}
		}
#endif
#endif
	} //DisableAll
	else
	{
		//FIXME: MOVE ME
		LocalPlayer.RanEdgeAntiAim = false;
		LocalPlayer.RanManualAntiAim = false;
	}

	//Store here for now, it's here so that if we open/close the hack menu or toggle disable all it doesn't make us snap due to old aimpunch values
	if (!LocalPlayer.IsAlive)
		LocalPlayer.LastAimPunch = angZero;
	else
		LocalPlayer.LastAimPunch = LocalPlayer.Entity->GetPunch();

	Interfaces::MDLCache->BeginLock();

	END_PROFILING(stat);
	return false;
}


