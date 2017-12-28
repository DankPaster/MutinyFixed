#include "Aimbot.h"
#include "Overlay.h"
#include "CSGO_HX.h"
#include "AutoWall.h"
#include "Triggerbot.h"
#include "FixMove.h"
#include "NoRecoil.h"
#include "HitChance.h"
#include "CreateMove.h"
#include "Targetting.h"
#include "LocalPlayer.h"
#include "pSilent.h"
#include "AutoBone.h"

float m_CurAimTime; //Current Aim Time for Smooth Aim
float LegitAimStepIncTimeOverShoot = 0.0f;
bool ReachedLegitAimStepTarget = false;
bool InitializedLegitAimStepDirection = false;
enum AimDirection { LEFT = 0, RIGHT };
AimDirection LegitAimStartDirection;
QAngle LegitAimStepInitialDelta;
int LegitAimCurveType = 0;

void ClearLegitAimStepVars()
{
	ReachedLegitAimStepTarget = false;
	LegitAimStepIncTimeOverShoot = 0.0f;
	m_CurAimTime = 0.0f; //Reset for new targets (Smooth Aim)
	InitializedLegitAimStepDirection = false;
	int r = rand() % 3;
	LegitAimCurveType = r != LegitAimCurveType ? r : rand() % 3;
}

//Courtesy of UC
Vector AnglesToPixels(QAngle SourceAngles, QAngle FinalAngles, float sensitivityy, float mpitch, float myaw)
{
#if 1
	QAngle delta = SourceAngles - FinalAngles;
	NormalizeAngles(delta);
	float xMove = (-delta.x) / (myaw * sensitivityy * 1);
	float yMove = (delta.y) / (mpitch * sensitivityy * 1);

	return Vector(yMove, xMove, 0.0f);
#else
	QAngle vDelta = SourceAngles - FinalAngles;
	NormalizeAngles(vDelta);

	vDelta.x = vDelta.x / mpitch;
	vDelta.y = vDelta.y / myaw;

	return Vector(vDelta.y, -vDelta.x, 0.0f);
#endif
}

QAngle PixelsDeltaToAnglesDelta(Vector PixelsDelta, float sensitivityy, float mpitch, float myaw)
{
	float xMove = (-PixelsDelta.x) * (myaw * sensitivityy * 1);
	float yMove = (PixelsDelta.y) * (mpitch * sensitivityy * 1);

	return QAngle(yMove, xMove, 0.0f);
}

void SmoothAngle(QAngle& From, QAngle& To, float Percent)
{
	QAngle VecDelta = From - To;
	NormalizeAngles(VecDelta);
	VecDelta.x *= Percent;
	VecDelta.y *= Percent;
	To = From - VecDelta;
	To.Clamp();
}

bool IsBoneVisible(bool AllowAutoWall, int TargetHitbox, Vector &TargetBonePos, Vector LocalEyePos, trace_t &tr, CBaseEntity* pPlayer, bool bforcehitgroup, int forcehitgroup, float& dmg, QAngle *localangles)
{
	if (AllowAutoWall && AutoWallChk.Checked)
	{
		int hitgroup = 0;
		float damage;
		CBaseEntity* EntityHit = Autowall(&TargetBonePos, LocalEyePos, localangles, LocalPlayer.Entity->GetWeapon(), &damage, hitgroup, pPlayer, TargetHitbox);
		if (damage > 0.0f)
		{
			dmg = damage;
			if (bforcehitgroup)
			{
				if (
					forcehitgroup & HITGROUP_ALL_FLAG
					|| forcehitgroup & HITGROUP_HEAD_FLAG && hitgroup == HITGROUP_HEAD
					|| forcehitgroup & HITGROUP_CHEST_FLAG && hitgroup == HITGROUP_CHEST
					|| forcehitgroup & HITGROUP_STOMACH_FLAG && hitgroup == HITGROUP_STOMACH
					|| forcehitgroup & HITGROUP_LEFTARM_FLAG && hitgroup == HITGROUP_LEFTARM
					|| forcehitgroup & HITGROUP_RIGHTARM_FLAG && hitgroup == HITGROUP_RIGHTARM
					|| forcehitgroup & HITGROUP_LEFTLEG_FLAG && hitgroup == HITGROUP_LEFTLEG
					|| forcehitgroup & HITGROUP_RIGHTLEG_FLAG && hitgroup == HITGROUP_RIGHTLEG)
				{
					return true;
				}
			}
			else if (MTargetting.IsHitgroupACurrentTarget(hitgroup))
			{
				return true;
			}
		}
	}
	else
	{
		UTIL_TraceLine(LocalEyePos, TargetBonePos, MASK_SHOT, LocalPlayer.Entity, &tr);
		Vector vecDir;
		AngleVectors(*localangles, &vecDir);
		VectorNormalizeFast(vecDir);
		CTraceFilter filter;
		filter.pSkip = (IHandleEntity*)LocalPlayer.Entity;
		UTIL_ClipTraceToPlayers(LocalEyePos, TargetBonePos + vecDir * 40.0f, 0x4600400B, &filter, &tr);

		if (tr.m_pEnt && tr.m_pEnt == pPlayer) // (bforcehitgroup ? tr.hitgroup == forcehitgroup : IsHitgroupACurrentTarget(tr.hitgroup)))
		{
			int hitgroup = tr.hitgroup;
			if (bforcehitgroup)
			{
				if (
					forcehitgroup & HITGROUP_ALL_FLAG
					|| forcehitgroup & HITGROUP_HEAD_FLAG && hitgroup == HITGROUP_HEAD
					|| forcehitgroup & HITGROUP_CHEST_FLAG && hitgroup == HITGROUP_CHEST
					|| forcehitgroup & HITGROUP_STOMACH_FLAG && hitgroup == HITGROUP_STOMACH
					|| forcehitgroup & HITGROUP_LEFTARM_FLAG && hitgroup == HITGROUP_LEFTARM
					|| forcehitgroup & HITGROUP_RIGHTARM_FLAG && hitgroup == HITGROUP_RIGHTARM
					|| forcehitgroup & HITGROUP_LEFTLEG_FLAG && hitgroup == HITGROUP_LEFTLEG
					|| forcehitgroup & HITGROUP_RIGHTLEG_FLAG && hitgroup == HITGROUP_RIGHTLEG)
				{
					//Give back min damage if no auto wall for all fucking features which need damage.
					Autowall(&TargetBonePos, LocalEyePos, localangles, LocalPlayer.Entity->GetWeapon(), &dmg, hitgroup, pPlayer, TargetHitbox);
					if (dmg <= 0.0f)
						dmg = 1.0f;

					return true;
				}
			}
			else if (MTargetting.IsHitgroupACurrentTarget(hitgroup))
			{
				Autowall(&TargetBonePos, LocalEyePos, localangles, LocalPlayer.Entity->GetWeapon(), &dmg, hitgroup, pPlayer, TargetHitbox);
				if (dmg <= 0.0f)
					dmg = 1.0f;

				return true;
			}
		}
	}
	dmg = 0.0f;
	return false;
}

