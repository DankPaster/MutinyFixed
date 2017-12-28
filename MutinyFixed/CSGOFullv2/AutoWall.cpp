#include "AutoWall.h"
#include "Overlay.h"
#include "TraceRay.h"
#include "Targetting.h"
#include "LocalPlayer.h"

float inline GetHitgroupDamageMultiplier(int iHitGroup)
{
	switch (iHitGroup)
	{
	case HITGROUP_HEAD:
	{
		return 4.0f;
	}
	case HITGROUP_STOMACH:
	{
		return 1.25f;
	}
	case HITGROUP_LEFTLEG:
	case HITGROUP_RIGHTLEG:
	{
		return 0.75f;
	}
	}
	return 1.0f;
}

void ScaleDamage(int hitgroup, CBaseEntity *enemy, float weapon_armor_ratio, float &current_damage)
{
	current_damage *= GetHitgroupDamageMultiplier(hitgroup);
	float m_flArmor = (float)enemy->GetArmor();
	if (m_flArmor > 0.0f && hitgroup < HITGROUP_LEFTLEG)  //don't do gear, or legs
	{
		if (hitgroup == HITGROUP_HEAD && !enemy->HasHelmet())
			return;

		float armorscaled = (weapon_armor_ratio * 0.5f) * current_damage;
		if ((current_damage - armorscaled) * 0.5f > m_flArmor)
			armorscaled = current_damage - (m_flArmor * 2.0f);
		current_damage = armorscaled;
	}
}

//Aimware fix traceray, semi-finished comments by Dylan, other comments by unknown

//Damage multipliers are off (check mutiny source code)
//Use pointscale - code in mutiny for making corners closer and further apart - makes multipoint much better, spent hours getting perfect values 
// LOVE JAKE Xx started reading through code on my $198 laptop so I can start being useful :)
void FixTraceRay(Vector end, Vector start, trace_t* oldtrace, CBaseEntity* ent) {
	auto mins = ent->GetMins();
	auto maxs = ent->GetMaxs();

	auto CenterOfBBOX = (maxs + mins) * 0.5f;
	auto origin = *ent->GetAbsOrigin();

	Vector CenterOfEntityInWorld = (CenterOfBBOX + origin);

	Vector delta(end - start);
	delta.Normalize();

	auto v20 = (CenterOfEntityInWorld - start);
	auto ForwardDot = delta.Dot(v20);

	float v23;

	if (ForwardDot >= 0.0f) {
		//Player is at or in front
		if (ForwardDot <= delta.Length())
		{
			auto fuck(CenterOfEntityInWorld - ((delta * ForwardDot) + start));
			v23 = fuck.Length();
		}
		else
		{
			auto lol(CenterOfEntityInWorld - end);
			v23 = -lol.Length();
		}
	}

	else
		v23 = -v20.Length();

	if (v23 <= 60.f) {
		Ray_t ray;
		ray.Init(start, end);

		trace_t trace;
		Interfaces::EngineTrace->ClipRayToEntity(ray, 0x4600400B, (IHandleEntity*)ent, &trace);

		if (oldtrace->fraction > trace.fraction)
			*oldtrace = trace;
	}
}

bool(__thiscall *HandleBulletPenetrationCSGO)(CBaseEntity *pEntityHit, float *flPenetration, int *SurfaceMaterial, char *IsSolid, trace_t *ray, Vector *vecDir,
	int unused, float flPenetrationModifier, float flDamageModifier, int unused2, int weaponmask, float flPenetration2, int *hitsleft,
	Vector *ResultPos, int unused3, int unused4, float *damage);


