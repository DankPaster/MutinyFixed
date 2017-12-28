#include "LegitBot.h"
#include "BaseEntity.h"
#include "BaseCombatWeapon.h"
#include "trace.h"
#include "Overlay.h"
#include "Interfaces.h"
#include "LocalPlayer.h"

Legitbot aimbot;
//std::unique_ptr<Legitbot> Legitbot::instance_;
//std::once_flag            Legitbot::onceFlag_;
#define FOV 180.0f

#define ANGLE_RANDOM_SCALE 1.05f
#define POSITION_RANDOM_SCALE 1.05f
#define RCS_SCALE 100.0f
#define SMOOTHING_SCALE 10.0f

Legitbot::Legitbot()
{
	bestTarget_ = nullptr;
	bestFov_ = 360.0f;
	hitboxPosition_ = Vector(0.0f, 0.0f, 0.0f);
	aimPosition_ = Vector(0.0f, 0.0f, 0.0f);
	viewAngles_ = QAngle(0.0f, 0.0f, 0.0f);
	aimAngles_ = QAngle(0.0f, 0.0f, 0.0f);
	weaponData_ = nullptr;
}

Legitbot::~Legitbot()
{
}

/*
Legitbot &Legitbot::GetInstance()
{
	std::call_once(onceFlag_,
		[] {
		instance_.reset(new Legitbot);
	});

	return *instance_.get();
}
*/

bool Legitbot::Think(CBaseEntity *localPlayer, CBaseCombatWeapon *localWeapon, WeaponInfo_t *weaponData, CUserCmd *command, bool& AimbotModifiedAngles)
{
	if (!IsValidAction(localPlayer, localWeapon, weaponData, command))
		return false;

	if (!GetTarget(localPlayer, localWeapon, command))
		return false;

	if (!SetTarget(localPlayer, localWeapon, command))
		return false;

	if (!SetAimbotAngles(localPlayer, localWeapon, command))
		return false;

	AimbotModifiedAngles = true;

	return true;
}

bool Legitbot::IsValidAction(CBaseEntity *localPlayer, CBaseCombatWeapon *localWeapon, WeaponInfo_t *weaponData, CUserCmd *command)
{
	if (!localWeapon || localWeapon->IsKnife(NULL) || localWeapon->IsGrenade(NULL))
		return false;

	if (localWeapon->GetWeaponID() == WEAPON_C4)
		return false;

	auto bPressingAimKey = (GetAsyncKeyState(AimbotKeyTxt.iValue) & (1 << 16));

	if (AimbotHoldKeyChk.Checked && !bPressingAimKey)
	{
		AllowedToTarget = true;
		if (bestTarget_)
			ResetTarget();
		return false;
	}
	
	static ULONGLONG ullNextRetargetTime = 0;
	if (bPressingAimKey && PressToRetargetChk.Checked)
	{
		ULONGLONG ullTime = GetTickCount64();
		if (ullTime >= ullNextRetargetTime)
		{
			AllowedToTarget = true;
			if (bestTarget_)
				ResetTarget();
			ullNextRetargetTime = ullTime + 300;
		}
	}

	if (AimbotActivateAfterXShotsTxt.iValue > 0)
		if (localPlayer->GetShotsFired() <= AimbotActivateAfterXShotsTxt.iValue)
			return false;

	weaponData_ = weaponData;
	viewAngles_ = command->viewangles;

	viewAngles_.NormalizeAngle();

	return true;
}

void Legitbot::ResetTarget()
{
	bestTarget_ = nullptr;
	bestFov_ = AimbotFOVTxt.flValue;//gpGlobals.AimbotFOV;//settings.aimbot.fov;
	hitboxPosition_ = Vector(0.0f, 0.0f, 0.0f);
	aimPosition_ = Vector(0.0f, 0.0f, 0.0f);
}

bool Legitbot::TargetStillExists(CBaseEntity* Entity)
{
	for (int i = 0; i <= 64; i++)
	{
		CBaseEntity *player = Interfaces::ClientEntList->GetClientEntity(i);
		if (player == Entity)
		{
			if (!player->GetDormant() && player->GetAlive())
				return true;
			break;
		}
	}
	return false;
}

