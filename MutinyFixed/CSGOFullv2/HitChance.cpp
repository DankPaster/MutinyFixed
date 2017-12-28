#include "HitChance.h"
#include "BaseCombatWeapon.h"
#include "Trace.h"
#include "Overlay.h"
#include "Aimbot.h"
#include "LocalPlayer.h"
#include <process.h>
#include "VMProtectDefs.h"

float LastShotHitPercentage = 0.0f;
float CurrentHitPercentage = 0.0f;
#define CUSTOM_TRACING

Vector hForward, hRight, hUp;
float wSpread;
float wCone;
bool RevolverRightClick;
QAngle ViewAnglesPunched;
Vector EndPoint;
std::vector<CSphere>m_cSpheres;
std::vector<COBB>m_cOBBs;
CBaseEntity* pHitchanceTargetEntity;

HANDLE HitChanceWorkerThread1 = NULL;
HANDLE HitChanceWorkerThread2 = NULL;
HANDLE HitChanceWorkerThread3 = NULL;
HANDLE HitChanceWorkerThread4 = NULL;

HC_Vars HitchanceWorkerArgs1;// (256 / 2, &firstseedshit, firstseedsneeded, 256 - firstseedsneeded);
HC_Vars HitchanceWorkerArgs2;
HC_Vars HitchanceWorkerArgs3;
HC_Vars HitchanceWorkerArgs4;

void __stdcall Parallel_HitChance(HC_Vars* args)
{
	while (1)
	{
		WaitForSingleObject(args->DoWork_Event, INFINITE);

		Vector LocalEyePos = LocalPlayer.Entity->GetEyePosition();
		args->seedshit = 0;
		int i = 0;
		while (i < args->maxseeds)
		{
			RandomSeed(i + 1);

			float B = RandomFloat(0.f, 2.0f * M_PI_F);
			float C = RandomFloat(0.0f, 1.0f);
			float Spread = C * wSpread;
			float D = RandomFloat(0.f, 2.0f * M_PI_F);

			if (RevolverRightClick)
				C = 1.0f - (C * C);

			float Cone = C * wCone;

			Vector vSpread((cos(B) * Spread) + (cos(D) * Cone), (sin(B) * Spread) + (sin(D) * Cone), 0), vDir;

			vDir.x = hForward.x + (hRight.x * vSpread.x) + (hUp.x * vSpread.y);
			vDir.y = hForward.y + (hRight.y * vSpread.x) + (hUp.y * vSpread.y);
			vDir.z = hForward.z + (hRight.z * vSpread.x) + (hUp.z * vSpread.y);

			VectorNormalizeFast(vDir);
			//vDir.NormalizeInPlace();

			QAngle SpreadView;
			Vector End;
			VectorAngles(vDir, SpreadView);

			NormalizeAnglesNoClamp(SpreadView);

			AngleVectors(ViewAnglesPunched - (SpreadView - ViewAnglesPunched), &End);

			VectorNormalizeFast(End);

			trace_t tr;
			Ray_t ray;

			Vector TraceEnd = EndPoint + (End * EndPoint.Dist(LocalEyePos));
			ray.Init(LocalEyePos, TraceEnd);

#ifdef CUSTOM_TRACING
			TRACE_HITBOX(pHitchanceTargetEntity, ray, tr, m_cSpheres, m_cOBBs);
#else
			CTraceFilterPlayersOnlyNoWorld filter;
			filter.AllowTeammates = false;
			filter.pSkip = (IHandleEntity*)LocalPlayer.Entity;
			filter.m_icollisionGroup = COLLISION_GROUP_NONE;
			Interfaces::EngineTrace->TraceRay(ray, MASK_SHOT, (ITraceFilter*)&filter, &tr);
#endif


#ifdef _DEBUG
#ifdef DRAW_HITCHANCE_LINE
			if (DrawAimbotBoneChk.Checked)
			{
				if (EXTERNAL_WINDOW)
				{
					Vector From;
					WorldToScreen(Vector(LocalEyePos.x, LocalEyePos.y, LocalEyePos.z - 0.005f), From);
					Vector To;
					WorldToScreen(TraceEnd, To);
					DrawLine(Vector2D(From.x, From.y), Vector2D(To.x, To.y), 255, 255, 0, 255);
				}
				else
					Interfaces::DebugOverlay->AddLineOverlay(LocalEyePos, TraceEnd, 255, 255, 0, true, 0.05f);
			}
#endif
#endif

			//if (tr.fraction < 1.f && tr.m_pEnt && tr.m_pEnt == pTargetEntity)
			if (tr.m_pEnt == pHitchanceTargetEntity)
			{
#ifndef CUSTOM_TRACING
				int hitgroup = tr.hitgroup;
				if ((TargetHitGroup & HITGROUP_ALL_FLAG)
					|| ((TargetHitGroup & HITGROUP_HEAD_FLAG) && hitgroup == HITGROUP_HEAD)
					|| ((TargetHitGroup & HITGROUP_CHEST_FLAG) && hitgroup == HITGROUP_CHEST)
					|| ((TargetHitGroup & HITGROUP_STOMACH_FLAG) && hitgroup == HITGROUP_STOMACH)
					|| ((TargetHitGroup & HITGROUP_LEFTARM_FLAG) && hitgroup == HITGROUP_LEFTARM)
					|| ((TargetHitGroup & HITGROUP_RIGHTARM_FLAG) && hitgroup == HITGROUP_RIGHTARM)
					|| ((TargetHitGroup & HITGROUP_LEFTLEG_FLAG) && hitgroup == HITGROUP_LEFTLEG)
					|| ((TargetHitGroup & HITGROUP_RIGHTLEG_FLAG) && hitgroup == HITGROUP_RIGHTLEG))
#endif
				{
					args->seedshit++;
				}
			}

			i++;

			if (args->seedshit >= args->seedsneeded || i - args->seedshit > args->maxallowedmiss)
				break;
		}
		SetEvent(args->Finished_Event);
		ResetEvent(args->DoWork_Event);
	}

	_endthreadex(0);
}

