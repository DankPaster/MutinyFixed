#include "NoRecoil.h"
#include "BaseCombatWeapon.h"
#include "Overlay.h"
#include "Events.h"
#include "LocalPlayer.h"

bool IsAllowedToNoRecoil(CBaseCombatWeapon* weapon)
{
	if (!weapon)
		return true;

	if (NoRecoilChk.Checked)
	{
		if (Time - LocalPlayer.TimeSinceWeaponChange < RECOIL_GRACE_PERIOD)
			return true;
#if 0 //TODO: FIX ME!
		if (LocalPlayerIsShot && !NoRecoilRageChk.Checked)
		{
			return false;
	}
#endif
		int ID = weapon->GetItemDefinitionIndex();
		if (NoRecoilPistolsShotgunsSnipersChk.Checked || (!weapon->IsPistol(ID) && !weapon->IsShotgun(ID) && !weapon->IsSniper(true, ID)))
			return true;
	}
	return false;
}

void NoRecoil(CBaseCombatWeapon* weapon, bool& DontSetViewAngles, QAngle& angles, QAngle& RealViewAngles)
{
#if 0
	if (!AimbotModifiedAngles/*!Fire*/)
	{
		//Cancel out view angle snap to target
		//angles = RealViewAngles;
		CancelOutViewAngleSnap = false; //Don't reset view angles again because we want no recoil!
	}
#endif
	if (weapon)
		weapon->UpdateAccuracyPenalty(); //TODO: check untrusted
	angles -= LocalPlayer.Entity->GetPunch() * 2.0f;
	//if (ALastTargetIndex && angles != RealViewAngles)
		//DontSetViewAngles = false;
}

void RemoveRecoil(QAngle& angles)//, int &buttons)
{
	//static QAngle lastpunch = QAngle(0, 0, 0);
	//if (weapon)
		//weapon->UpdateAccuracyPenalty();
	QAngle punch = LocalPlayer.Entity->GetPunch();
	QAngle punchdelta = punch - LocalPlayer.LastAimPunch;
	QAngle recoilsubtracted;
	float IsShotAimPunchDeltaLenSqr = fabsf(LocalPlayer.IsShotAimPunchDelta.LengthSqr());

	if (NoRecoilRageChk.Checked || (IsShotAimPunchDeltaLenSqr <= 0.001f && !LocalPlayer.IsWaitingToSetIsShotAimPunch && fabsf(punchdelta.x) < 5.0f))
	{
		if (NoVisualRecoilChk.Checked)
			recoilsubtracted = punch * 2.0f;
		else
			recoilsubtracted = punchdelta * 2.0f;

		recoilsubtracted.x *= NoRecoilYPercentTxt.flValue * 0.01f;
		recoilsubtracted.y *= NoRecoilXPercentTxt.flValue * 0.01f;

		angles -= recoilsubtracted;

		LocalPlayer.IsShotAimPunchDelta = angZero;
	}
	else
	{
		recoilsubtracted = angZero;
	}

	if (LocalPlayer.AimPunchSubtracted == angZero)
	{
		LocalPlayer.CurrentAimPunchDelta = punchdelta;
		LocalPlayer.AimPunchSubtracted = recoilsubtracted;
		//LocalPlayer.LastAimPunch = punch; //Removed because it's set at the end of CreateMove now

		if (IsShotAimPunchDeltaLenSqr > 0.001f)
		{
			Vector decay = LocalPlayer.Entity->GetPunchVel() * Interfaces::Globals->frametime;
			decay.x = fabsf(decay.x);
			decay.y = fabsf(decay.y);
			
			if (LocalPlayer.IsShotAimPunchDelta.x < 0.0f)
				LocalPlayer.IsShotAimPunchDelta.x += decay.x;
			else
				LocalPlayer.IsShotAimPunchDelta.x -= decay.x;

			if (LocalPlayer.IsShotAimPunchDelta.y < 0.0f)
				LocalPlayer.IsShotAimPunchDelta.y += decay.y;
			else
				LocalPlayer.IsShotAimPunchDelta.y -= decay.y;;

			LocalPlayer.IsShotAimPunchDelta.Clamp();
		}
	}

#if 0
	if ((buttons & IN_ATTACK))
	{
		if (LocalPlayer.Entity->GetShotsFired() > 0)
		{
			QAngle punch = LocalPlayer.Entity->GetPunch() * 2.0f;
			angles -= punch;
			lastpunch = angles;
#if 0
			static QAngle LastRecoilAngles = { 0.0f, 0.0f, 0.0f };
			if (LocalPlayer.Entity->GetShotsFired() > 0)
			{
				angles += LastRecoilAngles;
				// Add the old "viewpunch" so we get the "center" of the screen again
				QAngle punchangle = LocalPlayer.Entity->GetPunch() * 2.0f;
				angles -= punchangle;
				LastRecoilAngles = punchangle;
			}
			else
			{
				LastRecoilAngles = { 0.0f, 0.0f, 0.0f };
			}
		}
#endif
		}
	}
	else
	{
#if 0
		QAngle punch = LocalPlayer.Entity->GetPunch() * 2.0f;
		QAngle delta = lastpunch - punch;
		QAngle newshit = angles;
		newshit -= punch;
		angles = lastpunch;
		lastpunch = newshit;
#else
		angles -= LocalPlayer.Entity->GetPunch() * 2.0f;
#endif
	}
#endif
}

#ifdef SPARK_SCRIPT_TESTING
void SparkDebugNoRecoil()
{
	QAngle delta = LocalPlayer.CurrentEyeAngles - LocalPlayer.TargetEyeAngles;
	LocalPlayer.TargetEyeAngles = LocalPlayer.CurrentEyeAngles;

	static QAngle lastpunch = angZero;

	QAngle punch = LocalPlayer.Entity->GetPunch();
	int fov = GetFOV(LocalPlayer.Entity);
	if (fov == 0)
		fov = 90;
	int dx = ((punch.x - lastpunch.x) * 2.0f) * -((1920 / fov * 1.1f));
	int dy = (((punch.y - lastpunch.y) * 2.0f)) * (1920 / fov * 1.1f);
	mouse_event(MOUSEEVENTF_MOVE, dy, dx, 0, 0);

	lastpunch = LocalPlayer.Entity->GetPunch();
}
#endif