#pragma once
#include <Windows.h>
#include "misc.h"

class CBaseEntity;
class CBaseCombatWeapon;
struct CustomPlayer;

enum BONE_GROUPS
{
	BONE_GROUP_HEADNECK = 0,
	BONE_GROUP_CHESTTORSOPELVIS,
	BONE_GROUP_SHOULDERARMSHANDS,
	BONE_GROUP_LEGSFEET
};

enum Hitboxes
{
	HITBOX_HEAD,
	HITBOX_NECK,
	HITBOX_LOWER_NECK,
	HITBOX_PELVIS,
	HITBOX_BODY,
	HITBOX_THORAX,
	HITBOX_CHEST,
	HITBOX_UPPER_CHEST,
	HITBOX_LEFT_THIGH,
	HITBOX_RIGHT_THIGH,
	HITBOX_LEFT_CALF,
	HITBOX_RIGHT_CALF,
	HITBOX_LEFT_FOOT,
	HITBOX_RIGHT_FOOT,
	HITBOX_LEFT_HAND,
	HITBOX_RIGHT_HAND,
	HITBOX_LEFT_UPPER_ARM,
	HITBOX_LEFT_FOREARM,
	HITBOX_RIGHT_UPPER_ARM,
	HITBOX_RIGHT_FOREARM,
	HITBOX_MAX
};

#define MAX_BONES 28
#define MAX_HITBOXES 20
#define MAX_BONE_INDEX 27 //MAX_BONES - 1

extern int NUM_BONES;
extern int BONES_TO_CHECK[MAX_BONES];
extern const int HITBOX_GROUP_HEAD[2];
extern const int HITBOX_GROUP_TORSO[6];
extern const int HITBOX_GROUP_UPPERLIMBS[6];
extern const int HITBOX_GROUP_LOWERLIMBS[6];
extern const int HITBOX_GROUP_TORSO_FPS[3];
extern const int HITBOX_GROUP_UPPERLIMBS_FPS[2];
extern const int HITBOX_GROUP_LOWERLIMBS_FPS[4];
extern const int HITBOXES_ALL_BASIC[9];
extern int HITGROUPS_ALLOWED[7];
extern int NUM_HITGROUPS_ALLOWED;

struct CPlayerTarget
{
	float DistanceFromCrosshair;
	int Bone;
	//int BoneCheckIndex;
	Vector BonePos;
	QAngle IdealpSilentAngles;
	bool bAimAtpSilentAngles;
	bool PriorityTarget;
	bool RanHitChance;
};

class Targetting
{
public:
	BONE_GROUPS GetBoneGroup(int Bone);
	BONE_GROUPS GetHitboxGroup(int Hitbox) { return GetBoneGroup(Hitbox); }
	bool IsPlayerAValidTarget(CBaseEntity* Player);
	bool IsPlayerAValidTarget(CustomPlayer* Player);
	void SetHitboxGroup();
	bool IsHitgroupACurrentTarget(int hitgroup);
	bool IsHitgroupATarget(int hitgroup, CBaseEntity* pEntity);
	void UpdateBoneList();
	int HitboxToHitgroup(int Hitbox);
	int HitgroupToHitbox(int Hitgroup);
	bool HitboxIsArmOrHand(int Hitbox);
	bool HitboxIsLegOrFoot(int Hitbox);
	CBaseEntity* TraceToPosition(Vector LocalEyePos, Vector* TargetPos, Vector* vecDir, QAngle* const localangles, CBaseCombatWeapon* const weapon, float* const dmg, int& hitgroup, Vector* TraceHitPos);
	CBaseEntity* GetPlayerClosestToCrosshair(float MaxFOV);
	void RageAimStep(QAngle from, QAngle to, QAngle& dest, float flStep);
	void RageAimStep(float from, float to, float& dest, float flStep);
	void AimStepBackToCrosshair(bool& DontSetViewAngles);
	void SetHitChanceFlags();

	QAngle IdealpSilentAngles;
	bool bAimAtpSilentAngles;
	int CURRENT_HITCHANCE_FLAGS = 0;
	int CURRENT_HITGROUP_AIMING_AT = 0;
};

extern Targetting MTargetting;