bool IsNudgedBoneVisible(float nudgeXYamount, bool AllowAutoWall, int TargetHitbox, Vector &TargetBonePos, Vector LocalEyePos, trace_t &tr, CBaseEntity* pPlayer, bool bforcehitgroup, int forcehitgroup, float& dmg, float& BestDamage, Vector& BestBonePos, QAngle *localangles, float TargetHealth)
{
	//Try nudging different directions
	Vector centerpos = TargetBonePos;
	bool IsVisible = false;

	Vector vForward, vRight, vUp;
	QAngle aimboneangle = CalcAngle(LocalEyePos, centerpos);
	AngleVectors(aimboneangle, &vForward, &vRight, &vUp);

	VectorNormalizeFast(vForward);
	VectorNormalizeFast(vRight);
	VectorNormalizeFast(vUp);

	TargetBonePos = centerpos + (vRight * nudgeXYamount);

#ifdef _DEBUG
	Vector scn;
	if (DrawAimbotBoneChk.Checked && EXTERNAL_WINDOW)
	{
		WorldToScreen(TargetBonePos, scn);
		DrawBox(Vector2D(scn.x, scn.y), 5, 5, 2, 0, 255, 0, 255);
	}
#endif

	if (IsBoneVisible(AllowAutoWall, TargetHitbox, TargetBonePos, LocalEyePos, tr, pPlayer, bforcehitgroup, forcehitgroup, dmg, localangles))
	{
		if (!AutoBoneBestDamageChk.Checked)
		{
			BestDamage = dmg;
			BestBonePos = TargetBonePos;
			return true;
		}
		if (dmg > BestDamage)
		{
			BestDamage = dmg;
			BestBonePos = TargetBonePos;
			if (TargetHealth - BestDamage <= 0.0f || dmg >= MinimumDamageBeforeFiringTxt.flValue)
				return true;
		}
		IsVisible = true;
	}

	TargetBonePos = centerpos - (vRight * nudgeXYamount);

#ifdef _DEBUG
	if (DrawAimbotBoneChk.Checked && EXTERNAL_WINDOW)
	{
		WorldToScreen(TargetBonePos, scn);
		DrawBox(Vector2D(scn.x, scn.y), 5, 5, 2, 0, 255, 0, 255);
	}
#endif

	if (IsBoneVisible(AllowAutoWall, TargetHitbox, TargetBonePos, LocalEyePos, tr, pPlayer, bforcehitgroup, forcehitgroup, dmg, localangles))
	{
		if (!AutoBoneBestDamageChk.Checked)
		{
			BestDamage = dmg;
			BestBonePos = TargetBonePos;
			return true;
		}
		if (dmg > BestDamage)
		{
			BestDamage = dmg;
			BestBonePos = TargetBonePos;
			if (TargetHealth - BestDamage <= 0.0f || dmg >= MinimumDamageBeforeFiringTxt.flValue)
				return true;
		}
		IsVisible = true;
	}

	return IsVisible;
}


enum corners
{
	middle = 0,
	Bottom,
	Top,
	TopLeftForward,
	TopLeftBackwards,
	TopRightForward,
	TopRightBackwards,
	BottomLeftForward,
	BottomLeftBackwards,
	BottomRightForward,
	BottomRightBackwards,
	Plus1,
	Plus2,
	Plus25,
	Plus4,
	Plus5,
	MAXPOINTSTOSCAN,
};

