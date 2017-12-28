#if 0
#include "GameMovementSimulation.h"
#include "BaseEntity.h"
#include "Interfaces.h"
#include "ConVar.h"
const int nanmask = 255 << 23;
#define	IS_NAN(x) (((*(int *)&x)&nanmask)==nanmask)
#define	COORD_INTEGER_BITS			14
#define COORD_FRACTIONAL_BITS		5
#define COORD_DENOMINATOR			(1<<(COORD_FRACTIONAL_BITS))
#define COORD_RESOLUTION			(1.0/(COORD_DENOMINATOR))

CGameMovementSimulation::CGameMovementSimulation(void)
{

}

void CGameMovementSimulation::TracePlayerBBox(const Vector& start, const Vector& end, unsigned int fMask, int collisionGroup, trace_t& pm, CBaseEntity* pBaseEntity)
{
	Ray_t ray;
	CTraceFilter filter = CTraceFilter(pBaseEntity);
	ray.Init(start, end, pBaseEntity->GetCollideable()->OBBMins(), pBaseEntity->GetCollideable()->OBBMaxs());
	Interfaces::EngineTrace->TraceRay(ray, fMask, &filter, &pm);
}

void CGameMovementSimulation::SetAbsOrigin(CBaseEntity* pBaseEntity, const Vector &vec)
{
	if (pBaseEntity &&
		pBaseEntity->GetMoveType() == MOVETYPE_WALK)
	{
		trace_t pm;
		TracePlayerBBox(vec, vec, MASK_PLAYERSOLID, COLLISION_GROUP_PLAYER_MOVEMENT, pm, pBaseEntity);
		if (pm.startsolid || pm.allsolid || pm.fraction != 1.0f)
		{
			//player will get stuck, lets not?
			return;
		}
	}

	pBaseEntity->SetAbsOrigin(vec);
}

void CGameMovementSimulation::Friction(CBaseEntity* pBaseEntity)
{
	int i = pBaseEntity->index;

	m_EntityStruct[i].speed = pBaseEntity->GetVelocity().Length();

	if (m_EntityStruct[i].speed < 0.1f)
		return;

	if (pBaseEntity->GetFlags() & FL_ONGROUND)
	{
		m_EntityStruct[i].friction = Interfaces::Cvar->FindVar("sv_friction")->GetFloat() * pBaseEntity->GetFriction();

		m_EntityStruct[i].control = (m_EntityStruct[i].speed < Interfaces::Cvar->FindVar("sv_stopspeed")->fValue) ? Interfaces::Cvar->FindVar("sv_stopspeed")->fValue : m_EntityStruct[i].speed;

		m_EntityStruct[i].drop += m_EntityStruct[i].control * m_EntityStruct[i].friction * Interfaces::Globals->frametime;
	}

	m_EntityStruct[i].newspeed = m_EntityStruct[i].speed - m_EntityStruct[i].drop;

	if (m_EntityStruct[i].newspeed < 0)
		m_EntityStruct[i].newspeed = 0;

	if (m_EntityStruct[i].newspeed != m_EntityStruct[i].speed)
	{
		// Determine proportion of old speed we are using.
		m_EntityStruct[i].newspeed /= m_EntityStruct[i].speed;
		// Adjust velocity according to proportion.
		VectorMultiply(pBaseEntity->GetVelocity(), m_EntityStruct[i].newspeed, pBaseEntity->GetVelocity());
	}

	pBaseEntity->GetVelocity() -= (1.f - m_EntityStruct[i].newspeed) * pBaseEntity->GetVelocity();
}

void CGameMovementSimulation::Accelerate(CBaseEntity* pBaseEntity, Vector& wishdir, float wishspeed, float accel)
{
	int i = pBaseEntity->index;
	// See if we are changing direction a bit
	m_EntityStruct[i].currentspeed = pBaseEntity->GetVelocity().Dot(wishdir);

	// Reduce wishspeed by the amount of veer.
	m_EntityStruct[i].addspeed = wishspeed - m_EntityStruct[i].currentspeed;

	// If not going to add any speed, done.
	if (m_EntityStruct[i].addspeed <= 0)
		return;

	// Determine amount of accleration.
	m_EntityStruct[i].accelspeed = accel * Interfaces::Globals->frametime * wishspeed * pBaseEntity->GetFriction();

	// Cap at addspeed
	if (m_EntityStruct[i].accelspeed > m_EntityStruct[i].addspeed)
		m_EntityStruct[i].accelspeed = m_EntityStruct[i].addspeed;

	// Adjust velocity.
	for (i = 0; i<3; i++)
	{
		pBaseEntity->GetVelocity()[i] += m_EntityStruct[i].accelspeed * wishdir[i];
	}
}

