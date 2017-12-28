#pragma once
#include "CSGO_HX.h"
#include "CreateMove.h"
#include "ClassIDS.h"

class CGrenadePredictTraceFilter
{
public:
	virtual bool ShouldHitEntity(IHandleEntity* pEntityHandle, int contentsMask)
	{
		if (!pEntityHandle)
			return BaseShouldHitEntity(pSkip, (IHandleEntity*)pEntityHandle, m_icollisionGroup, contentsMask);


		//if (contentsMask & CONTENTS_WINDOW)
		//	return false;

		if (pEntityHandle == pSkip)
			return false;
		if (((CBaseEntity*)pEntityHandle)->IsPlayer())
			return false;
		ClientClass *clclass = ((CBaseEntity*)pEntityHandle)->GetClientClass();
		if (clclass)
		{
			if (ReadInt((uintptr_t)&clclass->m_ClassID) == ClassID::_CBaseCSGrenadeProjectile)
				return false;
		}
		return BaseShouldHitEntity(pSkip, (IHandleEntity*)pEntityHandle, m_icollisionGroup, contentsMask);
	}
	virtual TraceType_t	GetTraceType() const
	{
		return TRACE_EVERYTHING;
	}
	IHandleEntity* pSkip;
	int m_icollisionGroup;
};

class CTraceFilterIgnoreGrenades
{
public:
	virtual bool ShouldHitEntity(IHandleEntity* pEntityHandle, int contentsMask)
	{
		if (!pEntityHandle)
			return BaseShouldHitEntity(pSkip, (IHandleEntity*)pEntityHandle, m_icollisionGroup, contentsMask);

		ClientClass *clclass = ((CBaseEntity*)pEntityHandle)->GetClientClass();
		if (clclass)
		{
			if (ReadInt((uintptr_t)&clclass->m_ClassID) == ClassID::_CBaseCSGrenadeProjectile)
				return false;
		}
		return BaseShouldHitEntity(pSkip, (IHandleEntity*)pEntityHandle, m_icollisionGroup, contentsMask);
	}
	virtual TraceType_t	GetTraceType() const
	{
		return TRACE_EVERYTHING;
	}
	IHandleEntity* pSkip;
	int m_icollisionGroup;
};

void Tick(int buttons);
void PredictGrenade();
void PaintGrenadeTrajectory();
void Setup(CBasePlayer* pl, Vector& vecSrc, Vector& vecThrow, const QAngle& angEyeAngles);
void Simulate();
int Step(Vector& vecSrc, Vector& vecThrow, int tick, float interval);
bool CheckDetonate(const Vector& vecThrow, const trace_t& tr, int tick, float interval);
void AddGravityMove(Vector& move, Vector& vel, float frametime, bool onground);
void PushEntity(Vector& src, const Vector& move, trace_t& tr);
void ResolveFlyCollisionCustom(trace_t& tr, Vector& vecVelocity, float interval);
unsigned char PhysicsClipVelocity(const Vector& in, const Vector& normal, Vector& out, float overbounce);
