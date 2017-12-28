#include "GrenadePrediction.h"
#include "CViewSetup.h"
#include "BaseCombatWeapon.h"
#include <vector>
#include "trace.h"
#include "ConVar.h"
#include "PlayerList.h"
#include "AutoWall.h"
#include "LocalPlayer.h"
extern int DRAW_CALL;
extern int DRAW_CALL_MAX;
#if 1
enum acts {
ACT_NONE = 0,
ACT_THROW,
ACT_LOB,
ACT_DROP,
};

int GrenadeType = 0;
acts act = ACT_NONE;
std::vector <Vector>path;
static Vector grenademins(-2.0f, -2.0f, -2.0f);
static Vector grenademaxs(2.0f, 2.0f, 2.0f);
static int LastSimulateTick = 0;

void Tick(int buttons)
{
	int in_attack = (buttons & IN_ATTACK);
	int in_attack2 = (buttons & IN_ATTACK2);

	act = (in_attack && in_attack2) ? ACT_LOB :
		(in_attack2) ? ACT_DROP :
		(in_attack) ? ACT_THROW :
		ACT_NONE;
}

void PredictGrenade()
{
	if (!LocalPlayer.Entity)
		return;
	CBaseCombatWeapon* pWeapon = (CBaseCombatWeapon*)LocalPlayer.Entity->GetWeapon();
	if (!pWeapon)
		return;
	int wid = pWeapon->GetItemDefinitionIndex();
	if (pWeapon->IsGrenade(wid) && act != ACT_NONE)
	{
		GrenadeType = wid;
		int tick = ReadInt((uintptr_t)&Interfaces::Globals->tickcount);
		if (LastSimulateTick != tick)
		{
			Simulate();
			LastSimulateTick = tick;
		}
	}
	else
	{
		GrenadeType = 0; //WEAPON_NONE
	}
}