void CreateHitchanceWorkerThreads()
{
	static bool AlreadyCreatedThreads = false;
	if (!AlreadyCreatedThreads)
	{
		HitchanceWorkerArgs1.maxseeds = 256 / 4; // divide by # of threads
		HitchanceWorkerArgs2.maxseeds = 256 / 4;
		HitchanceWorkerArgs3.maxseeds = 256 / 4;
		HitchanceWorkerArgs4.maxseeds = 256 / 4;

		HitchanceWorkerArgs1.DoWork_Event = CreateEvent(NULL, TRUE, FALSE, NULL);
		HitchanceWorkerArgs1.Finished_Event = CreateEvent(NULL, TRUE, FALSE, NULL);
		HitchanceWorkerArgs2.DoWork_Event = CreateEvent(NULL, TRUE, FALSE, NULL);
		HitchanceWorkerArgs2.Finished_Event = CreateEvent(NULL, TRUE, FALSE, NULL);
		HitchanceWorkerArgs3.DoWork_Event = CreateEvent(NULL, TRUE, FALSE, NULL);
		HitchanceWorkerArgs3.Finished_Event = CreateEvent(NULL, TRUE, FALSE, NULL);
		HitchanceWorkerArgs4.DoWork_Event = CreateEvent(NULL, TRUE, FALSE, NULL);
		HitchanceWorkerArgs4.Finished_Event = CreateEvent(NULL, TRUE, FALSE, NULL);

		unsigned int firstthreadid;
		HitChanceWorkerThread1 = (HANDLE)_beginthreadex(nullptr, 0, (_beginthreadex_proc_type)&Parallel_HitChance, &HitchanceWorkerArgs1, 0, &firstthreadid);
		unsigned int secondthreadid;
		HitChanceWorkerThread2 = (HANDLE)_beginthreadex(nullptr, 0, (_beginthreadex_proc_type)&Parallel_HitChance, &HitchanceWorkerArgs2, 0, &secondthreadid);
		unsigned int thirdthreadid;
		HitChanceWorkerThread3 = (HANDLE)_beginthreadex(nullptr, 0, (_beginthreadex_proc_type)&Parallel_HitChance, &HitchanceWorkerArgs3, 0, &thirdthreadid);
		unsigned int fourththreadid;
		HitChanceWorkerThread4 = (HANDLE)_beginthreadex(nullptr, 0, (_beginthreadex_proc_type)&Parallel_HitChance, &HitchanceWorkerArgs4, 0, &fourththreadid);

		AlreadyCreatedThreads = true;
	}
}

