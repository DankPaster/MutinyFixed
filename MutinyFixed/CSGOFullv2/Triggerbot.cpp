#include "Triggerbot.h"
#include "Overlay.h"
#include "KnifeBot.h"
#include "AutoWall.h"
#include "pSilent.h"
#include "HitChance.h"
#include "Aimbot.h"
#include "player_lagcompensation.h"
#include "FixMove.h"
#include "LocalPlayer.h"
#include "GOTV.h"
#include "FakeLag.h"

CTriggerbot gTriggerbot;

void CTriggerbot::FireBullet(CBaseCombatWeapon* weapon, int& buttons, bool& DontSetViewAngles, bool RightClick)
{
	if (!(buttons & IN_ATTACK2))
	{
		DontSetViewAngles = false;
		buttons |= !RightClick ? IN_ATTACK : IN_ATTACK2;
		NextTimeTriggerBot = Time + ((float)TriggerBotDelayTxt.iValue / 1000.0f) - fmin(fmax(0.0f, Time - NextTimeTriggerBot), (1.0f / 32.0f));
	}
}

bool CTriggerbot::WeaponCanFire(CBaseCombatWeapon *weapon)
{
	if (weapon && !weapon->IsEmpty())
	{
		float tickinterval = ReadFloat((uintptr_t)&Interfaces::Globals->interval_per_tick);
		float frametime = ReadFloat((uintptr_t)&Interfaces::Globals->absoluteframetime);
		float timelost = frametime > tickinterval ? frametime - tickinterval : 0.0f;
		float m_flServerTime = (LocalPlayer.Entity->GetTickBase() * tickinterval) + timelost;
		if (m_flServerTime >= weapon->GetNextPrimaryAttack())
		{
			return true;
		}
	}
	return false;
}