void PaintGrenadeTrajectory()
{
	auto stat = START_PROFILING("PaintGrenadeTrajectory");
	if (LastSimulateTick != 0)
	{
		int tick = ReadInt((uintptr_t)&Interfaces::Globals->tickcount);
		if (LastSimulateTick == tick)
		{
			int size = path.size();

			if (GrenadeType && size > 1)
			{
				Vector screen1, screen2;
				Vector prev = path[0];

				const int RedR = 255;
				const int RedG = 0;
				const int RedB = 0;

				const int OrangeR = 255;
				const int OrangeG = 128;
				const int OrangeB = 0;

				const int GreenR = 0;
				const int GreenG = 255;
				const int GreenB = 0;

				const int YellowR = 255;
				const int YellowG = 255;
				const int YellowB = 0;

				Vector endpos = path[size - 1];
				Vector absendpos = endpos;
				float totaladded = 0.0f;
				while (totaladded < 30.0f)
				{
					if (Interfaces::EngineTrace->GetPointContents(endpos) == CONTENTS_EMPTY)
						break;
					totaladded += 2.0f;
					endpos.z += 2.0f;
				}

				//check if end pos has an enemy near it
				float bestdmg = 0.0f;
				static ColorRGBA redcol = { RedR, RedG, RedB, 255 };
				static ColorRGBA greencol = { GreenR, GreenG, GreenB, 255 };
				static ColorRGBA yellowgreencol = { 177, 253, 2, 255 };
				static ColorRGBA yellowcol = { YellowR, YellowG, YellowB, 255 };
				static ColorRGBA orangecol = { OrangeR, OrangeG, OrangeB, 255 };
				ColorRGBA *BestColor = &redcol;

				for (int i = 0; i < NumStreamedPlayers; i++)
				{
					CustomPlayer *pCPlayer = StreamedPlayers[i];
					if (pCPlayer->IsTarget)
					{
						float dist = (pCPlayer->BaseEntity->GetOrigin() - endpos).Length();
						if (dist < 350.0f)
						{
							CTraceFilterIgnoreGrenades filter;
							filter.pSkip = (IHandleEntity*)LocalPlayer.Entity;
							filter.m_icollisionGroup = COLLISION_GROUP_NONE;
							Ray_t ray;
							Vector NadeScreen;
							WorldToScreenCapped(endpos, NadeScreen);

							Vector vPelvis = pCPlayer->BaseEntity->GetBonePosition(HITBOX_PELVIS,/*pCPlayer->BaseEntity->GetSimulationTime()*/Interfaces::Globals->curtime, false, false);
							ray.Init(endpos, vPelvis);
							trace_t ptr;
							Interfaces::EngineTrace->TraceRay(ray, MASK_SHOT, (ITraceFilter*)&filter, &ptr);

							if (ptr.m_pEnt == pCPlayer->BaseEntity)
							{
								Vector PelvisScreen;
								WorldToScreenCapped(vPelvis, PelvisScreen);
								static float a = 105.0f;
								static float b = 25.0f;
								static float c = 140.0f;

								float d = (((pCPlayer->BaseEntity->GetOrigin() - absendpos).Length() - b) / c);
								float flDamage = a*exp(-d * d);
								HANDLE LocalPlayerObserveTarget = LocalPlayer.Entity->GetObserverTarget();
								int dmg = max((int)ceilf(CSGO_Armor(flDamage, pCPlayer->BaseEntity->GetArmor())), 0);
								//int healthafterexplosion = (int)(max((int)pEntityToDrawHealth->GetHealth() - max((int)ceilf(CSGO_Armor(flDamage, pEntityToDrawHealth->GetArmor())), 0), 0));
								char dmgstr[128];
								sprintf(dmgstr, "-%i", dmg);

								ColorRGBA *destcolor = dmg <= 0 ? &redcol : dmg >= 40.0f ? &greencol : dmg >= 30.0f ? &yellowgreencol : dmg >= 20.0f ? &yellowcol : &orangecol;
								DrawLine(Vector2D(NadeScreen.x, NadeScreen.y), Vector2D(PelvisScreen.x, PelvisScreen.y), destcolor->r, destcolor->g, destcolor->b, destcolor->a);
								DrawStringUnencrypted(dmgstr, Vector2D(PelvisScreen.x, PelvisScreen.y - 25), *destcolor, pFont);

								Vector midpoint = (vPelvis + absendpos) * 0.5f;
								Vector midpointscreen;
								WorldToScreenCapped(midpoint, midpointscreen);

								sprintf(dmgstr, "%.1f", (absendpos - vPelvis).Length());
								DrawStringUnencrypted(dmgstr, Vector2D(midpointscreen.x, midpointscreen.y), ColorRGBA(255, 255, 255, 255), pFontSmall);

								if (dmg > bestdmg)
								{
									BestColor = destcolor;
									bestdmg = dmg;
								}
							}
						}
					}
				}

				for (auto& cur : path)
				{
					if (WorldToScreenCapped(prev, screen1) && WorldToScreenCapped(cur, screen2))
					{
						DrawLine(Vector2D(screen1.x, screen1.y), Vector2D(screen2.x, screen2.y), BestColor->r, BestColor->g, BestColor->b, BestColor->a);
					}
					prev = cur;
				}
#if 0
				for (auto it = path.begin(), end = path.end(); it != end; ++it)
				{
					Vector cur = *it;
					if (WorldToScreenCapped(prev, screen1) && WorldToScreenCapped(cur, screen2))
					{
						DrawLine(Vector2D(screen1.x, screen1.y), Vector2D(screen2.x, screen2.y), BestColor->r, BestColor->g, BestColor->b, BestColor->a);
					}
					prev = cur;
				}
#endif

				if (WorldToScreen(prev, screen2))
				{
					DrawCircle(Vector2D(screen2.x, screen2.y), 10, ColorRGBA(255, 201, 14, 255), 8);
					char diststr[64];
					sprintf(diststr, "%.1f", (LocalPlayer.Entity->GetEyePosition() - prev).Length());
					DrawStringUnencrypted(diststr, Vector2D(screen2.x + 15, screen2.y - 5), ColorRGBA(255, 255, 255, 255), pFontSmall);
					trace_t tr;
					Ray_t ray;
					ray.Init(absendpos, absendpos + Vector(0.0f, 0.0f, -1.0f) * 1024.0f);
					CTraceFilter filter;
					filter.pSkip = (IHandleEntity*)LocalPlayer.Entity;
					filter.m_icollisionGroup = COLLISION_GROUP_NONE;
					Interfaces::EngineTrace->TraceRay(ray, MASK_SHOT, &filter, &tr);
					float len = (tr.endpos - absendpos).Length();
					if (len > 10.0f)
					{
						Vector enddownpos;
						WorldToScreen(tr.endpos, enddownpos);
						DrawLine(Vector2D(screen2.x, screen2.y), Vector2D(enddownpos.x, enddownpos.y), 195, 195, 195, 100);

						Vector midpoint = (absendpos + tr.endpos) * 0.5f;
						Vector midpointscreen;
						if (WorldToScreenCapped(midpoint, midpointscreen))
						{
							sprintf(diststr, "%.1f", len);
							DrawStringUnencrypted(diststr, Vector2D(midpointscreen.x + 5, midpointscreen.y), ColorRGBA(255, 255, 255, 100), pFontSmall);
						}
					}
				}
			}
		}
	}
	END_PROFILING(stat);
}