#ifndef USE_REBUILT_HANDLE_BULLET_PENETRATION
bool HandleGlassPenetration(trace_t &enter_trace, Vector *vecDir, int enter_material, float flPenetrationModifier, float flPenetration, Vector &result, float *bulletdamage)
{
	Vector end;
	trace_t trace_exit;
	if (!TraceToExitRebuilt(end, enter_trace, enter_trace.endpos, *vecDir, &trace_exit))
		return true;

	float damage = *bulletdamage;
	surfacedata_t *exit_surface_data = Interfaces::Physprops->GetSurfaceData(trace_exit.surface.surfaceProps);
	int exit_material = (int)ReadShort((uintptr_t)&exit_surface_data->game.gamematerial);

	float exit_surf_penetration_mod = ReadFloat((uintptr_t)&exit_surface_data->game.flPenetrationModifier);
	float final_damage_modifier = 0.16f;
	float combined_penetration_modifier = 0.0f;

	if (((enter_trace.contents & CONTENTS_GRATE) != 0) || (enter_material == 89) || (enter_material == 71))//|| (enter_material == 70))
	{
		combined_penetration_modifier = 3.0f;
		final_damage_modifier = 0.05f;
	}
	else
	{
		combined_penetration_modifier = (flPenetrationModifier + exit_surf_penetration_mod) * 0.5f;
	}

	if (enter_material == exit_material)
	{
		if (exit_material == 87 || exit_material == 85)
			combined_penetration_modifier = 3.0f;
		else if (exit_material == 76)
			combined_penetration_modifier = 2.0f;
	}

	float v34 = fmaxf(0.f, 1.0f / combined_penetration_modifier);
	float v35 = (damage * final_damage_modifier) + v34 * 3.0f * fmaxf(0.0f, (3.0f / flPenetration) * 1.25f);
	float thickness = (trace_exit.endpos - enter_trace.endpos).Length();

	thickness *= thickness;
	thickness *= v34;
	thickness /= 24.0f;

	float lost_damage = fmaxf(0.0f, v35 + thickness);

	if (lost_damage > damage)
		return true;

	if (lost_damage >= 0.0f)
		*bulletdamage -= lost_damage;

	if (*bulletdamage < 1.0f)
		return true;

	result = trace_exit.endpos;
	return false;
}
#else
bool inline handle_bullet_penetration(surfacedata_t* enter_surface, trace_t* enter_trace, Vector direction, Vector* origin/*, float range*/, float penetration, int& penetration_count, float& current_damage)
{
	int i = 1;
	bool a5 = (enter_trace->contents >> 3) & CONTENTS_SOLID;
	bool v19 = (enter_trace->surface.flags >> 7) & SURF_LIGHT;
	auto enter_material = enter_surface->game.material;

	Vector end;
	trace_t exit_trace;

	if (!penetration_count && !a5 && !v19 && enter_material != CHAR_TEX_GLASS)
	{
		if (enter_material != CHAR_TEX_GRATE)
			return false;
	}

	if (penetration_count <= 0)
		return false;

	if (!trace_to_exit(end, enter_trace->endpos, direction, enter_trace, &exit_trace))
	{
		if (!(Interfaces::EngineTrace->GetPointContents(end, MASK_SHOT_HULL, 0) & MASK_SHOT_HULL))
			return false;
	}

	auto exit_surfce_data = Interfaces::Physprops->GetSurfaceData(exit_trace.surface.surfaceProps);

	auto damage_modifier = 0.16f;

	auto average_penetration_modifier = (enter_surface->game.flPenetrationModifier + exit_surfce_data->game.flPenetrationModifier) / 2;

	if (enter_material == CHAR_TEX_GRATE || enter_material == CHAR_TEX_GLASS)
	{
		damage_modifier = 0.05f;
		average_penetration_modifier = 3.f;
	}

	else if (a5 || v19)
	{
		damage_modifier = 0.16f;
		average_penetration_modifier = 1.f;
	}

	if (enter_material == exit_surfce_data->game.material)
	{
		if (exit_surfce_data->game.material == CHAR_TEX_WOOD || exit_surfce_data->game.material == CHAR_TEX_CARDBOARD)
			average_penetration_modifier = 3.f;

		if (exit_surfce_data->game.material == CHAR_TEX_PLASTIC)
			average_penetration_modifier = 2.f;
	}

	auto modifier = max(0.f, 1.f / average_penetration_modifier);
	auto penetration_length = (exit_trace.endpos - enter_trace->endpos).Length();
	auto taken_damage = ((modifier * 3.f) * max(0.f, (3.f / penetration) * 1.25f) + (current_damage * damage_modifier)) + (((penetration_length * penetration_length) * modifier) / 24.f);

	current_damage -= max(0.f, taken_damage);

	if (current_damage < 1.f)
		return false;

	*origin = exit_trace.endpos;

	--penetration_count;

	return true;
}
#endif

