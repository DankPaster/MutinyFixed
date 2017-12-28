#pragma once


#include "BaseEntity.h"
#include "hDirectX.h"

#include <Windows.h>
#include <iostream>

#include <d3d9.h>
#include <d3dx9.h>
#pragma comment(lib, "d3d9.lib")
#pragma comment(lib, "d3dx9.lib")

#include <dwmapi.h>
#pragma comment(lib, "dwmapi.lib")
#include <vector>
#include "misc.h"
#include "CSGO_HX.h"
#include "Overlay.h"
#include <deque>

#define MAX_NEAR_BODY_YAW_RESOLVER_MISSES 6
#define MAX_RESOLVER_MISSES 19
#define MAX_FIRING_WHEN_SHOOTING_MISSES 3
#define MAX_BODY_YAW_MISSES 4
#define MAX_FAKEWALK_MISSES 8

extern float DefuseTimeRemaining;

enum ResolveYawModes : int
{
	NoYaw,
	AutomaticYaw,
	FakeSpins,
	LinearFake,
	RandomFake,
	CloseFake,
	BloodBodyRealDelta,
	BloodEyeRealDelta,
	BloodReal,
	AtTarget,
	InverseAtTarget,
	AverageLBYDelta,
	StaticFakeDynamic,
	FakeWalk, //WARNING: UPDATE 'CanFireAtLagCompensatedPlayer' if more are added
	StaticFake,
	Inverse,
	Back,
	Left,
	Right,
	CustomStaticYaw,
	CustomAdditiveYaw,
	//NOTE: NOT A REAL RESOLVE MODE BELOW, SIMPLY USED FOR REFERENCE
	WD,
	WD_StaticFake,
	WD_Velocity,
	WD_Target,
	WD_Spin,
	WD_Jitter,
	WD_Static,
	WD_FuckIt,
	BackTrackFire,
	BackTrackUp,
	BackTrackHit,
	BackTrackReal,
	BackTrackLby,
	NotBackTracked,
	Backtracked //NOTE: NOT A REAL RESOLVE MODE, SIMPLY USED FOR REFERENCE
};

enum ResolvePitchModes : int
{
	NoPitch,
	AutomaticPitch,
	Up,
	Down,
	CustomStaticPitch,
	CustomAdditivePitch
};

struct StoredNetvars;
struct StoredLowerBodyYaw;
struct BloodRecord;
#define scanstrength 70
struct Personalize 
{
	bool PriorityTarget;
	bool DontStatsThisShot; //Don't count misses or corrects for this shot
	ResolveYawModes EstimateYawModeInAir; //The mode the resolver will use when lower body is not current and player is in air
	ResolveYawModes EstimateYawMode; //The mode the resolver will use when lower body is not current
	ResolveYawModes ResolveYawMode; //The selected resolver mode in the menu for this player
	ResolvePitchModes ResolvePitchMode; //The selected pitch mode in the menu for this player
	ResolveYawModes LastResolveModeUsed; //The last resolve mode used
	bool WasInverseFakingWhileMoving;
	bool IgnoreAutoResolve;
	bool DontShootAtHeadNeck;
	float flPitch;
	float flYaw;
	float flLastPitch;
	float flLastYaw;
	float flLastPitchChange;
	float flLastYawChange;
	float lastLbyChcker;
	bool bEyeYawUpdated;
	bool bSpinbotting;
	bool isCorrected;
	int correctedbodyyawindex;
	int correctedfakewalkindex;
	int correctedindex;
	int JitterCounter;
	int correctedclosefakeindex;
	int correctedresolvewhenshootingindex;
	int LastHitGroupShotAt;
	QAngle LastEyeAnglesShotAt;
	QAngle EyeAnglesShotAt;
	int ShotsMissed;
	int iHitGroupPlayerWasShotOnServer;
	float flSimulationTimePlayerWasShot;
	float flSimulationTimeBloodWasSpawned;
	// Stuff for jake's history resolver

	int fakeLagHistoryCounter = 0;
	int historyPacketsChoked[20];
	bool UsingAwareAdaptive = false;
	

	float lbyUpdate = 0;
	float curTimeUpdatedLast = 0;
	float lastlby = 0;

	int counter = 0;
	float realValues[scanstrength];
	float fakeValues[scanstrength];
	float targetDValues[scanstrength];
	float velocityDValues[scanstrength];
	int ticksReal[scanstrength];

	int ShotsnotHit;
	int roundStart;

	QAngle angEyeAnglesResolvedFromBlood;
	std::deque<StoredLowerBodyYaw*> m_PlayerLowerBodyRecords;  //MUST BE LAST OR ELSE MEMSET WILL CORRUPT THIS
	std::deque<StoredNetvars*> m_PlayerRecords; //MUST BE LAST OR ELSE MEMSET WILL CORRUPT THIS
	std::deque<BloodRecord*> m_BloodRecords; //MUST BE LAST OR ELSE MEMSET WILL CORRUPT THIS
	inline bool ShouldResolve()
	{
		return !IgnoreAutoResolve && (ResolveYawMode != ResolveYawModes::NoYaw || ResolvePitchMode != ResolvePitchModes::NoPitch);
	}
};

extern CustomPlayer* pHighlightedPlayer;

extern Checkbox FireWhenEnemiesShootChk;
extern Checkbox AutoPelvisChk;
extern Checkbox IgnoreAutoResolveChk;
extern Checkbox AutoResolveAllChk;
extern Dropdown ResolvePitchDrop;
extern Dropdown ResolveYawDrop;

extern Checkbox DontTargetHeadNeckChk;
extern Checkbox ValveResolverChk;
extern Checkbox PriorityTargetChk;

extern Textbox ResolvePitchTxt;
extern Textbox ResolveYawTxt;

void ClearHighlightedPlayer();
void RunVisualHacks_DXHook();
void FillCustomPlayerStruct(CustomPlayer*const pCAPlayer, CBaseEntity*const pBaseEntity, int i);
void RunPerPlayerHacks();
void DrawPlayerList();

//http://www.unknowncheats.me/forum/counterstrike-global-offensive/183347-bomb-damage-indicator.html
inline float CSGO_Armor(float flDamage, int ArmorValue)
{
	float flArmorRatio = 0.5f;
	float flArmorBonus = 0.5f;
	if (ArmorValue > 0) {
		float flNew = flDamage * flArmorRatio;
		float flArmor = (flDamage - flNew) * flArmorBonus;

		if (flArmor > static_cast<float>(ArmorValue)) {
			flArmor = static_cast<float>(ArmorValue) * (1.f / flArmorBonus);
			flNew = flDamage - flArmor;
		}

		flDamage = flNew;
	}
	return flDamage;
}