void CTriggerbot::RunTriggerbot(CBasePlayer* TargetPlayer, CBaseCombatWeapon *weapon, QAngle* RealViewAngles, QAngle& angles, int flags, float& BestDamage, bool& DontSetViewAngles, DWORD* FramePointer, bool& RanHitChance)
{
	auto stat = START_PROFILING("RunTriggerbot");
	int WeaponID = weapon->GetItemDefinitionIndex();
	if (WeaponID != WEAPON_C4)
	{
		bool IsRevolver = WeaponID == WEAPON_REVOLVER;
		if (weapon->IsKnife(WeaponID))
		{
			if (KnifeBotChk.Checked)
			{
				//Knife Bot
				KnifeBot(buttons, TargetPlayer, &angles, weapon, DontSetViewAngles);
			}
		}
		else if (!weapon->IsGrenade(WeaponID))
		{
			bool Jumping = TriggerbotDontShootWhileJumpingChk.Checked && !(flags & FL_INWATER) && !(flags & FL_ONGROUND) && (fabsf(LocalPlayer.Entity->GetVelocity().z) <= 5.5f) && WeaponID != WEAPON_SSG08 && WeaponID != WEAPON_TASER;
			if (!Jumping)
			{
				if (TargetPlayer)
				{
					if (WeaponCanFire(weapon))
					{
						Vector LocalEyePos;
						CustomPlayer* pCAPlayer = &AllPlayers[ReadInt((uintptr_t)&TargetPlayer->index)];
#if 0
						if (ResolverChk.Checked)
						{
							LocalEyePos = !pCAPlayer->Backtracked ? pCAPlayer->SimulationTimeNetVars.LocalVars.LocalEyePos : pCAPlayer->LastUpdatedNetVars.LocalVars.LocalEyePos;
							LocalEyePos = LocalPlayer.Entity->GetEyePosition();
						}
						else
#endif
						{
							LocalEyePos = LocalPlayer.Entity->GetEyePosition();
						}
						if (pSilentChk.Checked)
						{
							//pSilent Firing
							if (RanHitChance || BulletWillHit(weapon, buttons, TargetPlayer, angles, LocalEyePos, nullptr, MTargetting.CURRENT_HITCHANCE_FLAGS))
							{
								//int iOriginalTickCount = CurrentUserCmd.cmd->tick_count;
								//gLagCompensation.AdjustTickCountForCmd(TargetPlayer, false);
								//if (!gAntiGOTV.GOTVIsRecording(CurrentUserCmd.cmd->tick_count))
								{
									FireBullet(weapon, buttons, DontSetViewAngles, IsRevolver);
									CustomPlayer* pCAPlayer = &AllPlayers[ReadInt((uintptr_t)&TargetPlayer->index)];
									pCAPlayer->Personalize.LastHitGroupShotAt = MTargetting.CURRENT_HITGROUP_AIMING_AT;
									pCAPlayer->Personalize.LastEyeAnglesShotAt = pCAPlayer->Personalize.EyeAnglesShotAt;
									pCAPlayer->Personalize.EyeAnglesShotAt = TargetPlayer->GetEyeAngles();
									gpSilent.Run_pSilent(*RealViewAngles, angles, FramePointer);
									PS_LastShotTime = ReadFloat((uintptr_t)&Interfaces::Globals->curtime);
									TriggerbotTargetEntity = TargetPlayer;
								}
								//CurrentUserCmd.cmd->tick_count = iOriginalTickCount;
							}
						}
						else
						{
							CustomPlayer* pCAPlayer = &AllPlayers[ReadInt((uintptr_t)&TargetPlayer->index)];
							bool manual = LocalPlayer.IsManuallyFiring(weapon);
							bool canFire = gLagCompensation.CanFireAtLagCompensatedPlayer(pCAPlayer);
							if (manual || (canFire && (RanHitChance || BulletWillHit(weapon, buttons, TargetPlayer, angles, LocalEyePos, nullptr, MTargetting.CURRENT_HITCHANCE_FLAGS))))
							{
								//Aimbot Firing
								bool WillBreakAntiAim = false;

								if (AntiBacktrackChk.Checked && !IsRevolver && !NoAntiAimsAreOn(true))
								{
									bool Lagging = Interfaces::Globals->absoluteframetime > Interfaces::Globals->interval_per_tick;
									if (!Lagging && FakeLagDropDown.index == FAKELAG_STATIC && FakeLagChokedTxt.iValue > 3) //Can't properly fake both pitch/yaw unless choking at least 4 ticks
									{
										//if (Interfaces::Globals->curtime - PS_LastShotTime > 0.05f)
										{
											//Conceal both the pitch and yaw when we fire so we can't get backtracked
											bool CmdsChokedIsEven = CurrentUserCmd.iCmdsChoked % 2 == 0;
											bool ValidFireTicksAreEven = (FakeLagChokedTxt.iValue - 1) % 2 == 0;
											//FIXME: AUTOMATICALLY CHOKE CORRECT AMOUNT IF CHOKEDTICKS <= 2
											//Below works also but shoots too slowly
											//if (CurrentUserCmd.bSendPacket || CurrentUserCmd.iCmdsChoked != (FakeLagChokedTxt.iValue - 1))

#if 0
											auto besttick = AllPlayers[TargetPlayer->index].GetBestBacktrackTick();
											if (!besttick || besttick != AllPlayers[TargetPlayer->index].LatestFiringTick)
												WillBreakAntiAim = true;

											if (besttick && !besttick->LowerBodyUpdated)
												WillBreakAntiAim = true;
#endif

											if (CurrentUserCmd.bSendPacket || CmdsChokedIsEven != ValidFireTicksAreEven)
												WillBreakAntiAim = true;
											else
												PS_LastShotTime = Interfaces::Globals->curtime;

											//CurrentUserCmd.iCmdsChoked = FakeLagChokedTxt.iValue;
											//CurrentUserCmd.bSendPacket = true;
										}
									}
								}

								if (!WillBreakAntiAim)
								{
									pCAPlayer->Personalize.LastHitGroupShotAt = MTargetting.CURRENT_HITGROUP_AIMING_AT;
									pCAPlayer->Personalize.LastEyeAnglesShotAt = pCAPlayer->Personalize.EyeAnglesShotAt;
									pCAPlayer->Personalize.EyeAnglesShotAt = TargetPlayer->GetEyeAngles();
									FireBullet(weapon, buttons, DontSetViewAngles, false);
									TriggerbotTargetEntity = TargetPlayer;

									pCAPlayer->ResolverFiredAtShootingAngles = FireWhenEnemiesShootChk.Checked && pCAPlayer->Backtracked && pCAPlayer->LatestFiringTick;
								}
							}
						}
					}
				}
				else
				{
					CBaseEntity *EntityHit;
					bool CanHit = false;
					int hitgroup = 0;
					Vector LocalEyePos = LocalPlayer.Entity->GetEyePosition();
					
					if (IsRevolver || WeaponCanFire(weapon))
					{
						if (AutoWallChk.Checked)
						{
							float dmg;
							EntityHit = Autowall(nullptr, LocalPlayer.Entity->GetEyePosition(), &angles, weapon, &dmg, hitgroup, nullptr, false);
							if (EntityHit && (dmg >= MinimumDamageBeforeFiringTxt.flValue || (float)EntityHit->GetHealth() - dmg <= 0))
							{
								CanHit = EntityHit->IsPlayer() && MTargetting.IsPlayerAValidTarget(EntityHit) && MTargetting.IsHitgroupATarget(hitgroup, EntityHit); //EntityHit pointer is checked inside the function
							}
						}
						else
						{
							Vector vecDir;

							AngleVectors(angles, &vecDir);
							VectorNormalizeFast(vecDir);
							Vector EndPos = LocalEyePos + (vecDir * 8192);
							trace_t tr;
							UTIL_TraceLine(LocalEyePos, EndPos, MASK_SHOT, LocalPlayer.Entity, &tr);
							CTraceFilter filter;
							filter.pSkip = (IHandleEntity*)LocalPlayer.Entity;
							filter.m_icollisionGroup = COLLISION_GROUP_NONE;
							UTIL_ClipTraceToPlayers(LocalEyePos, EndPos + vecDir * 40.0f, 0x4600400B, &filter, &tr);
							EntityHit = tr.m_pEnt;
							hitgroup = tr.hitgroup;
							CanHit = MTargetting.IsPlayerAValidTarget(EntityHit) && EntityHit->GetAlive() && MTargetting.IsHitgroupATarget(tr.hitgroup, EntityHit);
						}
						if (CanHit)
						{
							if (TriggerBotTimeToFireTxt.flValue != 0.0f && (TimeToShoot == 0.0f || Time - TimeToShoot >= 0.5f))
							{
								TimeToShoot = Time + TriggerBotTimeToFireTxt.flValue * 0.001f;
							}
							if (!IsRevolver && Time >= TimeToShoot)
							{
								CustomPlayer* pCAPlayer = &AllPlayers[ReadInt((uintptr_t)&EntityHit->index)];
								bool manual = LocalPlayer.IsManuallyFiring(weapon);
								bool canFire = gLagCompensation.CanFireAtLagCompensatedPlayer(pCAPlayer);
								if (manual || (canFire && BulletWillHit(weapon, buttons, EntityHit, angles, LocalEyePos, nullptr, HITGROUP_ALL_FLAG)))
								{
									//Triggerbot Firing
									pCAPlayer->Personalize.LastHitGroupShotAt = hitgroup;
									pCAPlayer->Personalize.LastEyeAnglesShotAt = pCAPlayer->Personalize.EyeAnglesShotAt;
									pCAPlayer->Personalize.EyeAnglesShotAt = EntityHit->GetEyeAngles();
									FireBullet(weapon, buttons, DontSetViewAngles, false);
									TimeToShoot = 0.0f;
									TriggerbotTargetEntity = EntityHit;

									//FIXME SHOULD THIS BE HERE
									pCAPlayer->ResolverFiredAtShootingAngles = FireWhenEnemiesShootChk.Checked && pCAPlayer->Backtracked && pCAPlayer->LatestFiringTick;
								}
							}
						}
						else
						{
							TimeToShoot = 0.0f;
						}
					}
				}
			}
		}
	}
	END_PROFILING(stat);
}

