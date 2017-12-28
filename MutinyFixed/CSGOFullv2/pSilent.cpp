#include "pSilent.h"
#include "Overlay.h"
#include "CreateMove.h"
#include "Netchan.h"
#include "BaseCombatWeapon.h"

int LastWeaponFiredCmdNr = 0;
int LastWeaponFireCmdTickCount = 0;
int pSilentTicksSkipped = 0;
bool pSilentBlockFiring = false;
QAngle pSilentBeforeAngles = QAngle(0, 0, 0);

pSilent gpSilent;

void pSilent::Push_pSilent()
{
	StoredRealAntiAim = (AntiAims)AntiAimRealDrop.index;
	StoredFakeAntiAim = (AntiAims)AntiAimFakeDrop.index;
	StoredPitchAntiAim = (AntiAimPitches)AntiAimPitchDrop.index;
	//AntiAimRealDrop.index = ANTIAIM_NONE;
	//AntiAimFakeDrop.index = ANTIAIM_NONE;
	//AntiAimPitchDrop.index = PITCH_NONE;
	StoredpSilent = pSilentChk.Checked;
	StoredSilentAim = SilentAimChk.Checked;
	StoredLegitAimStep = LegitAimStepChk.Checked;
	StoredRageAimStep = RageAimStepChk.Checked;
	StoredTriggerbotWhileAimbotting = TriggerbotWhileAimbottingChk.Checked;
	StoredManualAntiAim = ManualAntiAimChk.Checked;
	//ManualAntiAimChk.Checked = false;
	//FIXME FOR VALVE DS
	LegitAimStepChk.Checked = false;
	RageAimStepChk.Checked = false;
	TriggerbotWhileAimbottingChk.Checked = true;
	SilentAimChk.Checked = true;
	pSilentChk.Checked = true;
}

void pSilent::Pop_pSilent()
{
	AntiAimRealDrop.index = (int)StoredRealAntiAim;
	AntiAimFakeDrop.index = (int)StoredFakeAntiAim;
	AntiAimPitchDrop.index = (int)StoredPitchAntiAim;
	SilentAimChk.Checked = StoredSilentAim;
	LegitAimStepChk.Checked = StoredLegitAimStep;
	RageAimStepChk.Checked = StoredRageAimStep;
	TriggerbotWhileAimbottingChk.Checked = StoredTriggerbotWhileAimbotting;
	pSilentChk.Checked = StoredpSilent;
	ManualAntiAimChk.Checked = StoredManualAntiAim;
}

bool pSilent::TraceOutFromCrosshair(CBaseEntity* &pEntityHit, QAngle& TargetAngles, Vector& BestBonePos, float& BestDamage, QAngle* LocalEyeAngles, Vector LocalEyePos, CBaseCombatWeapon* weapon, int& HitgroupHit)
{
	if (pSilentFOVTxt.flValue > 5.0f)
		return false;

	bool WasAbleToHitEdgeOfPlayer = false;

	//For now, just brute force to find the nearest edge of a player we can hit
	//I know this is really bad, it's just a first implementation. Gotta start somewhere
	QAngle tmpangles;
	Vector vecDir;
	Vector EndPos;

	//basically trace forward, left, right, up, down, up right, up left, down left, down right

	float flCurrentpSilentDegrees = 0.0f;

	while (!WasAbleToHitEdgeOfPlayer && flCurrentpSilentDegrees <= pSilentFOVTxt.flValue)
	{
		flCurrentpSilentDegrees += (pSilentFOVTxt.flValue / 7);
		for (int i = 0; i < 9; i++)
		{
			tmpangles = *LocalEyeAngles;
			switch (i)
			{
			case 0:
				break;
			case 1:
				tmpangles.y += flCurrentpSilentDegrees;
				break;
			case 2:
				tmpangles.y -= flCurrentpSilentDegrees;
				break;
			case 3:
				tmpangles.x += flCurrentpSilentDegrees;
				break;
			case 4:
				tmpangles.x -= flCurrentpSilentDegrees;
				break;
			case 5:
				tmpangles.x += flCurrentpSilentDegrees;
				tmpangles.y += flCurrentpSilentDegrees;
				break;
			case 6:
				tmpangles.x -= flCurrentpSilentDegrees;
				tmpangles.x -= flCurrentpSilentDegrees;
				break;
			case 7:
				tmpangles.x += flCurrentpSilentDegrees;
				tmpangles.y -= flCurrentpSilentDegrees;
				break;
			case 8:
				tmpangles.x -= flCurrentpSilentDegrees;
				tmpangles.y += flCurrentpSilentDegrees;
				break;
			}

			tmpangles.Clamp();
			AngleVectors(tmpangles, &vecDir);
			VectorNormalizeFast(vecDir);
			WeaponInfo_t *data = weapon->GetCSWpnData();
			float flRange = data ? data->flRange : 4096.0f;
			EndPos = LocalEyePos + vecDir * flRange;

			float tdmg;
			HitgroupHit = 0;
			Vector HitPos;
			CBaseEntity *EntityHit = MTargetting.TraceToPosition(LocalEyePos, &EndPos, &vecDir, &tmpangles, weapon, &tdmg, HitgroupHit, &HitPos);
			WasAbleToHitEdgeOfPlayer = MTargetting.IsPlayerAValidTarget(EntityHit);

			if (WasAbleToHitEdgeOfPlayer)
			{
				//Do rudimentary point scaling
				if (tmpangles.x != LocalEyeAngles->x)
				{
					if (tmpangles.x < LocalEyeAngles->x)
						tmpangles.x -= 0.125f;
					else
						tmpangles.x += 0.125f;
				}
				if (tmpangles.y != LocalEyeAngles->y)
				{
					if (tmpangles.y < LocalEyeAngles->y)
						tmpangles.y -= 0.125f;
					else
						tmpangles.y += 0.125f;
				}
				tmpangles.Clamp();

				//FoundBone = true;
				//CanTraceALine = true;
				BestBonePos = HitPos;
				BestDamage = tdmg;
				TargetAngles = tmpangles;
				pEntityHit = EntityHit;
				break;
			}
		}
	}
	return WasAbleToHitEdgeOfPlayer;
}