void CGameMovementSimulation::StayOnGround(CBaseEntity* pBaseEntity)
{
	int i = pBaseEntity->index;
	Vector start(*pBaseEntity->GetAbsOrigin());
	Vector end(*pBaseEntity->GetAbsOrigin());
	start.z += 2;
	end.z -= pBaseEntity->GetStepSize();

	// See how far up we can go without getting stuck

	TracePlayerBBox(*pBaseEntity->GetAbsOrigin(), start, MASK_PLAYERSOLID, COLLISION_GROUP_PLAYER_MOVEMENT, m_EntityStruct[i].trace, pBaseEntity);
	start = m_EntityStruct[i].trace.endpos;

	// using trace.startsolid is unreliable here, it doesn't get set when
	// tracing bounding box vs. terrain

	// Now trace down from a known safe position
	TracePlayerBBox(start, end, MASK_PLAYERSOLID, COLLISION_GROUP_PLAYER_MOVEMENT, m_EntityStruct[i].trace, pBaseEntity);
	if (m_EntityStruct[i].trace.fraction > 0.0f &&			// must go somewhere
		m_EntityStruct[i].trace.fraction < 1.0f &&			// must hit something
		!m_EntityStruct[i].trace.startsolid &&				// can't be embedded in a solid
		m_EntityStruct[i].trace.plane.normal[2] >= 0.7)		// can't hit a steep slope that we can't stand on anyway
	{
		float flDelta = fabs(pBaseEntity->GetAbsOrigin()->z - m_EntityStruct[i].trace.endpos.z);

		//This is incredibly hacky. The real problem is that trace returning that strange value we can't network over.
		if (flDelta > 0.5f * COORD_RESOLUTION)
		{
			SetAbsOrigin(pBaseEntity, m_EntityStruct[i].trace.endpos);
		}
	}
}