bool Legitbot::GetTarget(CBaseEntity *localPlayer, CBaseCombatWeapon *localWeapon, CUserCmd *command)
{
	if (bestTarget_)
	{
		if (TargetStillExists(bestTarget_))
		{
			if (!GetTargetHitbox(localPlayer, bestTarget_, 0, 1))
				ResetTarget();
			else if (!IsValidTarget(localPlayer, bestTarget_, TARGET_FOV, true))
				ResetTarget();
			else
				return true;
		}
		else
			ResetTarget();
	}
	else
		ResetTarget();

	if (!AimbotAutoTargetSwitchChk.Checked && !AllowedToTarget)
		return false;

	for (int i = 0; i <= 64; i++)
	{
		if (i == localPlayer->index)
			continue;

		CBaseEntity *player = Interfaces::ClientEntList->GetClientEntity(i);
		if (!player || !player->IsPlayer() || player->GetDormant() || (player->GetTeam() == localPlayer->GetTeam() && !TargetTeammatesChk.Checked) || !player->GetAlive() || player->GetHealth() <= 0)
			continue;

		//if (settings.aimbot.check_jumping)
		//	if (!(player->GetFlags() & FL_ONGROUND))
		//		continue;

		if (!GetTargetHitbox(localPlayer, player, 0, 1))
			continue;

		if (!IsValidTarget(localPlayer, player, TARGET_FOV, false))
			continue;
	}

	if (bestTarget_)
	{
		AllowedToTarget = false;
		return true;
	}

	return false;
}

bool Legitbot::SetTarget(CBaseEntity *localPlayer, CBaseCombatWeapon *localWeapon, CUserCmd *command)
{
	ApplyPositionRandomization(AimbotPosRandomScaleTxt.flValue);//settings.aimbot.position_random_scale);

	// basically a cooler CalcAngle
	aimAngles_ = CalcAngle(localPlayer->GetEyePosition(), aimPosition_);
	//aimAngles_ = (aimPosition_ - localPlayer->GetEyePosition()).ToAngle();

	aimAngles_.NormalizeAngle();

	return true;
}

bool Legitbot::IsValidTarget(CBaseEntity *localPlayer, CBaseEntity *target, int method, bool ExistingTarget)
{
	const Vector localEyePosition = localPlayer->GetEyePosition();

	//if (settings.aimbot.check_smoke)
	//	if (Utilities::IsBehindSmoke(localEyePosition, hitboxPosition_))
	//		return false;

	switch (method)
	{
	case TARGET_FOV:
	{
		float fov = GetFovFromCoordsRegardlessofDistance(localEyePosition, hitboxPosition_, viewAngles_ + (localPlayer->GetPunch() * 2.0f));

		if (fov > AimbotFOVTxt.flValue || (!ExistingTarget && fov > bestFov_))
			return false;

		bestFov_ = fov;

		break;
	}
	default: return false;
	}

	bestTarget_ = target;
	aimPosition_ = hitboxPosition_;

	return true;
}

bool Legitbot::GetTargetHitbox(CBaseEntity *localPlayer, CBaseEntity *target, int hitbox, int selection)
{
	switch (selection)
	{
	case HITBOX_STATIC: return GetTargetHitboxPosition(localPlayer, target, hitbox);
	case HITBOX_DYNAMIC: return GetTargetHitboxDynamic(localPlayer, target, hitbox);
	default: break;
	}

	return false;
}

bool Legitbot::DidHitEnemy(CBaseEntity *localPlayer, CBaseEntity *target, Vector pos)
{
	trace_t tr;
	CTraceFilterPlayersOnly filter;
	filter.pSkip = (IHandleEntity*)localPlayer;
	filter.m_icollisionGroup = 0;
	Ray_t ray;
	ray.Init(localPlayer->GetEyePosition(), pos);
	QAngle angDir = CalcAngle(localPlayer->GetEyePosition(), pos);

	int MASK = 0x4600400B;
	Interfaces::EngineTrace->TraceRay(ray, MASK, &filter, &tr);

	Vector vecForward;
	AngleVectors(angDir, &vecForward);
	vecForward.NormalizeInPlace();
	UTIL_ClipTraceToPlayers(localPlayer->GetEyePosition(), pos + (vecForward * 40.0f), MASK, &filter, &tr);

	if (tr.m_pEnt == target)
		return true;

	//if (settings.aimbot.check_visible)
	//	if (Utilities::IsVisible(localPlayer, target, localPlayer->GetEyePosition(), hitboxPosition_))
	//		return true;

	return false;
}