//Brute forces different points of a bone to find a specific area that is visible
#ifdef NEW_MULTIPOINT
bool FindMultiPointVisible(int Bone, bool OnlyAutoWall, Vector &TargetBonePos, Vector &LocalEyePos, trace_t &tr, CBaseEntity* pPlayer, bool bforcehitgroup, int forcehitgroup, float& dmg, QAngle *localangles, Vector TempHitBoxPos[2])
{
	if (!AimbotMultiPointChk.Checked || (AimbotMultiPointPrioritiesOnlyChk.Checked && !AllPlayers[pPlayer->index].Personalize.PriorityTarget))
	{
		return IsBoneVisible(true, Bone, TargetBonePos, LocalEyePos, tr, pPlayer, bforcehitgroup, forcehitgroup, dmg, localangles);
	}

	BONE_GROUPS BoneGroup = MTargetting.GetBoneGroup(Bone);

	bool IsHead = BoneGroup == BONE_GROUP_HEADNECK;

	bool NoMultiPointTargetting = false;

	if (!AimbotMultiPointHeadNeckChk.Checked && IsHead)
		return IsBoneVisible(true, Bone, TargetBonePos, LocalEyePos, tr, pPlayer, bforcehitgroup, forcehitgroup, dmg, localangles);
	else if (!AimbotMultiPointChestTorsoPelvisChk.Checked && BoneGroup == BONE_GROUP_CHESTTORSOPELVIS)
		return IsBoneVisible(true, Bone, TargetBonePos, LocalEyePos, tr, pPlayer, bforcehitgroup, forcehitgroup, dmg, localangles);
	else if (!AimbotMultiPointShoulderArmsHandsChk.Checked && BoneGroup == BONE_GROUP_SHOULDERARMSHANDS)
		return IsBoneVisible(true, Bone, TargetBonePos, LocalEyePos, tr, pPlayer, bforcehitgroup, forcehitgroup, dmg, localangles);
	else if (!AimbotMultiPointLegsFeetChk.Checked && BoneGroup == BONE_GROUP_LEGSFEET)
		return IsBoneVisible(true, Bone, TargetBonePos, LocalEyePos, tr, pPlayer, bforcehitgroup, forcehitgroup, dmg, localangles);

	//I rarely comment but I am going to comment this so you can understand what is going on.

	//Set bounding corners of the hitboxes
	Vector vMax = TempHitBoxPos[0];
	Vector vMin = TempHitBoxPos[1];

	//Set middle of hitbox
	Vector Middle = (vMin + vMax) * 0.5f;
	Vector Point = Middle;
	float pointscale = PointScaleTxt.flValue;

	float diffz = abs(vMax.z - vMin.z)* 1.1;
	vMax.z = Middle.z + (diffz * pointscale);
	vMin.z = Middle.z - (diffz * pointscale);

	float multiplier = 1.1f;
	//Head u want to go a little wider
	if (Bone == HITBOX_HEAD)
		multiplier = 4;
	float diffx = abs(vMax.x - vMin.x)* multiplier;
	if (diffx > 4 && Bone == HITBOX_HEAD)
		diffx /= 2;
	vMax.x = Middle.x + (diffx * pointscale);
	vMin.x = Middle.x - (diffx * pointscale);

	float diffy = abs(vMax.y - vMin.y)* multiplier;
	if (diffy > 4 && Bone == HITBOX_HEAD)
		diffy += (diffy - 4) * 0.5;
	vMax.y = Middle.y + (diffy * pointscale);
	vMin.y = Middle.y - (diffy * pointscale);


	float bestdamage = 0;
	bool shouldmultipoint = false;
	if (Bone == HITBOX_HEAD || Bone == HITBOX_PELVIS || Bone == HITBOX_BODY
		|| Bone == HITBOX_RIGHT_THIGH || Bone == HITBOX_LEFT_THIGH || Bone == HITBOX_CHEST)
	{
		shouldmultipoint = true;
	}

	//Change for loop of effectiveness of multipoint

	int forloop = 4;
#if 0
	if (AimbotAimHeadNeckChk.Checked && !AimbotAimTorsoChk.Checked &&
		!AimbotAimLegsFeetChk.Checked && !AimbotAimArmsHandsChk.Checked)
		forloop = 16;
#endif

	if (shouldmultipoint)
	{
		int bestpoint = -1;
		for (int i = 0; i < forloop; i++)
		{
			switch (i)
			{
			case middle:
				Point = (vMin + vMax) * 0.5f;
				break;
			case Bottom:
				Point = (vMin + vMax) * 0.5f;
				Point.z = vMin.z;
				break;
			case Top:
				Point = (vMin + vMax) * 0.5f;
				Point.z = vMax.z;
				break;
			case TopLeftForward:
				Point = vMax;
				break;
			case TopLeftBackwards:
				Point = vMax;
				Point.x = vMin.x;
				break;
			case TopRightForward:
				Point = vMax;
				Point.y = vMin.y;
				break;
			case TopRightBackwards:
				Point.z = vMax.z;
				Point.y = vMin.y;
				Point.x = vMin.x;
				break;
			case BottomLeftForward:
				Point = vMin;
				break;
			case BottomLeftBackwards:
				Point = vMin;
				Point.x = vMax.x;
				break;
			case BottomRightForward:
				Point = vMin;
				Point.y = vMax.y;
				break;
			case BottomRightBackwards:
				Point.z = vMin.z;
				Point.y = vMax.y;
				Point.x = vMax.x;
				break;
			case Plus1:
				Point = (vMin + vMax) * 0.5f;
				Point.z = vMax.z + 1;
				break;
			case Plus2:
				Point = (vMin + vMax) * 0.5f;
				Point.z = vMax.z + 2;
				break;
			case Plus25:
				Point = (vMin + vMax) * 0.5f;
				Point.z = vMax.z + 2.5;
				break;
			case Plus4:
				Point = (vMin + vMax) * 0.5f;
				Point.z = Middle.z + 4;
				break;
			case Plus5:
				Point = (vMin + vMax) * 0.5f;
				Point.z = Middle.z + 5;
				break;

			}
			float damage = 0;

			if (IsBoneVisible(true, Bone, Point, LocalEyePos, tr, pPlayer, bforcehitgroup, forcehitgroup, damage, localangles))
			{

				if (!AutoBoneBestDamageChk.Checked)
				{
					return true;
				}

				if (damage > bestdamage)
				{
					float TargetHealth = pPlayer->GetHealth();

					if (TargetHealth - dmg <= 0.0f)
						return true;

					bestpoint = i;
					bestdamage = damage;
					TargetBonePos = Point;
				}
			}

		}
	}
	else
	{
		return IsBoneVisible(true, Bone, TargetBonePos, LocalEyePos, tr, pPlayer, bforcehitgroup, forcehitgroup, dmg, localangles);
	}
	if (bestdamage <= 0.0f)
		return false;

	dmg = bestdamage;
	return true;
}
#else
bool FindMultiPointVisible(int Bone, bool OnlyAutoWall, Vector &TargetBonePos, Vector &LocalEyePos, trace_t &tr, CBaseEntity* pPlayer, bool bforcehitgroup, int forcehitgroup, float& dmg, QAngle *localangles, bool HittingTargetFPS)
{
	float TargetHealth = pPlayer->GetHealth();

	//Todo: rewrite function in a way that multipoint falls through and non-multipoint gets jumped out for faster performance
	if (!AimbotMultiPointChk.Checked || (AimbotMultiPointPrioritiesOnlyChk.Checked && !AllPlayers[pPlayer->index].Personalize.PriorityTarget))
	{
		return IsBoneVisible(true, Bone, TargetBonePos, LocalEyePos, tr, pPlayer, bforcehitgroup, forcehitgroup, dmg, localangles) && ((TargetHealth - dmg <= 0) || dmg >= MinimumDamageBeforeFiringTxt.flValue);
	}

	BONE_GROUPS BoneGroup = MTargetting.GetBoneGroup(Bone);

	bool IsHead = BoneGroup == BONE_GROUP_HEADNECK;

	if (IsHead)
	{
		if (!AimbotMultiPointHeadNeckChk.Checked)
			return IsBoneVisible(true, Bone, TargetBonePos, LocalEyePos, tr, pPlayer, bforcehitgroup, forcehitgroup, dmg, localangles) && ((TargetHealth - dmg <= 0) || dmg >= MinimumDamageBeforeFiringTxt.flValue);
	}
	else if (BoneGroup == BONE_GROUP_CHESTTORSOPELVIS)
	{
		if (!AimbotMultiPointChestTorsoPelvisChk.Checked)// || Bone == HITBOX_PELVIS || Bone == HITBOX_BODY)
			return IsBoneVisible(true, Bone, TargetBonePos, LocalEyePos, tr, pPlayer, bforcehitgroup, forcehitgroup, dmg, localangles) && ((TargetHealth - dmg <= 0) || dmg >= MinimumDamageBeforeFiringTxt.flValue);
	}
	else if (BoneGroup == BONE_GROUP_SHOULDERARMSHANDS)
	{
		if (!AimbotMultiPointShoulderArmsHandsChk.Checked
			|| (!HittingTargetFPS && MTargetting.HitboxIsArmOrHand(Bone)))
			return IsBoneVisible(true, Bone, TargetBonePos, LocalEyePos, tr, pPlayer, bforcehitgroup, forcehitgroup, dmg, localangles) && ((TargetHealth - dmg <= 0) || dmg >= MinimumDamageBeforeFiringTxt.flValue);
	}
	else
	{
		//BONE_GROUP_LEGSFEET
		if (!AimbotMultiPointLegsFeetChk.Checked
			|| (!HittingTargetFPS && MTargetting.HitboxIsLegOrFoot(Bone)))
			return IsBoneVisible(true, Bone, TargetBonePos, LocalEyePos, tr, pPlayer, bforcehitgroup, forcehitgroup, dmg, localangles) && ((TargetHealth - dmg <= 0) || dmg >= MinimumDamageBeforeFiringTxt.flValue);
	}

	float BestDamage = 0;
	Vector BestBonePos;

	Vector centerpos = TargetBonePos;

	const bool AllowAutoWall = true;

	if (IsBoneVisible(AllowAutoWall, Bone, TargetBonePos, LocalEyePos, tr, pPlayer, bforcehitgroup, forcehitgroup, dmg, localangles))
	{
		if (!AutoBoneBestDamageChk.Checked)
		{
			if (dmg >= MinimumDamageBeforeFiringTxt.flValue)
			{
				return true;
			}
		}

		if (TargetHealth - dmg <= 0.0f)
			return true;

		if (dmg >= MinimumDamageBeforeFiringTxt.flValue)
		{
			BestDamage = dmg;
			BestBonePos = TargetBonePos;
		}
	}

	TargetBonePos = centerpos;

	if (IsHead)
	{
		//Test nudging the head Z position first 

		//Try to see if aiming higher will hit the head
		//TargetBonePos.z += iterations == 1 ? 6.0f : 3.0f;
		TargetBonePos.z += 5.0f;
#ifdef _DEBUG
		if (DrawAimbotBoneChk.Checked && EXTERNAL_WINDOW)
		{
			Vector screen;
			WorldToScreen(TargetBonePos, screen);
			DrawBox(Vector2D(screen.x, screen.y), 5, 5, 2, 0, 255, 0, 255);
		}
#endif
		if (IsBoneVisible(AllowAutoWall, Bone, TargetBonePos, LocalEyePos, tr, pPlayer, bforcehitgroup, forcehitgroup, dmg, localangles))
		{
			if (!AutoBoneBestDamageChk.Checked)
			{
				if (dmg >= MinimumDamageBeforeFiringTxt.flValue)
				{
					return true;
				}
			}
			if (dmg > BestDamage)
			{
				if (TargetHealth - dmg <= 0.0f)
					return true;
				if (dmg >= MinimumDamageBeforeFiringTxt.flValue)
				{
					BestDamage = dmg;
					BestBonePos = TargetBonePos;
				}
			}
		}

		TargetBonePos = centerpos;

		//Try to see if aiming higher will hit the head
		TargetBonePos.z += 5.75f; //Top of head shouldn't be pointscaled to be honest, unless we had a separate head/body pointscale
								  //Vector screen;
								  //WorldToScreen(TargetBonePos, screen);
								  //DrawBox(Vector2D(screen.x, screen.y), 2, 2, 1, 255, 0, 0, 255);
		if (IsBoneVisible(AllowAutoWall, Bone, TargetBonePos, LocalEyePos, tr, pPlayer, bforcehitgroup, forcehitgroup, dmg, localangles))
		{
			if (!AutoBoneBestDamageChk.Checked)
			{
				if (dmg >= MinimumDamageBeforeFiringTxt.flValue)
				{
					return true;
				}
			}
			if (dmg > BestDamage)
			{
				if (TargetHealth - dmg <= 0.0f)
					return true;
				if (dmg >= MinimumDamageBeforeFiringTxt.flValue)
				{
					BestDamage = dmg;
					BestBonePos = TargetBonePos;
				}
			}
		}

		//Try nudging different directions
		TargetBonePos = centerpos;

		float BestNudgedDamage = BestDamage;
		Vector BestNudgedBonePos;
		if (IsNudgedBoneVisible(2.2f * PointScaleTxt.flValue, AllowAutoWall, Bone, TargetBonePos, LocalEyePos, tr, pPlayer, bforcehitgroup, forcehitgroup, dmg, BestNudgedDamage, BestNudgedBonePos, localangles, TargetHealth))
		{
			if (!AutoBoneBestDamageChk.Checked)
			{
				if (dmg >= MinimumDamageBeforeFiringTxt.flValue)
				{
					return true;
				}
			}

			if (BestNudgedDamage > BestDamage)
			{
				if (TargetHealth - BestNudgedDamage <= 0.0f)
				{
					TargetBonePos = BestNudgedBonePos;
					dmg = BestNudgedDamage;
					return true;
				}

				if (dmg >= MinimumDamageBeforeFiringTxt.flValue)
				{
					BestDamage = BestNudgedDamage;
					BestBonePos = BestNudgedBonePos;
				}
			}
		}
	}

	//Try nudging different directions left/right

	TargetBonePos = centerpos;

	float NewNudge;

	switch (Bone)
	{
	case HITBOX_UPPER_CHEST:
		NewNudge = 8.0f * PointScaleTxt.flValue;
		break;
	case HITBOX_THORAX:
		NewNudge = 5.5f * PointScaleTxt.flValue;
		break;
	case HITBOX_PELVIS:
		NewNudge = 4.8f * PointScaleTxt.flValue;
		break;
	case HITBOX_LEFT_FOREARM:
	case HITBOX_RIGHT_FOREARM:
		NewNudge = 1.8f * PointScaleTxt.flValue;
		break;
	case HITBOX_LEFT_CALF:
	case HITBOX_RIGHT_CALF:
	case HITBOX_LEFT_FOOT:
	case HITBOX_RIGHT_FOOT:
		NewNudge = 2.3f * PointScaleTxt.flValue;
		break;
	default:
		NewNudge = 0.0f;
	}

	if (NewNudge != 0.0f)
	{
		float BestNudgedDamage = BestDamage;
		Vector BestNudgedBonePos;

		if (IsNudgedBoneVisible(NewNudge, AllowAutoWall, Bone, TargetBonePos, LocalEyePos, tr, pPlayer, bforcehitgroup, forcehitgroup, dmg, BestNudgedDamage, BestNudgedBonePos, localangles, TargetHealth))
		{
			if (!AutoBoneBestDamageChk.Checked)
			{
				if (dmg >= MinimumDamageBeforeFiringTxt.flValue)
				{
					return true;
				}
			}

			if (BestNudgedDamage > BestDamage)
			{
				if (TargetHealth - BestNudgedDamage <= 0.0f)
				{
					TargetBonePos = BestNudgedBonePos;
					dmg = BestNudgedDamage;
					return true;
				}

				if (dmg >= MinimumDamageBeforeFiringTxt.flValue)
				{
					BestDamage = BestNudgedDamage;
					BestBonePos = BestNudgedBonePos;
				}
			}
		}
	}

	if (BestDamage <= 0.0f)
		return false;

	TargetBonePos = BestBonePos;
	dmg = BestDamage;
	return true;
}
#endif


