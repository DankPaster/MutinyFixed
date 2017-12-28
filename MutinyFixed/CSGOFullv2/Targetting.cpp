#include "Targetting.h"

#include "BaseEntity.h"
#include "GameMemory.h"
#include "LocalPlayer.h"
#include "AutoWall.h"
#include "Aimbot.h"
#include "VMProtectDefs.h"

Targetting MTargetting;

int NUM_BONES = 0;
const int HITBOX_GROUP_HEAD[2] = { HITBOX_HEAD, HITBOX_NECK };
const int HITBOX_GROUP_TORSO[6] = { HITBOX_LOWER_NECK, HITBOX_UPPER_CHEST, HITBOX_CHEST, HITBOX_THORAX, HITBOX_BODY, HITBOX_PELVIS };
const int HITBOX_GROUP_TORSO_FPS[3] = { HITBOX_UPPER_CHEST, HITBOX_CHEST, HITBOX_PELVIS };
const int HITBOX_GROUP_UPPERLIMBS[6] = { HITBOX_LEFT_UPPER_ARM, HITBOX_RIGHT_UPPER_ARM, HITBOX_LEFT_FOREARM, HITBOX_RIGHT_FOREARM, HITBOX_LEFT_HAND, HITBOX_RIGHT_HAND };
const int HITBOX_GROUP_UPPERLIMBS_FPS[2] = { HITBOX_LEFT_HAND, HITBOX_RIGHT_HAND };
const int HITBOX_GROUP_LOWERLIMBS[6] = { HITBOX_LEFT_THIGH, HITBOX_RIGHT_THIGH, HITBOX_LEFT_CALF, HITBOX_RIGHT_CALF, HITBOX_LEFT_FOOT, HITBOX_RIGHT_FOOT };
const int HITBOX_GROUP_LOWERLIMBS_FPS[4] = { HITBOX_LEFT_THIGH, HITBOX_RIGHT_THIGH, HITBOX_LEFT_FOOT, HITBOX_RIGHT_FOOT };

int BONES_TO_CHECK[MAX_BONES] = { 0 };
int NUM_HITGROUPS_ALLOWED = 7;
int HITGROUPS_ALLOWED[7] = { HITGROUP_HEAD, HITGROUP_CHEST, HITGROUP_STOMACH, HITGROUP_LEFTARM, HITGROUP_RIGHTARM, HITGROUP_LEFTLEG, HITGROUP_RIGHTLEG };
const int HITBOXES_ALL_BASIC[9] = { HITBOX_HEAD, HITBOX_CHEST, HITBOX_PELVIS, HITBOX_LEFT_HAND, HITBOX_RIGHT_HAND, HITBOX_LEFT_CALF, HITBOX_RIGHT_CALF, HITBOX_LEFT_FOOT, HITBOX_RIGHT_FOOT };

BONE_GROUPS Targetting::GetBoneGroup(int Bone)
{
#if 1
	//optimization
	if (Bone <= HITBOX_NECK)
		return BONE_GROUP_HEADNECK;
#else
	const int count = (sizeof(HITBOX_GROUP_HEAD) / sizeof(HITBOX_GROUP_HEAD[0]));
	for (int i = 0; i < count; i++)
	{
		if (HITBOX_GROUP_HEAD[i] == Bone)
			return BONE_GROUP_HEADNECK;
	}
#endif

	const int count2 = (sizeof(HITBOX_GROUP_TORSO) / sizeof(HITBOX_GROUP_TORSO[0]));
	for (int j = 0; j < count2; j++)
	{
		if (HITBOX_GROUP_TORSO[j] == Bone)
			return BONE_GROUP_CHESTTORSOPELVIS;
	}

	const int count3 = (sizeof(HITBOX_GROUP_UPPERLIMBS) / sizeof(HITBOX_GROUP_UPPERLIMBS[0]));
	for (int k = 0; k < count3; k++)
	{
		if (HITBOX_GROUP_UPPERLIMBS[k] == Bone)
			return BONE_GROUP_SHOULDERARMSHANDS;
	}

	const int count4 = (sizeof(HITBOX_GROUP_LOWERLIMBS) / sizeof(HITBOX_GROUP_LOWERLIMBS[0]));
	for (int l = 0; l < count4; l++)
	{
		if (HITBOX_GROUP_LOWERLIMBS[l] == Bone)
			return BONE_GROUP_LEGSFEET;
	}
	return BONE_GROUP_HEADNECK;
}