void CGameMovementSimulation::WalkMove(CBaseEntity* pBaseEntity)
{
	int i = pBaseEntity->index;
	g_pTools->AngleVectors(pBaseEntity->GetEyeAngles(), &m_EntityStruct[i].forward, &m_EntityStruct[i].right, &m_EntityStruct[i].up);

	float fmove = 450.f;
	float smove = 450.f;

	if (m_EntityStruct[i].forward[2] != 0)
	{
		m_EntityStruct[i].forward[2] = 0;
		VectorNormalize(m_EntityStruct[i].forward);
	}

	if (m_EntityStruct[i].right[2] != 0)
	{
		m_EntityStruct[i].right[2] = 0;
		VectorNormalize(m_EntityStruct[i].right);
	}

	for (i = 0; i<2; i++)
		m_EntityStruct[i].wishvel[i] = m_EntityStruct[i].forward[i] * fmove + m_EntityStruct[i].right[i] * smove;

	m_EntityStruct[i].wishvel[2] = 0;

	VectorCopy(m_EntityStruct[i].wishvel, m_EntityStruct[i].wishdir);

	m_EntityStruct[i].wishspeed = VectorNormalize(m_EntityStruct[i].wishdir);

	if ((m_EntityStruct[i].wishspeed != 0.0f) && (m_EntityStruct[i].wishspeed > pBaseEntity->GetMaxSpeed()))
	{
		VectorScale(m_EntityStruct[i].wishvel, pBaseEntity->GetMaxSpeed() / m_EntityStruct[i].wishspeed, m_EntityStruct[i].wishvel);
		m_EntityStruct[i].wishspeed = pBaseEntity->GetMaxSpeed();
	}

	pBaseEntity->GetVelocity()[2] = 0;
	Accelerate(pBaseEntity, m_EntityStruct[i].wishdir, m_EntityStruct[i].wishspeed, Interfaces::Cvar->FindVar("sv_accelerate")->fValue);
	pBaseEntity->GetVelocity()[2] = 0;

	VectorAdd(pBaseEntity->GetVelocity(), pBaseEntity->GetBaseVelocity(), pBaseEntity->GetVelocity());

	m_EntityStruct[i].spd = pBaseEntity->GetVelocity().Length();

	if (m_EntityStruct[i].spd < 1.0f)
	{
		pBaseEntity->GetVelocity().Init();
		VectorSubtract(pBaseEntity->GetVelocity(), pBaseEntity->GetBaseVelocity(), pBaseEntity->GetVelocity());
		return;
	}

	m_EntityStruct[i].dest[0] = *pBaseEntity->GetAbsOrigin()[0] + pBaseEntity->GetVelocity()[0] * Interfaces::Globals->frametime;
	m_EntityStruct[i].dest[1] = *pBaseEntity->GetAbsOrigin()[1] + pBaseEntity->GetVelocity()[1] * Interfaces::Globals->frametime;
	m_EntityStruct[i].dest[2] = *pBaseEntity->GetAbsOrigin()[2];

	TracePlayerBBox(*pBaseEntity->GetAbsOrigin(), m_EntityStruct[i].dest, MASK_PLAYERSOLID, COLLISION_GROUP_PLAYER_MOVEMENT, m_EntityStruct[i].pm, pBaseEntity);

	//g_pMoveHelper->SetHost(pBaseEntity);
	//g_pMoveHelper->m_outWishVel += m_EntityStruct[i].wishdir * m_EntityStruct[i].wishspeed;
	//g_pMoveHelper->SetHost(nullptr);

	if (m_EntityStruct[i].pm.fraction == 1)
	{
		SetAbsOrigin(pBaseEntity, m_EntityStruct[i].pm.endpos);

		VectorSubtract(pBaseEntity->GetVelocity(), pBaseEntity->GetBaseVelocity(), pBaseEntity->GetVelocity());

		StayOnGround(pBaseEntity);
		return;
	}

	//	StepMove(dest, pm);

	VectorSubtract(pBaseEntity->GetVelocity(), pBaseEntity->GetBaseVelocity(), pBaseEntity->GetVelocity());

	StayOnGround(pBaseEntity);
}

void CGameMovementSimulation::CheckVelocity(CBaseEntity* pBaseEntity)
{
	Vector org = *pBaseEntity->GetAbsOrigin();

	for (int i = 0; i < 3; i++)
	{
		if (IS_NAN(pBaseEntity->GetVelocity()[i]))
		{
			pBaseEntity->GetVelocity()[i] = 0;
		}

		if (IS_NAN(org[i]))
		{
			org[i] = 0;
			*pBaseEntity->GetAbsOrigin() = org;
		}

		if (pBaseEntity->GetVelocity()[i] > Interfaces::Cvar->FindVar("sv_maxvelocity")->fValue)
		{
			pBaseEntity->GetVelocity()[i] = Interfaces::Cvar->FindVar("sv_maxvelocity")->fValue;
		}
		else if (pBaseEntity->GetVelocity()[i] < -Interfaces::Cvar->FindVar("sv_maxvelocity")->fValue)
		{
			pBaseEntity->GetVelocity()[i] = -Interfaces::Cvar->FindVar("sv_maxvelocity")->fValue;
		}
	}
}

void CGameMovementSimulation::StartGravity(CBaseEntity* pBaseEntity)
{
	if (!pBaseEntity || !pBaseEntity->GetHealth())
		return;

	Vector pVel = pBaseEntity->GetVelocity();

	pVel[2] -= (Interfaces::Cvar->FindVar("sv_gravity")->fValue * 0.5f * Interfaces::Globals->interval_per_tick);
	pVel[2] += (pBaseEntity->GetBaseVelocity()[2] * Interfaces::Globals->interval_per_tick);

	pBaseEntity->GetVelocity() = pVel;

	Vector tmp = pBaseEntity->GetBaseVelocity();

	tmp[2] = 0.f;

	pBaseEntity->GetBaseVelocity() = tmp;
}