CPlayerTarget TPlayers[4096];
int IndexOfClosest = INVALID_PLAYER; //Index of the closest player to the crosshair, used with AllPlayers, not streamed
int LastCheckedIndex = 0;

bool FindNewAimbotTarget(QAngle *localangles, CBaseCombatWeapon *weapon, Vector& BestBonePos, bool& RanHitChance, bool& CanTraceLineToATarget)
{
	ClearTargettedBone();
	Vector LocalEyePos;

#if 0
	if (ResolverChk.Checked)
	{
		LocalEyePos = pCPlayer->Backtracked ? pCPlayer->LastUpdatedNetVars.LocalVars.LocalEyePos : pCPlayer->SimulationTimeNetVars.LocalVars.LocalEyePos;
	}
	else
#endif
	{
		LocalEyePos = LocalPlayer.Entity->GetEyePosition();
	}

	//First, trace directly forward to see if we are looking directly at an enemy
	//This allows the player to force target this player instead of the letting the aimbot decide, and ignores priority targets

	Vector vecDir;
	AngleVectors(*localangles, &vecDir);
	VectorNormalizeFast(vecDir);

	//WeaponInfo_t*(__thiscall* OriginalFn)(void*);
	//WeaponInfo_t* shit = (*(WeaponInfo_t***)weapon)[455];

	float flMaxTraceRange = weapon ? weapon->GetCSWpnData()->flRange : 8192;

	Vector EndPos = LocalEyePos + vecDir * flMaxTraceRange;

	Vector HitPos;
	float dmg;
	int hitgroup = 0;

	CBaseEntity *pAimingAt = MTargetting.TraceToPosition(LocalEyePos, &EndPos, &vecDir, localangles, weapon, &dmg, hitgroup, &HitPos);

	if (pAimingAt && MTargetting.IsPlayerAValidTarget(pAimingAt))
	{
		//We found an enemy directly forwards, so target them!
		MTargetting.bAimAtpSilentAngles = false; //Reset for each player checked
		int index = pAimingAt->index;
		CustomPlayer *pCPlayer = &AllPlayers[index];
		int BestBone = !pSilentChk.Checked ? HITBOX_HEAD : MTargetting.HitgroupToHitbox(hitgroup);
		if (pSilentChk.Checked || gAutoBone.Run_AutoBone(weapon, pCPlayer, localangles, LocalEyePos, BestBone, BestBonePos, RanHitChance, CanTraceLineToATarget))
		{
			if (pSilentChk.Checked)
			{
				BestBonePos = HitPos;
				CanTraceLineToATarget = true;
				RanHitChance = false;
			}
			else if (OnlyHitchanceInTriggerbotChk.Checked)
				RanHitChance = false;
			IndexOfClosest = INVALID_PLAYER;
			LastTargetIndex = index;
			LastBoneChecked = BestBone;
			MTargetting.SetHitboxGroup();
			MTargetting.SetHitChanceFlags();
			if (LastTargetIndex == INVALID_PLAYER || pCPlayer->Index != LastTargetIndex)
			{
				//Only clear per-target data if this is a new target
				ClearLegitAimStepVars();
			}
			return true;
		}
	}

	//Nothing directly straight forward, so search around for a target

	if (weapon && pSilentChk.Checked)
	{
		CBaseEntity *pEntityHit = nullptr;
		if (gpSilent.TraceOutFromCrosshair(pEntityHit, MTargetting.IdealpSilentAngles, BestBonePos, dmg, localangles, LocalEyePos, weapon, hitgroup))
		{
			CanTraceLineToATarget = true;
			LastBoneChecked = MTargetting.HitgroupToHitbox(hitgroup);
			MTargetting.SetHitboxGroup();
			MTargetting.SetHitChanceFlags();
			MTargetting.bAimAtpSilentAngles = true;
			LastTargetIndex = pEntityHit->index;
			IndexOfClosest = INVALID_PLAYER;
			LastCheckedIndex = 0;
			gTriggerbot.SetAllowedToFire(true);
			return true;
		}
	}

	//Get the max number of players we should attempt to target this tick
	int maxcheckindex = min(LastCheckedIndex + MaxTargetsPerTickTxt.iValue, NumStreamedPlayers);
	bool AllowAnyFOV = AimbotFOVTxt.flValue >= 180.0f;

	for (LastCheckedIndex; LastCheckedIndex < maxcheckindex; LastCheckedIndex++)
	{
		CustomPlayer *pCPlayer = StreamedPlayers[LastCheckedIndex];

		if (pCPlayer && pCPlayer->IsTarget && pCPlayer->LifeState == LIFE_ALIVE)
		{
			MTargetting.bAimAtpSilentAngles = false; //Reset for each player checked

			CBaseEntity* pPlayer = pCPlayer->BaseEntity;
			//TPlayers is a temporary list of players used for storage during targetting
			CPlayerTarget *pTPlayer = &TPlayers[LastCheckedIndex];

#if 1
			float BoneTargetTime = Interfaces::Globals->curtime;
#else
			float BoneTargetTime;
			StoredNetvars *BestTick = pCPlayer->GetBestBacktrackTick();
			if (BestTick)
				BoneTargetTime = BestTick->simulationtime;
			else
				BoneTargetTime = pPlayer->GetSimulationTime();
#endif

			//Find a bone that is inside the FOV
			for (int b = 0; b < (sizeof(HITBOXES_ALL_BASIC) / sizeof(HITBOXES_ALL_BASIC[0])); b++)
			{
				int hitbox = HITBOXES_ALL_BASIC[b];
				pTPlayer->BonePos = pPlayer->GetBonePosition(hitbox, BoneTargetTime, false, true);
				RanHitChance = false;
				float FOV = GetFovFromCoordsRegardlessofDistance(LocalEyePos, pTPlayer->BonePos, LocalPlayer.CurrentEyeAngles);

				if (AllowAnyFOV || FOV <= AimbotFOVTxt.flValue)
				{
					//Bone is inside the FOV! Now find out the best bone to target
					bool LineHitThisTarget = false;
					int BestBone = HITBOX_HEAD;
					if (gAutoBone.Run_AutoBone(weapon, pCPlayer, localangles, LocalEyePos, BestBone, pTPlayer->BonePos, RanHitChance, LineHitThisTarget))
					{
						if (OnlyHitchanceInTriggerbotChk.Checked)
							RanHitChance = false;

						FOV = GetFovFromCoordsRegardlessofDistance(LocalEyePos, pTPlayer->BonePos, LocalPlayer.CurrentEyeAngles);

						bool ExistingPlayer = IndexOfClosest != INVALID_PLAYER;

						if (!ExistingPlayer || FOV < TPlayers[IndexOfClosest].DistanceFromCrosshair)
						{
							if (!pCPlayer->Personalize.PriorityTarget && ExistingPlayer && TPlayers[IndexOfClosest].PriorityTarget)
							{
								if (!CanTraceLineToATarget)
									CanTraceLineToATarget = LineHitThisTarget;
								break; //Todo: CanFireAtLagCompensatedTarget
							}
							pTPlayer->bAimAtpSilentAngles = MTargetting.bAimAtpSilentAngles;
							pTPlayer->IdealpSilentAngles = MTargetting.IdealpSilentAngles;
							pTPlayer->DistanceFromCrosshair = FOV;
							pTPlayer->Bone = BestBone;
							pTPlayer->RanHitChance = RanHitChance;
							pTPlayer->PriorityTarget = pCPlayer->Personalize.PriorityTarget;
							IndexOfClosest = LastCheckedIndex;
							//Now reset bone checks for next player
							ClearTargettedBone();
						}
					}
					if (!CanTraceLineToATarget)
						CanTraceLineToATarget = LineHitThisTarget;
					if (OnlyHitchanceInTriggerbotChk.Checked)
						RanHitChance = false;
					break;
				}
			}
		}
		else
		{
			//dont count dead or teammates
			maxcheckindex = min(maxcheckindex + 1, NumStreamedPlayers);
		}
	}

	if (LastCheckedIndex >= NumStreamedPlayers)
	{
		LastCheckedIndex = 0;

		if (IndexOfClosest != INVALID_PLAYER && IndexOfClosest < NumStreamedPlayers)
		{
			//We got a target!
			int NewTargetIndex = StreamedPlayers[IndexOfClosest]->Index;

			if (LastTargetIndex == INVALID_PLAYER || NewTargetIndex != LastTargetIndex)
			{
				//Only clear per-target data if this is a new target
				ClearLegitAimStepVars();
			}
			if (StreamedPlayers[IndexOfClosest]->IsTarget)
			{
				LastTargetIndex = NewTargetIndex;
				CPlayerTarget *pTPlayer = &TPlayers[IndexOfClosest];
				LastBoneChecked = pTPlayer->Bone;
				BestBonePos = pTPlayer->BonePos;
				RanHitChance = pTPlayer->RanHitChance;
				MTargetting.bAimAtpSilentAngles = pTPlayer->bAimAtpSilentAngles;
				MTargetting.IdealpSilentAngles = pTPlayer->IdealpSilentAngles;
				MTargetting.SetHitboxGroup();
				MTargetting.SetHitChanceFlags();

#if 1
#ifdef _DEBUG
				CustomPlayer *pCPlayer = &AllPlayers[LastTargetIndex];
				wchar_t name[64];
				GetPlayerName(GetRadar(), pCPlayer->Index, name);
				int f = 0;
#endif
#endif

				IndexOfClosest = INVALID_PLAYER;
				return true;
			}
			else
			{
				//Target is no longer valid because StreamedPlayers struct changed, so reset
				LastTargetIndex = INVALID_PLAYER;
				IndexOfClosest = INVALID_PLAYER;
			}
		}
		else
		{
			//Couldn't find a target, so clear the last index
			LastTargetIndex = INVALID_PLAYER;
			IndexOfClosest = INVALID_PLAYER;
		}
	}
	RanHitChance = false;
	return false;
}

