#pragma once
#include "misc.h"
#include <string>
#include "NetVarManager.h"
#include "utlvectorsimple.h"
#include <vector>


class INetChannelInfo;
class ICollideable;
class CBaseAnimating;
class CBaseCombatWeapon;
class C_AnimationLayer;

extern bool* s_bEnableInvalidateBoneCache;

// edict->solid values
// NOTE: Some movetypes will cause collisions independent of SOLID_NOT/SOLID_TRIGGER when the entity moves
// SOLID only effects OTHER entities colliding with this one when they move - UGH!

// Solid type basically describes how the bounding volume of the object is represented
// NOTE: SOLID_BBOX MUST BE 2, and SOLID_VPHYSICS MUST BE 6
// NOTE: These numerical values are used in the FGD by the prop code (see prop_dynamic)
enum SolidType_t
{
	SOLID_NONE = 0,	// no solid model
	SOLID_BSP = 1,	// a BSP tree
	SOLID_BBOX = 2,	// an AABB
	SOLID_OBB = 3,	// an OBB (not implemented yet)
	SOLID_OBB_YAW = 4,	// an OBB, constrained so that it can only yaw
	SOLID_CUSTOM = 5,	// Always call into the entity for tests
	SOLID_VPHYSICS = 6,	// solid vphysics object, get vcollide from the model and collide with that
	SOLID_LAST,
};

enum MoveType_t
{
	MOVETYPE_NONE = 0,
	MOVETYPE_ISOMETRIC,
	MOVETYPE_WALK,
	MOVETYPE_STEP,
	MOVETYPE_FLY,
	MOVETYPE_FLYGRAVITY,
	MOVETYPE_VPHYSICS,
	MOVETYPE_PUSH,
	MOVETYPE_NOCLIP,
	MOVETYPE_LADDER,
	MOVETYPE_OBSERVER,
	MOVETYPE_CUSTOM,
	MOVETYPE_LAST = MOVETYPE_CUSTOM,
	MOVETYPE_MAX_BITS = 4
};

struct CustomPlayer;