bool Legitbot::IsNudgedBoneVisible(float nudgeXYamount, Vector &TargetBonePos, CBaseEntity *localPlayer, CBaseEntity *target, int hitboxID)
{
	//Try nudging different directions
	Vector centerpos = TargetBonePos;
	Vector vForward, vRight, vUp;
	QAngle aimboneangle = CalcAngle(localPlayer->GetEyePosition(), TargetBonePos);
	AngleVectors(aimboneangle, &vForward, &vRight, &vUp);
	vForward.NormalizeInPlace();
	vRight.NormalizeInPlace();
	vUp.NormalizeInPlace();

	TargetBonePos = centerpos + (vRight * nudgeXYamount);

	if (DidHitEnemy(localPlayer, target, TargetBonePos))
		return true;

	TargetBonePos = centerpos - (vRight * nudgeXYamount);

	if (DidHitEnemy(localPlayer, target, TargetBonePos))
		return true;

	return false;
}

bool Legitbot::GetTargetHitboxPosition(CBaseEntity *localPlayer, CBaseEntity *target, int hitboxID)
{
	hitboxPosition_ = target->GetBonePosition(hitboxID, Interfaces::Globals->curtime, true, false);

	if (hitboxPosition_ == Vector(0, 0, 0))
		return false;
	
	if (DidHitEnemy(localPlayer, target, hitboxPosition_))
		return true;
	else
	{
		if (!AimbotMultiPointChk.Checked || (AimbotMultiPointPrioritiesOnlyChk.Checked && !AllPlayers[target->index].Personalize.PriorityTarget))
			return false;

		if (hitboxID == HITBOX_HEAD)
		{
			if (!AimbotMultiPointHeadNeckChk.Checked)
				return false;

			Vector originalPos = hitboxPosition_;

			hitboxPosition_.z += 5.0f;
			if (DidHitEnemy(localPlayer, target, hitboxPosition_))
				return true;

			hitboxPosition_.z += 0.75f;
			if (DidHitEnemy(localPlayer, target, hitboxPosition_))
				return true;

			hitboxPosition_ = originalPos;

			if (IsNudgedBoneVisible(2.2f, hitboxPosition_, localPlayer, target, hitboxID))
				return true;
		}
		else
		{
			float NewNudge;
			switch (hitboxID)
			{
				case HITBOX_CHEST:
					if (!AimbotMultiPointChestTorsoPelvisChk.Checked)
						return false;
					NewNudge = 8.0f;
					break;
				case HITBOX_PELVIS:
					if (!AimbotMultiPointChestTorsoPelvisChk.Checked)
						return false;
					NewNudge = 4.8f;
					break;
				case HITBOX_LEFT_FOREARM:
				case HITBOX_RIGHT_FOREARM:
					if (!AimbotMultiPointShoulderArmsHandsChk.Checked)
						return false;
					NewNudge = 1.8f;
					break;
				case HITBOX_LEFT_CALF:
				case HITBOX_RIGHT_CALF:
					if (!AimbotMultiPointLegsFeetChk.Checked)
						return false;
					NewNudge = 2.3f;
					break;
				default:
					NewNudge = 0.0f;
			}

			if (NewNudge != 0.0f)
			{
				if (IsNudgedBoneVisible(NewNudge, hitboxPosition_, localPlayer, target, hitboxID))
					return true;
			}
		}
	}

	return false;
}

bool Legitbot::GetTargetHitboxDynamic(CBaseEntity *localPlayer, CBaseEntity *target, int hitboxID)
{
	int bestHitbox = -1;
	float bestFov = AimbotFOVTxt.flValue;

	static std::vector<int> hitboxes = {
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
		HITBOX_RIGHT_FOREARM
	};

	for (auto &hitbox : hitboxes)
	{
		//Filter out disallowed hitboxes. TODO: do this automatically with array like csgofullv2 later
		if (hitbox == HITBOX_HEAD || hitbox == HITBOX_NECK)
		{
			if (!AimbotAimHeadNeckChk.Checked)
				continue;
		}
		else if (hitbox < HITBOX_LEFT_THIGH)
		{
			if (!AimbotAimTorsoChk.Checked)
				continue;
		}
		else if (hitbox < HITBOX_LEFT_HAND)
		{
			if (!AimbotAimLegsFeetChk.Checked)
				continue;
		}
		else if (!AimbotAimArmsHandsChk.Checked)
			continue;
		Vector temp;
#ifdef DRAW_ALL_HITBOXES
		for (int i = 0; i < 18; i++)
		{
			temp = target->GetBonePosition(i);
			if (temp == Vector(0, 0, 0))
				continue;
			char str[8];
			sprintf(str, "%i", i);
			Interfaces::DebugOverlay->AddTextOverlay(temp, 0.2f, str);
		}
		return false;
#else
		temp = target->GetBonePosition(hitbox, Interfaces::Globals->curtime, true, false);
#endif

		float fov = GetFovFromCoordsRegardlessofDistance(localPlayer->GetEyePosition(), temp, viewAngles_ + (localPlayer->GetPunch() * 2.0f));
		if (fov < bestFov)
		{
			bestFov = fov;
			bestHitbox = hitbox;
		}
	}

	if (bestHitbox != -1)
	{
		if (GetTargetHitboxPosition(localPlayer, target, bestHitbox))
			return true;
	}

	return false;
}

