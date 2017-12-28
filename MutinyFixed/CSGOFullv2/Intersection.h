#pragma once
#include "Trace.h"

class COBB
{
public:
	Vector vecbbMin;
	Vector vecbbMax;
	matrix3x4_t *boneMatrix;
	int hitgroup;

	COBB(void) {};
	COBB(const Vector& bbMin, const Vector& bbMax, matrix3x4_t *matrix, int ihitgroup) { vecbbMin = bbMin; vecbbMax = bbMax; boneMatrix = matrix; hitgroup = ihitgroup; };
};

class CSphere
{
public:
	Vector m_vecCenter;
	float   m_flRadius = 0.f;
	//float   m_flRadius2 = 0.f; // r^2

	CSphere(void) {};
	CSphere(const Vector& vecCenter, float flRadius, int hitgroup) { m_vecCenter = vecCenter; m_flRadius = flRadius; Hitgroup = hitgroup; };

	int Hitgroup;
	bool intersectsRay(const Ray_t& ray);
	bool intersectsRay(const Ray_t& ray, Vector& vecIntersection);
};

void SetupCapsule(const Vector& vecMin, const Vector& vecMax, float flRadius, int hitgroup, std::vector<CSphere>&m_cSpheres);

bool InsersectRayWithOBB(const Ray_t& ray, Vector& vecbbMin, Vector& vecbbMax, const matrix3x4_t &boneMatrix);

//credits to http://www.scratchapixel.com/ for the nice explanation of the algorithm and
//An Efficient and Robust Ray–Box Intersection Algorithm, Amy Williams et al. 2004.
//for inventing it :D
bool IntersectRayWithAABB(Vector& origin, Vector& dir, Vector& min, Vector& max);

inline void TRACE_HITBOX(CBaseEntity* Entity, Ray_t& ray, trace_t &tr, std::vector<CSphere>&m_cSpheres, std::vector<COBB>&m_cOBBs)
{
	tr.m_pEnt = nullptr;

	for (auto& i : m_cSpheres)
	{
		if (i.intersectsRay(ray))
		{
			//Interfaces::DebugOverlay->AddBoxOverlay(intersectpos, Vector(-1, -1, -1), Vector(1, 1, 1), QAngle(0, 0, 0), 0, 255, 0, 255, 4.0f);
			tr.hitgroup = i.Hitgroup;
			tr.m_pEnt = Entity;
			return;
		}
	}

	if (!tr.m_pEnt)
	{
		for (auto& i : m_cOBBs)
		{
			if (InsersectRayWithOBB(ray, i.vecbbMin, i.vecbbMax, *i.boneMatrix))
			{
				tr.hitgroup = i.hitgroup;
				tr.m_pEnt = Entity;
				return;
			}
		}
	}
}