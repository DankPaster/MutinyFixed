#pragma once
#include "CreateMove.h"
#include "BaseEntity.h"
#include "BaseCombatWeapon.h"

//#define USE_REBUILT_HANDLE_BULLET_PENETRATION

enum
{
	CHAR_TEX_ANTLION = 'A',
	CHAR_TEX_BLOODYFLESH = 'B',
	CHAR_TEX_CONCRETE = 'C',
	CHAR_TEX_DIRT = 'D',
	CHAR_TEX_EGGSHELL = 'E',
	CHAR_TEX_FLESH = 'F',
	CHAR_TEX_GRATE = 'G',
	CHAR_TEX_ALIENFLESH = 'H',
	CHAR_TEX_CLIP = 'I',
	CHAR_TEX_PLASTIC = 'L',
	CHAR_TEX_METAL = 'M',
	CHAR_TEX_SAND = 'N',
	CHAR_TEX_FOLIAGE = 'O',
	CHAR_TEX_COMPUTER = 'P',
	CHAR_TEX_SLOSH = 'S',
	CHAR_TEX_TILE = 'T',
	CHAR_TEX_CARDBOARD = 'U',
	CHAR_TEX_VENT = 'V',
	CHAR_TEX_WOOD = 'W',
	CHAR_TEX_GLASS = 'Y',
	CHAR_TEX_WARPSHIELD = 'Z',
};

float GetHitgroupDamageMultiplier(int iHitGroup);
void FixTraceRay(Vector end, Vector start, trace_t* oldtrace, CBaseEntity* ent);
typedef bool(__fastcall* TraceToExitFn)(Vector&, trace_t&, float, float, float, float, float, float, trace_t*);
extern TraceToExitFn TraceToExitGame;
#ifndef USE_REBUILT_HANDLE_BULLET_PENETRATION
bool TraceToExitRebuilt(Vector &end, trace_t &enter_trace, Vector start, Vector dir, trace_t *exit_trace);
#else
bool trace_to_exit(Vector& end, Vector start, Vector dir, trace_t* enter_trace, trace_t* exit_trace);
#endif
//bool TraceToExit(Vector &end, trace_t &enter_trace, Vector start, Vector dir, trace_t *exit_trace);

CBaseEntity* Autowall(Vector *pTargetPos, Vector pStartingPos, QAngle *localangles, CBaseCombatWeapon *weapon, float *damage, int& hitgroup, CBaseEntity* pTarget, int TargetHitbox);

extern bool(__thiscall *HandleBulletPenetrationCSGO)(CBaseEntity *pEntityHit, float *flPenetration, int *SurfaceMaterial, char *IsSolid, trace_t *ray, Vector *vecDir,
	int unused, float flPenetrationModifier, float flDamageModifier, int unused2, int weaponmask, float flPenetration2, int *hitsleft,
	Vector *ResultPos, int unused3, int unused4, float *damage);