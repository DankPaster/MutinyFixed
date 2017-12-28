#include "AutoBone.h"
#include "Aimbot.h"
#include "HitChance.h"
#include "AutoWall.h"
#include "LocalPlayer.h"
#include "Triggerbot.h"

AutoBone gAutoBone;
//Thank you for your service shark but fuck you I am doing it my way
//Okay so this isn't a working auto bone dylan it is just some ideas I have come up with and have gone and 
//Written up, I would appreciated it if you could please look at them and decide if you think they are useful
//I believe each of these things are very useful :)

/*
- Errors that I have noticed with current, they may not be errors just what I think:
- You don't backtrack the position of the target player wellif you are backtracking them 200ms you need to move
their hitboxes back to, unless curtime does that but I doubt it :)
- You need to backtrack local player before running autobone to make sure angles are correct for trace, more important
in nospread or when moving.
- Need to actually move the player back when firing so angle to new target is correct since we are backtracking
*/
/*
- Thinks I am adding since I can and think will help
- Low intensity hitchance algorthym
- When localplayer quick peaks a smallperiod of time for spread to decrease
- Backtracking
*/



//So here is a half code and sudo of how I think we should change things, to make it work better especially with hit chance
//I would love it if you could look through. 
//Thank you
//Love Jake
#ifdef jake
bool AutoBone::AutoBone_BackTrack(CustomPlayer* pCPlayer, StoredNetvars *BestTick)
{
	//So we have already auto bonned at this tick! Well fuck yes party time cpu saving like a fuck ton
	if (BestTick->foundBone)
	{
		//Fuck yes at least somewhere on target was visible OMFG beste day of moi lyf.
		//Since we all like pelvis aiming lets check pelvis tho
		if (TargetEntityHealth - BestTick->pelvisDamage <= 0.f&& BodyAimIfLethalChk.Checked) // Best damage done via pelvis
		{
			//Best fucking day of my fucking life PELVIS AIM MOFO
			*pBestHitbox = HITBOX_PELVIS;
			*pBestHitboxPos = BestTick->bestPelvisPos;
			BestDamage = BestTick->pelvisDamage;
			return true;
		}
		if (TargetEntityHealth - BestTick->foundDamage <= 0.f)
		{
			*pBestHitbox = BestTick->foundBone;
			*pBestHitboxPos = BestTick->bestPelvisPos;
			BestDamage = BestTick->foundDamage;
			return true;
		}
		if (BestTick->foundHead)
		{
			//Lets check hit chance against head first

			//if(intialHeadScan(shit in here as per params)
			*pBestHitbox = HITBOX_HEAD;
			*pBestHitboxPos = BestTick->bestHeadPos;
			BestDamage = BestTick->headDamage;
			return true;
			//Okay so we found head last tick, lets check hit chance on it
		}
		if (BestTick->foundDamage >= MinimumDamageBeforeFiringTxt.flValue)
		{
			*pBestHitbox = BestTick->foundBone;
			*pBestHitboxPos = BestTick->bestPelvisPos;
			BestDamage = BestTick->foundDamage;
			return true;
		}
	}
	return false;
}
bool AutoBone::Run_AutoBone(CBaseCombatWeapon* weapon, CustomPlayer* pCAPlayer, QAngle *LocalEyeAngles, Vector LocalEyePos, int& BestBone, Vector& BestBonePos, bool& RanHitChance, bool& CanTraceALine)
{
	pPlayer = pCAPlayer->BaseEntity;
	if (!pPlayer)
		return false;
	if (!weapon)
		return false;
	if (weapon->IsKnife(NULL))
	{
		if (gAutoBone.AutoBone_Knife())
		{
			gTriggerbot.SetAllowedToFire(true);
			return true;
		}
		return false;
	}
	TargetEntityHealth = pPlayer->GetHealth();

	BoneTargetTime = Interfaces::Globals->curtime;//Unless curtime is backtrack time but i don't think ti is
												  //So if we are backtracking... We should really be getting the bone position at the point we are backtracking too... I mean hello?
	StoredNetvars *BestTick = pCPlayer->GetBestBacktrackTick();
	if (BestTick)
		BoneTargetTime = BestTick->simulationtime;
	else
		BoneTargetTime = pPlayer->GetSimulationTime();

	//So we may of already scanned the bones at this time so lets check
	if (BestTick->autoBonned)
	{
		if (!BestTick->foundBone) // Didn't find a bone then when we scanned why are we gonna find one now :)
			return false;
		if (AutoBone_BackTrack(pCAPlayer, BestTick))
		{
			BestBone = *pBestHitbox;
			BestBonePos = *pBestHitboxPos;
			RanHitChance = false;//Doubt it was run
			CanTraceALine = true;
			return true;
		}
	}
	//Okay so we haven't scanned the head thats okay


	//It is getting late and I am getting tired, I am going to start writting sloppy sudo so you get whats going on :),Please comment
	//On here or just pm me if you want me to add any of these features to the cheat, they are just ideas for you :)
	//Since I know you spent alot of time on thsi and I don't want to insult you by just redoing it myself :)
	if (AimbotAimTorsoChk.Checked)
	{
		//Scan just pelvis here, not chest just pelvis
		ScanPelvis();

		if (lethal)
			return true;
	}
	//So if pelvis killed them we have stopped here, but if we go onto here it clearly hasn't lets do the rest

	static bool detectPeakingPlayers = true; // do the whole if ducked, if running etc. etc.
	if (target->getvelocity() - < length()2d == 0)
		detectPeakingPlayers = false;
	//We could extrapolate their movement 16 ticks then scan head to see if we hit htem but thats over kill for now
	bool onlyHedashot = false;
	static float start = curtime;

	if (curtime - start > 100ms)
		detectPeakingPlayers = false;
	//100ms have

	if (detectPeakingPlayers)
	{
		onlyHedashot = true;
	}
	if (!torso, !legs, !arms)
		onlyHedashot = true;
	if (!ValveResolverChk.Checked && onlyHedashot)
		AutoBone_HeadOnly();
	bool didhit = ScanHead();
	if (headdamage < mindamage)
		didhit = false;
	//if(intialHeadScan(shit in here as per params)
	{
		if (BulletWillHit) // We ran 0 cpu check first so we aren't wasting cpu on obvious when it cant
		else
			didhit = false
	}
	else
		didhit = false;
	if (onlyhead)
		return didhit;
	if (LocalPlayerPeaked && tooLowHitcahnce)//Call function in hitchance
	{
		static float waitTimet = curtime;

		if (curtime - waitTimet < 100ms)
		{
			if (!BulletWillHit)
				return false;//Lets just wait until we can actually 
			else
				return true;
		}
		//100ms have
	}
	//Run normal hit scans from here on out.

	if (bestDamage <= 0)//Shit we got nothing
	{
		//Scan last 200ms worth of autobone ticks to ese if we hit pelvis for any damage, 
		//We could do head but we would have to update the head angles too
		//loop through histor find best one :)
	}

	//Remember to store history of these scans :)

}
#else
//std::deque<float>FrameTimes;
std::deque<double>TimeToRun;