//https://github.com/meepdarknessmeep/lua-flex/blob/9ef00a717071be921aea68bbc669b25cf62835f9/src/luabind/luabindg_util.cpp#L57-L134
//Heavily Modified by dylan
CBaseEntity* Autowall(Vector *pTargetPos, Vector pStartingPos, QAngle *localangles, CBaseCombatWeapon *weapon, float *damage, int& hitgroup, CBaseEntity* pTarget, int TargetHitbox)
{
	//HITBOX_HEAD
	//HITBOX_HEAD
	//HITBOX_RIGHT_UPPER_ARM
	int TargetHitgroup = MTargetting.HitboxToHitgroup(TargetHitbox);
	CBaseEntity *pInitialPlayerFound = nullptr;
	float InitialDamage;
	int InitialHitgroupFound;
#if 0
	Vector InitialPosFound;
#endif

	auto stat = START_PROFILING("AutoWall");
	if (weapon)
	{
		WeaponInfo_t *wpn_data = weapon->GetCSWpnData();
		if (wpn_data)
		{
			trace_t enter_tr;
			Vector vecDir;

			if (pTargetPos)
			{
				QAngle angles = CalcAngle(pStartingPos, *pTargetPos/*Pos*/);
				AngleVectors(angles, &vecDir);
			}
			else
			{
				AngleVectors(*localangles, &vecDir);
			}
			VectorNormalizeFast(vecDir);

			int hitsleft = 4;
			Vector result = pStartingPos;
			CBaseEntity *lastEntityHit = nullptr;
			float m_flPenetration = wpn_data->flPenetration;
			float bulletdmg = wpn_data->iDamage;
			float tmpdmg = bulletdmg;
			float flDistance = wpn_data->flRange;
			float flCurrentDistance = 0.0f;  //distance that the bullet has traveled so far
											 //float scaled = 0.0f; //unused after it's set..

			int weaponmask = weapon->GetItemDefinitionIndex() == WEAPON_TASER ? 0x1100 : 0x1002;

			while (bulletdmg > 0.0f)
			{
				flDistance = flDistance - flCurrentDistance;
				Vector EndPos = pStartingPos + vecDir * flDistance;

				UTIL_TraceLineIgnoreTwoEntities(result, EndPos, 0x4600400B, LocalPlayer.Entity, lastEntityHit, COLLISION_GROUP_NONE, &enter_tr);

				// Check for player hitboxes extending outside their collision bounds
				if (pTarget && TargetHitbox == HITBOX_HEAD) {
					//Pycache/aimware traceray fix for head while players are jumping
					FixTraceRay(EndPos + vecDir * 40.0f, pStartingPos, &enter_tr, pTarget);
				}
				else {
					CTraceFilterSkipTwoEntities filter((IHandleEntity*)LocalPlayer.Entity, (IHandleEntity*)lastEntityHit, COLLISION_GROUP_NONE);
					UTIL_ClipTraceToPlayers(pStartingPos, EndPos + vecDir * 40.0f, 0x4600400B, &filter, &enter_tr);
				}

				if (enter_tr.fraction == 1.0f)
				{
					if (pTarget && !(pTarget->GetFlags() & FL_ONGROUND))
					{
						//Pycache/aimware traceray fix for players that are jumping
						enter_tr.hitgroup = TargetHitgroup;
						enter_tr.m_pEnt = pTarget;
					}
					else
					{
						//Standard CSGO behavior
						//Don't attempt bullet penetration if we hit nothing
						break;
					}
				}

				surfacedata_t *entersurf = Interfaces::Physprops->GetSurfaceData(enter_tr.surface.surfaceProps);
				surfacegameprops_t *props = &entersurf->game;
				float flPenetrationModifier = props->flPenetrationModifier;
				char unkflag = (enter_tr.surface.flags >> 3) & 1;

				//calculate the damage based on the distance the bullet travelled.
				flCurrentDistance += flDistance * enter_tr.fraction;
				bulletdmg = powf(wpn_data->flRangeModifier, (flCurrentDistance * 0.002f)) * tmpdmg;

				if (flCurrentDistance > 3000.0f)
				{
					if (m_flPenetration > 0 || flPenetrationModifier <= 0.1f)
					{
						hitsleft = 0;
					}
				}
				else if (flPenetrationModifier <= 0.1f)
				{
					hitsleft = 0;
				}

				lastEntityHit = enter_tr.m_pEnt;

				if (enter_tr.hitgroup != 0)
				{
					//TODO: Deal with entities other than players such as chickens or NPCS?
					if (lastEntityHit && lastEntityHit->IsPlayer() && (TargetTeammatesChk.Checked || lastEntityHit->GetTeam() != LocalPlayer.Entity->GetTeam()) && (lastEntityHit->GetImmune() == false))
					{
						//Interfaces::DebugOverlay->AddBoxOverlay(enter_tr.endpos, Vector(-2, -2, -2), Vector(2, 2, 2), QAngle(vecDir.x, vecDir.y, vecDir.z), 255, 0, 0, 255, 2 * Interfaces::Globals->interval_per_tick);

						//Valid player!
						if (enter_tr.hitgroup == TargetHitgroup)
						{
							//If the hitgroup we hit is the one we are looking for, then just return immediately
#if 0
							static float nexttimedraw = 0.0f;
							if (Time >= nexttimedraw)
							{
								if (RunningFrame)
								{
									char txt[128];
									sprintf(txt, "Before scaled damage: %.6f", bulletdmg);
									DrawStringUnencrypted(txt, Vector2D(640, 480), 255, 0, 0, pFont);
								}
							}
#endif
							ScaleDamage(enter_tr.hitgroup, lastEntityHit, wpn_data->flArmorRatio, bulletdmg);
							*damage = bulletdmg;
							hitgroup = enter_tr.hitgroup;
							END_PROFILING(stat);
							return lastEntityHit;
						}
						else if (pInitialPlayerFound == nullptr)
						{
							//Otherwise, continue penetration to see if we can hit the target hitgroup
							//Store current values in case we can't
							pInitialPlayerFound = lastEntityHit;
							InitialDamage = bulletdmg;
							ScaleDamage(enter_tr.hitgroup, lastEntityHit, wpn_data->flArmorRatio, InitialDamage);
							InitialHitgroupFound = enter_tr.hitgroup;
#if 0
							InitialPosFound = enter_tr.endpos;
#endif
						}
					}
					else if (!AutoWallPenetrateTeammatesChk.Checked)
					{
						//We hit a teammate and don't want to hit them, quit now
						break;
					}
					//We hit a teammate and we want to penetrate to see if we can hit an enemy through the teammate
				}

#ifdef USE_REBUILT_HANDLE_BULLET_PENETRATION
				if (!handle_bullet_penetration(entersurf, &enter_tr, vecDir, &result, m_flPenetration, hitsleft, bulletdmg))
					break;
				tmpdmg = bulletdmg;
			}
		}
	}
	if (pInitialPlayerFound)
	{
		hitgroup = InitialHitgroupFound;
		*damage = InitialDamage;
#if 0
		char txt2[128];
		sprintf(txt2, "Estimated Bullet Damage: %.6f", InitialDamage);
		Interfaces::DebugOverlay->ClearAllOverlays();
		Interfaces::DebugOverlay->AddTextOverlay(Vector(InitialPosFound.x, InitialPosFound.y, InitialPosFound.z - 10.0f), 1 * Interfaces::Globals->interval_per_tick, txt2);
#endif
		END_PROFILING(stat);
		return pInitialPlayerFound;
	}
	hitgroup = 0;
	END_PROFILING(stat);
	return nullptr;
}

