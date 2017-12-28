#include "AutoGrief.h"
#include "LocalPlayer.h"

char *grieftargetstr = new char[13]{ 61, 8, 19, 31, 28, 90, 46, 27, 8, 29, 31, 14, 0 }; /*Grief Target*/
int LastGriefingTargetID = INVALID_PLAYER;
Vector GriefChestPos = { 0,0,0 };
void AutoGrief(int& buttons, QAngle &angles, QAngle RealViewAngles, int PressingAimbotKey)
{
	static float flNextTimeChangeGriefingTargets = 0.0f;
	if (PressingAimbotKey && Time >= flNextTimeChangeGriefingTargets)
	{
		Vector LocalEyePos = LocalPlayer.Entity->GetEyePosition();
		Vector vecForward;
		AngleVectors(RealViewAngles, &vecForward);
		VectorNormalizeFast(vecForward);
		trace_t tr;
		Ray_t ray;
		ray.Init(LocalEyePos, LocalEyePos + vecForward * 400.0f);
		CTraceFilterPlayersOnlyNoWorld filter;
		filter.AllowTeammates = true;
		filter.pSkip = (IHandleEntity*)LocalPlayer.Entity;
		filter.m_icollisionGroup = COLLISION_GROUP_NONE;
		Interfaces::EngineTrace->TraceRay(ray, MASK_SHOT, (ITraceFilter*)&filter, &tr);
		LastGriefingTargetID = tr.m_pEnt ? ReadInt((uintptr_t)&tr.m_pEnt->index) : INVALID_PLAYER;
		flNextTimeChangeGriefingTargets = Time + 0.1f;
	}

	if (LastGriefingTargetID != INVALID_PLAYER)
	{
		CBaseEntity *pGrief = Interfaces::ClientEntList->GetClientEntity(LastGriefingTargetID);
		if (pGrief && !pGrief->GetDormant() && pGrief->GetAlive())
		{
			float dist = pGrief->GetOrigin().Dist(LocalPlayer.Entity->GetOrigin());
			if (dist > 400.0f || (GetAsyncKeyState(VK_RETURN) & (1 << 16)))
			{
				LastGriefingTargetID = INVALID_PLAYER;
				return;
			}
			GriefChestPos = pGrief->GetBonePosition(HITBOX_CHEST, /*pGrief->GetSimulationTime()*/Interfaces::Globals->curtime, false, false);
			Vector targForward;
			QAngle griefeyeangles = pGrief->GetEyeAngles();

			float DuckAmount = pGrief->GetDuckAmount();
			if (DuckAmount > 0)
				buttons |= IN_DUCK;
			if (!(pGrief->GetFlags() & FL_ONGROUND) && (LocalPlayer.Entity->GetFlags() & FL_ONGROUND))
				buttons |= IN_JUMP;

			AngleVectors(griefeyeangles, &targForward);
			VectorNormalizeFast(targForward);
			Vector griefeyepos = pGrief->GetEyePosition();
			Vector infrontoftargetpos = griefeyepos + targForward * 2.0f;
			Vector spaceinfrontoftargetpos = griefeyepos + targForward * 50.0f;
			Vector LocalEyePos = LocalPlayer.Entity->GetEyePosition();
			QAngle blockerAngle = CalcAngle(LocalEyePos, infrontoftargetpos);
			ClampViewAngles(blockerAngle);
			float fovfromustotarget = GetFovRegardlessOfDistance(RealViewAngles, blockerAngle, (infrontoftargetpos - LocalEyePos).Length());
			float lookfov = GetFov(RealViewAngles, pGrief->GetEyeAngles());
			//if (fovfromustotarget <= 160.0f)
			{
				Vector heading = (spaceinfrontoftargetpos - LocalEyePos);
				Vector vecForward;
				Vector vecRight, vecUp;
				AngleVectors(angles, &vecForward, &vecRight, &vecUp);
				VectorNormalizeFast(vecForward);
				float forward = DotProduct(heading, vecForward);
				float side = DotProduct(heading, vecRight);

				if (forward > 0)
				{
					if (!(buttons & IN_BACK))
					{
						buttons |= IN_FORWARD;
						WriteFloat((uintptr_t)&CurrentUserCmd.cmd->forwardmove, 450.0f);
					}
				}
				else if (forward < 0)
				{
					if (!(buttons & IN_FORWARD))
					{
						buttons |= IN_BACK;
						WriteFloat((uintptr_t)&CurrentUserCmd.cmd->forwardmove, -450.0f);
					}
				}

				if (side < 0)
				{
					if (!(buttons & IN_MOVERIGHT))
					{
						buttons |= IN_MOVELEFT;
						WriteFloat((uintptr_t)&CurrentUserCmd.cmd->sidemove, -450.0f);
					}
				}
				else if (side > 0)
				{
					if (!(buttons & IN_MOVELEFT))
					{
						buttons |= IN_MOVERIGHT;
						WriteFloat((uintptr_t)&CurrentUserCmd.cmd->sidemove, 450.0f);
					}
				}

				if (fabsf(forward) < 0.5f)
				{

					float targetyaw = griefeyeangles.y + 180.0f;
					ClampY(targetyaw);

					float deltaouranglestotargetyaw = targetyaw - RealViewAngles.y;
					ClampY(deltaouranglestotargetyaw);
					float yawchange = (ReadFloat((uintptr_t)&Interfaces::Globals->interval_per_tick) * 250.0f);
					if (deltaouranglestotargetyaw < 0)
					{
						angles.y -= yawchange;
					}
					else
					{
						angles.y += yawchange;
					}
					ClampY(angles.y);
					if (fabsf(deltaouranglestotargetyaw) > 0.1f)
						Interfaces::Engine->SetViewAngles(angles); //FIXME: THIS IS OVERRIDED LATER IN CREATEMOVE!!
					//if (lookfov > 100.0f)
					{
						float delta = fabsf(blockerAngle.y - RealViewAngles.y);
						ClampY(delta);
						if (delta > 2.5f)
						{
							if (blockerAngle.y > RealViewAngles.y)
							{
								if (!(buttons & IN_MOVERIGHT))
								{
									buttons |= IN_MOVELEFT;
									WriteFloat((uintptr_t)&CurrentUserCmd.cmd->sidemove, -450.0f);
								}
							}
							else if (blockerAngle.y < RealViewAngles.y)
							{
								if (!(buttons & IN_MOVELEFT))
								{
									buttons |= IN_MOVERIGHT;
									WriteFloat((uintptr_t)&CurrentUserCmd.cmd->sidemove, 450.0f);
								}
							}
						}
					}
				}
			}
			//else
			{
				//LastGriefingTargetID = INVALID_PLAYER;
			}
		}
		else
		{
			LastGriefingTargetID = INVALID_PLAYER;
		}
	}
}