bool BulletWillHit(CBaseCombatWeapon* Weapon, int buttons, CBasePlayer* pTargetEntity, QAngle viewangles, Vector LocalEyePos, Vector *pos, unsigned int TargetHitGroup)
{
	VMP_BEGINMUTILATION("BWH")
	auto stat = START_PROFILING("HitChance");
	if (Weapon)
	{
		if (TargetHitGroup != HITGROUP_HEAD_FLAG)
		{
			if (AimbotHitPercentageBodyTxt.flValue == 0.0f)
			{
				END_PROFILING(stat);
				return true;
			}
		}
		else if (AimbotHitPercentageHeadTxt.flValue == 0.0f)
		{
			END_PROFILING(stat);
			return true;
		}

		//NormalizeAnglesNoClamp(viewangles);
		Vector Dir;
		// TODO: replace "cmd->buttons & IN_ATTACK2" with revolver mode check
		RevolverRightClick = (Weapon->GetItemDefinitionIndex() == WEAPON_REVOLVER) && (buttons & IN_ATTACK2);
		int SeedsHit = 0;
		//Weapon->UpdateAccuracyPenalty();
		wSpread = Weapon->GetWeaponSpread();
		wCone = Weapon->GetWeaponCone();
		QAngle PunchAngle = LocalPlayer.Entity->GetPunch() * 2.0f;
		ViewAnglesPunched = viewangles + PunchAngle;
		pHitchanceTargetEntity = pTargetEntity;

#ifdef _DEBUG
		if (isnan(wSpread) || isnan(wCone) || isnan(PunchAngle.x) || isnan(PunchAngle.y) || isnan(PunchAngle.z) || isnan(viewangles.x) || isnan(viewangles.y) || isnan(viewangles.z))
		{
			DebugBreak();
		}
#endif

		if (pos != nullptr)
		{
			AngleVectors(ViewAnglesPunched, &hForward, &hRight, &hUp);
			VectorNormalizeFast(hForward);
			VectorNormalizeFast(hRight);
			VectorNormalizeFast(hUp);
			EndPoint = *pos;
		}
		else
		{
			AngleVectors(ViewAnglesPunched, &hForward, &hRight, &hUp);
			VectorNormalizeFast(hForward);
			VectorNormalizeFast(hRight);
			VectorNormalizeFast(hUp);
			WeaponInfo_t *wpn_info = Weapon->GetCSWpnData();
			float flRange = wpn_info ? wpn_info->flRange : 4096.0f;
			EndPoint = LocalEyePos + hForward * flRange;
		}

#ifdef CUSTOM_TRACING
		auto setup = START_PROFILING("HitChance_HitboxSetup");

		matrix3x4_t boneMatrixes[MAXSTUDIOBONES];

		pTargetEntity->SetupBones(boneMatrixes, MAXSTUDIOBONES, BONE_USED_BY_HITBOX, /*GetSimulationTime()*/ Interfaces::Globals->curtime);

		mstudiohitboxset_t *set = Interfaces::ModelInfoClient->GetStudioModel(pTargetEntity->GetModel())->pHitboxSet(pTargetEntity->GetHitboxSet());

		for (int i = 0; i < set->numhitboxes; i++)
		{
			mstudiobbox_t *pbox = set->pHitbox(i);
			int Push = 0;

			if (TargetHitGroup == HITGROUP_ALL_FLAG)
			{
				if (pbox->radius != -1.0f)
					Push = 1;
				else
					Push = 2;
			}
			else
			{
				switch (pbox->group)
				{
				case HITGROUP_HEAD:
					if (TargetHitGroup & HITGROUP_HEAD_FLAG)
						Push = 1;
					break;
				case HITGROUP_CHEST:
					if (TargetHitGroup & HITGROUP_CHEST_FLAG)
						Push = 1;
					break;
				case HITGROUP_STOMACH:
					if (TargetHitGroup & HITGROUP_STOMACH_FLAG)
						Push = 1;
					break;
				case HITGROUP_LEFTARM:
					if (TargetHitGroup & HITGROUP_LEFTARM_FLAG)
					{
						if (pbox->radius != -1.0f)
							Push = 1;
						else
							Push = 2;
					}
					break;
				case HITGROUP_RIGHTARM:
					if (TargetHitGroup & HITGROUP_RIGHTARM_FLAG)
					{
						if (pbox->radius != -1.0f)
							Push = 1;
						else
							Push = 2;
					}
					break;
				case HITGROUP_LEFTLEG:
					if (TargetHitGroup & HITGROUP_LEFTLEG_FLAG)
					{
						if (pbox->radius != -1.0f)
							Push = 1;
						else
							Push = 2;
					}
					break;
				case HITGROUP_RIGHTLEG:
					if (TargetHitGroup & HITGROUP_RIGHTLEG_FLAG)
					{
						if (pbox->radius != -1.0f)
							Push = 1;
						else
							Push = 2;
					}
					break;
				case HITGROUP_GEAR:
				case HITGROUP_GENERIC:
					if (TargetHitGroup & HITGROUP_CHEST_FLAG)
					{
						if (pbox->radius != -1.0f)
							Push = 1;
						else
							Push = 2;
					}
					break;
				}
			}
			if (Push == 1)
			{
				Vector vMin, vMax;
				VectorTransformZ(pbox->bbmin, boneMatrixes[pbox->bone], vMin);
				VectorTransformZ(pbox->bbmax, boneMatrixes[pbox->bone], vMax);
				auto stat4 = START_PROFILING("HitChance_SetupCapsule");
				SetupCapsule(vMin, vMax, pbox->radius, pbox->group, m_cSpheres);
				END_PROFILING(stat4);
			}
			else if (Push == 2)
			{
				m_cOBBs.push_back(COBB(pbox->bbmin, pbox->bbmax, &boneMatrixes[pbox->bone], pbox->group));
			}
		}
		END_PROFILING(setup)
#endif

		//Calculate how many seeds we need to hit
		float HitChanceMultiplier = ((TargetHitGroup != HITGROUP_HEAD_FLAG ? AimbotHitPercentageBodyTxt.flValue : AimbotHitPercentageHeadTxt.flValue) / 100.0f);
		int totalseedsneeded = (int)(256 * HitChanceMultiplier);
		int individualseedsneeded = (int)((256 / 4) * HitChanceMultiplier);
		int individualmaxallowedmiss = (256 / 4) - individualseedsneeded;

		int remainderseedsneeded = totalseedsneeded - (individualseedsneeded * 4);
		int remaindermaxallowedmiss = (256 - totalseedsneeded) - (individualmaxallowedmiss * 4);

		HitchanceWorkerArgs1.seedsneeded = individualseedsneeded + remainderseedsneeded;
		HitchanceWorkerArgs1.maxallowedmiss = individualmaxallowedmiss + remaindermaxallowedmiss;
		HitchanceWorkerArgs2.seedsneeded = individualseedsneeded;
		HitchanceWorkerArgs2.maxallowedmiss = individualmaxallowedmiss;
		HitchanceWorkerArgs3.seedsneeded = individualseedsneeded;
		HitchanceWorkerArgs3.maxallowedmiss = individualmaxallowedmiss;
		HitchanceWorkerArgs4.seedsneeded = individualseedsneeded;
		HitchanceWorkerArgs4.maxallowedmiss = individualmaxallowedmiss;

		CreateHitchanceWorkerThreads();

		SetEvent(HitchanceWorkerArgs1.DoWork_Event);
		SetEvent(HitchanceWorkerArgs2.DoWork_Event);
		SetEvent(HitchanceWorkerArgs3.DoWork_Event);
		SetEvent(HitchanceWorkerArgs4.DoWork_Event);

		WaitForSingleObject(HitchanceWorkerArgs1.Finished_Event, INFINITE);
		WaitForSingleObject(HitchanceWorkerArgs2.Finished_Event, INFINITE);
		WaitForSingleObject(HitchanceWorkerArgs3.Finished_Event, INFINITE);
		WaitForSingleObject(HitchanceWorkerArgs4.Finished_Event, INFINITE);

		ResetEvent(HitchanceWorkerArgs1.Finished_Event);
		ResetEvent(HitchanceWorkerArgs2.Finished_Event);
		ResetEvent(HitchanceWorkerArgs3.Finished_Event);
		ResetEvent(HitchanceWorkerArgs4.Finished_Event);

		SeedsHit = (HitchanceWorkerArgs1.seedshit + HitchanceWorkerArgs2.seedshit) + (HitchanceWorkerArgs3.seedshit + HitchanceWorkerArgs4.seedshit);

		m_cSpheres.clear();
		m_cOBBs.clear();

		if (SeedsHit >= totalseedsneeded)
		{
			CurrentHitPercentage = ((float)SeedsHit / totalseedsneeded) * 100.0f;
			END_PROFILING(stat);
			return true;
		}

		CurrentHitPercentage = ((float)SeedsHit / totalseedsneeded) * 100.0f;
		END_PROFILING(stat);
		return false;
	}

	//No weapon
	CurrentHitPercentage = 0.0f;
	END_PROFILING(stat);
	return false;
	VMP_END
}