#else
				NoTraceRayEffects = true;
				if (!(enter_tr.contents & CONTENTS_WINDOW))
				{
					//int material = ReadInt((uintptr_t)&entersurf->game.gamematerial);
					if (HandleBulletPenetrationCSGO(LocalPlayer.Entity, &m_flPenetration, (int*)&entersurf->game.gamematerial, &unkflag, &enter_tr, &vecDir, 0, flPenetrationModifier, entersurf->game.flDamageModifier, 0, weaponmask, m_flPenetration, &hitsleft, &result, 0, 0, &bulletdmg))
					{
						//Cannot penetrate this surface, exit now
						break;
					}
				}
				else
				{
					if (HandleGlassPenetration(enter_tr, &vecDir, entersurf->game.gamematerial, flPenetrationModifier, m_flPenetration, result, &bulletdmg))
					{
						break;
					}
				}
				NoTraceRayEffects = false;
				tmpdmg = bulletdmg;
			}
		}
	}
	NoTraceRayEffects = false;

	if (pInitialPlayerFound)
	{
		hitgroup = InitialHitgroupFound;
		*damage = InitialDamage;
		END_PROFILING(stat);
		return pInitialPlayerFound;
	}

	hitgroup = 0;
	END_PROFILING(stat);
	return nullptr;
}
#endif

TraceToExitFn TraceToExitGame;

