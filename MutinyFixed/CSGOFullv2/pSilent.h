#pragma once
#include "CreateMove.h"
#include "FixMove.h"

class pSilent
{
public:
	int LastWeaponFiredCmdNr;
	int LastWeaponFireCmdTickCount;
	int pSilentTicksSkipped;
	bool pSilentBlockFiring;
	QAngle pSilentBeforeAngles;
	void Run_pSilent(QAngle RealViewAngles, QAngle& angles, DWORD* FramePointer);
	void Push_pSilent();
	void Pop_pSilent();
	bool TraceOutFromCrosshair(CBaseEntity*  &pEntityHit, QAngle& TargetAngles, Vector& BestBonePos, float& BestDamage, QAngle* LocalEyeAngles, Vector LocalEyePos, CBaseCombatWeapon* weapon, int& HitgroupHit);
	BOOL PressingpSilentKey;
private:
	AntiAims StoredRealAntiAim;
	AntiAims StoredFakeAntiAim;
	AntiAimPitches StoredPitchAntiAim;
	bool StoredpSilent, StoredSilentAim, StoredLegitAimStep, StoredRageAimStep, StoredTriggerbotWhileAimbotting, StoredManualAntiAim;
};

extern pSilent gpSilent;