//Reset targetted bone
void ClearTargettedBone()
{
	LastBoneCheckIndex = 0; //Reset index to check
	LastBoneChecked = BONES_TO_CHECK[0]; //Get first, most important bone/hitbox
	MTargetting.SetHitboxGroup();
	MTargetting.SetHitChanceFlags();
}

//Reset the current target
void ClearAimbotTarget()
{
	ClearTargettedBone();
#if 0
	//Used with june 3 autobone
	if (LastTargetIndex != INVALID_PLAYER)
	{
		CustomPlayer *pCPlayer = &AllPlayers[LastTargetIndex];
		pCPlayer->flTimeHeadWasVisibleButNotHittable = 0.0f;
		pCPlayer->flTimeBodyWasFirstVisible = 0.0f;
	}
#endif
	LastTargetIndex = INVALID_PLAYER;
	WaitForRePress = true;
}

char *aimbottargetstr = new char[14]{ 59, 19, 23, 24, 21, 14, 90, 46, 27, 8, 29, 31, 14, 0 }; /*Aimbot Target*/

																							  //Aims to the target, returns true if we are aiming directly at the target position
bool AimToTheTarget(CBaseCombatWeapon* weapon, int buttons, QAngle &angles, QAngle* LastAngle, Vector LocalEyePos, Vector& TargetBonePos, CustomPlayer* pCPlayer, bool& AimbotModifiedAngles, int PressingTriggerbotKey)
{
	if (!pCPlayer->BaseEntity)
		return false;

	if (weapon && !weapon->GetCSWpnData())
		return false;

	CBaseEntity *pEntity = pCPlayer->BaseEntity;
	LocalPlayer.LastAimbotBonePosition = TargetBonePos;
	LocalPlayer.LastAimbotCurrentFeetYawClamped = ClampYr(pEntity->GetCurrentFeetYaw());
	LocalPlayer.LastAimbotGoalFeetYawClamped = ClampYr(pEntity->GetGoalFeetYaw());
	LocalPlayer.LastAimbotLowerBodyYaw = ClampYr(pEntity->GetLowerBodyYaw());

	if (DrawAimbotBoneChk.Checked)
	{
		//Draw the hitbox
		if (!EXTERNAL_WINDOW)
		{
			DecStr(aimbottargetstr, 13);
			Interfaces::DebugOverlay->AddTextOverlay(Vector(TargetBonePos.x, TargetBonePos.y, TargetBonePos.z - 1.0f), 0.05f, aimbottargetstr);
			EncStr(aimbottargetstr, 13);
			Interfaces::DebugOverlay->AddBoxOverlay(TargetBonePos, Vector(-2, -2, -2), Vector(1, 1, 1), angles, 255, 0, 0, 255, 0.05f);
		}
		else
		{
			Vector screen;
			WorldToScreen(TargetBonePos, screen);
			DrawBox(Vector2D(screen.x, screen.y), 5, 5, 2, 255, 127, 39, 255);
		}
	}

	//	static float NextTime_MoveMouse = 0.0f;
	//	if (Time >= NextTime_MoveMouse)
	//	{
	//Found line of sight to target! Aim at the target!
#if 0
	if (LegitAimStepChk.Checked)
	{
		ExtrapolateOrigin(&TargetBonePos, TargetBonePos, &pCPlayer->BaseEntity->GetVelocity(), 0.025f);
	}
#endif

	//If we are spinning or antiaiming and we want to spin between bullets fired, return
	if ((!NoAntiAimsAreOn(false)) && SpinBetweenShotsChk.Checked)
	{
		//FIXME: turning off autofire makes autostop/autoscope not work
		if (!gTriggerbot.WeaponCanFire(weapon) || (!AutoFireChk.Checked && !PressingTriggerbotKey) || Time < gTriggerbot.NextTimeTriggerBot || (ReadByte((uintptr_t)&weapon->GetCSWpnData()->bFullAuto) == 0 && LocalPlayer.Entity->GetShotsFired() > 0))
			return false;
	}

	StoredNetvars *BestTick = pCPlayer->GetBestBacktrackTick();
	StoredNetvars *CurrentTick = pCPlayer->Personalize.m_PlayerRecords.size() ? pCPlayer->Personalize.m_PlayerRecords.front() : nullptr;;

	if (ResolverChk.Checked && ((BestTick && !BestTick->ReceivedPoseParametersAndAnimations)
		//
		|| (pCPlayer->TicksChoked > 0 && !pCPlayer->Backtracked && CurrentTick && CurrentTick->FireAtPelvis && (Interfaces::Globals->curtime - pCPlayer->TimeLowerBodyUpdated) < LowerBodyWaitTimeBeforeFiringTxt.flValue)
		|| (pCPlayer->Personalize.ShouldResolve() && DontShootAfterXMissesTxt.iValue && pCPlayer->Personalize.ShotsMissed > DontShootAfterXMissesTxt.iValue)
		))
		//NOTE: NEED TO FULLY TEST BEFORE PUTTING INTO PRODUCTION
		//|| (pCPlayer->TicksChoked > 0 && !pCPlayer->Backtracked && CurrentTick && CurrentTick->FireAtPelvis && (Interfaces::Globals->curtime - pCPlayer->TimeLowerBodyUpdated) < LowerBodyWaitTimeBeforeFiringTxt.flValue)))
	{
		return false;
	}

#ifdef _DEBUG
	//#define FIRE_ONLY_WHEN_ENEMIES_SHOOT
#ifdef FIRE_ONLY_WHEN_ENEMIES_SHOOT
	//Fire only at shot backtrack ticks
	if (FireWhenEnemiesShootChk.Checked && (!BestTick || BestTick != pCPlayer->LatestFiringTick))
		return false;
#endif
#endif

	auto stat = START_PROFILING("AimToTheTarget");

	QAngle OldAngles;
	if (SilentAimChk.Checked)
	{
		QAngle LA = *LastAngle;
		angles = LA;
		OldAngles = LA;
	}
	else
	{
		OldAngles = angles;
	}

	AimbotModifiedAngles = true;
	QAngle TargetAngles;
	Vector TargetBonePosOvershoot;

	if (LegitAimStepChk.Checked)
	{
		Vector vForward, vRight, vUp;
		AngleVectors(angles, &vForward, &vRight, &vUp);

		QAngle dt = (CalcAngle(LocalEyePos, TargetBonePos) - angles);
		dt.Clamp();

		if (!InitializedLegitAimStepDirection)
		{
			LegitAimStartDirection = dt.y > 0 ? AimDirection::LEFT : AimDirection::RIGHT;
			LegitAimStepInitialDelta = dt;
			InitializedLegitAimStepDirection = true;
		}

		if (ReachedLegitAimStepTarget)
		{
			if (fabsf(dt.y) > 15.0f || fabsf(dt.x) > 15.0f)
			{
				ClearLegitAimStepVars();
			}
		}

		static QAngle BoneNudgeAmt = { 0,0,0 };

		if (ReachedLegitAimStepTarget)
		{
			float overshootyaw = (pCPlayer->BaseEntity->GetOrigin() - LocalPlayer.Entity->GetOrigin()).Length() < 300.0f ? 0.01f : 0.5f;
			TargetAngles = CalcAngle(LocalEyePos, pCPlayer->BaseEntity->GetVelocity().Length() > 0.1f ? (TargetBonePos + (vRight * BoneNudgeAmt.y * overshootyaw) + (vUp * BoneNudgeAmt.x)) : (TargetBonePos + (vRight * (BoneNudgeAmt.y * overshootyaw)) + (vUp * BoneNudgeAmt.x))); //Clamped in functions
		}
		else
		{
			BoneNudgeAmt = { SORandomFloat(-1.1f, 1.1f), SORandomFloat(-1.1f, 1.1f), 0.0f };
			if (LegitAimStartDirection == AimDirection::RIGHT)
			{
				float overshootamt = (fabsf(LegitAimStepInitialDelta.y) * 0.2f);
				if (overshootamt < 5.0f)
					overshootamt += 5.0f;
				TargetBonePosOvershoot = TargetBonePos + vRight * overshootamt;
			}
			else
			{
				float overshootamt = (fabsf(LegitAimStepInitialDelta.y) * 0.2f);
				if (overshootamt < 5.0f)
					overshootamt += 5.0f;
				TargetBonePosOvershoot = TargetBonePos - vRight * overshootamt;
			}
			TargetAngles = CalcAngle(LocalEyePos, TargetBonePosOvershoot); //Clamped in functions
		}
		//Only change angles if angle is a big enough difference, so we don't perfectly aim at player breathing anims
		//if ((TargetAngles - angles).Length() > 0.1f)
		//	angles = TargetAngles;
	}
	else
	{
		if (!MTargetting.bAimAtpSilentAngles)
			TargetAngles = CalcAngle(LocalEyePos, TargetBonePos); //Clamped in functions
		else
			TargetAngles = MTargetting.IdealpSilentAngles;
	}

	if (IsAllowedToNoRecoil(LocalPlayer.CurrentWeapon))
	{
		QAngle LocalPlayerPunch = LocalPlayer.Entity->GetPunch();
		LocalPlayer.AimbotAimPunchSubtracted.x = (LocalPlayerPunch.x * 2.0f) * NoRecoilYPercentTxt.flValue * 0.01f;
		LocalPlayer.AimbotAimPunchSubtracted.y = (LocalPlayerPunch.y * 2.0f) * NoRecoilXPercentTxt.flValue * 0.01f;
		TargetAngles -= LocalPlayer.AimbotAimPunchSubtracted;
	}

#ifdef _DEBUG
	//Draw the end position for debugging recoil
	if (EXTERNAL_WINDOW)
	{
		Vector vForward;
		AngleVectors(TargetAngles, &vForward);
		VectorNormalizeFast(vForward);
		trace_t tr;
		UTIL_TraceLine(LocalPlayer.Entity->GetEyePosition(), LocalPlayer.Entity->GetEyePosition() + vForward * 4096.0f, MASK_SHOT, LocalPlayer.Entity, &tr);
		Vector screen;
		if (WorldToScreen(tr.endpos, screen))
		{
			//pSprite->Begin(D3DXSPRITE_ALPHABLEND | D3DXSPRITE_DO_NOT_ADDREF_TEXTURE);
			DrawBox(Vector2D(screen.x - 3, screen.y - 3), 5, 5, 2, 0, 0, 255, 255);
			DrawStringUnencrypted("Ideal Angles", Vector2D(screen.x - 3, screen.y + 5), ColorRGBA(0, 0, 255, 255), pFont);
			//pSprite->End();
		}
	}
#endif

	if (LegitAimStepChk.Checked)
	{
		angles = TargetAngles; //Must set this or else it will completely break the RCS
							   //Don't want to scale past 1
		static float LastAimStepTime = 0.0f;
		if (LastAimStepTime == 0.0f)
			LastAimStepTime = Time;
		float AimStepFrameTime = fmin(Time - LastAimStepTime, 0.1f);
		LastAimStepTime = Time;
		m_CurAimTime += AimStepFrameTime;
		float fov = fmaxf(0.001f, GetFov(OldAngles, TargetAngles));
		float zoomsub = GetZoomLevel(LocalPlayer.Entity) ? 0.05f : 0.0f;

		float SmoothTime = m_CurAimTime * (AimbotSpeedTxt.flValue - zoomsub) + (ReachedLegitAimStepTarget ? 0.1f / fov : 0.33f / fov);

		if (ReachedLegitAimStepTarget)
		{
			float LocalPlayerVelLen = LocalPlayer.Entity->GetVelocity().Length();
			if (LocalPlayerVelLen > 0.0f)
			{
				SmoothTime *= SORandomFloat(0.35f, 0.45f);
			}
			else if (pCPlayer->BaseEntity->GetVelocity().Length() > 0.1f)
			{
				SmoothTime *= SORandomFloat(0.45f, 0.75f);
			}
		}

		if (SmoothTime > 0.92f)
			SmoothTime = 1.0f;

		CUserCmd *cmd = CurrentUserCmd.cmd;

		if (abs(cmd->mousedx) > 2 || abs(cmd->mousedy) > 2)
		{
			SmoothTime *= 0.25f;
			m_CurAimTime = fmaxf(0.0f, m_CurAimTime - (AimStepFrameTime * 2.0f));
		}

		SmoothAngle(OldAngles, angles, SmoothTime);
		{
			QAngle delta_angle = (angles - OldAngles);
			NormalizeAngles(delta_angle);
			if (LegitAimCurveType == 0)
			{
				//+ = under - = over
				if (delta_angle.y > 0.4f)
				{
					float inc = SORandomFloat(0.265f, 0.291f) - fmin(0.0f, (1.f / fov)) - SmoothTime;
					if (LegitAimStepInitialDelta.x < 0)
						angles.x -= inc;
					else
						angles.x += inc;
					angles.ClampX();
				}
				else if (delta_angle.y < -0.4f)
				{
					float inc = SORandomFloat(0.252f, 0.294f) - fmin(0.0f, (1.f / fov)) - SmoothTime;
					if (LegitAimStepInitialDelta.x < 0)
						angles.x -= inc;
					else
						angles.x += inc;
					angles.ClampX();
				}
				else if (delta_angle.y > 0.2f)
				{
					float inc = SORandomFloat(-0.002f, 0.0385f) - fmin(0.0f, (1.f / fov)) - SmoothTime;
					if (LegitAimStepInitialDelta.x < 0)
						angles.x -= inc;
					else
						angles.x += inc;
					angles.ClampX();
				}
				else if (delta_angle.y < -0.2f)
				{
					float inc = SORandomFloat(-0.00212f, 0.032f) - fmin(0.0f, (1.f / fov)) - SmoothTime;
					if (LegitAimStepInitialDelta.x < 0)
						angles.x -= inc;
					else
						angles.x += inc;
					angles.ClampX();
				}
				else
				{
					ReachedLegitAimStepTarget = true;
				}
			}
			else if (LegitAimCurveType == 1)
			{
				//+ = under - = over
				if (delta_angle.y > 0.4f)
				{
					float inc = SORandomFloat(0.265f, 0.331f) - fmin(0.0f, (1.f / fov)) - SmoothTime;
					if (LegitAimStepInitialDelta.x < 0)
						angles.x -= inc;
					else
						angles.x += inc;
					angles.ClampX();
				}
				else if (delta_angle.y < -0.4f)
				{
					float inc = SORandomFloat(0.252f, 0.324f) - fmin(0.0f, (1.f / fov)) - SmoothTime;
					if (LegitAimStepInitialDelta.x < 0)
						angles.x -= inc;
					else
						angles.x += inc;
					angles.ClampX();
				}
				else if (delta_angle.y > 0.2f)
				{
					float inc = SORandomFloat(-0.002f, 0.0385f) - fmin(0.0f, (1.f / fov)) - SmoothTime;
					if (LegitAimStepInitialDelta.x < 0)
						angles.x -= inc;
					else
						angles.x += inc;
					angles.ClampX();
				}
				else if (delta_angle.y < -0.2f)
				{
					float inc = SORandomFloat(-0.00212f, 0.032f) - fmin(0.0f, (1.f / fov)) - SmoothTime;
					if (LegitAimStepInitialDelta.x < 0)
						angles.x -= inc;
					else
						angles.x += inc;
					angles.ClampX();
				}
				else
				{
					ReachedLegitAimStepTarget = true;
				}
			}
			else if (LegitAimCurveType == 2)
			{
				//+ = under - = over
				if (delta_angle.y > 0.4f)
				{
					float inc = SORandomFloat(0.2f, 0.25f) - fmin(0.0f, (1.f / fov)) - SmoothTime;
					if (LegitAimStepInitialDelta.x < 0)
						angles.x -= inc;
					else
						angles.x += inc;
					angles.ClampX();
				}
				else if (delta_angle.y < -0.4f)
				{
					float inc = SORandomFloat(0.15f, 0.20f) - fmin(0.0f, (1.f / fov)) - SmoothTime;
					if (LegitAimStepInitialDelta.x < 0)
						angles.x -= inc;
					else
						angles.x += inc;
					angles.ClampX();
				}
				else if (delta_angle.y > 0.2f)
				{
					float inc = SORandomFloat(-0.005f, 0.018f) - fmin(0.0f, (1.f / fov)) - SmoothTime;
					if (LegitAimStepInitialDelta.x < 0)
						angles.x -= inc;
					else
						angles.x += inc;
					angles.ClampX();
				}
				else if (delta_angle.y < -0.2f)
				{
					float inc = SORandomFloat(-0.001f, 0.04f) - fmin(0.0f, (1.f / fov)) - SmoothTime;
					if (LegitAimStepInitialDelta.x < 0)
						angles.x -= inc;
					else
						angles.x += inc;
					angles.ClampX();
				}
				else
				{
					ReachedLegitAimStepTarget = true;
				}
			}
		}
#if 0
		if (ReachedLegitAimStepTarget || SmoothTime >= 0.99f)
		{
			//LegitAimStepIncTimeOverShoot += AimStepFrameTime;
			if (LegitAimStepIncTimeOverShoot > SORandomFloat(0.0125f, 0.05f))
			{
				//ClearLegitAimStepVars();
			}
		}
#endif
	}

	if (!AimbotAimXChk.Checked)
	{
		angles.y = OldAngles.y;
	}
	else if (RageAimStepChk.Checked)
	{
		QAngle delta_angle = (TargetAngles - OldAngles);// .Normalized();;
		NormalizeAngles(delta_angle);

		if (delta_angle.y > AimbotSpeedTxt.flValue)
			angles.y += AimbotSpeedTxt.flValue;
		else if (delta_angle.y < -AimbotSpeedTxt.flValue)
			angles.y -= AimbotSpeedTxt.flValue;
		else
			angles.y = TargetAngles.y;

		angles.ClampY();
	}
	else if (!LegitAimStepChk.Checked)
	{
		angles.y = TargetAngles.y;
	}

	if (!AimbotAimYChk.Checked)
	{
		angles.x = OldAngles.x;
	}
	else if (RageAimStepChk.Checked)
	{
		QAngle delta_angle = (TargetAngles - OldAngles);// .Normalized();;
		NormalizeAngles(delta_angle);

		if (delta_angle.x > AimbotSpeedTxt.flValue)
			angles.x += AimbotSpeedTxt.flValue;
		else if (delta_angle.x < -AimbotSpeedTxt.flValue)
			angles.x -= AimbotSpeedTxt.flValue;
		else
			angles.x = TargetAngles.x;

		angles.ClampX();
	}
	else if (!LegitAimStepChk.Checked)
	{
		angles.x = TargetAngles.x;
	}

	if (!SilentAimChk.Checked)
	{
		angles.Clamp();
		//Interfaces::Engine->SetViewAngles(angles);
	}

	//	if ((float)AimbotMouseMoveDelayTxt.iValue <= 0)
	//		NextTime_MoveMouse = Time;
	//	else
	//		NextTime_MoveMouse = Time + ((float)AimbotMouseMoveDelayTxt.iValue / 1000.0f);

	END_PROFILING(stat);

	if (angles != TargetAngles)
	{
		return false;
	}
	return true;
	//}
}