class CBaseEntity
{
public:
	char				__pad[0x64];
	int					index;
	CustomPlayer* CBaseEntity::ToCustomPlayer();
	int					GetHealth();
	int					GetTeam();
	int					GetFlags();
	void SetFlags(int flags);
	int					GetTickBase();
	void SetTickBase(int base);
	int					GetShotsFired();
	void SetMoveType(int type);
	int					GetMoveType();
	int					GetModelIndex();
	void SetModelIndex(int index);
	int					GetHitboxSet();
	void SetHitboxSet(int set);
	int					GetUserID();
	int					GetArmor();
	int					GetCollisionGroup();
	int					PhysicsSolidMaskForEntity();
	CBaseEntity*		GetOwner();
	int					GetGlowIndex();
	BOOL				GetAlive();
	BOOLEAN				GetAliveServer();
	void CalcAbsolutePositionServer();
	BOOLEAN				GetDormant();
	BOOLEAN				GetImmune();
	bool				IsEnemy();
	bool				IsVisible(int bone);
	bool				m_visible;
	BOOLEAN				IsBroken();
	BOOLEAN				HasHelmet();
	BOOLEAN HasDefuseKit();
	BOOLEAN IsDefusing();
	BOOLEAN				IsFlashed();
	bool				IsTargetingLocal();
	float				GetFlashDuration();
	void SetFlashDuration(float dur);
	float				GetBombTimer();
	QAngle				GetViewPunch();
	void SetViewPunch(QAngle punch);
	QAngle				GetPunch();
	void SetPunch(QAngle punch);
	Vector              GetPunchVel();
	void SetPunchVel(Vector vel);
	QAngle				GetEyeAngles();
	QAngle				GetEyeAnglesServer();
	QAngle* EyeAngles();
	void SetEyeAngles(QAngle angles);
	QAngle GetRenderAngles();
	void SetRenderAngles(QAngle angles);
	Vector				GetOrigin();
	Vector				GetNetworkOrigin();
	Vector GetOldOrigin();
	void SetOrigin(Vector origin);
	void SetNetworkOrigin(Vector origin);
	void SetAbsOrigin(const Vector &origin);
	QAngle GetAbsAngles();
	QAngle GetAbsAnglesServer();
	Vector WorldSpaceCenter();
	Vector				GetEyePosition();
	SolidType_t         GetSolid();
	BOOLEAN ShouldCollide(int collisionGroup, int contentsMask);
	BOOLEAN IsTransparent();
	Vector GetBonePosition(int HitboxID, float time, bool ForceSetup, bool StoreInCache);
	Vector GetBonePositionTotal(int HitboxID, float time, Vector &BoneMax, Vector &BoneMin, bool ForceSetup, bool StoreInCache);
	Vector GetBonePositionCachedOnly(int HitboxID, float time, matrix3x4_t* matrixes);
	void GetBonePosition(int iBone, Vector &origin, QAngle &angles, float time, bool ForceSetup, bool StoreInCache);
	bool CacheBones();
	bool				SetupBones(matrix3x4_t *pBoneToWorldOut, int nMaxBones, int boneMask, float currentTime);
	Vector GetVelocity();
	void SetVelocity(Vector velocity);
	void SetAbsVelocity(Vector velocity);
	Vector GetBaseVelocity();
	void SetBaseVelocity(Vector velocity);
	Vector				GetPredicted(Vector p0);
	ICollideable*		GetCollideable();
	void		GetPlayerInfo(player_info_t *dest);
	model_t*			GetModel();
	void SetModel(model_t* mod);
	std::string			GetName();
	void			GetSteamID(char* dest);
	std::string			GetLastPlace();
	CBaseCombatWeapon*	GetWeapon();
	ClientClass*		GetClientClass();
	CBaseAnimating* GetBaseAnimating();
	void InvalidateBoneCache();
	HANDLE GetObserverTarget();
	BOOLEAN IsPlayer();
	CUserCmd* GetLastUserCommand();
	BOOLEAN IsConnected();
	BOOLEAN IsSpawned();
	BOOLEAN IsActive();
	BOOLEAN IsFakeClient();
	bool IsTargetingLocalPunch();
	QAngle GetAngleRotation();
	void SetAngleRotation(QAngle rot);
	void SetAbsAngles(const QAngle& rot);
	Vector GetMins();
	void SetMins(Vector mins);
	Vector GetMaxs();
	void SetMaxs(Vector maxs);
	int GetSequence();
	void SetSequence(int seq);
	void GetPoseParameterRange(int index, float& flMin, float& flMax);
	float GetPoseParameter(int index);
	float GetPoseParameterUnscaled(int index);
	float GetPoseParameterUnScaledServer(int index);
	int LookupPoseParameter(char *name);
	void SetPoseParameter(int index, float p);
	void SetPoseParameterScaled(int index, float p);
	void CopyPoseParameters(float* dest);
	unsigned char GetClientSideAnimation();
	void SetClientSideAnimation(unsigned char a);
	float GetCycle();
	void SetCycle(float cycle);
	float GetNextAttack();
	void SetNextAttack(float att);
	BOOLEAN IsDucked();
	void SetDucked(bool ducked);
	BOOLEAN IsDucking();
	void SetDucking(bool ducking);
	BOOLEAN IsInDuckJump();
	void SetInDuckJump(bool induckjump);
	int GetDuckTimeMsecs();
	void SetDuckTimeMsecs(int msecs);
	int GetJumpTimeMsecs();
	void SetJumpTimeMsecs(int msecs);
	float GetFallVelocity();
	void SetFallVelocity(float vel);
	Vector GetViewOffset();
	void SetViewOffset(Vector off);
	int GetNextThinkTick();
	void SetNextThinkTick(int tick);
	float GetDuckAmount();
	void SetDuckAmount(float duckamt);
	float GetDuckSpeed();
	void SetDuckSpeed(float spd);
	float GetVelocityModifier();
	void SetVelocityModifier(float vel);
	float GetPlaybackRate();
	void SetPlaybackRate(float rate);
	float GetAnimTime();
	void SetAnimTime(float time);
	float GetSimulationTime();
	float GetSimulationTimeServer();
	float GetOldSimulationTime();
	void SetSimulationTime(float time);
	float GetLaggedMovement();
	void SetLaggedMovement(float mov);
	CBaseEntity* GetGroundEntity();
	void SetGroundEntity(CBaseEntity* groundent);
	Vector GetVecLadderNormal();
	void SetVecLadderNormal(Vector norm);
	float GetLowerBodyYaw();
	float GetLowerBodyYawServer();
	void SetLowerBodyYaw(float yaw);
	void* GetPlayerAnimState();
	void* GetPlayerAnimStateServer();
	bool IsWeapon();
	bool IsProjectile();
	bool IsFlashGrenade();
	bool IsChicken();
	void DrawHitboxes(ColorRGBA color, float livetimesecs);
	void DrawHitboxesFromCache(ColorRGBA color, float livetimesecs, matrix3x4_t *matrix);
	INetChannelInfo* GetNetChannel();
	void DisableInterpolation();
	void EnableInterpolation();
	void UpdateClientSideAnimation();
	float GetLastClientSideAnimationUpdateTime();
	void SetLastClientSideAnimationUpdateTime(float time);
	int GetLastClientSideAnimationUpdateGlobalsFrameCount();
	void SetLastClientSideAnimationUpdateGlobalsFrameCount(int framecount);
	float FrameAdvance(float flInterval);
	int GetEffects();
	void SetEffects(int effects);
	int GetObserverMode();
	void SetObserverMode(int mode);
	CUtlVectorSimple* GetAnimOverlayStruct() const;
	C_AnimationLayer* GetAnimOverlay(int i);
	int GetNumAnimOverlays() const;
	void CopyAnimLayers(C_AnimationLayer* dest);
	void InvalidatePhysicsRecursive(int nChangeFlags);
	Vector* GetAbsOrigin();
	Vector GetAbsOriginServer();
	int entindex();
	int entindexServer();
	float GetCurrentFeetYawServer();
	float GetCurrentFeetYaw();
	void SetCurrentFeetYaw(float yaw);
	float GetGoalFeetYawServer();
	float GetGoalFeetYaw();
	void SetGoalFeetYaw(float yaw);
	float GetFriction();
	void SetFriction(float friction);
	float GetStepSize();
	void SetStepSize(float stepsize);
	float GetMaxSpeed();
	void SetMaxSpeed(float maxspeed);
	bool IsParentChanging();
	void SetLocalVelocity(const Vector& vecVelocity);
	int GetTakeDamage() { return *(DWORD*)((DWORD)this + 0x27C); }
	inline const char* GetClassname() {
		return ((const char*(__thiscall*)(CBaseEntity*)) *(DWORD*)(*(DWORD*)this + 0x22C) )(this);
	}
	inline int GetUnknownInt()
	{
		return (*(int(__thiscall **)(CBaseEntity*))(*(DWORD *)this + 0x1E8))(this);
	}
};

class VTHook;

struct HookedEntity
{
	int index;
	CBaseEntity* entity;
	VTHook* hook;
	DWORD OriginalHookedSub1;
};


#if 0
extern std::vector<HookedEntity*> HookedEntities;

void DestructHookedEntity(CBaseEntity* me);
void DeleteEntityHook(VTHook* hook);
BOOLEAN /*_fastcall*/ HookedEntityShouldInterpolate(CBaseEntity* me);
void HookAllBaseEntityNonLocalPlayers();
void ValidateAllEntityHooks();
#endif