#ifndef USE_REBUILT_HANDLE_BULLET_PENETRATION
bool TraceToExitRebuilt(Vector &end, trace_t &enter_trace, Vector start, Vector dir, trace_t *exit_trace)
{
	float distance = 0.0f;

	while (distance <= 90.0f)
	{
		distance += 4.0f;
		end = start + dir * distance;

		int point_contents = Interfaces::EngineTrace->GetPointContents(end, MASK_SHOT_HULL | CONTENTS_HITBOX, NULL);

		if (point_contents & MASK_SHOT_HULL && (!(point_contents & CONTENTS_HITBOX)))
			continue;

		auto new_end = end - (dir * 4.0f);

		Ray_t ray;
		ray.Init(end, new_end);
		Interfaces::EngineTrace->TraceRay(ray, MASK_SHOT, 0, exit_trace);

		if (exit_trace->startsolid && exit_trace->surface.flags & SURF_HITBOX)
		{
			ray.Init(end, start);
			CTraceFilter filter;
			filter.pSkip = (IHandleEntity*)exit_trace->m_pEnt;
			Interfaces::EngineTrace->TraceRay(ray, 0x600400B, &filter, exit_trace);

			if (exit_trace->DidHit() && !exit_trace->startsolid)
			{
				end = exit_trace->endpos;
				return true;
			}
			continue;
		}

		if (!exit_trace->DidHit() || exit_trace->startsolid)
		{
			if (exit_trace->m_pEnt)
			{
				if (enter_trace.DidHitNonWorldEntity())
					return true;
			}
			continue;
		}

		if (((exit_trace->surface.flags >> 7) & 1) && !((enter_trace.surface.flags >> 7) & 1))
			continue;

		if (exit_trace->plane.normal.Dot(dir) <= 1.0f)
		{
			float fraction = exit_trace->fraction * 4.0f;
			end = end - (dir * fraction);
			return true;
		}
	}
	return false;
}
#else
//IsEntityBreakable reversed by dylan

bool inline FClassnameIs(CBaseEntity *pEntity, const char* szClassname)
{
	return !strcmp(pEntity->GetClassname(), szClassname) ? true : false;
}

bool IsEntityBreakable(CBaseEntity* pEntity)
{
	if (!pEntity)
		return false;

	int iHealth = pEntity->GetHealth();

	if ((iHealth >= 0 || pEntity->GetUnknownInt() <= 0))//(v2 = pEntity->GetUnknownInt(), result = 1, v2 <= 0)))
	{
		if (pEntity->GetTakeDamage() != DAMAGE_YES)
		{
#ifndef NO_CUSTOM_FIXES
			char *NetworkName = pEntity->GetClientClass()->m_pNetworkName;
			//if (NetworkName[1] == 'F' || NetworkName[4] == 'c' || NetworkName[5] == 'B' || NetworkName[9] == 'h')
			if (*(unsigned*)NetworkName != 0x65724243)
#endif
				return false;
		}

		int CollisionGroup = pEntity->GetCollisionGroup();

		if (CollisionGroup != COLLISION_GROUP_PUSHAWAY && CollisionGroup != COLLISION_GROUP_BREAKABLE_GLASS && CollisionGroup != COLLISION_GROUP_NONE)
			return false;

		if (iHealth > 200)
			return false;

		DWORD pPhysicsInterface = ((DWORD(__cdecl *)(CBaseEntity*, DWORD, DWORD, DWORD, DWORD))AdrOf_IsEntityBreakableCall)(pEntity, 0, IsEntityBreakable_FirstCall_Arg1, IsEntityBreakable_FirstCall_Arg2, 0);
		if (pPhysicsInterface)
		{
			if (((bool(__thiscall*)(DWORD)) *(DWORD*)(*(DWORD*)pPhysicsInterface))(pPhysicsInterface) != PHYSICS_MULTIPLAYER_SOLID)
				return false;
		}
		else
		{
#ifdef NO_CUSTOM_FIXES
			if (FClassnameIs(pEntity, "func_breakable") || FClassnameIs(pEntity, "func_breakable_surf"))
			{
				if (FClassnameIs(pEntity, "func_breakable_surf"))
				{
					// don't try to break it if it has already been broken
					if (pEntity->IsBroken())
						return false;
				}
			}
#else
			const char *szClassname = pEntity->GetClassname();
			if (szClassname[0] == 'f' && szClassname[5] == 'b' && szClassname[13] == 'e')
			{
				if (szClassname[15] == 's')
				{
					// don't try to break it if it has already been broken
					if (pEntity->IsBroken())
						return false;
				}
			}
#endif
			else if (pEntity->PhysicsSolidMaskForEntity() & CONTENTS_PLAYERCLIP)
			{
				// hostages and players use CONTENTS_PLAYERCLIP, so we can use it to ignore them
				return false;
			}
		}

		DWORD pBreakableInterface = ((DWORD(__cdecl *)(CBaseEntity*, DWORD, DWORD, DWORD, DWORD))AdrOf_IsEntityBreakableCall)(pEntity, 0, IsEntityBreakable_SecondCall_Arg1, IsEntityBreakable_SecondCall_Arg2, 0);
		if (pBreakableInterface)
		{
			// Bullets don't damage it - ignore
			float DamageModBullet = ((float(__thiscall*)(DWORD)) *(DWORD*)(*(DWORD*)pPhysicsInterface + 0xC))(pBreakableInterface);
			if (DamageModBullet <= 0.0f)
				return false;
		}
	}

	return true;
}


