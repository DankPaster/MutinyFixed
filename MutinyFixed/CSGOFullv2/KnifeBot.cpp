#include "KnifeBot.h"
#include "Aimbot.h"
#include "Overlay.h"
#include "Triggerbot.h"
#include "BaseCombatWeapon.h"
#include "LocalPlayer.h"
#include "VMProtectDefs.h"

#define KNIFE_DISTANCE_CLOSE 60.0f
#define KNIFE_DISTANCE_FAR 73.0f

//check if angle is between angles, FIXME: MOVE ME
static inline BOOL angle_is_between_angles(float N, float a, float b) {
	//glorious shit code commencing
	while (N < -180.0f)
		N += 360.0f;

	while (N >  180.0f)
		N -= 360.0f;

	if (N > 180.f)
		N = 180.f;
	if (N < -180.f)
		N = -180.f;

	while (a < -180.0f)
		a += 360.0f;

	while (a >  180.0f)
		a -= 360.0f;

	if (a > 180.f)
		a = 180.f;
	if (a < -180.f)
		a = -180.f;

	while (b < -180.0f)
		b += 360.0f;

	while (b >  180.0f)
		b -= 360.0f;

	if (b > 180.f)
		b = 180.f;
	if (b < -180.f)
		b = -180.f;

	if (a < b)
		return a <= N && N <= b;
	return a <= N || N <= b;
}

bool KnifePlayer(int &buttons, QAngle *LocalEyeAngles, CBaseEntity* pPlayer, CBaseCombatWeapon *weapon)
{
	float distance = (pPlayer->GetOrigin() - LocalPlayer.Entity->GetOrigin()).Length();
	if (distance <= KNIFE_DISTANCE_FAR)
	{
		int armor = pPlayer->GetArmor();
		int health = pPlayer->GetHealth();
		float m_flServerTime = LocalPlayer.Entity->GetTickBase() * ReadFloat((uintptr_t)&Interfaces::Globals->interval_per_tick);
		float TimeSinceFire = m_flServerTime - weapon->GetNextPrimaryAttack();
		if (distance <= KNIFE_DISTANCE_CLOSE)
		{
			//bool helmet = tr.m_pEnt->HasHelmet();
			QAngle targetplayerangles = pPlayer->GetEyeAngles();
			BOOL IsBack = angle_is_between_angles(LocalEyeAngles->y, targetplayerangles.y - 45.0f, targetplayerangles.y + 45.0f);
			if (IsBack)
			{
				buttons |= IN_ATTACK2;
				gTriggerbot.NextTimeTriggerBot = Time + 0.2f;
			}
			else
			{
				if (TimeSinceFire < 0.6f)
				{
					if (armor > 0)
					{
						if (health - 21 <= 0)
							buttons |= IN_ATTACK;
						else
							buttons |= IN_ATTACK2;
					}
					else
					{
						if (health - 25 <= 0)
							buttons |= IN_ATTACK;
						else
							buttons |= IN_ATTACK2;
					}
				}
				else
				{
					if (armor > 0)
					{
						if (health - 34 <= 0)
							buttons |= IN_ATTACK;
						else
							buttons |= IN_ATTACK2;
					}
					else
					{
						if (health - 40 <= 0)
							buttons |= IN_ATTACK;
						else
							buttons |= IN_ATTACK2;
					}
				}
				gTriggerbot.NextTimeTriggerBot = Time + 0.2f;
			}
		}
		else
		{
			//distance too far to rightclick
			buttons |= IN_ATTACK;
			gTriggerbot.NextTimeTriggerBot = Time + 0.1f;
		}
		return true;
	}
	return false;
}

struct CKnifeTarget
{
	float DistanceFromCrosshair;
	int Bone;
	Vector BonePos;
	CBaseEntity* BaseEntity;
};

