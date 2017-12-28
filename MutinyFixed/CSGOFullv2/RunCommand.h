#pragma once
#include "CreateMove.h"
extern float flArrayCorrect[128][4];

struct PredVariables
{
	QAngle aimpunch;
	Vector aimpunchvel;
	Vector viewoffset;
	Vector origin;
	int TickBase;
};

extern PredVariables PredictedVars[150];
extern int lastpredictedcommandnr;
extern int lastcommandnr;

void RestorePredictedVariables(CBaseEntity* pEntity, bool ShouldRestoreOrigin, PredVariables* vars);