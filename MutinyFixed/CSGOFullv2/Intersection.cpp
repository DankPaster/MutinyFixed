#include "Intersection.h"
#include "Math.h"

//-----------------------------------------------------------------------------
//
// IntersectInfiniteRayWithSphere
//
// Returns whether or not there was an intersection. 
// Returns the two intersection points
//
//-----------------------------------------------------------------------------
bool IntersectInfiniteRayWithSphere(const Vector &vecRayOrigin, const Vector &vecRayDelta,
	const Vector &vecSphereCenter, float flRadius, float *pT1, float *pT2)
{
	// Solve using the ray equation + the sphere equation
	// P = o + dt
	// (x - xc)^2 + (y - yc)^2 + (z - zc)^2 = r^2
	// (ox + dx * t - xc)^2 + (oy + dy * t - yc)^2 + (oz + dz * t - zc)^2 = r^2
	// (ox - xc)^2 + 2 * (ox-xc) * dx * t + dx^2 * t^2 +
	//		(oy - yc)^2 + 2 * (oy-yc) * dy * t + dy^2 * t^2 +
	//		(oz - zc)^2 + 2 * (oz-zc) * dz * t + dz^2 * t^2 = r^2
	// (dx^2 + dy^2 + dz^2) * t^2 + 2 * ((ox-xc)dx + (oy-yc)dy + (oz-zc)dz) t +
	//		(ox-xc)^2 + (oy-yc)^2 + (oz-zc)^2 - r^2 = 0
	// or, t = (-b +/- sqrt( b^2 - 4ac)) / 2a
	// a = DotProduct( vecRayDelta, vecRayDelta );
	// b = 2 * DotProduct( vecRayOrigin - vecCenter, vecRayDelta )
	// c = DotProduct(vecRayOrigin - vecCenter, vecRayOrigin - vecCenter) - flRadius * flRadius;

	Vector vecSphereToRay;
	VectorSubtract(vecRayOrigin, vecSphereCenter, vecSphereToRay);

	float a = DotProduct(vecRayDelta, vecRayDelta);

	// This would occur in the case of a zero-length ray
	if (a == 0.0f)
	{
		*pT1 = *pT2 = 0.0f;
		return vecSphereToRay.LengthSqr() <= flRadius * flRadius;
	}

	float b = 2 * DotProduct(vecSphereToRay, vecRayDelta);
	float c = DotProduct(vecSphereToRay, vecSphereToRay) - flRadius * flRadius;
	float flDiscrim = b * b - 4 * a * c;
	if (flDiscrim < 0.0f)
		return false;

	flDiscrim = sqrt(flDiscrim);
	float oo2a = 0.5f / a;
	*pT1 = (-b - flDiscrim) * oo2a;
	*pT2 = (-b + flDiscrim) * oo2a;
	return true;
}

//-----------------------------------------------------------------------------
//
// IntersectRayWithSphere
//
// Returns whether or not there was an intersection. 
// Returns the two intersection points, clamped to (0,1)
//
//-----------------------------------------------------------------------------
bool CSphere::intersectsRay(const Ray_t& ray, Vector& vecIntersection)
{
	float T1, T2;
	if (!IntersectInfiniteRayWithSphere(ray.m_Start, ray.m_Delta, m_vecCenter, m_flRadius, &T1, &T2))
		return false;

	if (T1 > 1.0f || T2 < 0.0f)
		return false;

	// Clamp it!
	if (T1 < 0.0f)
		T1 = 0.0f;
	if (T2 > 1.0f)
		T2 = 1.0f;

	vecIntersection = ray.m_Start + ray.m_Delta * T1;

	return true;
}

bool CSphere::intersectsRay(const Ray_t& ray)
{
	float T1, T2;
	if (!IntersectInfiniteRayWithSphere(ray.m_Start, ray.m_Delta, m_vecCenter, m_flRadius, &T1, &T2))
		return false;

	return T2 >= 0.0f && T1 <= 1.0f;
}