#define END_AUTOBONE_PROFILING \
	TimeToRun.push_front(QPCTime() - StartTime); \
	if (TimeToRun.size() > 32) \
	TimeToRun.pop_back(); \
	END_PROFILING(stat);

//Returns true if it found a bone to target
bool AutoBone::Run_AutoBone(CBaseCombatWeapon* weapon, CustomPlayer* pCAPlayer, QAngle *LocalEyeAngles, Vector LocalEyePos, int& BestBone, Vector& BestBonePos, bool& RanHitChance, bool& CanTraceALine)
{
	pPlayer = pCAPlayer->BaseEntity;
	if (!pPlayer)
		return false;

	BoneTargetTime = Interfaces::Globals->curtime;
#if 0
	StoredNetvars *BestTick = pCPlayer->GetBestBacktrackTick();
	if (BestTick)
		BoneTargetTime = BestTick->simulationtime;
	else
		BoneTargetTime = pPlayer->GetSimulationTime();
#endif
	auto stat = START_PROFILING("AutoBone");
	bool RunStatistics = Interfaces::Globals->tickcount != TickRanFrameStatistics;
	double StartTime = QPCTime();
	//static float AvgFrameTime = 0.0f;
	double AvgTimeToRun = 0.0f;
	double TotalTimeToRun = 0.0f;
	for (auto time : TimeToRun)
		TotalTimeToRun += time;

	if (TimeToRun.size())
		AvgTimeToRun = TotalTimeToRun / TimeToRun.size();

	if (RunStatistics)
	{
		TimesRunThisFrame = 0;
		/*
		FrameTimes.push_front(Interfaces::Globals->absoluteframetime);
		if (FrameTimes.size() > 3)
		FrameTimes.pop_back();

		float TotalTime = 0.0f;
		for (auto ft : FrameTimes)
		TotalTime += ft;

		AvgFrameTime = TotalTime / FrameTimes.size();
		*/

		TickRanFrameStatistics = Interfaces::Globals->tickcount;
	}
	TimesRunThisFrame++;

	//Initialize autobone vars
	AlreadyIncrementedTotalBodyTimeVisible = false;
	CanTraceALine = false;
	RanHitChance = false;
	CheckedBodyAimHitbox = false;
	BestDamage = 0.0f;
	TargetEntityHealth = pPlayer->GetHealth();
	pCPlayer = pCAPlayer;
	pWeapon = weapon;
	pLocalEyeAngles = LocalEyeAngles;
	pLocalEyePos = &LocalEyePos;
	pBestHitbox = &BestBone;
	pBestHitboxPos = &BestBonePos;
	pCanTraceALine = &CanTraceALine;

	bool HeadOnly = false;

	if (Interfaces::Globals->curtime - pCPlayer->flLastTimeRanAutoBone >= 5.0f)
	{
		pCPlayer->flTotalTimeBodyWasVisible = 0.0f;
		pCPlayer->flTotalTimeTriedAimingAtHead = 0.0f;
	}

	pCPlayer->flLastTimeRanAutoBone = Interfaces::Globals->curtime;

	bool RanHitChanceHead = false;
	bool RanHitChanceTorso = false;
	bool RanHitChanceArms = false;
	bool RanHitChanceLegs = false;
	HittingTargetFPS = Interfaces::Globals->absoluteframetime + (AvgTimeToRun * TimesRunThisFrame) <= (1.0f / TargetFPSTxt.flValue);// || AvgFrameTime <= TARGET_FRAMETIME;
	lessIntensive = !HittingTargetFPS; // should make this a menu option and use it throughout chat on things to reduce scans multipoint etc.
	OnlyHitchanceInTriggerbot = !HittingTargetFPS || OnlyHitchanceInTriggerbotChk.Checked;
	//Could scan head bounding corners of players (ones esp use)
	//Check if they are hitable and if none of htem are then player clearly isn't
	//so if they are hitable its up to 4 extra scans however if they aren't visible
	//Its awesome bc we can fuck them off without scanning 30 + ish spots
	bool FoundBone = false;
	bool DontShootAtBodyYet = false;

	if (weapon)
	{
		if (weapon->IsKnife(NULL))
		{
			if (gAutoBone.AutoBone_Knife())
			{
				gTriggerbot.SetAllowedToFire(true);
				END_AUTOBONE_PROFILING
				return true;
			}
		}
		else
		{
			//FIXME
			//if player is air jumping and resolved and autobaim is enabled and they are bhopping then baim
			const bool AirJumping = false;//!pCPlayer->Backtracked && pCPlayer->Personalize.ShouldResolve() && AutoPelvisChk.Checked && ((pPlayer->GetGroundEntity() == nullptr || !(pPlayer->GetFlags() & FL_ONGROUND)) && pPlayer->GetVelocity().Length2D() > 1.0f); //(pCPlayer->IsMoving)
			StoredNetvars *targettick;
			StoredNetvars *backtracktick = pCPlayer->GetBestBacktrackTick();
			targettick = backtracktick ? backtracktick : pCPlayer->Personalize.m_PlayerRecords.size() > 0 ? pCPlayer->Personalize.m_PlayerRecords.front() : nullptr;

			//NOTE: NEED TO FULLY TEST BEFORE PUTTING INTO PRODUCTION
			//float BaimBelowThisHealth = ((float)pCPlayer->BaseEntity->GetHealth() * (MinimumDamageBeforeFiringTxt.flValue)) / 100;

			float BaimBelowThisHealth = weapon->IsPistol(NULL) ? (BodyAimBelowThisHealthTxt.flValue * 0.25f) : BodyAimBelowThisHealthTxt.flValue;

			bool OriginalFireAtPelvis = pCPlayer->FireAtPelvis;

			//NOTE: NEED TO FULLY TEST BEFORE PUTTING INTO PRODUCTION
			if (!pCPlayer->FireAtPelvis && pCPlayer->TicksChoked > 0 && (AirJumping ||
#ifdef BODY_AIM_FIRST_BEFORE_FIRING_AT_BLOOD_TICK
			(!backtracktick || backtracktick != pCPlayer->LatestBloodTick) ||
#else
				(BodyAimIfNoBacktrackTickChk.Checked && !backtracktick) ||
#endif
				(AutoPelvisChk.Checked && (!targettick || targettick->FireAtPelvis)) || pCPlayer->BaseEntity->GetHealth() < BaimBelowThisHealth))
				//(AutoPelvisChk.Checked && (!targettick || targettick->FireAtPelvis) ) ||( pelvisdamage >= BaimBelowThisHealth && MinimumDamageBeforeFiringTxt.flValue > 0)))
			{
				if (AimbotAimTorsoChk.Checked || AimbotAimArmsHandsChk.Checked || AimbotAimLegsFeetChk.Checked)
				{
					pCPlayer->FireAtPelvis = true;
				}
			}

			//Velocity2D for movement:
			//Crouch: 81.6
			//Running: 240.0
			//Walking: 124.8

			bool NoExtremeties = false;

			if (DetectPeekersChk.Checked && !ValveResolverChk.Checked && AimbotAimHeadNeckChk.Checked && !pCPlayer->Personalize.DontShootAtHeadNeck && !pCPlayer->FireAtPelvis
				&& !AimbotAimTorsoChk.Checked && !AimbotAimLegsFeetChk.Checked && !AimbotAimArmsHandsChk.Checked)
			{
				//return AutoBone_HsNoSpread();
			}
			//Fuck body aim if they are peeking, it misses too much
			if (DetectPeekersChk.Checked/* && !pCPlayer->FireAtPelvis*/)
			{
				if (pPlayer->GetDuckAmount() < 0.002f && pPlayer->GetVelocity().Length2D() >= 32.0f)//NOTE: lower body yaw updates at 1.1 vel so we want to ideally shoot head if its greater than that
				{																					//while this is true ^ if they slow peak the corner so that their head isn't shown you want to hit their legs if they are slow
					if (HeadshotOnlyPeekersChk.Checked && AimbotAimHeadNeckChk.Checked)
					{
						HeadOnly = true;
					}
					else if (AimbotAimHeadNeckChk.Checked || AimbotAimTorsoChk.Checked)
					{
						NoExtremeties = true;
					}
				}
			}
			if (pSilentChk.Checked)
			{
				if (gAutoBone.AutoBone_pSilent())
				{
					RanHitChance = false;
					gTriggerbot.SetAllowedToFire(true);
					pCPlayer->FireAtPelvis = OriginalFireAtPelvis;
					END_AUTOBONE_PROFILING
					return true;
				}
			}
			else
			{
				//Regular hitbox aim
				if (AimbotAimHeadNeckChk.Checked && !pCPlayer->Personalize.DontShootAtHeadNeck && !pCPlayer->FireAtPelvis
					&& !AimbotAimTorsoChk.Checked && !AimbotAimLegsFeetChk.Checked && !AimbotAimArmsHandsChk.Checked)
					HeadOnly = true;

				bool ShouldCheckBodyFirst = false;

				if (AimbotAimTorsoChk.Checked)
				{
					//Check body first if we need to fire at body
					if (pCPlayer->FireAtPelvis)
					{
						ShouldCheckBodyFirst = true;
					} //Otherwise, check the body first if target has full health and our weapon can kill in one hit
					else if (TargetEntityHealth >= 100)
					{
						if (LocalPlayer.WeaponVars.iItemDefinitionIndex == WEAPON_AWP || MinimumWeaponDamage >= 100)
							ShouldCheckBodyFirst = true;
					} //Finally, check the body first if they could die from a gunshot to any body part (UNDONE: not accounting for distance)
					else if (TargetEntityHealth - MinimumWeaponDamage <= 0)
					{
						ShouldCheckBodyFirst = true;
					}
					else if (BodyAimIfLethalChk.Checked)
					{
						ShouldCheckBodyFirst = true;
					}
				}

				if (ShouldCheckBodyFirst)
				{
					bool Lethal = false;
					bool DealsMinimumDamage = false;
					bool BodyIsDesired = pCPlayer->FireAtPelvis || BodyAimIfLethalChk.Checked;

					//First, check chest and pelvis
					if (BodyIsDesired && gAutoBone.AutoBone_ForcePelvisChest(&Lethal, &DealsMinimumDamage))
					{
						if (Lethal || !AutoBoneBestDamageChk.Checked)
						{
							RanHitChance = !OnlyHitchanceInTriggerbot;
							pCPlayer->FireAtPelvis = OriginalFireAtPelvis;
							gTriggerbot.SetAllowedToFire(true);
							END_AUTOBONE_PROFILING
							return true;
						}

						FoundBone = true;
					}

					//Now, check the rest of the torso
					Lethal = false;
					if (gAutoBone.AutoBone_Torso(&Lethal, &DealsMinimumDamage))
					{
						if (!AutoBoneBestDamageChk.Checked || (Lethal && (BodyIsDesired || !HeadOnly)))
						{
							RanHitChance = !OnlyHitchanceInTriggerbot;
							pCPlayer->FireAtPelvis = OriginalFireAtPelvis;
							gTriggerbot.SetAllowedToFire(true);
							END_AUTOBONE_PROFILING
							return true;
						}

						FoundBone = true;
					}
					RanHitChanceTorso = true;
				}

				if (!pCPlayer->FireAtPelvis && !pCPlayer->Personalize.DontShootAtHeadNeck && AimbotAimHeadNeckChk.Checked)
				{
					bool Lethal = false;
					bool DealsMinimumDamage = false;
					if (gAutoBone.AutoBone_HeadNeck(&Lethal, &DealsMinimumDamage))
					{
						if (Lethal || DealsMinimumDamage || HeadOnly || !AutoBoneBestDamageChk.Checked)
						{
							RanHitChance = !OnlyHitchanceInTriggerbot;
							pCPlayer->FireAtPelvis = OriginalFireAtPelvis;
							gTriggerbot.SetAllowedToFire(true);
							END_AUTOBONE_PROFILING
							return true;
						}
						FoundBone = true;
					}
					else if (pCPlayer->flTotalTimeTriedAimingAtHead != 0.0f)
					{
						if (pCPlayer->flTotalTimeTriedAimingAtHead < SpreadReduceWaitTimeTxt.flValue)
						{
							//We want to give the aimbot enough time to let the spread die down so we can hit the head before just body aiming
							DontShootAtBodyYet = true;
						}
					}
					RanHitChanceHead = true;
				}

				//If we want to target only the head, wait up to 2.0 seconds for the head to be hittable, otherwise try the body instead
				if (pCPlayer->flTotalTimeBodyWasVisible > BodyAimAfterHeadNotVisibleForXSecsTxt.flValue)
				{
					HeadOnly = false;
				}

				if (!ShouldCheckBodyFirst && AimbotAimTorsoChk.Checked)
				{
					bool Lethal = false;
					bool DealsMinimumDamage = false;
					if (gAutoBone.AutoBone_Torso(&Lethal, &DealsMinimumDamage))
					{
						if (Lethal || DealsMinimumDamage || !AutoBoneBestDamageChk.Checked)
						{
							RanHitChance = !OnlyHitchanceInTriggerbot;
							pCPlayer->FireAtPelvis = OriginalFireAtPelvis;
							END_AUTOBONE_PROFILING
							if (DontShootAtBodyYet || HeadOnly)
							{
								gTriggerbot.SetAllowedToFire(false);
								return true;
							}
							gTriggerbot.SetAllowedToFire(true);
							return true;
						}

						FoundBone = true;
					}
					RanHitChanceTorso = true;
				}

				if (AimbotAimArmsHandsChk.Checked)
				{
					bool Lethal = false;
					bool DealsMinimumDamage = false;
					if (gAutoBone.AutoBone_UpperLimbs(NoExtremeties, &Lethal, &DealsMinimumDamage))
					{
						if (Lethal || DealsMinimumDamage || !AutoBoneBestDamageChk.Checked)
						{
							RanHitChance = !OnlyHitchanceInTriggerbot;
							pCPlayer->FireAtPelvis = OriginalFireAtPelvis;
							END_AUTOBONE_PROFILING
							if (DontShootAtBodyYet || HeadOnly)
							{
								gTriggerbot.SetAllowedToFire(false);
								return true;
							}
							gTriggerbot.SetAllowedToFire(true);
							return true;
						}

						FoundBone = true;
					}
					RanHitChanceArms = true;
				}

				if (AimbotAimLegsFeetChk.Checked)
				{
					bool Lethal = false;
					bool DealsMinimumDamage = false;
					if (gAutoBone.AutoBone_LowerLimbs(NoExtremeties, &Lethal, &DealsMinimumDamage))
					{
						if (Lethal || DealsMinimumDamage || !AutoBoneBestDamageChk.Checked)
						{
							RanHitChance = !OnlyHitchanceInTriggerbot;
							pCPlayer->FireAtPelvis = OriginalFireAtPelvis;
							END_AUTOBONE_PROFILING
							if (DontShootAtBodyYet || HeadOnly)
							{
								gTriggerbot.SetAllowedToFire(false);
								return true;
							}
							gTriggerbot.SetAllowedToFire(true);
							return true;
						}

						FoundBone = true;
					}
					RanHitChanceLegs = true;
				}

				//If we have not found any bone and we are forcing body, then see if head is available anyway
				if (pCPlayer->FireAtPelvis && !FoundBone && (pCPlayer->TicksChoked == 0 || (Interfaces::Globals->curtime - pCPlayer->TimeLowerBodyUpdated >= TIME_TO_TICKS(LowerBodyWaitTimeBeforeFiringTxt.flValue))))
				{
					if (gAutoBone.AutoBone_HeadOnly())
					{
						RanHitChance = !OnlyHitchanceInTriggerbot;
						pCPlayer->FireAtPelvis = OriginalFireAtPelvis;
						gTriggerbot.SetAllowedToFire(true);
						END_AUTOBONE_PROFILING
						return true;
					}
				}
			}
			pCPlayer->FireAtPelvis = OriginalFireAtPelvis;
		}
	}


	if (FoundBone)
	{
		RanHitChance = !OnlyHitchanceInTriggerbot;

		switch (BestBone)
		{
		case HITBOX_HEAD:
		case HITBOX_NECK:
			gTriggerbot.SetAllowedToFire(true);
			break;
		default:
			if (HeadOnly || DontShootAtBodyYet)
				gTriggerbot.SetAllowedToFire(false);
			else
				gTriggerbot.SetAllowedToFire(true);
		}
	}

	END_AUTOBONE_PROFILING
	return FoundBone;
}