bool Targetting::IsPlayerAValidTarget(CBaseEntity* Player)
{
	return Player && Player->IsPlayer() && !Player->GetImmune() && (TargetTeammatesChk.Checked || (Player->GetTeam() != LocalPlayer.Entity->GetTeam() && Player->GetTeam() != TEAM_NONE));
}

bool Targetting::IsPlayerAValidTarget(CustomPlayer* Player)
{
	return (Player->BaseEntity && TargetTeammatesChk.Checked || Player->BaseEntity->GetTeam() != LocalPlayer.Entity->GetTeam()) && !Player->BaseEntity->GetImmune();
}

//Sets the current hitbox group we are aiming at
void Targetting::SetHitboxGroup()
{
	ProfStats* stat = START_PROFILING("SetHitboxGroup");
	if (LockOnToHitgroupsOnlyChk.Checked)
	{
		//This function is pretty shit..
		NUM_HITGROUPS_ALLOWED = 0;
		if (AimbotAimHeadNeckChk.Checked)
		{
			const int count = (sizeof(HITBOX_GROUP_HEAD) / sizeof(HITBOX_GROUP_HEAD[0]));
			for (int i = 0; i < count; i++)
			{
				int bone = HITBOX_GROUP_HEAD[i];
				if (LastBoneChecked == bone)
				{
					CURRENT_HITGROUP_AIMING_AT = HITGROUP_HEAD;
					HITGROUPS_ALLOWED[NUM_HITGROUPS_ALLOWED++] = HITGROUP_HEAD;
					HITGROUPS_ALLOWED[NUM_HITGROUPS_ALLOWED++] = HITGROUP_CHEST;
					//break;
					END_PROFILING(stat);
					return; //revert to break if we ever want to allow multiple hitgroups at once
				}
			}
		}
		if (AimbotAimTorsoChk.Checked)
		{
			const int count = (sizeof(HITBOX_GROUP_TORSO) / sizeof(HITBOX_GROUP_TORSO[0]));
			for (int i = 0; i < count; i++)
			{
				Hitboxes bone = (Hitboxes)HITBOX_GROUP_TORSO[i];
				if (LastBoneChecked == bone)
				{
					if (bone <= HITBOX_BODY || bone >= HITBOX_LEFT_THIGH)
						CURRENT_HITGROUP_AIMING_AT = HITGROUP_STOMACH;
					else
						CURRENT_HITGROUP_AIMING_AT = HITGROUP_CHEST;
					HITGROUPS_ALLOWED[NUM_HITGROUPS_ALLOWED++] = HITGROUP_STOMACH;
					//break;
					END_PROFILING(stat);
					return; //revert to break if we ever want to allow multiple hitgroups at once
				}
			}
		}
		if (AimbotAimArmsHandsChk.Checked)
		{
			const int count = (sizeof(HITBOX_GROUP_UPPERLIMBS) / sizeof(HITBOX_GROUP_UPPERLIMBS[0]));
			for (int i = 0; i < count; i++)
			{
				int bone = HITBOX_GROUP_UPPERLIMBS[i];
				if (LastBoneChecked == bone)
				{
					//FUTURE FIXME: change to separate || if more hitboxes are added
					if (bone >= HITBOX_LEFT_UPPER_ARM || bone == HITBOX_LEFT_HAND)
						CURRENT_HITGROUP_AIMING_AT = HITGROUP_LEFTARM;
					else
						CURRENT_HITGROUP_AIMING_AT = HITGROUP_RIGHTARM;
					HITGROUPS_ALLOWED[NUM_HITGROUPS_ALLOWED++] = HITGROUP_LEFTARM;
					HITGROUPS_ALLOWED[NUM_HITGROUPS_ALLOWED++] = HITGROUP_RIGHTARM;
					//break;
					END_PROFILING(stat);
					return; //revert to break if we ever want to allow multiple hitgroups at once
				}
			}
		}
		if (AimbotAimLegsFeetChk.Checked)
		{
			const int count = (sizeof(HITBOX_GROUP_LOWERLIMBS) / sizeof(HITBOX_GROUP_LOWERLIMBS[0]));
			for (int i = 0; i < count; i++)
			{
				int bone = HITBOX_GROUP_LOWERLIMBS[i];
				if (LastBoneChecked == bone)
				{
					if (bone == HITBOX_RIGHT_THIGH || bone == HITBOX_RIGHT_CALF || bone == HITBOX_RIGHT_FOOT)
						CURRENT_HITGROUP_AIMING_AT = HITGROUP_RIGHTLEG;
					else
						CURRENT_HITGROUP_AIMING_AT = HITGROUP_LEFTLEG;

					HITGROUPS_ALLOWED[NUM_HITGROUPS_ALLOWED++] = HITGROUP_LEFTLEG;
					HITGROUPS_ALLOWED[NUM_HITGROUPS_ALLOWED++] = HITGROUP_RIGHTLEG;
					//break;
					END_PROFILING(stat);
					return; //revert to break if we ever want to allow multiple hitgroups at once
				}
			}
		}
	}
	END_PROFILING(stat);
}