CBaseEntity* FindKnifeTarget(QAngle *LocalEyeAngles, Vector &TargetBonePos)
{
	CKnifeTarget KPlayers[MAX_PLAYERS];
	DWORD IndexOfClosest = INVALID_PLAYER; //Index of the closest player to the crosshair, used with AllPlayers, not streamed
	for (int i = 0; i < NumStreamedPlayers; i++)
	{
		CustomPlayer *pCPlayer = StreamedPlayers[i];
		if (!pCPlayer)
			continue;
		if (!pCPlayer->Dormant && pCPlayer->IsTarget && pCPlayer->LifeState == LIFE_ALIVE)
		{
			CBaseEntity *pPlayer = pCPlayer->BaseEntity;
			Vector TargetBonePos;
			TargetBonePos = pPlayer->GetBonePosition(LastBoneChecked, /*pPlayer->GetSimulationTime()*/Interfaces::Globals->curtime, false, true);

#if 0
			if (!BoneIsStreamed(&pCPlayer->Origin, &TargetBonePos))//if (!BoneIsStreamed(BoneMatrix, &pPlayer->Origin, TargetBonePos, pPlayer->BaseEntity))
			{
				//Failed to find a valid bone!!!
				ClearTargettedBone();
				continue;
			}
#endif

			//TODO FIXME!!!!!!!!!!!! don't use CenterOfScreen for this, use point relative to recoil
			Vector W2S;
			bool IsInsideKnifeFOV = false;
			Vector TestPos;
			//Find a bone that is streamed
			const int count = (sizeof(HITBOX_GROUP_TORSO) / sizeof(HITBOX_GROUP_TORSO[0]));
			for (int k = 0; k < count; k++)
			{
				int Bone = HITBOX_GROUP_TORSO[k];
				TestPos = pPlayer->GetBonePosition(Bone, /*pPlayer->GetSimulationTime()*/Interfaces::Globals->curtime, false, true);
	
				if (/*BoneIsStreamed(&pCPlayer->Origin, &TestPos) && */WorldToScreen(TestPos, W2S))
				{
					//FOUND BONE!
					trace_t tr;
					float dmg = 0.0f;
					if (IsBoneVisible(false, Bone, TestPos, LocalPlayer.Entity->GetEyePosition(), tr, pPlayer, true, HITGROUP_CHEST_FLAG | HITGROUP_LEFTARM_FLAG | HITGROUP_RIGHTARM_FLAG, dmg, LocalEyeAngles))
					{
						float distancefromch = DistanceBetweenPoints(W2S, CenterOfScreen);
						if (IndexOfClosest == INVALID_PLAYER || distancefromch < KPlayers[IndexOfClosest].DistanceFromCrosshair)
						{
							KPlayers[i].DistanceFromCrosshair = distancefromch;
							KPlayers[i].Bone = Bone;
							KPlayers[i].BonePos = TestPos;
							KPlayers[i].BaseEntity = pPlayer;
							IndexOfClosest = i;
						}
						break;
					}
				}
			}
		}
	}
	if (IndexOfClosest != INVALID_PLAYER)
	{
		CKnifeTarget *pKPlayer = &KPlayers[IndexOfClosest];
		float fov = GetFovFromCoordsRegardlessofDistance(LocalPlayer.Entity->GetEyePosition(), pKPlayer->BonePos, *LocalEyeAngles);
		if (fov <= 42.5f)
		{
			TargetBonePos = pKPlayer->BonePos;
			return pKPlayer->BaseEntity;
		}
	}
	return nullptr;
}

void KnifeBot(int &buttons, CBaseEntity* pPlayer, QAngle* LocalEyeAngles, CBaseCombatWeapon *weapon, bool &DontSetViewAngles)
{
	if (pPlayer)
	{
		if (KnifePlayer(buttons, LocalEyeAngles, pPlayer, weapon))
			DontSetViewAngles = false;
	}
	else
	{
		Vector vecDir;
		Vector EyePos = LocalPlayer.Entity->GetEyePosition();
		AngleVectors(*LocalEyeAngles, &vecDir);
		VectorNormalizeFast(vecDir);
		Vector EndPos = EyePos + (vecDir * 16384);
		trace_t tr;
		UTIL_TraceLine(EyePos, EndPos, MASK_SHOT, LocalPlayer.Entity, &tr);
		CTraceFilter filter;
		filter.pSkip = (IHandleEntity*)LocalPlayer.Entity;
		UTIL_ClipTraceToPlayers(EyePos, EndPos + vecDir * 40.0f, 0x4600400B, &filter, &tr);
#if 0
		//Draw the distance between us and target
		if (tr.m_pEnt)
		{
			char text[1024];
			sprintf(text, "%.2f", (tr.endpos - EyePos).Length());
			DrawStringUnencrypted(text, Vector2D(640, 480), 255, 0, 0, pFont);
		}
#endif
		bool CanHit = tr.m_pEnt && (TargetTeammatesChk.Checked || tr.m_pEnt->GetTeam() != LocalPlayer.Entity->GetTeam()) && tr.m_pEnt->GetAlive() && tr.m_pEnt->GetHealth() > 0 && MTargetting.IsHitgroupACurrentTarget(tr.hitgroup);
		if (CanHit)
		{
			KnifeBot(buttons, tr.m_pEnt, LocalEyeAngles, weapon, DontSetViewAngles);
		}
		else
		{
			Vector TargetBonePos;
			CBaseEntity *pPlayerNearCrosshair = FindKnifeTarget(LocalEyeAngles, TargetBonePos);
			if (pPlayerNearCrosshair)
			{
				KnifeBot(buttons, pPlayerNearCrosshair, LocalEyeAngles, weapon, DontSetViewAngles);
			}
		}
	}
}