void pSilent::Run_pSilent(QAngle RealViewAngles, QAngle& angles, DWORD* FramePointer)
{
	//First, store your current angles and movement factors before doing any kind of anti-aim/spinbot/jitterbot/memebot
	float forwardmove = ReadFloat((uintptr_t)&CurrentUserCmd.cmd->forwardmove);
	float sidemove = ReadFloat((uintptr_t)&CurrentUserCmd.cmd->sidemove);
	float upmove = ReadFloat((uintptr_t)&CurrentUserCmd.cmd->upmove);

	//Get angles perpendicular to our yaw
	QAngle perpangles = angles;
	perpangles.x = 0.0f;

	if (sidemove < 0.0f)
		perpangles.y += 90.0f;
	else
		perpangles.y -= 90.0f;

	perpangles = perpangles.Normalize();
	perpangles.ClampY();

	// Now we need to make each viewangle apparent with each Vector using AngleVectors
	Vector viewForwardPS, viewRightPS, viewUpPS;
	AngleVectors(perpangles, &viewForwardPS, &viewRightPS, &viewUpPS);
	perpangles.x = angles.x;
	perpangles.ClampY();

	Vector aimUpPS, aimForwardPS, aimRightPS;
	AngleVectors(perpangles, &aimForwardPS, &aimRightPS, &aimUpPS);

	//Now we can normalize our current angles so they play nicely with the new ones when we do our dot products
	VectorNormalizeFast(viewForwardPS);
	VectorNormalizeFast(viewRightPS);
	VectorNormalizeFast(viewUpPS);

	VectorNormalizeFast(aimForwardPS);
	VectorNormalizeFast(aimRightPS);
	VectorNormalizeFast(aimUpPS);

	float newforwardPS, newrightPS;
	newforwardPS = DotProduct(viewForwardPS * forwardmove, aimForwardPS) + DotProduct(viewRightPS * sidemove, aimForwardPS) + DotProduct(viewUpPS * upmove, aimForwardPS);
	newrightPS = DotProduct(viewForwardPS * forwardmove, aimRightPS) + DotProduct(viewRightPS * sidemove, aimRightPS) + DotProduct(viewUpPS * upmove, aimRightPS);
	//Now we do our dot products to fix the previous movement factors we stored with the applied antiaim/angle manipulation:
	WriteFloat((uintptr_t)&CurrentUserCmd.cmd->forwardmove, clamp(newforwardPS, -450.0f, 450.0f));
	WriteFloat((uintptr_t)&CurrentUserCmd.cmd->sidemove, clamp(newrightPS, -450.0f, 450.0f));
	
	if (CurrentUserCmd.bSendPacket)
	{
		CurrentUserCmd.bSendPacket = false;
		CurrentUserCmd.SetIsFakeAngle(false);
		CurrentUserCmd.iCmdsChoked = FakeLagChokedTxt.iValue; //Don't allow choking more than 1 fire tick
	}

	//DWORD ClientState = (DWORD)ReadInt(pClientState);
	//INetChannel *netchan = (INetChannel*)ReadInt((uintptr_t)(ClientState + 0x9C));
	//netchan->m_nChokedPackets += 10000;

	LastWeaponFireCmdTickCount = CurrentUserCmd.cmd->tick_count; //CurrentUserCmd.cmd->tick_count + 1;
	//CurrentUserCmd.cmd->tick_count = INT_MAX;
}