void CGameMovementSimulation::FinishGravity(CBaseEntity* pBaseEntity)
{
	float ent_gravity;

	ent_gravity = Interfaces::Cvar->FindVar("sv_gravity")->fValue;

	pBaseEntity->GetVelocity()[2] -= (ent_gravity * Interfaces::Cvar->FindVar("sv_gravity")->fValue * Interfaces::Globals->frametime * 0.5);

	CheckVelocity(pBaseEntity);
}

void CGameMovementSimulation::CheckFalling(CBaseEntity* pBaseEntity)
{
	// this function really deals with landing, not falling, so early out otherwise
	if (pBaseEntity->GetFallVelocity() <= 0)
		return;

	if (!pBaseEntity->GetHealth() && pBaseEntity->GetFallVelocity() >= 303.0f)
	{
		bool bAlive = true;
		float fvol = 0.5;

		//
		// They hit the ground.
		//
		if (pBaseEntity->GetVelocity().z < 0.0f)
		{
			// Player landed on a descending object. Subtract the velocity of the ground entity.

			pBaseEntity->SetFallVelocity(max(0.1f, pBaseEntity->GetFallVelocity() + pBaseEntity->GetVelocity().z));
		}

		if (pBaseEntity->GetFallVelocity() > 526.5f)
		{
			fvol = 1.0;
		}
		else if (pBaseEntity->GetFallVelocity() > 526.5f / 2)
		{
			fvol = 0.85;
		}
		else if (pBaseEntity->GetFallVelocity() < 173)
		{
			fvol = 0;
		}

		//todo: fix animation here to PLAYER_WALK
	}

	pBaseEntity->SetFallVelocity(0);
}

void CGameMovementSimulation::AirAccelerate(CBaseEntity *pBaseEntity, Vector& wishdir, float wishspeed, float accel)
{
	int i = pBaseEntity->index;
	m_EntityStruct[i].wishspd = wishspeed;

	// Cap speed
	if (m_EntityStruct[i].wishspd > 30)
		m_EntityStruct[i].wishspd = 30;

	// Determine veer amount
	m_EntityStruct[i].currentspeed = pBaseEntity->GetVelocity().Dot(wishdir);

	// See how much to add
	m_EntityStruct[i].addspeed = m_EntityStruct[i].wishspd - m_EntityStruct[i].currentspeed;

	// If not adding any, done.
	if (m_EntityStruct[i].addspeed <= 0)
		return;

	// Determine acceleration speed after acceleration
	m_EntityStruct[i].accelspeed = accel * wishspeed * Interfaces::Globals->frametime * pBaseEntity->GetFriction();

	// Cap it
	if (m_EntityStruct[i].accelspeed > m_EntityStruct[i].addspeed)
		m_EntityStruct[i].accelspeed = m_EntityStruct[i].addspeed;

	// Adjust pmove vel.
	for (int x = 0; x<3; x++)
	{
		Vector velocity = pBaseEntity->GetVelocity();
		velocity.x += m_EntityStruct[i].accelspeed * wishdir[x];
		pBaseEntity->SetVelocity(velocity);

		//g_pMoveHelper->SetHost(pBaseEntity);
		//g_pMoveHelper->m_outWishVel[x] += m_EntityStruct[i].accelspeed * wishdir[x];
		//g_pMoveHelper->SetHost(nullptr);
	}
}

int CGameMovementSimulation::ClipVelocity(CBaseEntity *pBaseEntity, Vector& in, Vector& normal, Vector& out, float overbounce)
{
	int i = pBaseEntity->index;
	m_EntityStruct[i].angle = normal[2];

	m_EntityStruct[i].blocked = 0x00;         // Assume unblocked.
	if (m_EntityStruct[i].angle > 0)			// If the plane that is blocking us has a positive z component, then assume it's a floor.
		m_EntityStruct[i].blocked |= 0x01;	// 
	if (!m_EntityStruct[i].angle)				// If the plane has no Z, it is vertical (wall/step)
		m_EntityStruct[i].blocked |= 0x02;	// 


											// Determine how far along plane to slide based on incoming direction.
	m_EntityStruct[i].backoff = DotProduct(in, normal) * overbounce;

	for (int j = 0; j<3; j++)
	{
		m_EntityStruct[i].change = normal[j] * m_EntityStruct[i].backoff;
		out[j] = in[j] - m_EntityStruct[i].change;
	}

	// iterate once to make sure we aren't still moving through the plane
	float adjust = DotProduct(out, normal);
	if (adjust < 0.0f)
	{
		out -= (normal * adjust);
		//		Msg( "Adjustment = %lf\n", adjust );
	}

	// Return blocking flags.
	return m_EntityStruct[i].blocked;
}