//is the current hitgroup a target, used with aimbot
bool Targetting::IsHitgroupACurrentTarget(int hitgroup)
{
	if (!LockOnToHitgroupsOnlyChk.Checked)
		return true;

	for (int i = 0; i < NUM_HITGROUPS_ALLOWED; i++)
	{
		if (hitgroup == HITGROUPS_ALLOWED[i])
		{
			return true;
		}
	}
	return false;
}

//generic hitgroup test, works for everything
bool Targetting::IsHitgroupATarget(int hitgroup, CBaseEntity* pEntity)
{
	bool HitHead = true;
	if (pEntity->IsPlayer())
	{
		CustomPlayer * pCPlayer = &AllPlayers[pEntity->index];
		if (!pCPlayer->Dormant)
		{
			if (pCPlayer->Personalize.DontShootAtHeadNeck && !pCPlayer->FireAtPelvis)
				HitHead = false;
		}
	}

	switch (hitgroup)
	{
	case HITGROUP_HEAD:
		return AimbotAimHeadNeckChk.Checked && HitHead;
	case HITGROUP_LEFTARM:
	case HITGROUP_RIGHTARM:
		return AimbotAimArmsHandsChk.Checked;
	case HITGROUP_CHEST:
	case HITGROUP_STOMACH:
		return AimbotAimTorsoChk.Checked;
	case HITGROUP_LEFTLEG:
	case HITGROUP_RIGHTLEG:
		return AimbotAimLegsFeetChk.Checked;
	}
	return false;
}

void Targetting::UpdateBoneList()
{
	NUM_BONES = 0;
	if (AimbotAimHeadNeckChk.Checked)
	{
		BONES_TO_CHECK[NUM_BONES++] = HITBOX_HEAD;
		BONES_TO_CHECK[NUM_BONES++] = HITBOX_NECK;
	}
	if (AimbotAimTorsoChk.Checked)
	{
		BONES_TO_CHECK[NUM_BONES++] = HITBOX_LOWER_NECK;
		BONES_TO_CHECK[NUM_BONES++] = HITBOX_UPPER_CHEST;
		BONES_TO_CHECK[NUM_BONES++] = HITBOX_CHEST;
		BONES_TO_CHECK[NUM_BONES++] = HITBOX_THORAX;
		BONES_TO_CHECK[NUM_BONES++] = HITBOX_BODY;
		BONES_TO_CHECK[NUM_BONES++] = HITBOX_PELVIS;
	}
	if (AimbotAimArmsHandsChk.Checked)
	{
		BONES_TO_CHECK[NUM_BONES++] = HITBOX_LEFT_UPPER_ARM;
		BONES_TO_CHECK[NUM_BONES++] = HITBOX_LEFT_FOREARM;
		BONES_TO_CHECK[NUM_BONES++] = HITBOX_LEFT_HAND;
		BONES_TO_CHECK[NUM_BONES++] = HITBOX_RIGHT_UPPER_ARM;
		BONES_TO_CHECK[NUM_BONES++] = HITBOX_RIGHT_FOREARM;
		BONES_TO_CHECK[NUM_BONES++] = HITBOX_RIGHT_HAND;
	}
	if (AimbotAimLegsFeetChk.Checked)
	{
		BONES_TO_CHECK[NUM_BONES++] = HITBOX_LEFT_THIGH;
		BONES_TO_CHECK[NUM_BONES++] = HITBOX_LEFT_CALF;
		BONES_TO_CHECK[NUM_BONES++] = HITBOX_LEFT_FOOT;
		BONES_TO_CHECK[NUM_BONES++] = HITBOX_RIGHT_THIGH;
		BONES_TO_CHECK[NUM_BONES++] = HITBOX_RIGHT_CALF;
		BONES_TO_CHECK[NUM_BONES++] = HITBOX_RIGHT_FOOT;
	}
}