//Check if localPlayer just quick peaked
//If they just quick peaked ofc they are gonna have high spread
//Let's just give them some time to chill out
//Going to log the weaponCone by default for each gun standing still
//So I can get a whereabouts it should be or when should I wait until :)
bool localPlayerIsPeaking(CBaseCombatWeapon* Weapon)
{
	float speed = LocalPlayer.Entity->GetVelocity().Length2D();
	static int lastSpeed = speed;
	static bool normal = true;
	if (speed > 5 && lastSpeed > 5) // probably just quick peaked
	{
		normal = false;

	}
	//Should give a shit ton of leaniance on the values
	if (Weapon->GetWeaponCone() > 0.01)//0.01 is an arbritory value designed to simulate what it should be still or close enough
	{//Then we have gone back to normal
		normal = true;
		return true;
	}
	return false;
}

//Initial head scan will give a ball part option of hitchance
bool intialHeadScan(CBaseCombatWeapon* Weapon, Vector vMin, Vector vMax, int hitChance)
{
	//GetWeaponCone = getInaccuracy i think
	int screenWidth = 1920;//Probably a function somewhere to get that but it can be hard coded by now
	float radius = (screenWidth / 90.f * (Weapon->GetWeaponCone() * 50));
	radius *= 1.3f;//Lets just make circle a little bigger because why not
	float areafloat = 3.14159 * radius * radius;

	if (localPlayerIsPeaking(Weapon))
	{
		//This is badness, we should wait a little bit until we want to head shot,
		//We are going to still scan but we can set up a timer later to wait a certain
		//Amount of time until we shoot
	}
	Vector w2sMin, w2sMax;
	WorldToScreen(vMin, w2sMax);
	WorldToScreen(vMin, w2sMin);//Maybe should use debug overlay consol thing as that will work even if player is off screen? Idk if this does
								//If not I can just do some trig about distances, and distance between max and mins etc. etc. and find out the area of hitbox anyways

	float hitboxArea = (w2sMax.z - w2sMin.z)*(w2sMax.z - w2sMin.z) * 2;//Close enough to a square times 2 is there just to make up for it

																	   //I could do a whole check intercepts and get the actual area intersecting, however..... based on pass inaccuracies i ahve introduced
	if (hitboxArea > areafloat * (hitChance / 100))
	{
		return true;
	}
	return false;
}








