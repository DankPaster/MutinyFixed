#include "GameEventListener.h"
#include "Events.h"
#include "Aimbot.h"
#include "EncryptString.h"
#include "player_lagcompensation.h"
#include "Overlay.h"
#include "HitChance.h"
#include "LocalPlayer.h"

char *bulletimpactstr = new char[14]{ 24, 15, 22, 22, 31, 14, 37, 19, 23, 10, 27, 25, 14, 0 }; /*bullet_impact*/
char *playerhurtstr = new char[12]{ 10, 22, 27, 3, 31, 8, 37, 18, 15, 8, 14, 0 }; /*player_hurt*/
char *player_deathstr = new char[13]{ 10, 22, 27, 3, 31, 8, 37, 30, 31, 27, 14, 18, 0 }; /*player_death*/

void CreateEventListeners()
{
	CGameEventListener *ImpactListener = new CGameEventListener(bulletimpactstr, GameEvent_BulletImpact, false);
	CGameEventListener *PlayerHurtListener = new CGameEventListener(playerhurtstr, GameEvent_PlayerHurt, false);
	CGameEventListener *PlayerDeath = new CGameEventListener(player_deathstr, GameEvent_PlayerDeath, false);

}

void DecayPlayerIsShot()
{
	if (!LocalPlayer.IsAlive)
	{
		LocalPlayer.IsShot = false;
	} 
	else if (LocalPlayer.IsShot)
	{
		QAngle Punch = LocalPlayer.Entity->GetPunch() * 2.0f;
		if (Punch == QAngle(0.0f, 0.0f, 0.0f))
		{
			LocalPlayer.IsShot = false;
		}
	}
}

char *useridstr = new char[7]{ 15, 9, 31, 8, 19, 30, 0 }; /*userid*/
char *attackerstr = new char[9]{ 27, 14, 14, 27, 25, 17, 31, 8, 0 }; /*attacker*/
char *hitgroupstr = new char[9]{ 18, 19, 14, 29, 8, 21, 15, 10, 0 }; /*hitgroup*/
char *useridstr2 = new char[7]{ 15, 9, 31, 8, 19, 30, 0 }; /*userid*/
char *dmg_healthstr = new char[11]{ 30, 23, 29, 37, 18, 31, 27, 22, 14, 18, 0 }; /*dmg_health*/

int TickHitPlayer = 0;
int TickHitWall = 0;
int OriginalCorrectedBodyYawIndex = 0;
int OriginalCloseFakeIndex = 0;
int OriginalCorrectedIndex = 0;
int OriginalResolveWhenShootingIndex = 0;
int OriginalShotsMissed = 0;
int OriginalCorrectedFakeWalkIndex = 0;
bool OriginalIsCorrected = 0;
bool OriginalDontStatsThisShot = 0;

void GameEvent_PlayerDeath(IGameEvent* gameEvent)
{
	if (DisableAllChk.Checked || !LocalPlayer.Entity || !gameEvent)
		return;

		DecStr(useridstr, 6);
		int iUser = gameEvent->GetInt(useridstr);
		EncStr(useridstr, 6);
		int iUserEnt = Interfaces::Engine->GetPlayerForUserID(iUser);
		if (!iUserEnt)
			return;

		CBaseEntity* gamer = Interfaces::ClientEntList->GetClientEntity(iUserEnt);
		if (!gamer)
			return;
		int index = ReadInt((uintptr_t)&gamer->index);
		CustomPlayer *pCPlayer = &AllPlayers[index];

		pCPlayer->Personalize.roundStart = pCPlayer->Personalize.ShotsnotHit;
}