bool AutoBone::AutoBone_HeadOnly()
{
	auto stat = START_PROFILING("AutoBone_HeadOnly");
	float PotentialDamage;
	Vector PotentialHitboxPos;
	trace_t tr;

	const int PotentialHitbox = HITBOX_HEAD;
	LastBoneChecked = PotentialHitbox;
	MTargetting.SetHitboxGroup();
	MTargetting.SetHitChanceFlags();

#ifdef NEW_MULTIPOINT
	if (FindMultiPointVisible(PotentialHitbox, true, PotentialHitboxPos, *pLocalEyePos, tr, pPlayer, true, HITGROUP_HEAD_FLAG, PotentialDamage, pLocalEyeAngles, TempHitBoxPos, HittingTargetFPS))
#else
	if (FindMultiPointVisible(PotentialHitbox, true, PotentialHitboxPos, *pLocalEyePos, tr, pPlayer, true, HITGROUP_HEAD_FLAG, PotentialDamage, pLocalEyeAngles, HittingTargetFPS))
#endif
	{
		*pCanTraceALine = true;

		//Head is a valid target now. Instead of aiming at the existing bone, aim at the head now		
		if (OnlyHitchanceInTriggerbot || BulletWillHit(pWeapon, buttons, pPlayer, CalcAngle(*pLocalEyePos, PotentialHitboxPos), *pLocalEyePos, &PotentialHitboxPos, HITGROUP_HEAD_FLAG))
		{
			*pBestHitbox = PotentialHitbox;
			*pBestHitboxPos = PotentialHitboxPos;
			BestDamage = PotentialDamage;
			pCPlayer->flTotalTimeTriedAimingAtHead = 0.0f;
			END_PROFILING(stat);
			return true;
		}
		pCPlayer->flTotalTimeTriedAimingAtHead += Interfaces::Globals->interval_per_tick;
	}
	else
	{
		pCPlayer->flTotalTimeTriedAimingAtHead = 0.0f;
	}
	END_PROFILING(stat);
	return false;
}