void Setup(CBasePlayer* pl, Vector& vecSrc, Vector& vecThrow, const QAngle& angEyeAngles)
{
	QAngle angThrow = angEyeAngles;
	float pitch = angThrow.x;

	if (angThrow.x <= 90.0f)
	{
		if (angThrow.x < -90.0f)
		{
			angThrow.x += 360.0f;
		}
	}
	else
	{
		angThrow.x -= 360.0f;
	}
	angThrow.x = angThrow.x - (90.0f - fabs(angThrow.x)) * 10.0f / 90.0f;

	// Gets ThrowVelocity from weapon files
	// Clamped to [15,750]
	float flVel = 750.0f * 0.9f;

	// Do magic on member of grenade object [esi+9E4h]
	// m1=1  m1+m2=0.5  m2=0
	static const float power[] = { 1.0f, 1.0f, 0.5f, 0.0f };
	float b = power[act];
	// Clamped to [0,1]
	b = b * 0.7f;
	b = b + 0.3f;
	flVel *= b;

	Vector vForward, vRight, vUp;
	AngleVectors(angThrow, &vForward, &vRight, &vUp);

	vecSrc = pl->GetEyePosition();
	float off = (power[act] * 12.0f) - 12.0f;
	vecSrc.z += off;

	// Game calls UTIL_TraceHull here with hull and assigns vecSrc tr.endpos
	trace_t tr;
	Vector vecDest = vecSrc;

	vecDest.MultAdd(vForward, 22.0f);

	CGrenadePredictTraceFilter filter;
	filter.pSkip = (IHandleEntity*)LocalPlayer.Entity;
	filter.m_icollisionGroup = COLLISION_GROUP_NONE;
	UTIL_TraceHull(vecSrc, vecDest, grenademins, grenademaxs, (ITraceFilter*)&filter, tr);

	// After the hull trace it moves 6 units back along vForward
	// vecSrc = tr.endpos - vForward * 6
	Vector vecBack = vForward; vecBack *= 6.0f;
	vecSrc = tr.endpos;
	vecSrc -= vecBack;

	// Finally calculate velocity
	vecThrow = pl->GetVelocity(); vecThrow *= 1.25f;
	vecThrow.MultAdd(vForward, flVel);
}