int Targetting::HitboxToHitgroup(int Hitbox)
{
	switch (Hitbox)
	{
	case HITBOX_HEAD:
	case HITBOX_NECK:
		return HITGROUP_HEAD;
	case HITBOX_LOWER_NECK:
	case HITBOX_UPPER_CHEST:
	case HITBOX_CHEST:
	case HITBOX_THORAX:
	case HITBOX_LEFT_UPPER_ARM:
	case HITBOX_RIGHT_UPPER_ARM:
		return HITGROUP_CHEST;
	case HITBOX_PELVIS:
	case HITBOX_LEFT_THIGH:
	case HITBOX_RIGHT_THIGH:
	case HITBOX_BODY:
		return HITGROUP_STOMACH;
	case HITBOX_LEFT_CALF:
	case HITBOX_LEFT_FOOT:
		return HITGROUP_LEFTLEG;
	case HITBOX_RIGHT_CALF:
	case HITBOX_RIGHT_FOOT:
		return HITGROUP_RIGHTLEG;
	case HITBOX_LEFT_FOREARM:
	case HITBOX_LEFT_HAND:
		return HITGROUP_LEFTARM;
	case HITBOX_RIGHT_FOREARM:
	case HITBOX_RIGHT_HAND:
		return HITGROUP_RIGHTARM;
	default:
		return HITGROUP_STOMACH;
	}
}

int Targetting::HitgroupToHitbox(int Hitgroup)
{
	switch (Hitgroup)
	{
	case HITGROUP_HEAD:
		return HITBOX_HEAD;
	case HITGROUP_CHEST:
		return HITBOX_CHEST;
	case HITGROUP_STOMACH:
		return HITBOX_PELVIS;
	case HITGROUP_LEFTARM:
		return HITBOX_LEFT_FOREARM;
	case HITGROUP_RIGHTARM:
		return HITBOX_RIGHT_FOREARM;
	case HITGROUP_LEFTLEG:
		return HITBOX_LEFT_CALF;
	case HITGROUP_RIGHTLEG:
		return HITBOX_RIGHT_CALF;
	default:
		return HITBOX_BODY;
	}
}

bool Targetting::HitboxIsArmOrHand(int Hitbox)
{
	switch (Hitbox)
	{
		case HITBOX_LEFT_FOREARM:
		case HITBOX_LEFT_HAND:
		case HITBOX_RIGHT_FOREARM:
		case HITBOX_RIGHT_HAND:
			return true;
	}
	return false;
}

bool Targetting::HitboxIsLegOrFoot(int Hitbox)
{
	switch (Hitbox)
	{
		case HITBOX_LEFT_CALF:
		case HITBOX_LEFT_FOOT:
		case HITBOX_RIGHT_CALF:
		case HITBOX_RIGHT_FOOT:
			return true;
	}
	return false;
}

CBaseEntity* Targetting::TraceToPosition(Vector LocalEyePos, Vector* TargetPos, Vector* vecDir, QAngle* const localangles, CBaseCombatWeapon* const weapon, float* const dmg, int& hitgroup, Vector* TraceHitPos)
{
	if (AutoWallChk.Checked)
	{
		float dmg;
		return Autowall(TargetPos, LocalEyePos, localangles, weapon, &dmg, hitgroup, nullptr, false);
	}
	else
	{
		Vector vecDirNew;

		if (!vecDir)
		{
			QAngle angle = CalcAngle(LocalEyePos, *TargetPos);
			AngleVectors(angle, &vecDirNew);
			VectorNormalizeFast(vecDirNew);
			vecDir = &vecDirNew;
		}
		trace_t tr;
		CTraceFilter filter;
		filter.pSkip = (IHandleEntity*)LocalPlayer.Entity;
		filter.m_icollisionGroup = COLLISION_GROUP_NONE;
		Ray_t ray;
		ray.Init(LocalEyePos, *TargetPos);
		Interfaces::EngineTrace->TraceRay(ray, MASK_SHOT, &filter, &tr);
		UTIL_ClipTraceToPlayers(LocalEyePos, *TargetPos + (*vecDir * 40.0f), 0x4600400B, &filter, &tr);
		*TraceHitPos = tr.endpos;
		hitgroup = tr.hitgroup;
		return tr.m_pEnt;
	}
}