bool AutoBone::AutoBone_HeadNeck(bool *FoundLethalHitbox, bool *DealsMinimumDamage)
{
	auto stat = START_PROFILING("AutoBone_HeadNeck");
	bool FoundHitbox = false;
	bool CanHit = false;
	bool CanTrace = false;

	int PotentialHitbox;
	float PotentialDamage;
	Vector PotentialHitboxPos;
	trace_t tr;

	const int count = (sizeof(HITBOX_GROUP_HEAD) / sizeof(HITBOX_GROUP_HEAD[0]));
	for (int i = 0; i < count; i++)
	{
		PotentialHitbox = HITBOX_GROUP_HEAD[i];
		LastBoneChecked = PotentialHitbox;
		MTargetting.SetHitboxGroup();
		MTargetting.SetHitChanceFlags();
#ifdef NEW_MULTIPOINT
		Vector TempHitBoxPos[2];
		PotentialHitboxPos = pPlayer->GetBonePositionTotal(ResetBone, BoneTargetTime, TempHitBoxPos[0], TempHitBoxPos[1], false, true);
		if (FindMultiPointVisible(PotentialHitbox, true, PotentialHitboxPos, *pLocalEyePos, tr, pPlayer, true, HITGROUP_HEAD_FLAG, PotentialDamage, pLocalEyeAngles, TempHitBoxPos, HittingTargetFPS))
#else
		PotentialHitboxPos = pPlayer->GetBonePosition(PotentialHitbox, BoneTargetTime, false, true);
		if (FindMultiPointVisible(PotentialHitbox, true, PotentialHitboxPos, *pLocalEyePos, tr, pPlayer, true, HITGROUP_HEAD_FLAG, PotentialDamage, pLocalEyeAngles, HittingTargetFPS))
#endif
		{
			*pCanTraceALine = true;
			CanTrace = true;

			if (OnlyHitchanceInTriggerbot || BulletWillHit(pWeapon, buttons, pPlayer, CalcAngle(*pLocalEyePos, PotentialHitboxPos), *pLocalEyePos, &PotentialHitboxPos, HITGROUP_HEAD_FLAG))
			{
				if (PotentialDamage > BestDamage)
				{
					FoundHitbox = true;
					*pBestHitbox = PotentialHitbox;
					*pBestHitboxPos = PotentialHitboxPos;
					BestDamage = PotentialDamage;
					bool Lethal = TargetEntityHealth - PotentialDamage <= 1.0f;
					if (PotentialDamage >= MinimumWeaponDamage)
					{
						pCPlayer->flTotalTimeTriedAimingAtHead = 0.0f;
						*DealsMinimumDamage = true;
						*FoundLethalHitbox = Lethal;
						END_PROFILING(stat);
						return true;
					}
					if (Lethal)
					{
						pCPlayer->flTotalTimeTriedAimingAtHead = 0.0f;
						*FoundLethalHitbox = true;
						END_PROFILING(stat);
						return true;
					}
				}
			}
			else
			{
				//Hitbox is visible but hitchance is not satisfied
				if (PotentialHitbox == HITBOX_GROUP_HEAD[0])
				{
					//If we can't meet hitchance on the head, then chances are we won't be able to hit the neck either, so don't even bother checking it
					VisibleHeadGroupHitbox = PotentialHitbox;
					VisibleHeadGroupHitboxPos = PotentialHitboxPos;
					break;
				}
			}

			VisibleHeadGroupHitbox = PotentialHitbox;
			VisibleHeadGroupHitboxPos = PotentialHitboxPos;
		}
	}

	if (CanTrace)
	{
		pCPlayer->flTotalTimeTriedAimingAtHead = CanHit ? 0.0f : pCPlayer->flTotalTimeTriedAimingAtHead + Interfaces::Globals->interval_per_tick;
	}
	else
	{
		//Reset total time tried to aim at head to 0 since the hitboxes aren't visible at all
		pCPlayer->flTotalTimeTriedAimingAtHead = 0.0f;
	}

	END_PROFILING(stat);
	return FoundHitbox;
}

