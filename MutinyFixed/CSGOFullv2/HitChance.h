#pragma once
#include "CreateMove.h"
#include "Intersection.h"

extern float LastShotHitPercentage;
extern float CurrentHitPercentage;

struct HC_Vars 
{
	int maxseeds;
	int seedshit;
	int seedsneeded;
	int maxallowedmiss;
	HANDLE DoWork_Event;
	HANDLE Finished_Event;
};

bool intialHeadScan(CBaseCombatWeapon* Weapon, Vector vMin, Vector vMax, int hitChance);
bool BulletWillHit(CBaseCombatWeapon* Weapon, int buttons, CBasePlayer* pTargetEntity, QAngle viewangles, Vector LocalEyePos, Vector *pos, unsigned int TargetHitGroup);