bool CTriggerbot::RevolverWillFireThisTick(CBaseCombatWeapon* weapon)
{
	float outGoingLatency = (gLagCompensation.GetTrueNetworkLatency(0) + gLagCompensation.GetTrueNetworkLatency(1))*1.1;
	float flFireReadyTime = weapon->GetPostPoneFireReadyTime();
	if (flFireReadyTime > 0 && flFireReadyTime < Interfaces::Globals->curtime + outGoingLatency*2)
		return true;

	return false;
}

void CTriggerbot::AutoCockRevolver(int &buttons, CBaseCombatWeapon* weapon)
{
	if (!AutoRevolverChk.Checked || buttons & IN_RELOAD || buttons & IN_ATTACK2)
		return;

	if (RevolverWillFireThisTick(weapon))
		buttons &= ~IN_ATTACK;
	else
		buttons |= IN_ATTACK;
}

//Returns false if can't find a player
#if 0
bool LegacyTriggerBot(int PlayerInCrosshair, CBaseCombatWeapon *weapon)
{
	if (PlayerInCrosshair != INVALID_PLAYER && PlayerInCrosshair <= MAX_PLAYERS /*(DWORD)MaxPlayers*/)
	{
		CustomPlayer *player = &AllPlayers[PlayerInCrosshair];
		if (!player->Dormant && player->LifeState == LIFE_ALIVE && player->IsTarget)
		{
			if (WeaponCanFire(weapon))
			{
				WriteInt((uintptr_t)&CurrentUserCmd.cmd->buttons, ReadInt((uintptr_t)&CurrentUserCmd.cmd->buttons) | IN_ATTACK);
				TriggerbotTargetEntity = player->BaseEntity;
				return true;
			}
		}
	}
	return false;
}
#endif