//If we need to fire at the body first, then check pelvis and chest first before moving on to other bones for a bigger potential area for hitting the target
bool AutoBone::AutoBone_ForcePelvisChest(bool *FoundLethalHitbox, bool *DealsMinimumDamage)
{
	auto stat = START_PROFILING("AutoBone_ForcePelvisChest");
	CheckedBodyAimHitbox = true;

	int BodyAimHitboxAlreadyChecked;
	int PotentialHitbox;
	float PotentialDamage;
	Vector PotentialHitboxPos;
	trace_t tr;

	//It is always easier to hit pelvis due to anti aims and shit, maybe should be default pelvis... Or pelvis then chest,
	//We can remove pelvis and chest from torso scans to save cpu
	PotentialHitbox = pPlayer->GetDuckAmount() > 0.1f ? HITBOX_CHEST : HITBOX_PELVIS;
	LastBoneChecked = PotentialHitbox;
	BodyAimHitboxAlreadyChecked = PotentialHitbox;
	MTargetting.SetHitboxGroup();
	MTargetting.SetHitChanceFlags();

#ifdef NEW_MULTIPOINT
	Vector TempHitBoxPos[2];
	TempPos = pPlayer->GetBonePositionTotal(PotentialHitbox, BoneTargetTime, TempHitBoxPos[0], TempHitBoxPos[1], false, true);
	if (FindMultiPointVisible(ResetBone, true, PotentialHitboxPos, *pLocalEyePos, tr, pPlayer, true, HITGROUP_CHEST_FLAG | HITGROUP_STOMACH_FLAG, PotentialDamage, pLocalEyeAngles, TempHitBoxPos, HittingTargetFPS))
#else
	PotentialHitboxPos = pPlayer->GetBonePosition(BodyAimHitboxAlreadyChecked, BoneTargetTime, false, true);
	if (FindMultiPointVisible(PotentialHitbox, true, PotentialHitboxPos, *pLocalEyePos, tr, pPlayer, true, HITGROUP_CHEST_FLAG | HITGROUP_STOMACH_FLAG, PotentialDamage, pLocalEyeAngles, HittingTargetFPS))
#endif
	{
		*pCanTraceALine = true;

		if (!AlreadyIncrementedTotalBodyTimeVisible)
		{
			pCPlayer->flTotalTimeBodyWasVisible += Interfaces::Globals->interval_per_tick;
			AlreadyIncrementedTotalBodyTimeVisible = true;
		}

		if (OnlyHitchanceInTriggerbot || BulletWillHit(pWeapon, buttons, pPlayer, CalcAngle(*pLocalEyePos, PotentialHitboxPos), *pLocalEyePos, &PotentialHitboxPos, HITGROUP_CHEST_FLAG | HITGROUP_STOMACH_FLAG))
		{
			if (!pCPlayer->FireAtPelvis)
			{
				if (PotentialDamage > BestDamage)
				{
					if (TargetEntityHealth - PotentialDamage <= 0.0f)
						*FoundLethalHitbox = true;

					if (PotentialDamage >= MinimumWeaponDamage)
						*DealsMinimumDamage = true;

					BestDamage = PotentialDamage;
					*pBestHitbox = PotentialHitbox;
					*pBestHitboxPos = PotentialHitboxPos;
					END_PROFILING(stat);
					return true;
				}
			}
			else
			{
				//Note: No best damage check!
				if (TargetEntityHealth - PotentialDamage <= 0.0f)
					*FoundLethalHitbox = true;

				if (PotentialDamage >= MinimumWeaponDamage)
					*DealsMinimumDamage = true;

				BestDamage = PotentialDamage;
				*pBestHitbox = PotentialHitbox;
				*pBestHitboxPos = PotentialHitboxPos;
				END_PROFILING(stat);
				return true;
			}
		}
	}
	//NOTE: DO NOT SET TOTAL TIME BODY WAS VISIBLE TO 0 HERE SINCE THIS IS ONLY CHECKING TWO HITBOXES, WAIT FOR THE TORSO FUNCTION TO RUN FIRST

	END_PROFILING(stat);
	return false;
}

