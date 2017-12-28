#pragma once
#include "CreateMove.h"


class AutoBone
{
public:
	bool AutoBone_HeadOnly();
	bool AutoBone_HeadNeck(bool* FoundLethalHitbox, bool* DealsMinimumDamage);
	bool AutoBone_ForcePelvisChest(bool* FoundLethalHitbox, bool* DealsMinimumDamage);
	bool AutoBone_Torso(bool* FoundLethalHitbox, bool* DealsMinimumDamage);
	bool AutoBone_UpperLimbs(bool NoExtremeties, bool* FoundLethalHitbox, bool* DealsMinimumDamage);
	bool AutoBone_LowerLimbs(bool NoExtremeties, bool* FoundLethalHitbox, bool* DealsMinimumDamage);
	bool AutoBone_Knife();
	bool AutoBone_pSilent();
	bool AutoBone_HsNoSpread();
	bool Run_AutoBone(CBaseCombatWeapon* weapon, CustomPlayer* pCPlayer, QAngle *LocalEyeAngles, Vector LocalEyePos, int& BestBone, Vector& BestBonePos, bool& RanHitChance, bool& CanTraceALine);
	bool AutoBone_BackTrack(CustomPlayer* pCPlayer, StoredNetvars *BestTick);
private:
	CBaseCombatWeapon *pWeapon;
	CBaseEntity *pPlayer;
	CustomPlayer *pCPlayer;
	Vector *pLocalEyePos;
	QAngle *pLocalEyeAngles;
	bool *pCanTraceALine;
	float BestDamage;
	float BoneTargetTime;
	int *pBestHitbox;
	int MinimumWeaponDamage;
	Vector *pBestHitboxPos;
	int TimesRunThisFrame = 0;
	int TickRanFrameStatistics = 0;
	int BodyAimHitboxAlreadyChecked;
	bool CheckedBodyAimHitbox;
	float TargetEntityHealth;
	bool HittingTargetFPS;
	bool OnlyHitchanceInTriggerbot;
	bool lessIntensive;
	int VisibleHeadGroupHitbox;
	Vector VisibleHeadGroupHitboxPos;
	bool AlreadyIncrementedTotalBodyTimeVisible;
};

extern AutoBone gAutoBone;