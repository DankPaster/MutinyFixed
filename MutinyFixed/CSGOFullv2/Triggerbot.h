#pragma once
#include "CreateMove.h"
#include "BaseCombatWeapon.h"

class CTriggerbot
{
public:
	void FireBullet(CBaseCombatWeapon* weapon, int& buttons, bool& DontSetViewAngles, bool RightClick);
	bool WeaponCanFire(CBaseCombatWeapon *weapon);
	void RunTriggerbot(CBasePlayer* TargetPlayer, CBaseCombatWeapon *weapon, QAngle *RealViewAngles, QAngle& angles, int flags, float& BestDamage, bool& DontSetViewAngles, DWORD* FramePointer, bool& RanHitChance);
	bool RevolverWillFireThisTick(CBaseCombatWeapon *weapon);
	void AutoCockRevolver(int &buttons, CBaseCombatWeapon* weapon);
#if 0
	bool LegacyTriggerBot(int PlayerInCrosshair, CBaseCombatWeapon *weapon);
#endif
	bool IsAllowedToFire() { return bAllowedToFire; }
	void SetAllowedToFire(bool CanFire) { bAllowedToFire = CanFire; }

	float NextTimeTriggerBot = 0.0f;
	float PS_LastShotTime = 0.0f;
	float TimeToShoot = 0.0f;
	CBaseEntity* TriggerbotTargetEntity = nullptr;
private:
	bool bAllowedToFire;
};

extern CTriggerbot gTriggerbot;