bool AutoBone::AutoBone_Torso(bool* FoundLethalHitbox, bool *DealsMinimumDamage)
{
	auto stat = START_PROFILING("AutoBone_Torso");
	bool FoundHitbox = false;
	bool CanTrace = false;

	int PotentialHitbox;
	float PotentialDamage;
	Vector PotentialHitboxPos;
	trace_t tr;

	bool AlreadySkippedBodyAimHitbox = false;
	const int count = (sizeof(HITBOX_GROUP_TORSO) / sizeof(HITBOX_GROUP_TORSO[0]));
	int reps = count;
	if (lessIntensive)
	{
		reps = (sizeof(HITBOX_GROUP_TORSO_FPS) / sizeof(HITBOX_GROUP_TORSO_FPS[0]));
	}

	for (int i = reps - 1; i >= 0; i--)
	{
		if (lessIntensive)
			PotentialHitbox = HITBOX_GROUP_TORSO_FPS[i];
		else
			PotentialHitbox = HITBOX_GROUP_TORSO[i];

		//Check to see if we should skip a hitbox
		if (!AlreadySkippedBodyAimHitbox)
		{
			if (!CheckedBodyAimHitbox)
				AlreadySkippedBodyAimHitbox = true;
			else if (PotentialHitbox == BodyAimHitboxAlreadyChecked)
			{
				AlreadySkippedBodyAimHitbox = true;
				continue;
			}
		}

		LastBoneChecked = PotentialHitbox;
		MTargetting.SetHitboxGroup();
		MTargetting.SetHitChanceFlags();
#ifdef NEW_MULTIPOINT
		Vector TempHitBoxPos[2];
		PotentialHitboxPos = pPlayer->GetBonePositionTotal(PotentialHitbox, BoneTargetTime, TempHitBoxPos[0], TempHitBoxPos[1], false, true);

		if (FindMultiPointVisible(PotentialHitbox, true, PotentialHitboxPos, *pLocalEyePos, tr, pPlayer, true, HITGROUP_CHEST_FLAG | HITGROUP_STOMACH_FLAG, PotentialDamage, pLocalEyeAngles, TempHitBoxPos, HittingTargetFPS))
#else
		PotentialHitboxPos = pPlayer->GetBonePosition(PotentialHitbox, BoneTargetTime, false, true);

		if (FindMultiPointVisible(PotentialHitbox, true, PotentialHitboxPos, *pLocalEyePos, tr, pPlayer, true, HITGROUP_CHEST_FLAG | HITGROUP_STOMACH_FLAG, PotentialDamage, pLocalEyeAngles, HittingTargetFPS))
#endif
		{
			CanTrace = true;
			*pCanTraceALine = true;

			if (!AlreadyIncrementedTotalBodyTimeVisible)
			{
				pCPlayer->flTotalTimeBodyWasVisible += Interfaces::Globals->interval_per_tick;
				AlreadyIncrementedTotalBodyTimeVisible = true;
			}

			if (OnlyHitchanceInTriggerbot || BulletWillHit(pWeapon, buttons, pPlayer, CalcAngle(*pLocalEyePos, PotentialHitboxPos), *pLocalEyePos, &PotentialHitboxPos, HITGROUP_CHEST_FLAG | HITGROUP_STOMACH_FLAG))
			{
				if (PotentialDamage > BestDamage)
				{
					FoundHitbox = true;
					*pBestHitbox = PotentialHitbox;
					*pBestHitboxPos = PotentialHitboxPos;
					BestDamage = PotentialDamage;
					bool Lethal = TargetEntityHealth - PotentialDamage <= 0.9f;

					if (PotentialDamage >= MinimumWeaponDamage)
					{
						*DealsMinimumDamage = true;
						*FoundLethalHitbox = Lethal;
						END_PROFILING(stat);
						return true;
					}

					if (Lethal)
					{
						*FoundLethalHitbox = true;
						END_PROFILING(stat);
						return true;
					}
				}
			}
		}
	}

	if (!CanTrace && !AlreadyIncrementedTotalBodyTimeVisible)
	{
		pCPlayer->flTotalTimeBodyWasVisible = 0.0f;
	}

	END_PROFILING(stat);
	return FoundHitbox;
}

bool AutoBone::AutoBone_UpperLimbs(bool NoExtremeties, bool* FoundLethalHitbox, bool *DealsMinimumDamage)
{
	auto stat = START_PROFILING("AutoBone_UpperLimbs");
	bool FoundHitbox = false;
	bool CanTrace = false;

	int PotentialHitbox;
	float PotentialDamage;
	Vector PotentialHitboxPos;
	trace_t tr;

	int count = NoExtremeties ? 2 : (sizeof(HITBOX_GROUP_UPPERLIMBS) / sizeof(HITBOX_GROUP_UPPERLIMBS[0]));
	int reps = count;
	if (lessIntensive)
	{
		if (NoExtremeties)
			return false;
		reps = (sizeof(HITBOX_GROUP_UPPERLIMBS_FPS) / sizeof(HITBOX_GROUP_UPPERLIMBS_FPS[0]));
	}


	for (int i = 0; i < reps; i++)
	{
		if (lessIntensive)
			PotentialHitbox = HITBOX_GROUP_UPPERLIMBS_FPS[i];
		else
			PotentialHitbox = HITBOX_GROUP_UPPERLIMBS[i];
		LastBoneChecked = PotentialHitbox;
		MTargetting.SetHitboxGroup();
		MTargetting.SetHitChanceFlags();
#ifdef NEW_MULTIPOINT
		Vector TempHitBoxPos[2];
		PotentialHitboxPos = pPlayer->GetBonePositionTotal(PotentialHitbox, BoneTargetTime, TempHitBoxPos[0], TempHitBoxPos[1], false, true);

		if (FindMultiPointVisible(PotentialHitbox, true, PotentialHitboxPos, *pLocalEyePos, tr, pPlayer, true, HITGROUP_LEFTARM_FLAG | HITGROUP_RIGHTARM_FLAG, PotentialDamage, pLocalEyeAngles, TempHitBoxPos, HittingTargetFPS))
#else
		PotentialHitboxPos = pPlayer->GetBonePosition(PotentialHitbox, BoneTargetTime, false, true);

		int flags = HITGROUP_LEFTARM_FLAG | HITGROUP_RIGHTARM_FLAG;

		if (PotentialHitbox == HITBOX_LEFT_UPPER_ARM || PotentialHitbox == HITBOX_RIGHT_UPPER_ARM)
			flags |= HITGROUP_CHEST_FLAG;

		if (FindMultiPointVisible(PotentialHitbox, true, PotentialHitboxPos, *pLocalEyePos, tr, pPlayer, true, flags, PotentialDamage, pLocalEyeAngles, HittingTargetFPS))
#endif
		{
			CanTrace = true;
			*pCanTraceALine = true;
			if (!AlreadyIncrementedTotalBodyTimeVisible)
			{
				pCPlayer->flTotalTimeBodyWasVisible += Interfaces::Globals->interval_per_tick;
				AlreadyIncrementedTotalBodyTimeVisible = true;
			}
			if (OnlyHitchanceInTriggerbot || BulletWillHit(pWeapon, buttons, pPlayer, CalcAngle(*pLocalEyePos, PotentialHitboxPos), *pLocalEyePos, &PotentialHitboxPos, flags))
			{
				if (PotentialDamage > BestDamage)
				{
					FoundHitbox = true;
					*pBestHitbox = PotentialHitbox;
					*pBestHitboxPos = PotentialHitboxPos;
					BestDamage = PotentialDamage;
					bool Lethal = TargetEntityHealth - PotentialDamage <= 0.0f;

					if (PotentialDamage >= MinimumWeaponDamage)
					{
						*DealsMinimumDamage = true;
						*FoundLethalHitbox = Lethal;
						END_PROFILING(stat);
						return true;
					}
					if (Lethal)
					{
						*FoundLethalHitbox = true;
						END_PROFILING(stat);
						return true;
					}
				}
			}
		}
	}

	if (!CanTrace && !AlreadyIncrementedTotalBodyTimeVisible)
	{
		pCPlayer->flTotalTimeBodyWasVisible = 0.0f;
	}

	END_PROFILING(stat);
	return FoundHitbox;
}

