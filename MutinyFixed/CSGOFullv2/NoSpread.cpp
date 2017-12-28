#include "NoSpread.h"
void NoSpread(QAngle& angles)
{

	return;
#if 0
	CBaseCombatWeapon* weapon = LocalPlayer.Entity->GetWeapon();
	if (weapon)
	{
		if (CurrentUserCmd.random_seed > 0)
			printf("test\n");
		RandomSeed((ReadInt((uintptr_t)&CurrentUserCmd.random_seed) & 0xFF) + 1);
		weapon->UpdateAccuracyPenalty();
		float fRand1 = RandomFloat(0.0f, 1.0f);
		float fRandPi1 = RandomFloat(0.0f, 2.0f * (float)M_PI);
		float fRand2 = RandomFloat(0.0f, 1.f);
		float fRandPi2 = RandomFloat(0.0f, 2.0f * (float)M_PI);
		if (weapon->GetWeaponID() == WEAPON_REVOLVER && ReadInt((uintptr_t)&CurrentUserCmd.cmd->buttons) & IN_ATTACK2)
		{
			fRand1 = 1.f - fRand1 * fRand1;
			fRand2 = 1.f - fRand2 * fRand2;
		}

		float fRandInaccuracy = fRand1 * weapon->GetWeaponCone();
		float fRandSpread = fRand2 * weapon->GetWeaponSpread();
#ifdef _DEBUG
#if 0
		char str[128];
		sprintf(str, "%.5f", fRandSpread);
		DrawStringUnencrypted(str, Vector2D(960, 540), 255, 255, 0, pFont);
#endif
#endif

		float fSpreadX = cos(fRandPi1) * fRandInaccuracy + cos(fRandPi2) * fRandSpread;
		float fSpreadY = sin(fRandPi1) * fRandInaccuracy + sin(fRandPi2) * fRandSpread;
		Vector vecForward, vecRight, vecUp, vecAntiDir;
		AngleVectors(angles, &vecForward, &vecRight, &vecUp);
		vecAntiDir = vecForward + (vecRight * fSpreadX) + (vecUp * fSpreadY);
		vecAntiDir.NormalizeInPlace();
		QAngle qSpreadAngles;
		VectorAngles(vecAntiDir, qSpreadAngles);

		qSpreadAngles += (angles - qSpreadAngles);
		angles = qSpreadAngles;
	}
#endif
}