int CGameMovementSimulation::TryPlayerMove(CBaseEntity *pBaseEntity, Vector *pFirstDest, trace_t *pFirstTrace)
{
	int i = pBaseEntity->index;
	m_EntityStruct[i].numbumps = 4;           // Bump up to four times

	m_EntityStruct[i].blocked = 0;           // Assume not blocked
	m_EntityStruct[i].numplanes = 0;           //  and not sliding along any planes

	VectorCopy(pBaseEntity->GetVelocity(), m_EntityStruct[i].original_velocity);  // Store original velocity
	VectorCopy(pBaseEntity->GetVelocity(), m_EntityStruct[i].primal_velocity);

	m_EntityStruct[i].allFraction = 0;
	m_EntityStruct[i].time_left = Interfaces::Globals->frametime;   // Total time for this movement operation.

	m_EntityStruct[i].new_velocity.Init();

	for (m_EntityStruct[i].bumpcount = 0; m_EntityStruct[i].bumpcount < m_EntityStruct[i].numbumps; m_EntityStruct[i].bumpcount++)
	{
		if (pBaseEntity->GetVelocity().Length() == 0.0)
			break;

		VectorMA(*pBaseEntity->GetAbsOrigin(), m_EntityStruct[i].time_left, pBaseEntity->GetVelocity(), m_EntityStruct[i].end);

		if (pFirstDest && m_EntityStruct[i].end == *pFirstDest)
			m_EntityStruct[i].pm = *pFirstTrace;
		else
		{
			TracePlayerBBox(*pBaseEntity->GetAbsOrigin(), m_EntityStruct[i].end, MASK_PLAYERSOLID, COLLISION_GROUP_PLAYER_MOVEMENT, m_EntityStruct[i].pm, pBaseEntity);
		}


		m_EntityStruct[i].allFraction += m_EntityStruct[i].pm.fraction;

		if (m_EntityStruct[i].pm.allsolid)
		{
			VectorCopy(m_EntityStruct[i].vec3_origin, pBaseEntity->GetVelocity());
			return 4;
		}

		if (m_EntityStruct[i].pm.fraction > 0)
		{
			if (m_EntityStruct[i].numbumps > 0 && m_EntityStruct[i].pm.fraction == 1)
			{
				// There's a precision issue with terrain tracing that can cause a swept box to successfully trace
				// when the end position is stuck in the triangle.  Re-run the test with an uswept box to catch that
				// case until the bug is fixed.
				// If we detect getting stuck, don't allow the movement
				trace_t stuck;
				TracePlayerBBox(m_EntityStruct[i].pm.endpos, m_EntityStruct[i].pm.endpos, MASK_PLAYERSOLID, COLLISION_GROUP_PLAYER_MOVEMENT, stuck, pBaseEntity);
				if (stuck.startsolid || stuck.fraction != 1.0f)
				{
					//Msg( "Player will become stuck!!!\n" );
					VectorCopy(m_EntityStruct[i].vec3_origin, pBaseEntity->GetVelocity());
					break;
				}
			}

			// actually covered some distance
			SetAbsOrigin(pBaseEntity, m_EntityStruct[i].pm.endpos);
			VectorCopy(pBaseEntity->GetVelocity(), m_EntityStruct[i].original_velocity);
			m_EntityStruct[i].numplanes = 0;
		}

		// If we covered the entire distance, we are done
		//  and can return.
		if (m_EntityStruct[i].pm.fraction == 1)
		{
			break;		// moved the entire distance
		}

		//MoveHelper()->AddToTouched(pm, mv->m_vecVelocity);

		// If the plane we hit has a high z component in the normal, then
		//  it's probably a floor
		if (m_EntityStruct[i].pm.plane.normal[2] > 0.7)
		{
			m_EntityStruct[i].blocked |= 1;		// floor
		}
		// If the plane has a zero z component in the normal, then it's a 
		//  step or wall
		if (!m_EntityStruct[i].pm.plane.normal[2])
		{
			m_EntityStruct[i].blocked |= 2;		// step / wall
		}

		m_EntityStruct[i].time_left -= m_EntityStruct[i].time_left * m_EntityStruct[i].pm.fraction;

		if (m_EntityStruct[i].numplanes >= 5)
		{
			VectorCopy(m_EntityStruct[i].vec3_origin, pBaseEntity->GetVelocity());


			break;
		}

		// Set up next clipping plane
		VectorCopy(m_EntityStruct[i].pm.plane.normal, m_EntityStruct[i].planes[m_EntityStruct[i].numplanes]);
		m_EntityStruct[i].numplanes++;

		int x;
		int j;
		if (m_EntityStruct[i].numplanes == 1 &&
			pBaseEntity->GetMoveType() == MOVETYPE_WALK &&
			pBaseEntity->GetFlags() & FL_ONGROUND)
		{
			for (x = 0; x < m_EntityStruct[i].numplanes; x++)
			{
				if (m_EntityStruct[i].planes[x][2] > 0.7)
				{
					// floor or slope
					ClipVelocity(pBaseEntity, m_EntityStruct[i].original_velocity, m_EntityStruct[i].planes[x], m_EntityStruct[i].new_velocity, 1);
					VectorCopy(m_EntityStruct[i].new_velocity, m_EntityStruct[i].original_velocity);
				}
				else
				{
					ClipVelocity(pBaseEntity, m_EntityStruct[i].original_velocity, m_EntityStruct[i].planes[x], m_EntityStruct[i].new_velocity, 1.0 + Interfaces::Cvar->FindVar("sv_bounce")->fValue * (1 - pBaseEntity->GetFriction()));
				}
			}

			VectorCopy(m_EntityStruct[i].new_velocity, pBaseEntity->GetVelocity());
			VectorCopy(m_EntityStruct[i].new_velocity, m_EntityStruct[i].original_velocity);
		}
		else
		{
			for (int x = 0; x < m_EntityStruct[i].numplanes; x++)
			{

				for (j = 0; j<m_EntityStruct[i].numplanes; j++)
					if (j != i)
					{
						// Are we now moving against this plane?
						if (pBaseEntity->GetVelocity().Dot(m_EntityStruct[i].planes[j]) < 0)
							break;	// not ok
					}
				if (j == m_EntityStruct[i].numplanes)  // Didn't have to clip, so we're ok
					break;
			}

			// Did we go all the way through plane set
			if (x != m_EntityStruct[i].numplanes)
			{	// go along this plane
				// pmove.velocity is set in clipping call, no need to set again.
				;
			}
			else
			{	// go along the crease
				if (m_EntityStruct[i].numplanes != 2)
				{
					VectorCopy(m_EntityStruct[i].vec3_origin, pBaseEntity->GetVelocity());
					break;
				}
				CrossProduct(m_EntityStruct[i].planes[0], m_EntityStruct[i].planes[1], m_EntityStruct[i].dir);
				m_EntityStruct[i].dir.NormalizeInPlace();
				m_EntityStruct[i].d = m_EntityStruct[i].dir.Dot(pBaseEntity->GetVelocity());
				VectorScale(m_EntityStruct[i].dir, m_EntityStruct[i].d, pBaseEntity->GetVelocity());
			}

			//
			// if original velocity is against the original velocity, stop dead
			// to avoid tiny occilations in sloping corners
			//
			m_EntityStruct[i].d = pBaseEntity->GetVelocity().Dot(m_EntityStruct[i].primal_velocity);
			if (m_EntityStruct[i].d <= 0)
			{
				//Con_DPrintf("Back\n");
				VectorCopy(m_EntityStruct[i].vec3_origin, pBaseEntity->GetVelocity());
				break;
			}
		}
	}

	if (m_EntityStruct[i].allFraction == 0)
	{
		VectorCopy(m_EntityStruct[i].vec3_origin, pBaseEntity->GetVelocity());
	}

	// Check if they slammed into a wall
	float fSlamVol = 0.0f;

	float fLateralStoppingAmount = m_EntityStruct[i].primal_velocity.Length2D() - pBaseEntity->GetVelocity().Length2D();
	if (fLateralStoppingAmount > 580 * 2.0f)
	{
		fSlamVol = 1.0f;
	}
	else if (fLateralStoppingAmount > 580)
	{
		fSlamVol = 0.85f;
	}


	return m_EntityStruct[i].blocked;
}