bool AutoBone::AutoBone_LowerLimbs(bool NoExtremeties, bool* FoundLethalHitbox, bool *DealsMinimumDamage)
{
	auto stat = START_PROFILING("AutoBone_LowerLimbs");
	bool FoundHitbox = false;
	bool CanTrace = false;

	int PotentialHitbox;
	float PotentialDamage;
	Vector PotentialHitboxPos;
	trace_t tr;

	int count = NoExtremeties ? 2 : (sizeof(HITBOX_GROUP_LOWERLIMBS) / sizeof(HITBOX_GROUP_LOWERLIMBS[0]));
	int reps = count;
	if (lessIntensive)
	{
		reps = NoExtremeties ? 2 : (sizeof(HITBOX_GROUP_UPPERLIMBS_FPS) / sizeof(HITBOX_GROUP_UPPERLIMBS_FPS[0]));
	}

	for (int i = 0; i < reps; i++)
	{
		if (lessIntensive)
			PotentialHitbox = HITBOX_GROUP_LOWERLIMBS_FPS[i];
		else
			PotentialHitbox = HITBOX_GROUP_LOWERLIMBS[i];
		LastBoneChecked = PotentialHitbox;
		MTargetting.SetHitboxGroup();
		MTargetting.SetHitChanceFlags();
#ifdef NEW_MULTIPOINT
		Vector TempHitBoxPos[2];
		PotentialHitboxPos = pPlayer->GetBonePositionTotal(PotentialHitbox, BoneTargetTime, TempHitBoxPos[0], TempHitBoxPos[1], false, true);
		if (FindMultiPointVisible(PotentialHitbox, true, PotentialHitboxPos, *pLocalEyePos, tr, pPlayer, true, HITGROUP_LEFTLEG_FLAG | HITGROUP_RIGHTLEG_FLAG, PotentialDamage, pLocalEyeAngles, TempHitBoxPos, HittingTargetFPS))
#else
		PotentialHitboxPos = pPlayer->GetBonePosition(PotentialHitbox, BoneTargetTime, false, true);
		if (FindMultiPointVisible(PotentialHitbox, true, PotentialHitboxPos, *pLocalEyePos, tr, pPlayer, true, HITGROUP_LEFTLEG_FLAG | HITGROUP_RIGHTLEG_FLAG, PotentialDamage, pLocalEyeAngles, HittingTargetFPS))
#endif
		{
			CanTrace = true;
			*pCanTraceALine = true;
			if (!AlreadyIncrementedTotalBodyTimeVisible)
			{
				pCPlayer->flTotalTimeBodyWasVisible += Interfaces::Globals->interval_per_tick;
				AlreadyIncrementedTotalBodyTimeVisible = true;
			}
			if (OnlyHitchanceInTriggerbot || BulletWillHit(pWeapon, buttons, pPlayer, CalcAngle(*pLocalEyePos, PotentialHitboxPos), *pLocalEyePos, &PotentialHitboxPos, HITGROUP_LEFTLEG_FLAG | HITGROUP_RIGHTLEG_FLAG))
			{
				if (PotentialDamage > BestDamage)
				{
					FoundHitbox = true;
					*pBestHitbox = PotentialHitbox;
					*pBestHitboxPos = PotentialHitboxPos;
					BestDamage = PotentialDamage;
					bool Lethal = TargetEntityHealth - PotentialDamage <= 0.0f;
					if (PotentialDamage >= (MinimumWeaponDamage * GetHitgroupDamageMultiplier(tr.hitgroup)))
					{
						*DealsMinimumDamage = true;
						*FoundLethalHitbox = Lethal;
						END_PROFILING(stat);
						return true;
					}
					if (Lethal)
					{
						*FoundLethalHitbox = true;
						END_PROFILING(stat);
						return true;
					}
				}
			}
		}
	}

	if (!CanTrace && !AlreadyIncrementedTotalBodyTimeVisible)
	{
		pCPlayer->flTotalTimeBodyWasVisible = 0.0f;
	}

	END_PROFILING(stat);
	return FoundHitbox;
}

bool AutoBone::AutoBone_Knife()
{
	auto stat = START_PROFILING("AutoBone_Knife");
	bool FoundBone = false;

	if (KnifeBotChk.Checked)
	{
		//Knife AutoBone only aims at the torso

		float PotentialDamage = 0.0f;
		trace_t tr;
		Vector PotentialHitboxPos;
		const int counter = (sizeof(HITBOX_GROUP_TORSO) / sizeof(HITBOX_GROUP_TORSO[0]));
		for (int i = 0; i < counter; i++)
		{
			int PotentialHitbox = HITBOX_GROUP_TORSO[i];
#ifdef NEW_MULTIPOINT
			Vector TempHitBoxPos[2];
			TempPos = pPlayer->GetBonePositionTotal(ResetBone, BoneTargetTime, TempHitBoxPos[0], TempHitBoxPos[1], false, true);
			if (FindMultiPointVisible(PotentialHitbox, true, PotentialHitboxPos, *pLocalEyePos, tr, pPlayer, true, HITGROUP_STOMACH_FLAG | HITGROUP_CHEST_FLAG, PotentialDamage, pLocalEyeAngles, TempHitBoxPos, HittingTargetFPS))
#else
			PotentialHitboxPos = pPlayer->GetBonePosition(PotentialHitbox, BoneTargetTime, false, true);
			if (FindMultiPointVisible(PotentialHitbox, true, PotentialHitboxPos, *pLocalEyePos, tr, pPlayer, true, HITGROUP_STOMACH_FLAG | HITGROUP_CHEST_FLAG, PotentialDamage, pLocalEyeAngles, HittingTargetFPS))
#endif
			{
				if (PotentialDamage > BestDamage)
				{
					*pCanTraceALine = true;
					FoundBone = true;
					BestDamage = PotentialDamage;
					*pBestHitbox = PotentialHitbox;
					*pBestHitboxPos = PotentialHitboxPos;
				}
			}
		}
	}

	END_PROFILING(stat);
	return FoundBone;
}

bool AutoBone::AutoBone_pSilent()
{
	auto stat = START_PROFILING("AutoBone_pSilent");
	//Target hitbox nearest to FOV
	//Todo: proper multipoint support for it

	bool FoundBone = false;

	float ClosestFOV = 360.0f;
	QAngle dLocalEyeAngles = *pLocalEyeAngles;

	for (int i = 0; i < NUM_BONES; i++)
	{
		int PotentialHitbox = BONES_TO_CHECK[i];

		LastBoneChecked = PotentialHitbox;
		MTargetting.SetHitboxGroup();
		MTargetting.SetHitChanceFlags();

#ifdef NEW_MULTIPOINT
		Vector TempHitBoxPos[2];
		Vector PotentialHitboxPos = pPlayer->GetBonePositionTotal(ResetBone, BoneTargetTime, TempHitBoxPos[0], TempHitBoxPos[1], false, true);
#else
		Vector PotentialHitboxPos = pPlayer->GetBonePosition(PotentialHitbox, BoneTargetTime, false, true);
#endif
		trace_t tr;
		float PotentialDamage;
		bool Visible;

		if (PotentialHitbox != HITBOX_HEAD) //Fall through for other bones for speed purposes, though it'd probably be faster to just duplicate code than use bools
			Visible = IsBoneVisible(true, PotentialHitbox, PotentialHitboxPos, *pLocalEyePos, tr, pPlayer, false, 0, PotentialDamage, pLocalEyeAngles);
		else
#ifdef NEW_MULTIPOINT
			Visible = FindMultiPointVisible(PotentialHitbox, true, PotentialHitboxPos, *pLocalEyePos, tr, pPlayer, true, HITGROUP_HEAD_FLAG, PotentialDamage, pLocalEyeAngles, TempHitBoxPos, HittingTargetFPS);
#else
			Visible = FindMultiPointVisible(PotentialHitbox, true, PotentialHitboxPos, *pLocalEyePos, tr, pPlayer, true, HITGROUP_HEAD_FLAG, PotentialDamage, pLocalEyeAngles, HittingTargetFPS);
#endif

		if (Visible)
		{
			*pCanTraceALine = true;

			float FOV = GetFovFromCoordsRegardlessofDistance(*pLocalEyePos, PotentialHitboxPos, dLocalEyeAngles);
			if (FOV < ClosestFOV && FOV <= pSilentFOVTxt.flValue)
			{
				FoundBone = true;
				*pBestHitboxPos = PotentialHitboxPos;
				*pBestHitbox = PotentialHitbox;
				BestDamage = PotentialDamage;
				ClosestFOV = FOV;
			}
		}
	}

	END_PROFILING(stat);
	return FoundBone;
}


