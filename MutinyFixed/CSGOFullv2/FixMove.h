#pragma once
#include "CreateMove.h"
#include "Overlay.h"
#include "LocalPlayer.h"

#define ANTIAIM_DEFAULT 1

enum AntiAims : int {
	ANTIAIM_NONE = 0,
	ANTIAIM_DOWNBACK,
	ANTIAIM_SIDEWAYS,
	ANTIAIM_LEGIT,
	ANTIAIM_SKATE,
	ANTIAIM_BALLERINAFAKE,
	ANTIAIM_SLOWSPINFAKE,
	ANTIAIM_SPIN,
	ANTIAIM_EXPERIMENTAL,
	ANTIAIM_EXPERIMENTAL2,
	ANTIAIM_EXPERIMENTALFORWARDS,
	ANTIAIM_EXPERIMENTALFORWARDS_INVERTED,
	ANTIAIM_TESTING,
	ANTIAIM_TESTING2,
	ANTIAIM_CLASSICAL,
	ANTIAIM_CLASSICAL180,
	ANTIAIM_CLASSICAL90,
	ANTIAIM_INVERSE_SWITCH,
	ANTIAIM_SIDEWAYS_JITTER
};

enum AntiAimPitches : int
{
	PITCH_NONE = 0,
	PITCH_UP,
	PITCH_DOWN,
	PITCH_JITTERDOWN,
	PITCH_JITTERUPDOWN,
	PITCH_LISP1, //FAKEDOWN,
	PITCH_LISP2, //FAKEDOWN2,
	PITCH_FAKEDOWN,
	PITCH_TDOWN,
	//PITCH_FAKEUP,
};

void Do_AntiAims(QAngle& TargetAAAngles, bool& LegitAAed, DWORD* FramePointer);
void Do_AntiAimsPitch(QAngle& TargetAAAngles, bool& LegitAAed, DWORD* FramePointer);
void RunGrenadeKnifeAA(bool& Ignore);
void MoveFix(QAngle angles, QAngle OriginalAngs, Vector Move);
float PickRandomAngle(float numangles, ...);
void FixMove_Spinbot(QAngle& angles, QAngle LastAngle, QAngle RealViewAngles, DWORD* FramePointer);
void FixMove_Spinbot_Unhittable(QAngle& angles, QAngle LastAngle, QAngle RealViewAngles, DWORD* FramePointer);
void FixMove_Experimental(CBaseEntity* AimTarget, int buttons, QAngle& angles, QAngle LastAngle, QAngle RealViewAngles, DWORD* FramePointer);
void FixMove_ExperimentalForwards(CBaseEntity* AimTarget, int buttons, QAngle& angles, QAngle LastAngle, QAngle RealViewAngles, DWORD* FramePointer);
void FixMove_ExperimentalForwards_Inverted(CBaseEntity* AimTarget, int buttons, QAngle& angles, QAngle LastAngle, QAngle RealViewAngles, DWORD* FramePointer);
void FixMove_Experimental2(CBaseEntity* AimTarget, int buttons, QAngle& angles, QAngle LastAngle, QAngle RealViewAngles, DWORD* FramePointer);
void FixMove_AntiAim(QAngle &angles, QAngle LastAngle, QAngle RealViewAngles, CBaseEntity* AimTarget, DWORD* FramePointer);
void FixMove_AntiAimMemester(QAngle& angles, QAngle LastAngle, QAngle RealViewAngles, DWORD* FramePointer);
bool FixMove_LegitAntiAim(QAngle &angles, int buttons, QAngle LastAngle, QAngle RealViewAngles, DWORD* FramePointer, CBaseCombatWeapon *weapon);
void FixMove_AntiAimSkate(QAngle& angles, QAngle LastAngle, QAngle RealViewAngles, DWORD* FramePointer);
void FixMove_SideAntiAim(QAngle &angles, QAngle LastAngle, QAngle RealViewAngles, CBaseEntity* AimTarget, DWORD* FramePointer);
void FixMove_Testing(QAngle &angles, QAngle LastAngle, QAngle RealViewAngles, CBaseEntity* AimTarget, DWORD* FramePointer);
void FixMove_Testing2(QAngle &angles, QAngle LastAngle, QAngle RealViewAngles, CBaseEntity* AimTarget, DWORD* FramePointer);
bool EdgeAntiAim(QAngle &angles, QAngle *RealViewAngles, float flWall, float flCornor, DWORD* FramePointer);
bool FixMove_ManualAntiAim(QAngle &angles, QAngle RealViewAngles, DWORD* FramePointer);
void FixMove_Lisp180AntiAim(QAngle &angles, QAngle LastAngle, QAngle RealViewAngles, CBaseEntity* AimTarget, DWORD* FramePointer);
void FixMove_Lisp1802AntiAim(QAngle &angles, QAngle LastAngle, QAngle RealViewAngles, CBaseEntity* AimTarget, DWORD* FramePointer);
void FixMove_Lisp90AntiAim(QAngle &angles, QAngle LastAngle, QAngle RealViewAngles, CBaseEntity* AimTarget, DWORD* FramePointer);
void FixMove_Lisp902AntiAim(QAngle &angles, QAngle LastAngle, QAngle RealViewAngles, CBaseEntity* AimTarget, DWORD* FramePointer);
void FixMove_SilentAim(QAngle angles, QAngle RealViewAngles);

inline bool NoAntiAimsAreOn(bool IncludeLegitAntiAim) { 
	if (AntiAimPitchDrop.index == PITCH_NONE && 
		( (AntiAimRealDrop.index == ANTIAIM_NONE && AntiAimFakeDrop.index == ANTIAIM_NONE) || (AntiAimRealDrop.index == ANTIAIM_LEGIT && !IncludeLegitAntiAim) )
		&& !LocalPlayer.RanEdgeAntiAim && !LocalPlayer.RanManualAntiAim
		)
			return true;

	return false;
}