void SetupCapsule(const Vector& vecMin, const Vector& vecMax, float flRadius, int hitgroup, std::vector<CSphere>&m_cSpheres)
{
	auto vecDelta = (vecMax - vecMin);
	VectorNormalizeFast(vecDelta);
	auto vecCenter = vecMin;

	m_cSpheres.push_back(CSphere{ vecMin, flRadius, hitgroup });

	for (size_t i = 1; i < std::floor(vecMin.Dist(vecMax)); ++i) {
		m_cSpheres.push_back(CSphere{ vecMin + vecDelta * static_cast< float >(i), flRadius, hitgroup });
	}

	m_cSpheres.push_back(CSphere{ vecMax, flRadius, hitgroup });
}

bool IntersectRayWithAABB(Vector& origin, Vector& dir, Vector& min, Vector& max)
{
	float tmin, tmax, tymin, tymax, tzmin, tzmax;

	if (dir.x >= 0)
	{
		tmin = (min.x - origin.x) / dir.x;
		tmax = (max.x - origin.x) / dir.x;
	}
	else
	{
		tmin = (max.x - origin.x) / dir.x;
		tmax = (min.x - origin.x) / dir.x;
	}

	if (dir.y >= 0)
	{
		tymin = (min.y - origin.y) / dir.y;
		tymax = (max.y - origin.y) / dir.y;
	}
	else
	{
		tymin = (max.y - origin.y) / dir.y;
		tymax = (min.y - origin.y) / dir.y;
	}

	if (tmin > tymax || tymin > tmax)
		return false;

	if (tymin > tmin)
		tmin = tymin;

	if (tymax < tmax)
		tmax = tymax;

	if (dir.z >= 0)
	{
		tzmin = (min.z - origin.z) / dir.z;
		tzmax = (max.z - origin.z) / dir.z;
	}
	else
	{
		tzmin = (max.z - origin.z) / dir.z;
		tzmax = (min.z - origin.z) / dir.z;
	}

	if (tmin > tzmax || tzmin > tmax)
		return false;

	//behind us
	if (tmin < 0 || tmax < 0)
		return false;

	return true;
}

void VectorITransform(const Vector *in1, const matrix3x4_t& in2, Vector *out)
{
	Assert(s_bMathlibInitialized);
	float in1t[3];

	in1t[0] = in1->x - in2[0][3];
	in1t[1] = in1->y - in2[1][3];
	in1t[2] = in1->z - in2[2][3];

	out->x = in1t[0] * in2[0][0] + in1t[1] * in2[1][0] + in1t[2] * in2[2][0];
	out->y = in1t[0] * in2[0][1] + in1t[1] * in2[1][1] + in1t[2] * in2[2][1];
	out->z = in1t[0] * in2[0][2] + in1t[1] * in2[1][2] + in1t[2] * in2[2][2];
}

// assume in2 is a rotation and rotate the input vector
void VectorIRotate(const Vector *in1, const matrix3x4_t& in2, Vector *out)
{
	Assert(s_bMathlibInitialized);
	Assert(in1 != out);
	out->x = DotProduct((const float*)in1, in2[0]);
	out->y = DotProduct((const float*)in1, in2[1]);
	out->z = DotProduct((const float*)in1, in2[2]);
}

bool InsersectRayWithOBB(const Ray_t& ray, Vector& vecbbMin, Vector& vecbbMax, const matrix3x4_t &boneMatrix)
{
	//Transform ray into model space of hitbox so we only have to deal with an AABB instead of OBB
	Vector ray_trans, dir_trans;
	VectorITransform(&ray.m_Start, boneMatrix, &ray_trans);
	VectorIRotate(&ray.m_Delta, boneMatrix, &dir_trans); //only rotate direction vector! no translation!

	return IntersectRayWithAABB(ray_trans, dir_trans, vecbbMin, vecbbMax);
}