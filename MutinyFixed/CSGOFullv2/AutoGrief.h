#pragma once
#include "CSGO_HX.h"
#include "CreateMove.h"
extern char *grieftargetstr;
extern int LastGriefingTargetID;
extern Vector GriefChestPos;
void AutoGrief(int& buttons, QAngle& angles, QAngle RealViewAngles, int PressingAimbotKey);