void GameEvent_PlayerHurt(IGameEvent* gameEvent)
{
	if (DisableAllChk.Checked || !LocalPlayer.Entity || !gameEvent)
		return;

	//if (LocalPlayer.Entity->GetAlive())
	{
		DecStr(useridstr, 6);
		int iUser = gameEvent->GetInt(useridstr);
		EncStr(useridstr, 6);
		int iUserEnt = Interfaces::Engine->GetPlayerForUserID(iUser);
		if (!iUserEnt)
			return;

		CBaseEntity* gamer = Interfaces::ClientEntList->GetClientEntity(iUserEnt);
		if (!gamer)
			return;
		int index = ReadInt((uintptr_t)&gamer->index);
		CustomPlayer *pCPlayer = &AllPlayers[index];

		if (!pCPlayer->BaseEntity)
			return;

		if (gamer == LocalPlayer.Entity)
		{
			//This is used to stop no recoil if player is shot, and future stuff
			LocalPlayer.IsShot = true;
			LocalPlayer.IsWaitingToSetIsShotAimPunch = true;
			//DecStr(attackerstr, 8);
			//int iAttacker = gameEvent->GetInt(attackerstr);
			//EncStr(attackerstr, 8);
		}
		else if ((DrawDamageChk.Checked) && !pCPlayer->Dormant && (DrawDamageChk.Checked || pCPlayer->Personalize.ShouldResolve()))
		{
			int LocalUserID = LocalPlayer.Entity->GetUserID();
			DecStr(attackerstr, 8);
			int iAttacker = gameEvent->GetInt(attackerstr);
			EncStr(attackerstr, 8);
			StoredNetvars *currentrecord = pCPlayer->GetCurrentRecord();
			pCPlayer->Personalize.flSimulationTimePlayerWasShot = currentrecord ? currentrecord->simulationtime : pCPlayer->BaseEntity->GetSimulationTime();
			DecStr(hitgroupstr, 8);
			int iHitGroup = gameEvent->GetInt(hitgroupstr);
			EncStr(hitgroupstr, 8);
			pCPlayer->Personalize.iHitGroupPlayerWasShotOnServer = iHitGroup;
			
			if (iAttacker == LocalUserID)
			{
				if (LastTargetIndex != INVALID_PLAYER)
				{
					int dmg = gameEvent->GetInt(dmg_healthstr);
					CustomPlayer *Target = &AllPlayers[LastTargetIndex];
					if (dmg > 1)
						Target->Personalize.ShotsnotHit--;
				}
				if (DrawDamageChk.Checked)
				{
					DecStr(dmg_healthstr, 10);
					int dmg = gameEvent->GetInt(dmg_healthstr);
					if (LastPlayerDamageGiven == gamer)
					{
						iDamageGivenToTarget += dmg;
						iHitsToTarget++;
					}
					else
					{
						iDamageGivenToTarget = dmg;
						iHitsToTarget = 1;
					}
					iLastHitgroupHit = iHitGroup;
					LastPlayerDamageGiven = gamer;
					flTimeDamageWasGiven = ReadFloat((uintptr_t)&Interfaces::Globals->curtime);
					EncStr(dmg_healthstr, 10);
				}

				//if (ResolverChk.Checked)
				{
					//int tickcount = ReadFloat((uintptr_t)&Interfaces::Globals->tickcount);
					//char str[100];
					//sprintf(str, "%i", dmg);
					//DrawStringUnencrypted(str, Vector2D(640, 550), 255, 0, 0, pFont);
					//int iAttackerEnt = Interfaces::Engine->GetPlayerForUserID(iAttacker);
					//Vector ShotFrom = LocalPlayer.Entity->GetEyePosition();
					//pCPlayer->Personalize.LastFiredAtTick = tickcount;
					//pCPlayer->Personalize.LastHitTick = tickcount;
					//pCPlayer->Personalize.HitGroup = iHitGroup;
					Personalize *pPersonalize = &pCPlayer->Personalize;
					//Don't count shots made when player is moving while jumping. Only count shots made if they jump up and down
					CBaseEntity* Entity = pCPlayer->BaseEntity;

					bool AirJumping = ((Entity->GetGroundEntity() == nullptr || !(Entity->GetFlags() & FL_ONGROUND)) && Entity->GetVelocity().Length2D() > 1.0f);
					int tickcount = ReadInt((uintptr_t)&Interfaces::Globals->tickcount);
					if (TickHitWall == tickcount)
					{
						pPersonalize->correctedclosefakeindex = OriginalCloseFakeIndex;
						pPersonalize->correctedindex = OriginalCorrectedIndex;
						pPersonalize->isCorrected = OriginalIsCorrected;
						pPersonalize->DontStatsThisShot = OriginalDontStatsThisShot;
						pPersonalize->correctedresolvewhenshootingindex = OriginalResolveWhenShootingIndex;
						pPersonalize->ShotsMissed = OriginalShotsMissed;
						pPersonalize->correctedbodyyawindex = OriginalCorrectedBodyYawIndex;
						pPersonalize->correctedfakewalkindex = OriginalCorrectedFakeWalkIndex;
					}
					if (tickcount != TickHitPlayer)
					{
						TickHitPlayer = tickcount;
						pPersonalize->ShotsMissed = 0;

						if (FireWhenEnemiesShootChk.Checked && pCPlayer->ResolverFiredAtShootingAngles)
						{
							if (iHitGroup != pPersonalize->LastHitGroupShotAt)
							{
								if (++pPersonalize->correctedresolvewhenshootingindex > (MAX_FIRING_WHEN_SHOOTING_MISSES - 1))
									pPersonalize->correctedresolvewhenshootingindex = 0;
							}
							pPersonalize->DontStatsThisShot = false;
						}
						else
						{
							if (!pPersonalize->DontStatsThisShot)
							{
								if (!AirJumping)
								{
#ifdef _DEBUG
									wchar_t PlayerName[32];
									GetPlayerName(GetRadar(), Entity->index, PlayerName);
									AllocateConsole();
									printf("Player_Hurt Event\n%S\nSimulation Time: %f\nTickcount: %i\n\n", PlayerName, Entity->GetSimulationTime(), Interfaces::Globals->tickcount);
#endif
									if (iHitGroup == pPersonalize->LastHitGroupShotAt)
									{
										//pCPlayer->LastUpdatedNetVars.tickcount = TIME_TO_TICKS(pCPlayer->CurrentNetVars.simulationtime);//ReadInt((uintptr_t)&Interfaces::Globals->tickcount);
										pPersonalize->isCorrected = true;
									}
									else //if (LastShotHitPercentage >= 82.0f || AimbotHitPercentageHeadTxt.flValue < 5)
									{
										pPersonalize->isCorrected = false;
										if (++pPersonalize->correctedbodyyawindex > (MAX_BODY_YAW_MISSES - 1))
											pPersonalize->correctedbodyyawindex = 0;

										if (++pPersonalize->correctedfakewalkindex > (MAX_FAKEWALK_MISSES - 1))
											pPersonalize->correctedfakewalkindex = 0;

										if (pCPlayer->CloseFakeAngle)
										{
											if (++pPersonalize->correctedclosefakeindex > (MAX_NEAR_BODY_YAW_RESOLVER_MISSES - 1))
												pPersonalize->correctedclosefakeindex = 0;
										}
										else
										{
											if (++pPersonalize->correctedindex > (MAX_RESOLVER_MISSES - 1))
												pPersonalize->correctedindex = 0;
										}
									}
								}
								pPersonalize->DontStatsThisShot = false;
							}
						}
					}
				}
			}
			else
			{
				//Not us that attacked this player
				DecStr(hitgroupstr, 8);
				int iHitGroup = gameEvent->GetInt(hitgroupstr);
				EncStr(hitgroupstr, 8);
				pCPlayer->Personalize.iHitGroupPlayerWasShotOnServer = iHitGroup;
				pCPlayer->Personalize.flSimulationTimePlayerWasShot = pCPlayer->BaseEntity->GetSimulationTime();
			}
		}
	}
}