bool BulletWillHitRays(CBaseCombatWeapon* Weapon, int buttons, CBasePlayer* pTargetEntity, QAngle viewangles, Vector LocalEyePos, Vector *pos, unsigned int TargetHitGroup)
{
	if (Weapon)
	{
		if (TargetHitGroup != HITGROUP_HEAD_FLAG)
		{
			if (AimbotHitPercentageBodyTxt.flValue == 0.0f)
				return true;
		}
		else if (AimbotHitPercentageHeadTxt.flValue == 0.0f)
			return true;

		//NormalizeAnglesNoClamp(viewangles);
		Vector Forward, Right, Up, Dir;
		// TODO: replace "cmd->buttons & IN_ATTACK2" with revolver mode check
		bool RevolverRightClick = (Weapon->GetItemDefinitionIndex() == WEAPON_REVOLVER) && (buttons & IN_ATTACK2);
		int SeedsHit = 0;
		//Weapon->UpdateAccuracyPenalty();
		float wSpread = Weapon->GetWeaponSpread();
		float wCone = Weapon->GetWeaponCone();
		QAngle PunchAngle = LocalPlayer.Entity->GetPunch() * 2.0f;
		QAngle ViewAnglesPunched = viewangles + PunchAngle;
		Vector EndPoint;

		if (pos != nullptr)
		{
			AngleVectors(ViewAnglesPunched, &Forward, &Right, &Up);
			VectorNormalizeFast(Forward);
			VectorNormalizeFast(Right);
			VectorNormalizeFast(Up);
			EndPoint = *pos;
		}
		else
		{
			AngleVectors(ViewAnglesPunched, &Forward, &Right, &Up);
			VectorNormalizeFast(Forward);
			VectorNormalizeFast(Right);
			VectorNormalizeFast(Up);
			WeaponInfo_t *wpn_info = Weapon->GetCSWpnData();
			float flRange = wpn_info ? wpn_info->flRange : 4096.0f;
			EndPoint = LocalEyePos + Forward * flRange;
		}

		//Calculate how many seeds we need to hit
		int seedsneeded = (int)(256 * ((TargetHitGroup != HITGROUP_HEAD_FLAG ? AimbotHitPercentageBodyTxt.flValue : AimbotHitPercentageHeadTxt.flValue) / 100.0f));
		int maxAllowedMiss = 256 - seedsneeded;
		int i = 0;
		while (SeedsHit < seedsneeded &&
			i < 256 &&
			(i - SeedsHit) > maxAllowedMiss)
		{
			RandomSeed(i + 1);

			float A = RandomFloat(0.0f, 1.0f);
			float B = RandomFloat(0.f, 2.0f * M_PI_F);

			float C = RandomFloat(0.0f, 1.0f);
			float D = RandomFloat(0.f, 2.0f * M_PI_F);

			if (RevolverRightClick)
				C = 1.0f - (C * C);

			float Spread = A * wSpread;
			float Cone = C * wCone;

			Vector vSpread((cos(B) * Spread) + (cos(D) * Cone), (sin(B) * Spread) + (sin(D) * Cone), 0), vDir;

			vDir.x = Forward.x + (Right.x * vSpread.x) + (Up.x * vSpread.y);
			vDir.y = Forward.y + (Right.y * vSpread.x) + (Up.y * vSpread.y);
			vDir.z = Forward.z + (Right.z * vSpread.x) + (Up.z * vSpread.y);

			vDir.NormalizeInPlace();

			QAngle SpreadView;
			Vector End;
			VectorAngles(vDir, SpreadView);

			NormalizeAnglesNoClamp(SpreadView);

			AngleVectors(ViewAnglesPunched - (SpreadView - ViewAnglesPunched), &End);

			VectorNormalizeFast(End);

			Vector TraceEnd = EndPoint + (End * EndPoint.Dist(LocalEyePos));

			QAngle targetAngle = CalcAngle(LocalEyePos, TraceEnd);
			//Use
			//https://www.unknowncheats.me/forum/counterstrike-global-offensive/161306-efficient-ray-capsule-intersection-algorithm.html
			//Don't know where to find capsual from. 
		}

		//CurrentHitPercentage = ((float)SeedsHit * 0.00390625f) * 100.f; //(seedshit / 256) * 100
		if (seedsneeded != 0)
		{
			CurrentHitPercentage = ((float)SeedsHit / seedsneeded) * 100.f; //(seedshit / 256) * 100

			if (TargetHitGroup == HITGROUP_HEAD_FLAG)
				return CurrentHitPercentage >= AimbotHitPercentageHeadTxt.flValue;

			return CurrentHitPercentage >= AimbotHitPercentageBodyTxt.flValue;
		}
		else
		{
			CurrentHitPercentage = 100.0f;
			return true;
		}
	}
	//No weapon
	CurrentHitPercentage = 0.0f;
	return false;
}