void CGameMovementSimulation::AirMove(CBaseEntity *pBaseEntity)
{
	int i = pBaseEntity->index;

	AngleVectors(pBaseEntity->GetEyeAngles(), &m_EntityStruct[i].forward, &m_EntityStruct[i].right, &m_EntityStruct[i].up);  // Determine movement angles

	//g_pMoveHelper->SetHost(pBaseEntity);
	m_EntityStruct[i].fmove = g_pMoveHelper->m_flForwardMove;
	m_EntityStruct[i].smove = g_pMoveHelper->m_flSideMove;
	//g_pMoveHelper->SetHost(nullptr);

	// Zero out z components of movement vectors
	m_EntityStruct[i].forward[2] = 0;
	m_EntityStruct[i].right[2] = 0;
	VectorNormalize(m_EntityStruct[i].forward);  // Normalize remainder of vectors
	VectorNormalize(m_EntityStruct[i].right);    // 

	for (int x = 0; x<2; x++)       // Determine x and y parts of velocity
		m_EntityStruct[i].wishvel[x] = m_EntityStruct[i].forward[x] * m_EntityStruct[i].fmove + m_EntityStruct[i].right[x] * m_EntityStruct[i].smove;
	m_EntityStruct[i].wishvel[2] = 0;             // Zero out z part of velocity

	VectorCopy(m_EntityStruct[i].wishvel, m_EntityStruct[i].wishdir);   // Determine maginitude of speed of move
	m_EntityStruct[i].wishspeed = VectorNormalize(m_EntityStruct[i].wishdir);

	g_pMoveHelper->SetHost(pBaseEntity);
	if (m_EntityStruct[i].wishspeed != 0 && (m_EntityStruct[i].wishspeed > g_pMoveHelper->m_flMaxSpeed))
	{
		VectorScale(m_EntityStruct[i].wishvel, g_pMoveHelper->m_flMaxSpeed / m_EntityStruct[i].wishspeed, m_EntityStruct[i].wishvel);
		m_EntityStruct[i].wishspeed = g_pMoveHelper->m_flMaxSpeed;
	}
	g_pMoveHelper->SetHost(nullptr);

	AirAccelerate(pBaseEntity, m_EntityStruct[i].wishdir, m_EntityStruct[i].wishspeed, Interfaces::Cvar->FindVar("sv_airaccelerate")->fValue);

	// Add in any base velocity to the current velocity.
	VectorAdd(pBaseEntity->GetVelocity(), pBaseEntity->GetBaseVelocity(), pBaseEntity->GetVelocity());

	TryPlayerMove(pBaseEntity, &m_EntityStruct[i].dest, &m_EntityStruct[i].trace);

	// Now pull the base velocity back out.   Base velocity is set if you are on a moving object, like a conveyor (or maybe another monster?)
	VectorSubtract(pBaseEntity->GetVelocity(), pBaseEntity->GetBaseVelocity(), pBaseEntity->GetVelocity());
}


void CGameMovementSimulation::FullWalkMove(CBaseEntity* pBaseEntity)
{
	StartGravity(pBaseEntity);

	Friction(pBaseEntity);

	CheckVelocity(pBaseEntity);

	if (pBaseEntity->GetFlags() & FL_ONGROUND)
	{
		WalkMove(pBaseEntity);

		pBaseEntity->GetVelocity().z = 0.0;
	}
	else
	{
		AirMove(pBaseEntity);
	}

	CheckVelocity(pBaseEntity);

	FinishGravity(pBaseEntity);

	CheckFalling(pBaseEntity);
}


CGameMovementSimulation* GameMovementSimulation = nullptr;
#endif