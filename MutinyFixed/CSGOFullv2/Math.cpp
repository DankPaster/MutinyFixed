#include "math.h"
#include <math.h>

//For extrapolation
#include "GameMemory.h"
#include "Interfaces.h"

void inline ExtrapolateOrigin(Vector* origin, Vector& targetorigin, Vector* velocity, float extraptime)
{
	if (*velocity != vecZero)
	{
		//compensate because entities are received half way through the frame loop on average
		//extrapolate for 1 tick into the future
		float ExtrapolateTime = extraptime;// +(ReadFloat((uintptr_t)&Interfaces::Globals->frametime) * 0.5f) + (ReadFloat((uintptr_t)&Interfaces::Globals->interval_per_tick));
		targetorigin.x = origin->x + velocity->x * ExtrapolateTime;
		targetorigin.y = origin->y + velocity->y * ExtrapolateTime;
		targetorigin.z = origin->z + velocity->z * ExtrapolateTime;// +9.81f * ExtrapolateTime * ExtrapolateTime / 2;
	}
	else
	{
		targetorigin = *origin;
	}
}

#if 0
void inline SinCos(float radians, float *sine, float *cosine)
{
	*sine = sinf(radians);
	*cosine = cosf(radians);
}
#endif

void VectorAngles(const Vector& forward, QAngle &angles)
{
#if 0
	if (forward[1] == 0.0f && forward[0] == 0.0f)
	{
		angles[0] = (forward[2] > 0.0f) ? 270.0f : 90.0f;
		angles[1] = 0.0f;
	}
	else
	{
		float len2d = //sqrtf(square(forward[0]) + square(forward[1]));

		angles[0] = RAD2DEG(atan2f(-forward[2], len2d));
		angles[1] = RAD2DEG(atan2f(forward[1], forward[0]));

		if (angles[0] < 0.0f) angles[0] += 360.0f;
		if (angles[1] < 0.0f) angles[1] += 360.0f;
	}

	angles[2] = 0.0f;
#endif
	float tmp, yaw, pitch;

	if (forward[2] == 0.0f && forward[0] == 0.0f)
	{
		yaw = 0;

		if (forward[2] > 0.0f)
			pitch = 90.0f;
		else
			pitch = 270.0f;
	}
	else
	{
		yaw = (atan2f(forward[1], forward[0]) * 180.0f / M_PI);

		if (yaw < 0.0f)
			yaw += 360.0f;

		float sqin = forward[0] * forward[0] + forward[1] * forward[1];
		SSESqrt(&tmp, &sqin);
		//tmp = sqrt(forward[0] * forward[0] + forward[1] * forward[1]);
		pitch = (atan2f(-forward[2], tmp) * 180.0f / M_PI);

		if (pitch < 0.0f)
			pitch += 360.0f;
	}

	NormalizeFloat(pitch);

	NormalizeFloat(yaw);

	if (pitch > 89.0f)
		pitch = 89.0f;
	else if (pitch < -89.0f)
		pitch = -89.0f;

	angles[0] = pitch;
	angles[1] = yaw;
	angles[2] = 0;
}

void VectorAngles3D(Vector& vecForward, QAngle& vecAngles)
{
	Vector vecView; ;
	if (vecForward[1] == 0.f && vecForward[0] == 0.f)
	{
		vecView[0] = 0.f; ;
		vecView[1] = 0.f; ;
	}
	else
	{
		vecView[1] = atan2(vecForward[1], vecForward[0]) * 180.f / M_PI; ;

		if (vecView[1] < 0.f)
			vecView[1] += 360; ;

		vecView[2] = sqrt(vecForward[0] * vecForward[0] + vecForward[1] * vecForward[1]); ;

		vecView[0] = atan2(vecForward[2], vecView[2]) * 180.f / M_PI; ;
	}

	vecAngles[0] = -vecView[0]; ;
	vecAngles[1] = vecView[1]; ;
	vecAngles[2] = 0.f; ;
}

void inline AngleVectors(const QAngle &angles, Vector *forward)
{
	float sp, sy, cp, cy;
	SinCos(DEG2RAD(angles.y), &sy, &cy);
	SinCos(DEG2RAD(angles.x), &sp, &cp);

	forward->x = cp*cy;
	forward->y = cp*sy;
	forward->z = -sp;
}

void AngleVectors(QAngle &angles, Vector *forward, Vector *right, Vector *up)
{
	float sr, sp, sy, cr, cp, cy;

	SinCos(DEG2RAD(angles.y), &sy, &cy);
	SinCos(DEG2RAD(angles.x), &sp, &cp);
	SinCos(DEG2RAD(angles.z), &sr, &cr);

	if (forward)
	{
		forward->x = cp*cy;
		forward->y = cp*sy;
		forward->z = -sp;
	}

	if (right)
	{
		right->x = (-1 * sr*sp*cy + -1 * cr*-sy);
		right->y = (-1 * sr*sp*sy + -1 * cr*cy);
		right->z = -1 * sr*cp;
	}

	if (up)
	{
		up->x = (cr*sp*cy + -sr*-sy);
		up->y = (cr*sp*sy + -sr*cy);
		up->z = cr*cp;
	}
}