//bool ghettoasfhitchance(int hitboxID, CBaseEntity* Player, int hitchance)
//{
//	//From mutiny v1
//	if (hitchance <= 0)
//		return true; 
//
//	Vector a; 
//	Vector b; 
//	Vector c; 
//	Vector d; 
//
//
//	matrix3x4 matrix[128]; 
//	if (!Player->SetupBones(matrix, 128, 0x00000100, GetTickCount64())) return 0; 
//	const model_t* mod = Player->GetModel(); 
//	if (!mod) return 0; 
//	studiohdr_t* hdr = Interfaces.g_pModelInfo->GetStudioModel(mod); 
//	if (!hdr) return 0; 
//	mstudiohitboxset_t* set = hdr->GetHitboxSet(0); 
//	if (!set) return 0; 
//	mstudiobbox_t* hitbox = set->pHitbox(hitboxID); 
//	if (!hitbox) return 0; 
//	Vector vMin, vMax, vCenter, sCenter; 
//	g_Math.VectorTransform2(hitbox->bbmin, matrix[hitbox->bone], vMin); 
//	g_Math.VectorTransform2(hitbox->bbmax, matrix[hitbox->bone], vMax); 
//	Vector angs = Vector(0, 0, 0); 
//	CalcAngle(vMin, vMax, angs); 
//	vMin.z -= 4; 
//	vMax.z += 4; 
//
//	Interfaces.g_pDebugOverlay->ScreenPosition(vMin, a); 
//	Interfaces.g_pDebugOverlay->ScreenPosition(vMax, b); 
//	float area = 0;
//	//Some size manipulation cause people complated too much about it not shooting enough
//	if (a.y < b.y)
//		area = abs(b.y - a.y) * abs(b.y - a.y);
//	else
//		area = abs(a.y - b.y) * abs(a.y - b.y); 
//
//	area += 125; 
//
//	if (hitboxID != (int)CSGOHitboxID::Head && hitboxID != (int)CSGOHitboxID::Neck)
//	{
//		area *= 1.5; 
//	}
//	else if (hitboxID == (int)CSGOHitboxID::Head && hitchance > 75)
//	{
//		area /= 1.5; 
//	}
//	if (hitboxID != (int)CSGOHitboxID::Pelvis)
//	{
//		area *= 2; 
//	}
//	float Innac = Hacks.LocalWeapon->GetInaccuracy(); 
//	int w = 0, h = 0;
//	Interfaces.pEngine->GetScreenSize(w, h); 
//	float size = (w / 90.f * (Innac * 50)); 
//	float areafloat = PI * size * size; 
//	float mutiplier = hitchance; 
//	mutiplier /= 100; 
//	if (area > areafloat * (mutiplier))
//	{
//		return true; 
//	}
//	return false; 
//}


//Maths for sphere ray tracing
//https://www.scratchapixel.com/lessons/3d-basic-rendering/minimal-ray-tracer-rendering-simple-shapes/ray-sphere-intersection

//Thanks http://geomalgorithms.com/a07-_distance.html