void GameEvent_BulletImpact(IGameEvent* gameEvent)
{
	if (DisableAllChk.Checked || !LocalPlayer.Entity || !gameEvent)
		return;

	DecStr(useridstr2, 6);
	int iUser = gameEvent->GetInt(useridstr2);
	EncStr(useridstr2, 6);

	if (iUser == LocalPlayer.Entity->GetUserID())
	{
		if (LastTargetIndex != INVALID_PLAYER)
		{
			//int tickcount = ReadFloat((uintptr_t)&Interfaces::Globals->curtime);
			Vector Impact;
			Impact.x = gameEvent->GetFloat("x");
			Impact.y = gameEvent->GetFloat("y");
			Impact.z = gameEvent->GetFloat("z");
			Vector ShotFrom = LocalPlayer.Entity->GetEyePosition();
			//ImpactTime = 0;

			CustomPlayer *pCPlayer = &AllPlayers[LastTargetIndex];

			Personalize *pPersonalize = &pCPlayer->Personalize;
			if (!pCPlayer->Dormant && pPersonalize->ShouldResolve())
			{
#if 0
				CTraceFilterPlayersOnlyNoWorld filter;
				filter.AllowTeammates = false;
				Ray_t ray;
				trace_t tr;
				ray.Init(ShotFrom, Impact);
				filter.pSkip = (IHandleEntity*)LocalPlayer.Entity;
				filter.m_icollisionGroup = COLLISION_GROUP_NONE;
				Interfaces::EngineTrace->TraceRay(ray, MASK_SHOT, (ITraceFilter*)&filter, &tr);
				if (tr.m_pEnt && ReadInt((uintptr_t)&tr.m_pEnt->index) == LastTargetIndex)
#else
				//if (LastShotHitPercentage >= 82.0f || AimbotHitPercentageHeadTxt.flValue < 5)
#endif
				{
					pPersonalize->ShotsnotHit++;
					if (pPersonalize->ShotsnotHit > 100)
						pPersonalize->ShotsnotHit = 1;
					CBaseEntity *Entity = pCPlayer->BaseEntity;
					//Don't count shots made when player is moving while jumping. Only count shots made if they jump up and down
					//FIX ME - cause issues in nospread downbelow
					bool AirJumping = ((Entity->GetGroundEntity() == nullptr || !(Entity->GetFlags() & FL_ONGROUND)) && Entity->GetVelocity().Length2D() > 1.0f);
					int tickcount = ReadInt((uintptr_t)&Interfaces::Globals->tickcount);
					if (tickcount != TickHitWall)
					{
						TickHitWall = tickcount;
						OriginalDontStatsThisShot = pPersonalize->DontStatsThisShot;
						OriginalIsCorrected = pPersonalize->isCorrected;
						OriginalCorrectedIndex = pPersonalize->correctedindex;
						OriginalCloseFakeIndex = pPersonalize->correctedclosefakeindex;
						OriginalResolveWhenShootingIndex = pPersonalize->correctedresolvewhenshootingindex;
						OriginalShotsMissed = pPersonalize->ShotsMissed;
						OriginalCorrectedBodyYawIndex = pPersonalize->correctedbodyyawindex;
						OriginalCorrectedFakeWalkIndex = pPersonalize->correctedfakewalkindex;
						if (tickcount != TickHitPlayer)
						{
							TickHitWall = tickcount;
							pPersonalize->ShotsMissed++;
							if (++pPersonalize->correctedbodyyawindex > (MAX_BODY_YAW_MISSES - 1))
								pPersonalize->correctedbodyyawindex = 0;

							if (++pPersonalize->correctedfakewalkindex > (MAX_BODY_YAW_MISSES - 1))
								pPersonalize->correctedfakewalkindex = 0;

							if (FireWhenEnemiesShootChk.Checked && pCPlayer->ResolverFiredAtShootingAngles)
							{
								if (++pPersonalize->correctedresolvewhenshootingindex > (MAX_FIRING_WHEN_SHOOTING_MISSES - 1))
									pPersonalize->correctedresolvewhenshootingindex = 0;
								pPersonalize->DontStatsThisShot = false;
							}
							else
							{
								if (!pPersonalize->DontStatsThisShot)
								{
									if (!AirJumping)
									{
										pPersonalize->isCorrected = false;
										if (pCPlayer->CloseFakeAngle)
										{
											if (++pPersonalize->correctedclosefakeindex > (MAX_NEAR_BODY_YAW_RESOLVER_MISSES - 1))
												pPersonalize->correctedclosefakeindex = 0;
										}
										else
										{
											if (++pPersonalize->correctedindex > (MAX_RESOLVER_MISSES - 1))
												pPersonalize->correctedindex = 0;
										}
									}
									pPersonalize->DontStatsThisShot = false;
								}
							}
						}
					}
				}
			}
		}

		if (DrawDamageChk.Checked)
		{
			float curtime = ReadFloat((uintptr_t)&Interfaces::Globals->curtime);
			if (iDamageGivenToTarget <= 0)
			{
				iHitsToTarget = 0;
				iLastHitgroupHit = 0;
				flTimeDamageWasGiven = curtime;
			}
			else if (curtime - flTimeDamageWasGiven >= 1.2f)
			{
				iHitsToTarget = 0;
				iDamageGivenToTarget = 0;
				iLastHitgroupHit = 0;
				LastPlayerDamageGiven = nullptr;
				flTimeDamageWasGiven = curtime;
			}
		}
	}
}