//Run this on headshot only nospread servers, fixes some issues
//Also scans alot more then normal multipoint
//Nospread therefore no on gives a flying fuck about hitchance
bool AutoBone::AutoBone_HsNoSpread()
{
	int hitbox = HITBOX_HEAD;
	bool canHit = false;
	trace_t tr;
	float damage = 0;
	Vector vMin, vMax;
	Vector PotentialHitboxPos = pPlayer->GetBonePositionTotal(hitbox, BoneTargetTime, vMax, vMin, false, true);

	//Do basic scans first
	//Don't care if you think we can shot or not.
	FindMultiPointVisible(hitbox, true, PotentialHitboxPos, *pLocalEyePos, tr, pPlayer, true, HITGROUP_HEAD_FLAG, damage, pLocalEyeAngles, HittingTargetFPS);
	if (damage >= MinimumDamageBeforeFiringTxt.flValue && damage > 0)
	{
		*pCanTraceALine = true;
		canHit = true;
		*pBestHitbox = hitbox;
		*pBestHitboxPos = PotentialHitboxPos;
		BestDamage = damage;
		if (TargetEntityHealth - damage <= 0.0f)
		{
			return true;
		}
	}

	if (!AimbotMultiPointHeadNeckChk.Checked)
	{
		if (canHit)
			return true;
		else
			return false;
	}
	if (canHit)
		return true;
	else
		return false;

	//Done intial bullshit scanning, now lets get intense and slightly sexual ;)

	//lets do pointscale shit
	float pointscale = PointScaleTxt.flValue;

	float diffz = abs(vMax.z - vMin.z)* 1.1;
	vMax.z = PotentialHitboxPos.z + (diffz * pointscale);
	vMin.z = PotentialHitboxPos.z - (diffz * pointscale);

	//While the top is how you really want to do pointscale, we are too cool for that and we want to change it up a bit

	float multiplier = 1.1f;
	multiplier = 4;
	float diffx = abs(vMax.x - vMin.x)* multiplier;
	diffx /= 2;
	vMax.x = PotentialHitboxPos.x + (diffx * pointscale);
	vMin.x = PotentialHitboxPos.x - (diffx * pointscale);
	float diffy = abs(vMax.y - vMin.y)* multiplier;
	diffy += (diffy - 4) * 0.5;
	vMax.y = PotentialHitboxPos.y + (diffy * pointscale);
	vMin.y = PotentialHitboxPos.y - (diffy * pointscale);


	enum points //I didn't know you could put an enum into a bool, thats cool
	{

		middle = 0,
		Bottom,
		Top,
		TopLeftForward,
		TopLeftBackwards,
		TopRightForward,
		TopRightBackwards,
		BottomLeftForward,
		BottomLeftBackwards,
		BottomRightForward,
		BottomRightBackwards,
		Plus1,
		Plus2,
		Plus25,
		Plus4,
		Plus5,
		MAXPOINTSTOSCAN
	};
	int amountToScan = MAXPOINTSTOSCAN;
	if (!HittingTargetFPS)
		amountToScan = 7;
	for (int i = middle; i < MAXPOINTSTOSCAN; i++)
	{
		//Lets scan all different points on the hitbox that we could possibly see
		Vector Point;
		switch (i)
		{
		case middle:
			Point = (vMin + vMax) * 0.5f;
			break;
		case Bottom:
			Point = (vMin + vMax) * 0.5f;
			Point.z = vMin.z;
			break;
		case Top:
			Point = (vMin + vMax) * 0.5f;
			Point.z = vMax.z;
			break;
		case TopLeftForward:
			Point = vMax;
			break;
		case TopLeftBackwards:
			Point = vMax;
			Point.x = vMin.x;
			break;
		case TopRightForward:
			Point = vMax;
			Point.y = vMin.y;
			break;
		case TopRightBackwards:
			Point.z = vMax.z;
			Point.y = vMin.y;
			Point.x = vMin.x;
			break;
		case BottomLeftForward:
			Point = vMin;
			break;
		case BottomLeftBackwards:
			Point = vMin;
			Point.x = vMax.x;
			break;
		case BottomRightForward:
			Point = vMin;
			Point.y = vMax.y;
			break;
		case BottomRightBackwards:
			Point.z = vMin.z;
			Point.y = vMax.y;
			Point.x = vMax.x;
			break;
		case Plus1:
			Point = (vMin + vMax) * 0.5f;
			Point.z = vMax.z + 1;
			break;
		case Plus2:
			Point = (vMin + vMax) * 0.5f;
			Point.z = vMax.z + 2;
			break;
		case Plus25:
			Point = (vMin + vMax) * 0.5f;
			Point.z = vMax.z + 2.5;
			break;
		case Plus4:
			Point = (vMin + vMax) * 0.5f;
			Point.z = PotentialHitboxPos.z + 4;
			break;
		case Plus5:
			Point = (vMin + vMax) * 0.5f;
			Point.z = PotentialHitboxPos.z + 5;
			break;

		}
		if (IsBoneVisible(true, hitbox, Point, *pLocalEyePos, tr, pPlayer, false, hitbox, damage, pLocalEyeAngles))
		{
			if (damage > BestDamage)
			{
				*pCanTraceALine = true;
				canHit = true;
				*pBestHitbox = hitbox;
				*pBestHitboxPos = Point;
				BestDamage = damage;
				if (TargetEntityHealth - damage <= 0.0f)
				{
					return true;
				}

			}
		}

	}

	if (canHit)
		return true;

	//haven't found anything we really want to hit, well in this case lets just give a normal ray trace since util_trace doesnt seem to work some times
	//When both players are bouncing, fricked if I know why hey.

	//I think this is the right filter meh
	CTraceFilterTargetSpecificEntity filter;
	filter.pSkip = (IHandleEntity*)LocalPlayer.Entity;
	filter.m_icollisionGroup = COLLISION_GROUP_NONE;
	filter.pTarget = pPlayer;
	Ray_t ray;

	ray.Init(*pLocalEyePos, PotentialHitboxPos);
	Interfaces::EngineTrace->TraceRay(ray, MASK_SOLID, &filter, &tr);

	if (tr.m_pEnt == pPlayer)//So this means util fucked up but its okay we still see them
	{
		*pCanTraceALine = true;
		canHit = true;
		*pBestHitbox = hitbox;
		*pBestHitboxPos = PotentialHitboxPos;
		BestDamage = 10;//Lets pretend they are doing 10 damage but who the fuck knows what the real damage is
		return true;
	}

	return false;
}

#endif