void Simulate()
{
	Vector vecSrc, vecThrow;
	QAngle eyeangs;
	Interfaces::Engine->GetViewAngles(eyeangs);
	Setup(LocalPlayer.Entity, vecSrc, vecThrow, eyeangs);

	float interval = ReadFloat((uintptr_t)&Interfaces::Globals->interval_per_tick);

	// Log positions 20 times per sec
	int logstep = static_cast<int>(0.05f / interval);
	int logtimer = 0;

	path.clear();
	for (unsigned int i = 0; i < path.max_size() - 1; ++i)
	{
		if (!logtimer)
			path.push_back(vecSrc);

		int s = Step(vecSrc, vecThrow, i, interval);
		if ((s & 1)) break;

		// Reset the log timer every logstep OR we bounced
		if ((s & 2) || logtimer >= logstep) logtimer = 0;
		else ++logtimer;

		if (vecThrow == vecZero)
			break;
	}
	path.push_back(vecSrc);
}
int Step(Vector& vecSrc, Vector& vecThrow, int tick, float interval)
{
	// Apply gravity
	Vector move;
	AddGravityMove(move, vecThrow, interval, false);

	// Push entity
	trace_t tr;
	PushEntity(vecSrc, move, tr);

	int result = 0;
	// Check ending conditions
	if (CheckDetonate(vecThrow, tr, tick, interval) || tr.fraction == 0.0f)
	{
		result |= 1;
	}

	// Resolve collisions
	if (tr.fraction != 1.0f)
	{
		result |= 2; // Collision!
		ResolveFlyCollisionCustom(tr, vecThrow, interval);
	}

	// Set new position
	vecSrc = tr.endpos;

	return result;
}

bool CheckDetonate(const Vector& vecThrow, const trace_t& tr, int tick, float interval)
{
	switch (GrenadeType)
	{
	case WEAPON_HEGRENADE:
	case WEAPON_FLASHBANG:
		// Pure timer based, detonate at 1.5s, checked every 0.2s
		return static_cast<float>(tick)*interval>1.5f && !(tick%static_cast<int>(0.2f / interval));

	case WEAPON_DECOY:
	case WEAPON_SMOKEGRENADE:
		// Velocity must be <0.1, this is only checked every 0.2s
		if (vecThrow.Length() < 0.1f)
		{
			int det_tick_mod = static_cast<int>(0.2f / interval);
			return !(tick%det_tick_mod);
		}
		return false;

	case WEAPON_INCGRENADE:
	case WEAPON_MOLOTOV:
		// Detonate when hitting the floor
		if (tr.fraction != 1.0f && tr.plane.normal.z>0.7f)
			return true;
		// OR we've been flying for too long

	default:
		return false;
	}
}

extern ConVar *sv_gravity;

void AddGravityMove(Vector& move, Vector& vel, float frametime, bool onground)
{
	Vector basevel(0.0f,0.0f,0.0f);

	move.x = (vel.x + basevel.x) * frametime;
	move.y = (vel.y + basevel.y) * frametime;

	if (onground)
	{
		move.z = (vel.z + basevel.z) * frametime;
	}
	else
	{
		// Game calls GetActualGravity( this );
		float gravity = (sv_gravity ? sv_gravity->GetFloat() : 800.0f) * 0.4f;

		float newZ = vel.z - (gravity * frametime);
		move.z = ((vel.z + newZ) / 2.0f + basevel.z) * frametime;

		vel.z = newZ;
	}
}
void PushEntity(Vector& src, const Vector& move, trace_t& tr)
{
	Vector vecAbsEnd = src + move;

	// Trace through world
	CGrenadePredictTraceFilter filter;
	filter.pSkip = (IHandleEntity*)LocalPlayer.Entity;
	filter.m_icollisionGroup = COLLISION_GROUP_NONE;

	UTIL_TraceHull(src, vecAbsEnd, grenademins, grenademaxs, (ITraceFilter*)&filter, tr);
}

