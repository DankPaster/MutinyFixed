#pragma once
#include "CreateMove.h"

#define	HITGROUP_HEAD_FLAG      (1<<0)
#define	HITGROUP_CHEST_FLAG		(1<<1)
#define	HITGROUP_STOMACH_FLAG	(1<<2)
#define HITGROUP_LEFTARM_FLAG	(1<<3)
#define HITGROUP_RIGHTARM_FLAG	(1<<4)
#define HITGROUP_LEFTLEG_FLAG	(1<<5)
#define HITGROUP_RIGHTLEG_FLAG	(1<<6)
#define HITGROUP_ALL_FLAG (1<<7)
//extern Vector ShotFrom;
//extern Vector Impact;
//extern float ImpactTime;
//extern bool JustShot;
extern int IndexOfClosest;
extern int LastCheckedIndex;
extern float m_CurAimTime;
void NormalizeAngles(QAngle& angles);
void NormalizeAngle(float& angle);
Vector AnglesToPixels(QAngle SourceAngles, QAngle FinalAngles, float sensitivityy, float mpitch, float myaw);
QAngle PixelsDeltaToAnglesDelta(Vector PixelsDelta, float sensitivityy, float mpitch, float myaw);
void SmoothAngle(QAngle& From, QAngle& To, float Percent);
//bool BoneIsStreamed(DWORD BoneMatrix, Vector *TargetPlayerOrigin, Vector& TargetBonePos, CBaseEntity* pEntity);
//bool FindBackupBone(DWORD BoneMatrix, Vector TargetPlayerOrigin, Vector& TargetBonePos);
bool IsBoneVisible(bool AllowAutoWall, int TargetHitbox, Vector &TargetBonePos, Vector LocalEyePos, trace_t &tr, CBaseEntity* pPlayer, bool bforcehitgroup, int forcehitgroup, float& dmg, QAngle *localangles);
bool FindNewAimbotTarget(QAngle *localangles, CBaseCombatWeapon *weapon, Vector& BestBonePos, bool& RanHitChance, bool& CanTraceLineToATarget);
void ClearAimbotTarget();
void ClearTargettedBone();
bool FindMultiPointVisible(int Bone, bool OnlyAutoWall, Vector &TargetBonePos, Vector &LocalEyePos, trace_t &tr, CBaseEntity* pPlayer, bool bforcehitgroup, int forcehitgroup, float& dmg, QAngle *localangles, bool HittingTargetFPS);
bool Aimbot(QAngle& angles, DWORD* FramePointer, int &buttons, bool& AimbotModifiedAngles, QAngle LastAngle, CBaseCombatWeapon *weapon, int PressingTriggerbotKey, bool& RanHitChance);
bool AimToTheTarget(CBaseCombatWeapon* weapon, int buttons, QAngle &angles, QAngle* LastAngle, Vector LocalEyePos, Vector& TargetBonePos, CustomPlayer* pCPlayer, bool& AimbotModifiedAngles, int PressingTriggerbotKey);