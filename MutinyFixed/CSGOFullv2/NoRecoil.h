#pragma once
#include "CreateMove.h"
bool IsAllowedToNoRecoil(CBaseCombatWeapon* weapon);
void NoRecoil(CBaseCombatWeapon* weapon, bool& DontSetViewAngles, QAngle& angles, QAngle& RealViewAngles);
void RemoveRecoil(QAngle& angles);//, int& buttons);
#ifdef SPARK_SCRIPT_TESTING
void SparkDebugNoRecoil();
#endif
#define RECOIL_GRACE_PERIOD 1.0f