float GetFov(const QAngle& viewAngle, const QAngle& aimAngle)
{
	Vector ang, aim;

	AngleVectors(viewAngle, &aim);
	AngleVectors(aimAngle, &ang);

	return RAD2DEG(acos(aim.Dot(ang) / aim.LengthSqr()));
}

float GetFovRegardlessOfDistance(const QAngle& viewAngle, const QAngle& aimAngle, float Distance)
{
	Vector ang, aim;

	AngleVectors(viewAngle, &aim);
	AngleVectors(aimAngle, &ang);

	return sin(acos(aim.Dot(ang) / aim.LengthSqr())) * Distance;
}

float GetFov(Vector source, Vector destination, QAngle angles)
{
	Vector forward;
	QAngle ang = CalcAngle(source, destination);
	AngleVectors(ang, &forward);

	Vector forward2;
	AngleVectors(angles, &forward2);

	return (acosf(forward2.Dot(forward) / std::pow(forward2.Length(), 2.0f)) * (180.0f / M_PI));
}

// fix aimbot issues using wrong math funcs in calcangle
QAngle CalcAngle(Vector src, Vector dst)
{
	Vector delta = src - dst;
	if (delta == vecZero)
	{
		return angZero;
	}

	float len = delta.Length();

	if (delta.z == 0.0f && len == 0.0f)
		return angZero;

	if (delta.y == 0.0f && delta.x == 0.0f)
		return angZero;

	QAngle angles;
	angles.x = (asinf(delta.z / delta.Length()) * M_RADPI);
	angles.y = (atanf(delta.y / delta.x) * M_RADPI);
	angles.z = 0.0f;
	if (delta.x >= 0.0f) { angles.y += 180.0f; }

	angles.Clamp();

	return angles;
}

float GetFovFromCoords(const Vector& from, const Vector& to, const QAngle& anglelooking)
{
	// Convert angles to normalized directional forward vector
	Vector Forward;
	AngleVectors(anglelooking, &Forward);

	// Get delta vector between our local eye position and passed vector
	Vector Delta = to - from;

	// Normalize our delta vector
	VectorNormalize(Delta);

	// Get dot product between delta position and directional forward vectors
	float DotProduct = Forward.Dot(Delta);

	// Time to calculate the field of view
	return RAD2DEG(acos(DotProduct));
}

//FOV is scaled based off distance
float GetFovFromCoordsRegardlessofDistance(const Vector& from, const Vector& to, const QAngle& anglelooking)
{
	// Convert angles to normalized directional forward vector
	Vector Forward;
	AngleVectors(anglelooking, &Forward);

	// Get delta vector between our local eye position and passed vector
	Vector Delta = to - from;

	float Distance = Delta.Length();

	// Normalize our delta vector
	VectorNormalize(Delta);

	// Get dot product between delta position and directional forward vectors
	float DotProduct = Forward.Dot(Delta);

	// Time to calculate the field of view
	return RAD2DEG(sin(DEG2RAD(acos(DotProduct))) * Distance);
}


float VectorDistance(Vector v1, Vector v2)
{
	float sqin = powf(v1.x - v2.x, 2) + powf(v1.y - v2.y, 2) + powf(v1.z - v2.z, 2);
	float sqout;
	SSESqrt(&sqout, &sqin);
	return sqout; //FASTSQRT(pow(v1.x - v2.x, 2) + pow(v1.y - v2.y, 2) + pow(v1.z - v2.z, 2));
}

void VectorTransform(Vector& in1, matrix3x4a_t& in2, Vector &out)
{
	out.x = in1.Dot(in2.m_flMatVal[0]) + in2.m_flMatVal[0][3];
	out.y = in1.Dot(in2.m_flMatVal[1]) + in2.m_flMatVal[1][3];
	out.z = in1.Dot(in2.m_flMatVal[2]) + in2.m_flMatVal[2][3];
}

float DotProductT(const float* a, const float* b)
{
	return (a[0] * b[0] + a[1] * b[1] + a[2] * b[2]);
}

void VectorTransformA(const float *in1, const matrix3x4_t& in2, float *out)
{

	out[0] = DotProductT(in1, in2[0]) + in2[0][3];
	out[1] = DotProductT(in1, in2[1]) + in2[1][3];
	out[2] = DotProductT(in1, in2[2]) + in2[2][3];
}


void inline VectorTransformZ(const Vector& in1, const matrix3x4_t &in2, Vector &out)
{
	VectorTransformA(&in1.x, in2, &out.x);
}

float GetDelta(float hspeed, float maxspeed, float airaccelerate)
{
	float term = (30.0f - (airaccelerate * maxspeed / 66.0f)) / hspeed;

	if (term < 1.0f && term > -1.0f) {
		return acosf(term);
	}

	return 0.f;
}

inline float RandFloat(float M, float N)
{
	return (float)(M + (rand() / (RAND_MAX / (N - M))));
}

inline Vector ExtrapolateTick(Vector p0, Vector v0)
{
	return vecZero; //dylan fix
	//return p0 + (v0 * I::Globals->interval_per_tick);
}