CBaseEntity* Targetting::GetPlayerClosestToCrosshair(float MaxFOV)
{
	CBaseEntity* AimTarget = nullptr;
	CPlayerTarget TPlayers[4096];
	int IndexOfClosest = INVALID_PLAYER; //Index of the closest player to the crosshair, used with AllPlayers, not streamed
	bool AllowAnyFOV = MaxFOV >= 180.0f;

	for (int i = 0; i < NumStreamedPlayers; i++)
	{
		CustomPlayer *pCPlayer = StreamedPlayers[i];

		if (pCPlayer && pCPlayer->IsTarget && pCPlayer->LifeState == LIFE_ALIVE)
		{
			CBaseEntity* pPlayer = pCPlayer->BaseEntity;

			if (!pPlayer)
				continue;

			CPlayerTarget *pTPlayer = &TPlayers[i];

			//Find a bone that is inside the FOV
			for (int b = 0; b < (sizeof(HITBOXES_ALL_BASIC) / sizeof(HITBOXES_ALL_BASIC[0])); b++)
			{
				pTPlayer->BonePos = pPlayer->GetBonePosition(HITBOXES_ALL_BASIC[b], /*pPlayer->GetSimulationTime()*/Interfaces::Globals->curtime, false, false);

				float FOV = GetFovFromCoordsRegardlessofDistance(LocalPlayer.Entity->GetEyePosition(), pTPlayer->BonePos, LocalPlayer.CurrentEyeAngles);

				if (AllowAnyFOV || FOV <= MaxFOV)
				{
					if (IndexOfClosest == INVALID_PLAYER || FOV < TPlayers[IndexOfClosest].DistanceFromCrosshair)
					{
						if (!pCPlayer->Personalize.PriorityTarget && IndexOfClosest != INVALID_PLAYER && TPlayers[IndexOfClosest].PriorityTarget)
							break; //Todo: CanFireAtLagCompensatedTarget
						pTPlayer->DistanceFromCrosshair = FOV;
						pTPlayer->PriorityTarget = pCPlayer->Personalize.PriorityTarget;
						IndexOfClosest = i;
						AimTarget = StreamedPlayers[i]->BaseEntity;
						break;
					}
				}
			}
		}
	}
	return AimTarget;
}

void Targetting::RageAimStep(QAngle from, QAngle to, QAngle& dest, float flStep)
{
	QAngle delta_angle = from - to;
	NormalizeAngles(delta_angle);

	if (delta_angle.x < -flStep)
		dest.x -= flStep;
	else if (delta_angle.x > flStep)
		dest.x += flStep;
	else
		dest.x = to.x;

	if (delta_angle.y < -flStep)
		dest.y -= flStep;
	else if (delta_angle.y > flStep)
		dest.y += flStep;
	else
		dest.y = to.y;
}

void Targetting::RageAimStep(float from, float to, float& dest, float flStep)
{
	float delta_angle = from - to;
	NormalizeAngle(delta_angle);

	if (delta_angle < -flStep)
		dest -= flStep;
	else if (delta_angle > flStep)
		dest += flStep;
	else
		dest = to;
}

void Targetting::AimStepBackToCrosshair(bool& DontSetViewAngles)
{
	QAngle delta_angle = LocalPlayer.TargetEyeAngles - LocalPlayer.LastEyeAngles; //use RealViewAngles?
	NormalizeAngles(delta_angle);

	float deg = GetFov(LocalPlayer.LastEyeAngles, LocalPlayer.TargetEyeAngles);
	//FIXME
	float IncrementAngle = SpinDegPerTickTxt.flValue; //(LegitAimStepChk.Checked && SilentAimChk.Checked) ? (AimbotSpeedTxt.flValue * 2.5f + (SORandomFloat(0.2f, 0.3f) / deg)) : 42.5f;


	if (delta_angle.y > IncrementAngle)
	{
		LocalPlayer.TargetEyeAngles.y = LocalPlayer.LastEyeAngles.y + IncrementAngle;
		DontSetViewAngles = false;
	}
	else if (delta_angle.y < -IncrementAngle)
	{
		LocalPlayer.TargetEyeAngles.y = LocalPlayer.LastEyeAngles.y - IncrementAngle;
		DontSetViewAngles = false;
	}
	if (delta_angle.x > IncrementAngle)
	{
		LocalPlayer.TargetEyeAngles.x = LocalPlayer.LastEyeAngles.x + IncrementAngle;
		DontSetViewAngles = false;
	}
	else if (delta_angle.x < -IncrementAngle)
	{
		LocalPlayer.TargetEyeAngles.x = LocalPlayer.LastEyeAngles.x - IncrementAngle;
		DontSetViewAngles = false;
	}
	NormalizeAngles(LocalPlayer.TargetEyeAngles);
}

//Set the flags so hitchance knows what group we're targetting
void Targetting::SetHitChanceFlags()
{
	switch (CURRENT_HITGROUP_AIMING_AT)
	{
	case HITGROUP_HEAD:
		CURRENT_HITCHANCE_FLAGS = HITGROUP_HEAD_FLAG;
		break;
	default:
		CURRENT_HITCHANCE_FLAGS = HITGROUP_ALL_FLAG;
		break;
	}
}