void ResolveFlyCollisionCustom(trace_t& tr, Vector& vecVelocity, float interval)
{
	// Calculate elasticity
	float flSurfaceElasticity;

	//Don't bounce off of players with perfect elasticity
	if (tr.m_pEnt && tr.m_pEnt->IsPlayer())
	{
		flSurfaceElasticity = 0.3f;
	}
	else
	{
		flSurfaceElasticity = 1.0f;
	}

	if (tr.contents & CONTENTS_WINDOW)
	{
		vecVelocity *= 0.65f;
		Vector dummy;
		trace_t exit;
		Vector newprev;
		if (path.size() > 1)
		{
			Vector cur = path[path.size() - 1];
			for (unsigned int i = path.size() - 2; i >= 0; i--)
			{
				newprev = path[i];
				if (newprev != cur && (newprev - cur).Length() > 0.01f)
				{
					break;
				}
			}
			QAngle angle = CalcAngle(newprev, cur);
			if (angle != QAngle(0.0f,0.0f,0.0f))
			{
				Vector vDir;
				AngleVectors(angle, &vDir);
				VectorNormalizeFast(vDir);
				float flDistance = 0;
				Vector end;

				while (flDistance <= 90.0f)
				{
					flDistance += 4.0f;

					end = tr.endpos + (vDir * flDistance);

					if ((Interfaces::EngineTrace->GetPointContents(end) & MASK_SOLID) == 0)
					{
						tr.endpos = end;
						break;
					}
				}
			}
		}
		return;
	}

// ReadFloat((uintptr_t)&entersurf->physics.elasticity); //1.0;  // Assume all surfaces have the same elasticity
	float flGrenadeElasticity = 0.45f; // GetGrenadeElasticity()
	float flTotalElasticity = flGrenadeElasticity * flSurfaceElasticity;
	if (flTotalElasticity>0.9f) flTotalElasticity = 0.9f;
	if (flTotalElasticity<0.0f) flTotalElasticity = 0.0f;

	// Calculate bounce
	Vector vecAbsVelocity;
	PhysicsClipVelocity(vecVelocity, tr.plane.normal, vecAbsVelocity, 2.0f);
	vecAbsVelocity *= flTotalElasticity;
	float flSpeedSqr = DotProduct(vecVelocity, vecVelocity);

	if (tr.plane.normal.z > 0.7f) // Floor
	{
		// Verify that we have an entity.
		vecVelocity = vecAbsVelocity;
		if (flSpeedSqr < (20 * 20))
		{
			vecAbsVelocity.Zero();
			vecVelocity = vecAbsVelocity;
		}
		else
		{
			vecAbsVelocity *= ((1.0f - tr.fraction) * interval);
			PushEntity(tr.endpos, vecAbsVelocity, tr);
		}
	}
	else
	{
		if (flSpeedSqr < (20 * 20))
		{
			vecAbsVelocity.Zero();
			vecVelocity = vecAbsVelocity;
		}
		else
		{
			vecVelocity = vecAbsVelocity;
		}
	}

#if 0
	// Stop completely once we move too slow
	float flSpeedSqr = vecAbsVelocity.LengthSqr();
	static const float flMinSpeedSqr = 20.0f * 20.0f; // 30.0f * 30.0f in CSS
	if (flSpeedSqr<flMinSpeedSqr)
	{
		vecAbsVelocity.Zero();
	}

	// Stop if on ground
	if (tr.plane.normal.z > 0.7f)
	{
		vecVelocity = vecAbsVelocity;
		vecAbsVelocity *= ((1.0f - tr.fraction) * interval);
		PushEntity(tr.endpos, vecAbsVelocity, tr);
	}
	else
	{
		vecVelocity = vecAbsVelocity;
	}
#endif
}
unsigned char PhysicsClipVelocity(const Vector& in, const Vector& normal, Vector& out, float overbounce)
{
	static const float STOP_EPSILON = 0.1f;

	float    backoff;
	float    change;
	float    angle;
	int        i;
	unsigned char blocked;

	blocked = 0;

	angle = normal[2];

	if (angle > 0)
	{
		blocked |= 1;        // floor
	}
	if (!angle)
	{
		blocked |= 2;        // step
	}

	backoff = in.Dot(normal) * overbounce;

	for (i = 0; i < 3; i++)
	{
		change = normal[i] * backoff;
		out[i] = in[i] - change;
		if (out[i] > -STOP_EPSILON && out[i] < STOP_EPSILON)
		{
			out[i] = 0;
		}
	}

	return blocked;
}
#endif