#include "player_lagcompensation.h"

//Returns true if we are aiming directly at the target position
bool Aimbot(QAngle& angles, DWORD* FramePointer, int &buttons, bool& AimbotModifiedAngles, QAngle LastAngle, CBaseCombatWeapon *weapon, int PressingTriggerbotKey, bool& RanHitChance)
{
	bool FindADifferentTarget = false; //find new target if we already have one
	Vector BestBonePos;
	bool FoundTargetThisTick = false;
	bool CanTraceLineToATarget = false; //Did we hit a line between us and any target, disregarding hitchance?

										//First check to see if the last targetted player is still valid
	if (LastTargetIndex != INVALID_PLAYER)
	{
		static float NextTimeCanRetarget = 0.0f;
		CustomPlayer *pCPlayer = &AllPlayers[LastTargetIndex];
		if (pCPlayer->Dormant || !pCPlayer->IsTarget || pCPlayer->LifeState != LIFE_ALIVE)
		{
			WaitForRePress = true;

			LastCheckedIndex = 0; //Reset tick based counters
			IndexOfClosest = INVALID_PLAYER;
			LastTargetIndex = INVALID_PLAYER; //Reset master 'current target'
		}
		else if (!AimbotHoldKeyChk.Checked && PressToRetargetChk.Checked && ((GetAsyncKeyState(AimbotKeyTxt.iValue) & (1 << 16)) || pSilentChk.Checked) && Time >= NextTimeCanRetarget)
		{
			//Player is auto-aiming and wants to re-target.
			//LastTargetIndex = INVALID_PLAYER;
			FindADifferentTarget = true;
			NextTimeCanRetarget = !pSilentChk.Checked ? Time + 0.1f : 0.0f;
		}
	}

	//FIXME: THIS PSILENT CHECK SHOULD NOT BE HERE, FIX LATER
	if (!pSilentChk.Checked && AimbotHoldKeyChk.Checked && !AimbotAutoTargetSwitchChk.Checked && WaitForRePress)
		return false; //If we are holding the aimkey and do not want auto-target switching, then just exit if we lost contact

	if (LastTargetIndex == INVALID_PLAYER || FindADifferentTarget)
	{
		//Find a target
		auto stat = START_PROFILING("FindNewAimbotTarget");
		FoundTargetThisTick = FindNewAimbotTarget(&angles, weapon, BestBonePos, RanHitChance, CanTraceLineToATarget);
		END_PROFILING(stat);
	}

	if (CanTraceLineToATarget && !FoundTargetThisTick)
	{
		if (AutoStopMovementChk.Checked && weapon && gTriggerbot.WeaponCanFire(weapon))
		{
			int id = weapon->GetItemDefinitionIndex();
			if (!weapon->IsKnife(id) && id != WEAPON_C4 && !weapon->IsGrenade(id))
			{
				LocalPlayer.StopPlayerMovement(CurrentUserCmd.cmd, buttons);
			}
		}
		if (AutoScopeSnipersChk.Checked && LocalPlayer.Entity && (GetZoomLevel(LocalPlayer.Entity) == 0) && (LocalPlayer.WeaponVars.IsSniper || LocalPlayer.WeaponVars.IsAutoSniper))
		{
			buttons |= IN_ATTACK2;
		}
	}

	if (LastTargetIndex != INVALID_PLAYER)
	{
		//Find a bone to aim at first

		CustomPlayer* pCPlayer = &AllPlayers[LastTargetIndex]; //Trust the pointer, we have enough checks above already
		CBaseEntity* LastTargetEntity = pCPlayer->BaseEntity;
		int BestBone;

		//See if we can find a bone we can hit. If not then find a new target

		//lagcomp->StartLagCompensation(LastTargetEntity, LagCompensationType::LAG_COMPENSATE_HITBOXES);

		Vector LocalEyePos;
#if 0
		if (ResolverChk.Checked)
		{
			LocalEyePos = !pCPlayer->Backtracked ? pCPlayer->SimulationTimeNetVars.LocalVars.LocalEyePos : pCPlayer->LastUpdatedNetVars.LocalVars.LocalEyePos;
		}
		else
#endif
		{
			LocalEyePos = LocalPlayer.Entity->GetEyePosition();
		}


		if (FoundTargetThisTick)
		{
			//Don't autobone twice to save tons of cpu
			return AimToTheTarget(weapon, buttons, angles, &LastAngle, LocalEyePos, BestBonePos, pCPlayer, AimbotModifiedAngles, PressingTriggerbotKey);
		}
		else if (gAutoBone.Run_AutoBone(weapon, pCPlayer, &angles, LocalEyePos, BestBone, BestBonePos, RanHitChance, CanTraceLineToATarget))
		{
			LastBoneChecked = BestBone;
			//SetLastBoneCheckIndex(BestBone);
			MTargetting.SetHitboxGroup();
			MTargetting.SetHitChanceFlags();
			if (OnlyHitchanceInTriggerbotChk.Checked)
				RanHitChance = false;
			//Now do the work of aiming at the target
			//We can hit this target, now attempt to aim at the player!
			return AimToTheTarget(weapon, buttons, angles, &LastAngle, LocalEyePos, BestBonePos, pCPlayer, AimbotModifiedAngles, PressingTriggerbotKey);
		}
		else
		{
			//Bone not valid, find a new target!!
			ClearAimbotTarget();
		}
		//	lagcomp->FinishLagCompensation(LastTargetEntity);
	}

	return false;
}