void Legitbot::ApplyPositionRandomization(float value)
{
	if (value <= 0.0f)
		return;

	float randomization[2] = { SORandomFloat(-value, value), SORandomFloat(-value, value) };

	Vector randomizedPosition = aimPosition_;

	randomizedPosition += Vector(
		randomization[0] * cosf(randomization[1]),
		randomization[0] * cosf(randomization[1]),
		randomization[0] * cosf(randomization[1])
	);

	aimPosition_ = randomizedPosition;
}

void Legitbot::ApplyAngleRandomization(float value)
{
	if (value <= 0.0f)
		return;

	float randomization[2] = { SORandomFloat(-value, value), SORandomFloat(-value, value) };

	QAngle randomizedAngles = aimAngles_;

	randomizedAngles += QAngle(
		randomization[0] * cosf(randomization[1]),
		randomization[0] * cosf(randomization[1]),
		0.0f
	);

	randomizedAngles.NormalizeAngle();

	aimAngles_ = randomizedAngles;
}

void Legitbot::ApplyRecoilCompensation(CBaseEntity *localPlayer, CBaseCombatWeapon *localWeapon, float value)
{
	if (value <= 0.0f)
		return;

	int weaponItemDefinitionIndex = localWeapon->GetItemDefinitionIndex();

	// set weapons you don't want being compensated here
	if (localWeapon->IsGrenade(weaponItemDefinitionIndex) || localWeapon->IsKnife(weaponItemDefinitionIndex))
		return;

	if (!NoRecoilPistolsShotgunsSnipersChk.Checked)
	{
		if (localWeapon->IsPistol(weaponItemDefinitionIndex) || localWeapon->IsSniper(true, weaponItemDefinitionIndex) || localWeapon->IsShotgun(weaponItemDefinitionIndex))
			return;
	}

	// you can randomize recoil control
	QAngle compensatedAngles = aimAngles_;
	QAngle LocalPlayerPunch = localPlayer->GetPunch();
	QAngle compensatedPunch = NoRecoilChk.Checked ? QAngle((LocalPlayerPunch.x * 2.0f) * (NoRecoilYPercentTxt.flValue / 100.0f), (LocalPlayerPunch.y * 2.0f) * (NoRecoilXPercentTxt.flValue / 100.0f), 0.0f) : QAngle(0,0,0);


	compensatedAngles.x -= compensatedPunch.x;
	compensatedAngles.y -= compensatedPunch.y;

	compensatedAngles.NormalizeAngle();

	LocalPlayer.AimPunchSubtracted = compensatedPunch;

	aimAngles_ = compensatedAngles;
}

void Legitbot::ApplyAngleSmoothing(float value)
{
	if (value <= 0.0f)
		return;

	QAngle deltaAngles = viewAngles_ - aimAngles_;

	deltaAngles.NormalizeAngle();

	// basic smoothing but it's really nice imo
	QAngle smoothedAngles = viewAngles_ - (deltaAngles / value);

	smoothedAngles.NormalizeAngle();

	aimAngles_ = smoothedAngles;
}

bool Legitbot::SetAimbotAngles(CBaseEntity *localPlayer, CBaseCombatWeapon *localWeapon, CUserCmd *command)
{
	ApplyAngleRandomization(AimbotAngleRandomScaleTxt.flValue);
	ApplyRecoilCompensation(localPlayer, localWeapon, 100.0f);
	ApplyAngleSmoothing(AimbotSmoothingScaleTxt.flValue);

	aimAngles_.Clamp();

	command->viewangles = aimAngles_;

	return true;
}