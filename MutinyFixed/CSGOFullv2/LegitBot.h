#ifndef LEGITBOT_H_
#define LEGITBOT_H_
//#include <mutex>
//#include <memory>

#include "misc.h"
#include "BaseCombatWeapon.h"
#include "Math.h"


class CBaseEntity;
class CBaseCombatWeapon;

class Legitbot
{
	enum TargettingMethod {
		TARGET_FOV
	};

	enum HitboxMethod {
		HITBOX_STATIC,
		HITBOX_DYNAMIC
	};

public:
	Legitbot();
	~Legitbot();
	Legitbot(const Legitbot &src);
	//virtual ~Legitbot();
	//static Legitbot &GetInstance();
	bool AllowedToTarget = true;
	CBaseEntity      *bestTarget_;

	bool Think(CBaseEntity *localPlayer, CBaseCombatWeapon *localWeapon, WeaponInfo_t *weaponData, CUserCmd *command, bool& AimbotModifiedAngles);

private:
	bool IsValidAction(CBaseEntity *localPlayer, CBaseCombatWeapon *localWeapon, WeaponInfo_t *weaponData, CUserCmd *command);

	void ResetTarget();
	bool TargetStillExists(CBaseEntity* Entity);
	bool GetTarget(CBaseEntity *localPlayer, CBaseCombatWeapon *localWeapon, CUserCmd *command);
	bool SetTarget(CBaseEntity *localPlayer, CBaseCombatWeapon *localWeapon, CUserCmd *command);
	bool IsValidTarget(CBaseEntity *localPlayer, CBaseEntity *target, int method, bool ExistingTarget);
	
	bool DidHitEnemy(CBaseEntity *localPlayer, CBaseEntity *target, Vector pos);
	bool GetTargetHitbox(CBaseEntity *localPlayer, CBaseEntity *target, int hitbox, int selection);
	bool IsNudgedBoneVisible(float nudgeXYamount, Vector &TargetBonePos, CBaseEntity *localPlayer, CBaseEntity *target, int hitboxID);
	bool GetTargetHitboxPosition(CBaseEntity *localPlayer, CBaseEntity *target, int hitboxID);
	bool GetTargetHitboxDynamic(CBaseEntity *localPlayer, CBaseEntity *target, int hitboxID);

	void ApplyPositionRandomization(float value);
	void ApplyAngleRandomization(float value);
	void ApplyRecoilCompensation(CBaseEntity *localPlayer, CBaseCombatWeapon *localWeapon, float value);
	void ApplyAngleSmoothing(float value);

	bool SetAimbotAngles(CBaseEntity *localPlayer, CBaseCombatWeapon *localWeapon, CUserCmd *command);

	float           bestFov_;
	Vector          hitboxPosition_;
	Vector          aimPosition_;
	QAngle          viewAngles_;
	QAngle          aimAngles_;
	WeaponInfo_t  *weaponData_;

//private:
	//static std::unique_ptr<Legitbot> instance_;
	//static std::once_flag onceFlag_;

	//Legitbot &operator=(const Legitbot &rhs);
};

inline float GetFieldOfView(const Vector &source, const Vector &destination, const QAngle &angles)
{
	Vector forward;
	QAngle ang = CalcAngle(source, destination);
	AngleVectors(ang, &forward);

	Vector forward2;
	AngleVectors(angles, &forward2);

	return (acosf(forward2.Dot(forward) / std::pow(forward2.Length(), 2.0f)) * (180.0f / M_PI));
}

extern Legitbot aimbot;

#endif