//CSGO's actual tracetoexit rebuilt by nitro
bool trace_to_exit(Vector& end, Vector start, Vector dir, trace_t* enter_trace, trace_t* exit_trace)
{
	float distance = 0.f;

	while (distance <= 90.f)
	{
		distance += 4.f;
		end = start + (dir * distance);
		auto a1 = end - (dir * 4.f);

		auto point_contents = Interfaces::EngineTrace->GetPointContents(end, MASK_SHOT_HULL | CONTENTS_HITBOX, 0);
		if (point_contents & MASK_SHOT_HULL && (!(point_contents & CONTENTS_HITBOX)))
			continue;

		Ray_t ray;
		ray.Init(end, a1);

		Interfaces::EngineTrace->TraceRay(ray, MASK_SHOT_HULL | CONTENTS_HITBOX, 0, exit_trace);

		if (exit_trace->startsolid && exit_trace->surface.flags & SURF_HITBOX)
		{
			CTraceFilter filter;
			filter.m_icollisionGroup = COLLISION_GROUP_NONE;
			filter.pSkip = (IHandleEntity*)exit_trace->m_pEnt;

			Ray_t ray2;
			ray2.Init(end, start);

			Interfaces::EngineTrace->TraceRay(ray2, MASK_SHOT_HULL, &filter, exit_trace);

			if (exit_trace->DidHit() && !exit_trace->startsolid)
			{
				end = exit_trace->endpos;
				return true;
			}

			continue;
		}

		if (!exit_trace->DidHit() || exit_trace->startsolid)
		{
			if (enter_trace->m_pEnt)
			{
				if (enter_trace->DidHitNonWorldEntity() && IsEntityBreakable(enter_trace->m_pEnt))
				{
					*exit_trace = *enter_trace;
					exit_trace->endpos = start + dir;
					return true;
				}
			}
			continue;
		}

		if ((exit_trace->surface.flags >> 7) & SURF_LIGHT)
		{
			if (IsEntityBreakable(exit_trace->m_pEnt) && IsEntityBreakable(enter_trace->m_pEnt))
			{
				end = exit_trace->endpos;
				return true;
			}

			if (!((enter_trace->surface.flags >> 7) & SURF_LIGHT))
				continue;
		}

		if (exit_trace->plane.normal.Dot(dir) <= 1.f)
		{
			end = end - (dir * (exit_trace->fraction * 4.f));
			return true;
		}
	}

	return false;
}
#endif

#if 0
bool TraceToExit(Vector &end, trace_t &enter_trace, Vector start, Vector dir, trace_t *exit_trace)
{
	_asm
	{
#if 0
		float x = start.x;
		float y = start.y;
		float z = start.z;
		float xt = dir.x;
		float yt = dir.y;
		float zt = dir.z;
		push exit_trace
			push zt
			push yt
			push xt
			push z
			push y
			push x
			mov edx, enter_trace
			mov ecx, end
			call TraceToExitGame
			add esp, 0x1C
#else
		push exit_trace
		lea ecx, dir
		push[ecx + 8] //z
		push[ecx + 4] //y
		push[ecx] //x
		lea eax, start
		push[eax + 8] //z
		push[eax + 4] //y
		push[eax] //x
		mov edx, enter_trace
		mov ecx, end
		call TraceToExitGame
		add esp, 0x1C
#endif
	}
}
#endif