#include "OverrideView.h"
#include "CreateMove.h"
#include "Overlay.h"
#include "GrenadePrediction.h"
#include "ThirdPerson.h"
#include "LocalPlayer.h"
float LocalFOV = 90.0f;
OverrideViewFN oOverrideView;
extern bool AllowCustomCode;

void __stdcall Hooks::OverrideView(CViewSetup *pSetup)
{
#if 0
	if (Interfaces::Engine->IsInGame() && Interfaces::Engine->IsConnected())
	{
		if (!LocalPlayer)
			return;

		static Vector newOrigin;

		short PressingGhostKey = GetAsyncKeyState(VK_XBUTTON1);

		if (/*GhostPlayerChk.Checked &&*/ PressingGhostKey)
		{
			float fSpeed = 5.f;
			int buttons = CurrentUserCmd.cmd->buttons;
			if (buttons & IN_DUCK)
				fSpeed = fSpeed * 0.45;

			if (buttons & IN_SPEED)
				fSpeed = fSpeed * 1.65;
			Vector vForward, vRight, vUp;
			AngleVectors(pSetup->angles, &vForward, &vRight, &vUp);

			if (buttons & IN_FORWARD) // W
				newOrigin += vForward * fSpeed;

			if (buttons & IN_MOVELEFT) // A
				newOrigin -= vRight * fSpeed;

			if (buttons & IN_MOVERIGHT) // D
				newOrigin += vRight * fSpeed;

			if (buttons & IN_BACK) // S
				newOrigin -= vForward * fSpeed;

			if (buttons & IN_JUMP || GetAsyncKeyState(VK_SPACE)) // Space
				newOrigin += vUp * fSpeed;

			if (buttons & IN_USE)
			{
				Vector vecEnd;
				trace_t tr;
				Ray_t ray;

				AngleVectors(pSetup->angles, &vecEnd);

				vecEnd = vecEnd * 8192.0f + newOrigin;

				ray.Init(newOrigin, vecEnd);

				Interfaces::EngineTrace->TraceRay(ray, CONTENTS_SOLID, NULL, &tr);

				newOrigin = tr.endpos;
			}
			pSetup->origin = newOrigin;
		}
		else
		{
			newOrigin = pSetup->origin;
		}
	}
#endif
	UpdateThirdPerson(pSetup);

	oOverrideView(pSetup);

	if (FOVChangerTxt.flValue != 90.0f && Interfaces::Engine->IsInGame() && !GetZoomLevel(LocalPlayer.Entity))
	{
		WriteFloat((uintptr_t)&pSetup->fov, FOVChangerTxt.flValue);
		LocalFOV = FOVChangerTxt.flValue;
	}
	else
	{
		LocalFOV = ReadFloat((uintptr_t)&pSetup->fov);
	}

	if (!DisableAllChk.Checked && AllowCustomCode)
	{
		if (GrenadeTrajectoryChk.Checked)
		{
			PredictGrenade();
		}
	}
}