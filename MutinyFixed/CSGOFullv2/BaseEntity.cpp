#include <Windows.h>
#include "BaseEntity.h"
#include "Offsets.h"
#include "VTHook.h"
#include "Trace.h"
#include "Math.h"
#include "Interfaces.h"
#include "Overlay.h"
#include "BaseAnimating.h"
#include "Interpolation.h"
#include "Animation.h"
#include "LocalPlayer.h"

bool* s_bEnableInvalidateBoneCache;

#if 0
std::vector<HookedEntity*> HookedEntities;

//FIXME: figure out how to hook ~C_BaseEntity
void DestructHookedEntity(CBaseEntity* me)
{
	for (auto hooked = HookedEntities.begin(); hooked != HookedEntities.end(); hooked++)
	{
		if ((*hooked)->entity != me)
			continue;
		(*hooked)->hook->ClearClassBase();
		delete (*hooked)->hook;
		/*hooked = */HookedEntities.erase(hooked);
		return;
	}
}

void ValidateAllEntityHooks()
{

	for (auto hooked = HookedEntities.begin(); hooked != HookedEntities.end(); hooked++)
	{
		CBaseEntity *entity = Interfaces::ClientEntList->GetClientEntity((*hooked)->index);
		if (entity != (*hooked)->entity)
		{
			(*hooked)->hook->ClearClassBase();
			delete (*hooked)->hook;
			hooked = HookedEntities.erase(hooked);
		}
	}
}

void DeleteEntityHook(VTHook* hook)
{
	for (auto hooked = HookedEntities.begin(); hooked != HookedEntities.end(); hooked++)
	{
		if ((*hooked)->hook != hook)
			continue;
		delete (*hooked)->hook;
		HookedEntities.erase(hooked);
		return;
	}
}

BOOLEAN __declspec (naked) HookedEntityShouldInterpolate(CBaseEntity* me)/*BOOLEAN _fastcall HookedEntityShouldInterpolate(CBaseEntity* me)*/
{
	__asm {
		xor al, al
		retn
	}
	//if (me == LocalPlayer.Entity)
	//	return 1;
	//return 0;
}

void HookAllBaseEntityNonLocalPlayers()
{
	for (int i = 0; i <= MAX_PLAYERS; i++)
	{
		CBaseEntity* Entity = Interfaces::ClientEntList->GetClientEntity(i);
		if (Entity && Entity->IsPlayer() && Entity != LocalPlayer.Entity)
		{
			CustomPlayer *pCAPlayer = &AllPlayers[Entity->index];
			if (!pCAPlayer->bHookedBaseEntity)
			{
				pCAPlayer->HookedBaseEntity = new HookedEntity;
				pCAPlayer->HookedBaseEntity->entity = Entity;
				pCAPlayer->HookedBaseEntity->index = Entity->index;
				pCAPlayer->HookedBaseEntity->hook = new VTHook((PDWORD*)Entity);
				//pCAPlayer->HookedBaseEntity->OriginalHookedSub1 = pCAPlayer->HookedBaseEntity->hook->HookFunction((DWORD)&HookedEntityShouldInterpolate, ShouldInterpolateIndex);
				pCAPlayer->bHookedBaseEntity = true;
				HookedEntities.push_back(pCAPlayer->HookedBaseEntity);
			}
		}
	}
}
#endif

CustomPlayer* CBaseEntity::ToCustomPlayer()
{
	return &AllPlayers[index];
}

int	CBaseEntity::GetHealth()
{
	return *reinterpret_cast<int*>((DWORD)this + m_iHealth);
}

int CBaseEntity::GetTeam()
{
	return ReadInt((DWORD)this + m_iTeamNum);//*(int*)((DWORD)this + m_iTeamNum);
}

int CBaseEntity::GetFlags()
{
	return ReadInt((DWORD)this + m_fFlags);//*(int*)((DWORD)this + m_fFlags);
}

void CBaseEntity::SetFlags(int flags)
{
	WriteInt((DWORD)this + m_fFlags, flags);
}

int CBaseEntity::GetTickBase()
{
	return ReadInt((DWORD)this + m_nTickBase);//*(int*)((DWORD)this + m_nTickBase);
}

void CBaseEntity::SetTickBase(int base)
{
	WriteInt((DWORD)this + m_nTickBase, base);
}

int CBaseEntity::GetShotsFired()
{
	return ReadInt((DWORD)this + m_iShotsFired);//*(int*)((DWORD)this + m_iShotsFired);
}

int CBaseEntity::GetMoveType()
{
	return ReadInt((DWORD)this + m_nMoveType);//*(int*)((DWORD)this + m_nMoveType);
}

void CBaseEntity::SetMoveType(int type)
{
	WriteInt((DWORD)this + m_nMoveType, type);
}

int CBaseEntity::GetModelIndex()
{
	return ReadInt((DWORD)this + m_nModelIndex);// *(int*)((DWORD)this + m_nModelIndex);
}

void CBaseEntity::SetModelIndex(int index)
{
	WriteInt((DWORD)this + m_nModelIndex, index);
}

int CBaseEntity::GetHitboxSet()
{
	return ReadInt((DWORD)this + m_nHitboxSet);//*(int*)((DWORD)this + m_nHitboxSet);
}

void CBaseEntity::SetHitboxSet(int set)
{
	WriteInt((DWORD)this + m_nHitboxSet, set);
}

int CBaseEntity::GetUserID()
{
	player_info_t info;
	GetPlayerInfo(&info);
	return info.userid; //this->GetPlayerInfo().userid; //DYLAN FIX
}

int CBaseEntity::GetArmor()
{
	return ReadInt((DWORD)this + m_ArmorValue);//*(int*)((DWORD)this + m_ArmorValue);
}

int CBaseEntity::PhysicsSolidMaskForEntity()
{
	typedef unsigned int(__thiscall* OriginalFn)(void*);
	return GetVFunc<OriginalFn>(this, 148)(this); //154
}

CBaseEntity* CBaseEntity::GetOwner()
{
	DWORD Handle = ReadInt((DWORD)this + m_hOwnerEntity);
	return (CBaseEntity*)Interfaces::ClientEntList->GetClientEntityFromHandle(Handle);
}

int CBaseEntity::GetGlowIndex()
{
	return ReadInt((DWORD)this + m_iGlowIndex);//*(int*)((DWORD)this + m_iGlowIndex);
}

float CBaseEntity::GetBombTimer()
{
	return 0.0f;//dylan fix
	//float bombTime = *(float*)((DWORD)this + m_flC4Blow);
	//float returnValue = bombTime - Interfaces::Globals->curtime;
	//return (returnValue < 0) ? 0.f : returnValue;
}

float CBaseEntity::GetFlashDuration()
{
	return ReadFloat((DWORD)this + m_flFlashDuration);//*(float*)((DWORD)this + m_flFlashDuration);
}

void CBaseEntity::SetFlashDuration(float dur)
{
	WriteFloat((DWORD)this + m_flFlashDuration, dur);
}

BOOLEAN CBaseEntity::IsFlashed()
{
	return (BOOLEAN)GetFlashDuration() > 0 ? true : false;
}

BOOL CBaseEntity::GetAlive()
{
	return (bool)(*(int*)((DWORD)this + m_lifeState) == 0);
}

BOOLEAN CBaseEntity::GetAliveServer()
{
	typedef BOOLEAN(__thiscall* OriginalFn)(CBaseEntity*);
	return GetVFunc<OriginalFn>(this, (0x114 / 4))(this);
}

void CBaseEntity::CalcAbsolutePositionServer()
{
#ifdef _DEBUG
	static DWORD absposfunc = NULL;
	if (!absposfunc)
	{
		const char* sig = "55  8B  EC  83  E4  F0  83  EC  68  56  8B  F1  57  8B  8E  D0  00  00  00";
		absposfunc = FindMemoryPattern(GetModuleHandleA("server.dll"), (char*)sig, strlen(sig));
		if (!absposfunc)
			DebugBreak();
	}
	((void(__thiscall*)(CBaseEntity*))absposfunc)(this);
#endif
}

BOOLEAN CBaseEntity::GetDormant()
{
	return (BOOLEAN)ReadByte((DWORD)this + m_bDormant);//*(bool*)((DWORD)this + m_bDormant);
}

BOOLEAN CBaseEntity::GetImmune()
{
	return (BOOLEAN)ReadByte((DWORD)this + m_bGunGameImmunity);//*(bool*)((DWORD)this + m_bGunGameImmunity);
}

BOOLEAN CBaseEntity::HasHelmet()
{
	return (BOOLEAN)ReadByte((DWORD)this + m_bHasHelmet);//*(bool*)((DWORD)this + m_bHasHelmet);
}

BOOLEAN CBaseEntity::HasDefuseKit()
{
	return (BOOLEAN)ReadInt((DWORD)this + m_bHasDefuser);
}

BOOLEAN CBaseEntity::IsDefusing()
{
	return (BOOLEAN)ReadInt((DWORD)this + m_bIsDefusing);
}

bool CBaseEntity::IsTargetingLocal()
{
	Vector src, dst, forward;
	trace_t tr;

	QAngle viewangle = this->GetEyeAngles();

	AngleVectors(viewangle, &forward);
	VectorNormalizeFast(forward);
	forward *= 8142.f;
	src = this->GetEyePosition();
	dst = src + forward;

	Ray_t ray;
	ray.Init(src, dst);

	CTraceFilterPlayersOnlyNoWorld filter;
	filter.AllowTeammates = true;
	filter.pSkip = (IHandleEntity*)this;

	Interfaces::EngineTrace->TraceRay(ray, MASK_SHOT, &filter, &tr);

	//UTIL_TraceLine(src, dst, MASK_SHOT, this, &tr);

	if (tr.m_pEnt == LocalPlayer.Entity)
		return true;

	return false;
}

model_t* CBaseEntity::GetModel()
{
	return (model_t*)ReadInt((DWORD)this + 0x6C); //DYLAN TEST THIS  //*(model_t**)((DWORD)this + 0x6C);
}

void CBaseEntity::SetModel(model_t* mod)
{
	WriteInt((DWORD)this + 0x6C, (int)mod);
}

bool CBaseEntity::IsEnemy()
{
	return false;//dylan fix
	//return (this->GetTeam() == G::LocalPlayer.Entity->GetTeam() || this->GetTeam() == 0) ? false : true;
}

bool CBaseEntity::IsVisible(int bone)
{
	/*
	Ray_t ray;
	trace_t tr;
	m_visible = false;
	ray.Init(G::LocalPlayer.Entity->GetEyePosition(), this->GetBonePosition(bone)); // replace with config->aimbone

	CTraceFilter filter;
	filter.pSkip = G::LocalPlayer.Entity;

	I::EngineTrace->TraceRay(ray, MASK_SHOT, &filter, &tr);

	if (tr.m_pEnt == this)
	{
		m_visible = true;
		return true;
	}

	return false;
	*/ //dylan fix
	return false;
}

BOOLEAN CBaseEntity::IsBroken()
{
	return (BOOLEAN)ReadByte((DWORD)this + m_bIsBroken);//*(bool*)((DWORD)this + m_bIsBroken);
}

QAngle CBaseEntity::GetViewPunch()
{
	QAngle tempangle;
	ReadAngle((DWORD)this + m_viewPunchAngle, tempangle);
	return tempangle;
	//return *(QAngle*)((DWORD)this + m_viewPunchAngle);
}

void CBaseEntity::SetViewPunch(QAngle punch)
{
	WriteAngle((DWORD)this + m_viewPunchAngle, punch);
}

QAngle CBaseEntity::GetPunch()
{
	QAngle tempangle;
	ReadAngle((DWORD)this + m_aimPunchAngle, tempangle);
	return tempangle;
	//return *(QAngle*)((DWORD)this + m_aimPunchAngle);
}

void CBaseEntity::SetPunch(QAngle punch)
{
	WriteAngle((DWORD)this + m_aimPunchAngle, punch);
}

Vector CBaseEntity::GetPunchVel()
{
	Vector tempangle;
	ReadVector((DWORD)this + m_aimPunchAngleVel, tempangle);
	return tempangle;
	//return *(QAngle*)((DWORD)this + m_viewPunchAngle);
}

void CBaseEntity::SetPunchVel(Vector vel)
{
	WriteVector((DWORD)this + m_aimPunchAngleVel, vel);
}

QAngle CBaseEntity::GetEyeAngles()
{
	//return ((QAngle(__thiscall*)(CBaseEntity* me))ReadInt(ReadInt((uintptr_t)this) + m_OffsetEyeAngles))(this);
	QAngle tempangle;
	ReadAngle((DWORD)this + m_angEyeAngles, tempangle);
	return tempangle;
	//return *(QAngle*)((DWORD)this + m_angEyeAngles);
}

QAngle CBaseEntity::GetEyeAnglesServer()
{
	QAngle tempangle;
	ReadAngle((DWORD)this + m_angEyeAnglesServer, tempangle);
	return tempangle;
}

QAngle* CBaseEntity::EyeAngles()
{
	typedef QAngle*(__thiscall* OriginalFn)(CBaseEntity*);
	return GetVFunc<OriginalFn>(this, EyeAnglesIndex)(this);
}

void CBaseEntity::SetEyeAngles(QAngle angles)
{
	WriteAngle((DWORD)this + m_angEyeAngles, angles);
}

QAngle CBaseEntity::GetRenderAngles()
{
	QAngle ret;
	QAngle* renderanglesadr = (QAngle*)((DWORD)LocalPlayer.Entity + dwRenderAngles);
	ReadAngle((uintptr_t)renderanglesadr, ret);
	return ret;
}

void CBaseEntity::SetRenderAngles(QAngle angles)
{
	QAngle* renderanglesadr = (QAngle*)((DWORD)LocalPlayer.Entity + dwRenderAngles);
	WriteAngle((uintptr_t)renderanglesadr, angles);
}

Vector CBaseEntity::GetNetworkOrigin()
{
	Vector tempvector;
	ReadVector((DWORD)this + m_vecOrigin, tempvector);
	return tempvector;
	//return *(Vector*)((DWORD)this + m_vecOrigin);
}

Vector CBaseEntity::GetOldOrigin()
{
	return *(Vector*)((DWORD)this + m_vecOldOrigin);
}

Vector CBaseEntity::GetOrigin()
{
	Vector tempvector;
	ReadVector((DWORD)this + m_vecOriginReal, tempvector);
	return tempvector;
	//return ((Vector(__thiscall*)(CBaseEntity* me))ReadInt(ReadInt((uintptr_t)this) + m_dwGetAbsOrigin))(this);
}

void CBaseEntity::SetNetworkOrigin(Vector origin)
{
	WriteVector((DWORD)this + m_vecOrigin, origin);
}

void CBaseEntity::SetOrigin(Vector origin)
{
	WriteVector((DWORD)this + m_vecOriginReal, origin);
}

void CBaseEntity::SetAbsOrigin(const Vector &origin)
{
	((void(__thiscall*)(CBaseEntity*, const Vector&))AdrOf_SetAbsOrigin)(this, origin);
}

QAngle CBaseEntity::GetAbsAngles()
{
#if 0
	__asm {
		mov eax, [ecx]
		call[eax + 0x2C]
		ret 4
	}
#endif
#if 1
	typedef QAngle*(__thiscall* OriginalFn)(void*);
	QAngle* ret = GetVFunc<OriginalFn>(this, (m_dwGetAbsAnglesOffset / 4))(this);
	QAngle retval;
	ReadAngle((uintptr_t)ret, retval);
	return retval;

	//int adr = ReadInt((uintptr_t)this);
	//QAngle test = ((QAngle(__thiscall*)(CBaseEntity* me))ReadInt(adr + m_dwGetAbsAnglesOffset))(this);

	//return test;

#endif
}

QAngle CBaseEntity::GetAbsAnglesServer()
{
	if ((*(DWORD *)((DWORD)this + 0x0D0) >> 11) & 1)
		CalcAbsolutePositionServer();
	return *(QAngle*)((DWORD)this + 0x1E4);
}

Vector CBaseEntity::WorldSpaceCenter()
{
	typedef Vector*(__thiscall* OriginalFn)(void*);
	Vector* ret = GetVFunc<OriginalFn>(this, (m_dwWorldSpaceCenterOffset / 4))(this);
	Vector retval;
	ReadVector((uintptr_t)ret, retval);
	return retval;
}

Vector CBaseEntity::GetEyePosition()
{
	//return ((Vector(__thiscall*)(CBaseEntity* me))ReadInt(ReadInt((uintptr_t)this) + m_OffsetEyePos))(this);
	if (this != LocalPlayer.Entity)
	{
		Vector origin = GetOrigin();

		Vector vDuckHullMin = Interfaces::GameMovement->GetPlayerMins(true);
		Vector vStandHullMin = Interfaces::GameMovement->GetPlayerMins(false);

		float fMore = (vDuckHullMin.z - vStandHullMin.z);

		Vector vecDuckViewOffset = Interfaces::GameMovement->GetPlayerViewOffset(true);
		Vector vecStandViewOffset = Interfaces::GameMovement->GetPlayerViewOffset(false);
		float duckFraction = GetDuckAmount();
		//Vector temp = GetViewOffset();
		float tempz = ((vecDuckViewOffset.z - fMore) * duckFraction) +
			(vecStandViewOffset.z * (1 - duckFraction));

		origin.z += tempz;

		return(origin);
	}
	else
	{
		return GetOrigin() + GetViewOffset();
	}
}

BOOLEAN CBaseEntity::ShouldCollide(int collisionGroup, int contentsMask)
{
	return ((BOOLEAN(__thiscall*)(CBaseEntity*, int, int))ReadInt(ReadInt((uintptr_t)this) + m_dwShouldCollide))(this, collisionGroup, contentsMask);
	//int adr = (*(int**)this)[193];
	//adr = adr - (int)ClientHandle;
	//typedef BOOLEAN(__thiscall* OriginalFn)(void*, int, int);
	//return GetVFunc<OriginalFn>(this, 193)(this, collisionGroup, contentsMask);
}

BOOLEAN CBaseEntity::IsTransparent()
{
	typedef BOOLEAN(__thiscall* OriginalFn)(void*);
	return GetVFunc<OriginalFn>(this, 50)(this);
}

SolidType_t CBaseEntity::GetSolid()
{
	return ((SolidType_t(__thiscall*)(CBaseEntity*))ReadInt(ReadInt((uintptr_t)this) + m_dwGetSolid))(this); //(SolidType_t)ReadInt((uintptr_t)this + m_nSolidType);
}

Vector CBaseEntity::GetBonePosition(int HitboxID, float time, bool ForceSetup, bool StoreInCache)
{
	//((CBaseEntity*)this->GetBaseAnimating())->InvalidateBoneCache();

	CustomPlayer *pCPlayer = &AllPlayers[index];
	CBaseAnimating* anim = this->GetBaseAnimating();

	if (anim)
	{
		//DISABLE BONE CACHINE UNTIL IT'S STORED INTO THE CORRECT RECORD
		ForceSetup = true;
		StoreInCache = false;

		matrix3x4_t *destBoneMatrixes;
		matrix3x4_t tempBoneMatrixes[MAXSTUDIOBONES];

		StoredNetvars* CurrentRecord = pCPlayer->Personalize.m_PlayerRecords.size() ? pCPlayer->Personalize.m_PlayerRecords.front() : nullptr;

		if (!CurrentRecord)
		{
			ForceSetup = true;
			StoreInCache = false;
		}

		//Force a setup if we have a bone cache and the cache does not contain updated poses/animations and we have received updated poses/animations
		if (!ForceSetup && CurrentRecord->bCachedBones && !CurrentRecord->bCachedBonesReceivedPoseParametersAndAnimations && CurrentRecord->ReceivedPoseParametersAndAnimations)
		{
			ForceSetup = true;
		}

		if (!ForceSetup && CurrentRecord->bCachedBones)
		{
			//Use cached bones
			destBoneMatrixes = CurrentRecord->CachedBoneMatrixes;
		}
		else
		{
			if (StoreInCache)
			{
				if (CurrentRecord->ReceivedPoseParametersAndAnimations)
				{
					//Let the cache know that it contains proper poses/animations
					CurrentRecord->bCachedBonesReceivedPoseParametersAndAnimations = true;
				}
				destBoneMatrixes = CurrentRecord->CachedBoneMatrixes;
				((CBaseEntity*)anim)->SetupBones(destBoneMatrixes, MAXSTUDIOBONES, BONE_USED_BY_HITBOX, time);
				CurrentRecord->iTickCachedBones = Interfaces::Globals->tickcount;
				CurrentRecord->bCachedBones = true;
			}
			else
			{
				//Use a temporary matrix buffer
				destBoneMatrixes = tempBoneMatrixes;
				((CBaseEntity*)anim)->SetupBones(destBoneMatrixes, MAXSTUDIOBONES, BONE_USED_BY_HITBOX, time);
			}
		}


		const model_t* mod = this->GetModel();
		if (mod)
		{
			studiohdr_t* hdr = Interfaces::ModelInfoClient->GetStudioModel(mod);
			if (hdr)
			{
				mstudiohitboxset_t* set = hdr->pHitboxSet(anim->GetHitboxSet());
				if (set)
				{
					mstudiobbox_t* hitbox = set->pHitbox(HitboxID);
					if (hitbox)
					{
						Vector vMin, vMax, vCenter;
						//matrix3x4a_t* scaled = static_cast<matrix3x4a_t*>(boneMatrixes);
						VectorTransformZ(hitbox->bbmin, destBoneMatrixes[hitbox->bone], vMin);
						VectorTransformZ(hitbox->bbmax, destBoneMatrixes[hitbox->bone], vMax);
						vCenter = (vMin + vMax) * 0.5f;
						return vCenter;
					}
				}
			}
		}
	}
	return vecZero;
}

Vector CBaseEntity::GetBonePositionTotal(int HitboxID, float time, Vector &BoneMax, Vector &BoneMin, bool ForceSetup, bool StoreInCache)
{
	CustomPlayer *pCPlayer = &AllPlayers[index];
	CBaseAnimating* anim = this->GetBaseAnimating();

	if (anim)
	{
		//DISABLE BONE CACHINE UNTIL IT'S STORED INTO THE CORRECT RECORD
		ForceSetup = true;
		StoreInCache = false;

		matrix3x4_t *destBoneMatrixes;
		matrix3x4_t tempBoneMatrixes[MAXSTUDIOBONES];

		StoredNetvars* CurrentRecord = pCPlayer->Personalize.m_PlayerRecords.size() ? pCPlayer->Personalize.m_PlayerRecords.front() : nullptr;

		if (!CurrentRecord)
		{
			ForceSetup = true;
			StoreInCache = false;
		}

		//Force a setup if we have a bone cache and the cache does not contain updated poses/animations and we have received updated poses/animations
		if (!ForceSetup && CurrentRecord->bCachedBones && !CurrentRecord->bCachedBonesReceivedPoseParametersAndAnimations && CurrentRecord->ReceivedPoseParametersAndAnimations)
		{
			ForceSetup = true;
		}

		if (!ForceSetup && CurrentRecord->bCachedBones)
		{
			//Use cached bones
			destBoneMatrixes = CurrentRecord->CachedBoneMatrixes;
		}
		else
		{
			if (StoreInCache)
			{
				if (CurrentRecord->ReceivedPoseParametersAndAnimations)
				{
					//Let the cache know that it contains proper poses/animations
					CurrentRecord->bCachedBonesReceivedPoseParametersAndAnimations = true;
				}
				destBoneMatrixes = CurrentRecord->CachedBoneMatrixes;
				((CBaseEntity*)anim)->SetupBones(destBoneMatrixes, MAXSTUDIOBONES, BONE_USED_BY_HITBOX, time);
				CurrentRecord->iTickCachedBones = Interfaces::Globals->tickcount;
				CurrentRecord->bCachedBones = true;
			}
			else
			{
				//Use a temporary matrix buffer
				destBoneMatrixes = tempBoneMatrixes;
				((CBaseEntity*)anim)->SetupBones(destBoneMatrixes, MAXSTUDIOBONES, BONE_USED_BY_HITBOX, time);
			}
		}

		const model_t* mod = this->GetModel();
		if (mod)
		{
			studiohdr_t* hdr = Interfaces::ModelInfoClient->GetStudioModel(mod);
			if (hdr)
			{
				mstudiohitboxset_t* set = hdr->pHitboxSet(anim->GetHitboxSet());
				if (set)
				{
					mstudiobbox_t* hitbox = set->pHitbox(HitboxID);
					if (hitbox)
					{
						VectorTransformZ(hitbox->bbmin, destBoneMatrixes[hitbox->bone], BoneMin);
						VectorTransformZ(hitbox->bbmax, destBoneMatrixes[hitbox->bone], BoneMax);
						Vector vCenter = (BoneMin + BoneMax) * 0.5f;
						return vCenter;
					}
				}
			}
		}
	}
	return vecZero;
}


void CBaseEntity::GetBonePosition(int iBone, Vector &origin, QAngle &angles, float time, bool ForceSetup, bool StoreInCache)
{
	//WriteInt((uintptr_t)this + AdrOf_m_nWritableBones, 0);
	//WriteInt((uintptr_t)this + (AdrOf_m_nWritableBones - 4), 0); //Readable bones
	//WriteInt((uintptr_t)this + AdrOf_m_iDidCheckForOcclusion, reinterpret_cast<int*>(AdrOf_m_dwOcclusionArray)[1]);
	//((CBaseEntity*)this->GetBaseAnimating())->InvalidateBoneCache();

	CustomPlayer *pCPlayer = &AllPlayers[index];
	CBaseAnimating* anim = this->GetBaseAnimating();

	if (anim)
	{
		//DISABLE BONE CACHINE UNTIL IT'S STORED INTO THE CORRECT RECORD
		ForceSetup = true;
		StoreInCache = false;

		matrix3x4_t *destBoneMatrixes;
		matrix3x4_t tempBoneMatrixes[MAXSTUDIOBONES];

		StoredNetvars* CurrentRecord = pCPlayer->Personalize.m_PlayerRecords.size() ? pCPlayer->Personalize.m_PlayerRecords.front() : nullptr;

		if (!CurrentRecord)
		{
			ForceSetup = true;
			StoreInCache = false;
		}

		//Force a setup if we have a bone cache and the cache does not contain updated poses/animations and we have received updated poses/animations
		if (!ForceSetup && CurrentRecord->bCachedBones && !CurrentRecord->bCachedBonesReceivedPoseParametersAndAnimations && CurrentRecord->ReceivedPoseParametersAndAnimations)
		{
			ForceSetup = true;
		}

		if (!ForceSetup && CurrentRecord->bCachedBones)
		{
			//Use cached bones
			destBoneMatrixes = CurrentRecord->CachedBoneMatrixes;
		}
		else
		{
			if (StoreInCache)
			{
				if (CurrentRecord->ReceivedPoseParametersAndAnimations)
				{
					//Let the cache know that it contains proper poses/animations
					CurrentRecord->bCachedBonesReceivedPoseParametersAndAnimations = true;
				}
				destBoneMatrixes = CurrentRecord->CachedBoneMatrixes;
				((CBaseEntity*)anim)->SetupBones(destBoneMatrixes, MAXSTUDIOBONES, BONE_USED_BY_HITBOX, time);
				CurrentRecord->iTickCachedBones = Interfaces::Globals->tickcount;
				CurrentRecord->bCachedBones = true;
			}
			else
			{
				//Use a temporary matrix buffer
				destBoneMatrixes = tempBoneMatrixes;
				((CBaseEntity*)anim)->SetupBones(destBoneMatrixes, MAXSTUDIOBONES, BONE_USED_BY_HITBOX, time);
			}
		}

		const model_t* mod = this->GetModel();
		if (mod)
		{
			studiohdr_t* hdr = Interfaces::ModelInfoClient->GetStudioModel(mod);
			if (hdr)
			{
				mstudiohitboxset_t* set = hdr->pHitboxSet(anim->GetHitboxSet());
				if (set)
				{
					mstudiobbox_t* hitbox = set->pHitbox(iBone);
					if (hitbox)
					{
						Vector vMin, vMax;
						VectorTransformZ(hitbox->bbmin, destBoneMatrixes[hitbox->bone], vMin);
						VectorTransformZ(hitbox->bbmax, destBoneMatrixes[hitbox->bone], vMax);
						origin = (vMin + vMax) * 0.5f;
						MatrixAngles(destBoneMatrixes[hitbox->bone], angles, origin);
					}
				}
			}
		}
	}
}

Vector CBaseEntity::GetBonePositionCachedOnly(int HitboxID, float time, matrix3x4_t* matrixes)
{
	CBaseAnimating* anim = this->GetBaseAnimating();

	if (anim)
	{
		const model_t* mod = this->GetModel();
		if (mod)
		{
			studiohdr_t* hdr = Interfaces::ModelInfoClient->GetStudioModel(mod);
			if (hdr)
			{
				mstudiohitboxset_t* set = hdr->pHitboxSet(anim->GetHitboxSet());
				if (set)
				{
					mstudiobbox_t* hitbox = set->pHitbox(HitboxID);
					if (hitbox)
					{
						Vector vMin, vMax;
						VectorTransformZ(hitbox->bbmin, matrixes[hitbox->bone], vMin);
						VectorTransformZ(hitbox->bbmax, matrixes[hitbox->bone], vMax);
						Vector vCenter = (vMin + vMax) * 0.5f;
						return vCenter;
					}
				}
			}
		}
	}
	return vecZero;
}

//returns true if we cached, TODO: FIX TIME
bool CBaseEntity::CacheBones()
{
	CustomPlayer *pCPlayer = &AllPlayers[index];
	if (!pCPlayer->Personalize.m_PlayerRecords.size())
		return false;
	StoredNetvars* CurrentRecord = pCPlayer->Personalize.m_PlayerRecords.front();

	if (!CurrentRecord->bCachedBones || (!CurrentRecord->bCachedBonesReceivedPoseParametersAndAnimations && CurrentRecord->ReceivedPoseParametersAndAnimations))
	{
		if (CurrentRecord->ReceivedPoseParametersAndAnimations)
			CurrentRecord->bCachedBonesReceivedPoseParametersAndAnimations = true;
		SetupBones(CurrentRecord->CachedBoneMatrixes, MAXSTUDIOBONES, BONE_USED_BY_HITBOX, Interfaces::Globals->curtime);
		CurrentRecord->iTickCachedBones = Interfaces::Globals->tickcount;
		CurrentRecord->bCachedBones = true;
		return true;
	}

	return false;
}

bool CBaseEntity::SetupBones(matrix3x4_t *pBoneToWorldOut, int nMaxBones, int boneMask, float currentTime)
{
	InvalidateBoneCache();
	*(int*)((DWORD)this + AdrOf_m_nWritableBones) = 0;
	//WriteInt((uintptr_t)this + (AdrOf_m_nWritableBones - 4), 0); //Readable bones
	*(int*)((DWORD)this + AdrOf_m_iDidCheckForOcclusion) = reinterpret_cast<int*>(AdrOf_m_dwOcclusionArray)[1];

	typedef bool(__thiscall* SetupBonesFn)(void*, matrix3x4_t*, int, int, float);
	return GetVFunc<SetupBonesFn>((void*)((uintptr_t)this + 0x4), 13)((void*)((uintptr_t)this + 0x4), pBoneToWorldOut, nMaxBones, boneMask, currentTime);
#if 0
	__asm
	{
		mov edi, this
		lea ecx, dword ptr ds : [edi + 0x4]
		mov edx, dword ptr ds : [ecx]
		push currentTime
		push boneMask
		push nMaxBones
		push pBoneToWorldOut
		call dword ptr ds : [edx + 0x34]
	}
#endif
}

Vector CBaseEntity::GetVelocity()
{
	Vector tempvector;
	ReadVector((DWORD)this + m_vecVelocity, tempvector);
	return tempvector;
}

void CBaseEntity::SetVelocity(Vector velocity)
{
	WriteVector((DWORD)this + m_vecVelocity, velocity);
}

void CBaseEntity::SetAbsVelocity(Vector velocity)
{
	((void(__thiscall*)(CBaseEntity*, Vector*))AdrOf_SetAbsVelocity)(this, &velocity);
}

Vector CBaseEntity::GetBaseVelocity()
{
	Vector tmp;
	ReadVector((DWORD)this + m_vecBaseVelocity, tmp);
	return tmp;
}

void CBaseEntity::SetBaseVelocity(Vector velocity)
{
	WriteVector((DWORD)this + m_vecBaseVelocity, velocity);
}

Vector CBaseEntity::GetPredicted(Vector p0)
{
	return vecZero;
	//return M::ExtrapolateTick(p0, this->GetVelocity()); //dylan fix
}

ICollideable* CBaseEntity::GetCollideable()
{
	return (ICollideable*)((DWORD)this + m_Collision);
}

void CBaseEntity::GetPlayerInfo(player_info_t *dest)
{
	Interfaces::Engine->GetPlayerInfo(index, dest);
}

std::string	CBaseEntity::GetName()
{
	player_info_t info;
	GetPlayerInfo(&info);
	return std::string(info.name);
}

void CBaseEntity::GetSteamID(char* dest)
{
	player_info_t info;
	GetPlayerInfo(&info);
	memcpy(dest, &info.guid, 33);
}

std::string CBaseEntity::GetLastPlace()
{
	return std::string((const char*)this + m_szLastPlaceName);
}

CBaseCombatWeapon* CBaseEntity::GetWeapon()
{
	//DWORD weaponData = *(DWORD*)((DWORD)this + m_hActiveWeapon);
	//return (CBaseCombatWeapon*)Interfaces::ClientEntList->GetClientEntityFromHandle(weaponData);
	DWORD weaponData = ReadInt((DWORD)this + m_hActiveWeapon);
	return (CBaseCombatWeapon*)Interfaces::ClientEntList->GetClientEntityFromHandle(weaponData);
	//dylan fix this^
	//int weaponIndex = (ReadInt((DWORD)this + m_hActiveWeapon) & 0xFFF) - 1;

	//return (CBaseCombatWeapon*)ReadInt(((DWORD)ClientHandle + m_dwEntityList + weaponIndex * 0x10) - 0x10);
}

ClientClass* CBaseEntity::GetClientClass()
{
	PVOID pNetworkable = (PVOID)((DWORD)(this) + 0x8);
	typedef ClientClass*(__thiscall* OriginalFn)(PVOID);
	return GetVFunc<OriginalFn>(pNetworkable, 2)(pNetworkable);
}

int CBaseEntity::GetCollisionGroup()
{
	return ReadInt((DWORD)this + m_CollisionGroup);//*(int*)((DWORD)this + m_CollisionGroup);
}

CBaseAnimating* CBaseEntity::GetBaseAnimating(void)
{
	int adr = ReadInt((uintptr_t)this);
	if (adr)
	{
		return ((CBaseAnimating*(__thiscall*)(CBaseEntity* me))ReadInt(adr + m_dwGetBaseAnimating))(this);
	}
	return NULL;
}

void CBaseEntity::InvalidateBoneCache()
{
	bool orig = *s_bEnableInvalidateBoneCache;
	*s_bEnableInvalidateBoneCache = true;
	((CBaseAnimating*(__fastcall*)(CBaseEntity*))AdrOfInvalidateBoneCache)(this);
	*s_bEnableInvalidateBoneCache = orig;
}

HANDLE CBaseEntity::GetObserverTarget()
{
	return (HANDLE)ReadInt((uintptr_t)this + m_hObserverTarget);
}

BOOLEAN CBaseEntity::IsPlayer()
{
	typedef BOOLEAN(__thiscall* OriginalFn)(CBaseEntity*);
	return GetVFunc<OriginalFn>(this, (m_dwIsPlayer / 4))(this);
	//return ((BOOLEAN(__thiscall*)(CBaseEntity* me))ReadInt(ReadInt((uintptr_t)this) + m_dwIsPlayer))(this);
}

CUserCmd* CBaseEntity::GetLastUserCommand()
{
	return (CUserCmd*)ReadInt(ReadInt((uintptr_t)this) + m_dwGetLastUserCommand);
}

BOOLEAN CBaseEntity::IsConnected()
{
	typedef BOOLEAN(__thiscall* OriginalFn)(CBaseEntity*);
	return GetVFunc<OriginalFn>(this, (m_dwIsConnected / 4))(this);
	//return ((BOOLEAN(__thiscall*)(CBaseEntity* me))ReadInt(ReadInt((uintptr_t)this) + m_dwIsConnected))(this);
}

BOOLEAN CBaseEntity::IsSpawned()
{
	typedef BOOLEAN(__thiscall* OriginalFn)(CBaseEntity*);
	return GetVFunc<OriginalFn>(this, (m_dwIsSpawned / 4))(this);
	//return ((BOOLEAN(__thiscall*)(CBaseEntity* me))ReadInt(ReadInt((uintptr_t)this) + m_dwIsSpawned))(this);
}

BOOLEAN CBaseEntity::IsActive()
{
	typedef BOOLEAN(__thiscall* OriginalFn)(CBaseEntity*);
	return GetVFunc<OriginalFn>(this, (m_dwIsActive / 4))(this);
	//return ((BOOLEAN(__thiscall*)(CBaseEntity* me))ReadInt(ReadInt((uintptr_t)this) + m_dwIsActive))(this);
}

BOOLEAN CBaseEntity::IsFakeClient()
{
	typedef BOOLEAN(__thiscall* OriginalFn)(CBaseEntity*);
	return GetVFunc<OriginalFn>(this, (m_dwIsFakeClient / 4))(this);
	//return ((BOOLEAN(__thiscall*)(CBaseEntity* me))ReadInt(ReadInt((uintptr_t)this) + m_dwIsFakeClient))(this);
}

bool CBaseEntity::IsTargetingLocalPunch()
{
	Vector src, dst, forward;
	trace_t tr;
	Ray_t ray;
	CTraceFilterIgnoreWorld filter;

	int index = ReadInt((uintptr_t)&this->index);
	CustomPlayer *pCAPlayer = &AllPlayers[index];
	if (!pCAPlayer->Dormant)
	{
		QAngle viewangle = pCAPlayer->Personalize.m_PlayerRecords.size() > 0 ? pCAPlayer->Personalize.m_PlayerRecords.front()->eyeangles : pCAPlayer->BaseEntity->GetEyeAngles();
		viewangle -= this->GetPunch() * 2.f;

		AngleVectors(viewangle, &forward);
		VectorNormalizeFast(forward);
		forward *= 8192.f;
		src = this->GetEyePosition();
		dst = src + forward;
		filter.pSkip = (IHandleEntity*)this;
		filter.m_icollisionGroup = COLLISION_GROUP_NONE;
		ray.Init(src, dst);

		Interfaces::EngineTrace->TraceRay(ray, MASK_SHOT, (ITraceFilter*)&filter, &tr);

		return tr.m_pEnt == LocalPlayer.Entity;
	}
	return false;
}

QAngle CBaseEntity::GetAngleRotation()
{
	QAngle temp;
	ReadAngle((DWORD)this + m_angRotation, temp);
	return temp;
}

void CBaseEntity::SetAngleRotation(QAngle rot)
{
	//((void(__thiscall*)(CBaseEntity*, QAngle*))AdrOf_SetAbsAngles)(this, &rot);
	WriteAngle((DWORD)this + m_angRotation, rot);
}

void CBaseEntity::SetAbsAngles(const QAngle &rot)
{
	((void(__thiscall*)(CBaseEntity*, const QAngle&))AdrOf_SetAbsAngles)(this, rot);
}

Vector CBaseEntity::GetMins()
{
	Vector temp;
	ReadVector((DWORD)this + m_vecMins, temp);
	return temp;
}

void CBaseEntity::SetMins(Vector mins)
{
	WriteVector((DWORD)this + m_vecMins, mins);
}

Vector CBaseEntity::GetMaxs()
{
	Vector temp;
	ReadVector((DWORD)this + m_vecMaxs, temp);
	return temp;
}

void CBaseEntity::SetMaxs(Vector maxs)
{
	WriteVector((DWORD)this + m_vecMaxs, maxs);
}

int CBaseEntity::GetSequence()
{
	return ReadInt((DWORD)this + m_nSequence);
}

void CBaseEntity::SetSequence(int seq)
{
	WriteInt((DWORD)this + m_nSequence, seq);
}

void CBaseEntity::GetPoseParameterRange(int index, float& flMin, float& flMax)
{
	GetPoseParameterRangeGame(this, index, flMin, flMax);
}

float CBaseEntity::GetPoseParameter(int index)
{
	return ReadFloat((DWORD)this + m_flPoseParameter + (sizeof(float) * index));
}

float CBaseEntity::GetPoseParameterUnscaled(int index)
{
	float flMin, flMax;
	GetPoseParameterRangeGame(this, index, flMin, flMax);
	return ReadFloat((DWORD)this + m_flPoseParameter + (sizeof(float) * index)) * (flMax - flMin) + flMin;
}

float CBaseEntity::GetPoseParameterUnScaledServer(int index)
{
#ifdef _DEBUG
	static DWORD getposeadr = NULL;
	if (!getposeadr)
	{
		const char *sig = "55  8B  EC  56  57  8B  F9  83  BF  9C  04  00  00  00  75  ??  A1  ??  ??  ??  ??  8B  30  8B  07  FF  50  18  8B  ??  ??  ??  ??  ??  50  FF  56  04  85  C0  74  ??  8B  CF  E8  7F  28  00  00  8B  B7  9C  04  00  00";
		getposeadr = FindMemoryPattern(GetModuleHandleA("server.dll"), (char*)sig, strlen(sig));
		if (!getposeadr)
			DebugBreak();
	}

	((void(__thiscall *)(CBaseEntity*, int))getposeadr)(this, index); //call as void to not fuck up xmm0 registers
	float res;
	__asm movss res, xmm0
	return res;
#endif
}

int CBaseEntity::LookupPoseParameter(char *name)
{
	return LookupPoseParameterGame(this, name);
}

void CBaseEntity::SetPoseParameter(int index, float p)
{
	float flMin, flMax;
	GetPoseParameterRangeGame(this, index, flMin, flMax);
	float scaledValue = (p - flMin) / (flMax - flMin);
#if 0
	DWORD dc = ReadInt(AdrOf_DataCacheSetPoseParmaeter);
	DWORD me = (DWORD)this;
	__asm {
		mov esi, dc
		mov ecx, esi
		mov eax, [esi]
		call [eax + 0x80]
		push index
		movss xmm2, scaledValue
		mov ecx, me
		call SetPoseParameterGame
		mov eax, [esi]
		mov ecx, esi
		call [eax + 0x84]
	}
#endif
	WriteFloat((DWORD)this + m_flPoseParameter + (sizeof(float) * index), scaledValue);
}

void CBaseEntity::SetPoseParameterScaled(int index, float p)
{
	WriteFloat((DWORD)this + m_flPoseParameter + (sizeof(float) * index), p);
}

void CBaseEntity::CopyPoseParameters(float* dest)
{
	float* flPose = (float*)((DWORD)this + m_flPoseParameter);
	memcpy(dest, flPose, sizeof(float) * 24);
}

unsigned char CBaseEntity::GetClientSideAnimation()
{
	return ReadByte((DWORD)this + m_bClientSideAnimation);
}

void CBaseEntity::SetClientSideAnimation(unsigned char a)
{
	WriteByte((DWORD)this + m_bClientSideAnimation, a);
}

float CBaseEntity::GetCycle()
{
	return ReadFloat((DWORD)this + m_flCycle);
}

void CBaseEntity::SetCycle(float cycle)
{
	WriteFloat((DWORD)this + m_flCycle, cycle);
}

float CBaseEntity::GetNextAttack()
{
	return ReadFloat((DWORD)this + m_flNextAttack);
}

void CBaseEntity::SetNextAttack(float att)
{
	WriteFloat((DWORD)this + m_flNextAttack, att);
}

BOOLEAN CBaseEntity::IsDucked()
{
	return (BOOLEAN)ReadByte((DWORD)this + m_bDucked);
}

void CBaseEntity::SetDucked(bool ducked)
{
	WriteByte((DWORD)this + m_bDucked, ducked);
}

BOOLEAN CBaseEntity::IsDucking()
{
	return (BOOLEAN)ReadByte((DWORD)this + m_bDucking);
}

void CBaseEntity::SetDucking(bool ducking)
{
	WriteByte((DWORD)this + m_bDucking, ducking);
}

BOOLEAN CBaseEntity::IsInDuckJump()
{
	return (BOOLEAN)ReadByte((DWORD)this + m_bInDuckJump);
}

void CBaseEntity::SetInDuckJump(bool induckjump)
{
	WriteByte((DWORD)this + m_bInDuckJump, induckjump);
}

int CBaseEntity::GetDuckTimeMsecs()
{
	return ReadInt((DWORD)this + m_nDuckTimeMsecs);
}

void CBaseEntity::SetDuckTimeMsecs(int msecs)
{
	WriteInt((DWORD)this + m_nDuckTimeMsecs, msecs);
}

int CBaseEntity::GetJumpTimeMsecs()
{
	return ReadInt((DWORD)this + m_nJumpTimeMsecs);
}

void CBaseEntity::SetJumpTimeMsecs(int msecs)
{
	WriteInt((DWORD)this + m_nJumpTimeMsecs, msecs);
}

float CBaseEntity::GetFallVelocity()
{
	return ReadFloat((DWORD)this + m_flFallVelocity);
}

void CBaseEntity::SetFallVelocity(float vel)
{
	WriteFloat((DWORD)this + m_flFallVelocity, vel);
}

Vector CBaseEntity::GetViewOffset()
{
	Vector tmp;
	ReadVector((DWORD)this + m_vecViewOffset, tmp);
	return tmp;
}

void CBaseEntity::SetViewOffset(Vector off)
{
	WriteVector((DWORD)this + m_vecViewOffset, off);
}

int CBaseEntity::GetNextThinkTick()
{
	return ReadInt((DWORD)this + m_nNextThinkTick);
}

void CBaseEntity::SetNextThinkTick(int tick)
{
	WriteInt((DWORD)this + m_nNextThinkTick, tick);
}

float CBaseEntity::GetDuckAmount()
{
	return ReadFloat((DWORD)this + m_flDuckAmount);
}

void CBaseEntity::SetDuckAmount(float duckamt)
{
	WriteFloat((DWORD)this + m_flDuckAmount, duckamt);
}

float CBaseEntity::GetDuckSpeed()
{
	return ReadFloat((DWORD)this + m_flDuckSpeed);
}

void CBaseEntity::SetDuckSpeed(float spd)
{
	WriteFloat((DWORD)this + m_flDuckSpeed, spd);
}

float CBaseEntity::GetVelocityModifier()
{
	return ReadFloat((DWORD)this + m_flVelocityModifier);
}

void CBaseEntity::SetVelocityModifier(float vel)
{
	WriteFloat((DWORD)this + m_flVelocityModifier, vel);
}

float CBaseEntity::GetPlaybackRate()
{
	return ReadFloat((DWORD)this + m_flPlaybackRate);
}

void CBaseEntity::SetPlaybackRate(float rate)
{
	WriteFloat((DWORD)this + m_flPlaybackRate, rate);
}

float CBaseEntity::GetAnimTime()
{
	return ReadFloat((DWORD)this + m_flAnimTime);
}

void CBaseEntity::SetAnimTime(float time)
{
	WriteFloat((DWORD)this + m_flAnimTime, time);
}

float CBaseEntity::GetSimulationTime()
{
	//this is sometimes being 0 causing crash
	return ReadFloat((DWORD)this + m_flSimulationTime);
}

float CBaseEntity::GetSimulationTimeServer()
{
	return *(float*)((DWORD)this + m_flSimulationTimeServer);
}

float CBaseEntity::GetOldSimulationTime()
{
	return *(float*)((DWORD)this + (m_flSimulationTime + 4));
}

void CBaseEntity::SetSimulationTime(float time)
{
	WriteFloat((DWORD)this + m_flSimulationTime, time);
}

float CBaseEntity::GetLaggedMovement()
{
	return ReadFloat((DWORD)this + m_flLaggedMovementValue);
}

void CBaseEntity::SetLaggedMovement(float mov)
{
	WriteFloat((DWORD)this + m_flLaggedMovementValue, mov);
}

CBaseEntity* CBaseEntity::GetGroundEntity()
{
	return (CBaseEntity*)ReadInt((DWORD)this + m_hGroundEntity);
}

void CBaseEntity::SetGroundEntity(CBaseEntity* groundent)
{
	WriteInt((DWORD)this + m_hGroundEntity, (int)groundent);
}

Vector CBaseEntity::GetVecLadderNormal()
{
	Vector tmp;
	ReadVector((DWORD)this + m_vecLadderNormal, tmp);
	return tmp;
}

void CBaseEntity::SetVecLadderNormal(Vector norm)
{
	WriteVector((DWORD)this + m_vecLadderNormal, norm);
}

float CBaseEntity::GetLowerBodyYaw()
{
	return ReadFloat((DWORD)this + m_flLowerBodyYawTarget);
}

void CBaseEntity::SetLowerBodyYaw(float yaw)
{
	WriteFloat((DWORD)this + m_flLowerBodyYawTarget, yaw);
}

void* CBaseEntity::GetPlayerAnimState()
{
	return (void*)ReadInt((uintptr_t)this + m_hPlayerAnimState);
}

void* CBaseEntity::GetPlayerAnimStateServer()
{
	return (void*)ReadInt((uintptr_t)(this) + m_hPlayerAnimStateServer);
}

#include "ClassIDS.h"
bool CBaseEntity::IsWeapon()
{
	//if (IsPlayer())
		//return false;

#if 1
	typedef bool(__thiscall* OriginalFn)(void*);
	return GetVFunc<OriginalFn>(this, 160)(this);
#else
	ClientClass *clclass = GetClientClass();
	int classid = ReadInt((uintptr_t)&clclass->m_ClassID);
#if 0
	char* networkname = clclass->m_pNetworkName;
	//CWeapon
	if ((networkname[0] == 'C' && networkname[1] == 'W' && networkname[2] == 'e') || (classid == _CAK47 || classid == _CDEagle))
		return true;
	return false;
#else
	switch (classid)
	{
	case _CWeaponXM1014:
	case _CWeaponTaser:
	case _CSmokeGrenade:
	case _CWeaponSG552:
	case _CSensorGrenade:
	case _CWeaponSawedoff:
	case _CWeaponNOVA:
	case _CIncendiaryGrenade:
	case _CMolotovGrenade:
	case _CWeaponM3:
	case _CKnifeGG:
	case _CKnife:
	case _CHEGrenade:
	case _CFlashbang:
	case _CWeaponElite:
	case _CDecoyGrenade:
	case _CDEagle:
	case _CWeaponUSP:
	case _CWeaponM249:
	case _CWeaponUMP45:
	case _CWeaponTMP:
	case _CWeaponTec9:
	case _CWeaponSSG08:
	case _CWeaponSG556:
	case _CWeaponSG550:
	case _CWeaponScout:
	case _CWeaponSCAR20:
	case _CSCAR17:
	case _CWeaponP90:
	case _CWeaponP250:
	case _CWeaponP228:
	case _CWeaponNegev:
	case _CWeaponMP9:
	case _CWeaponMP7:
	case _CWeaponMP5Navy:
	case _CWeaponMag7:
	case _CWeaponMAC10:
	case _CWeaponM4A1:
	case _CWeaponHKP2000:
	case _CWeaponGlock:
	case _CWeaponGalilAR:
	case _CWeaponGalil:
	case _CWeaponG3SG1:
	case _CWeaponFiveSeven:
	case _CWeaponFamas:
	case _CWeaponBizon:
	case _CWeaponAWP:
	case _CWeaponAug:
	case _CAK47:
	case _CSmokeGrenadeProjectile:
	case _CSensorGrenadeProjectile:
	case _CMolotovProjectile:
	case _CDecoyProjectile:
		return true;
	}
	return false;
#endif
#endif
}

bool CBaseEntity::IsProjectile()
{
	//if (IsPlayer())
	//return false;

	ClientClass *clclass = GetClientClass();
	int classid = ReadInt((uintptr_t)&clclass->m_ClassID);

	switch (classid)
	{
		case _CSmokeGrenadeProjectile:
		case _CSensorGrenadeProjectile:
		case _CMolotovProjectile:
		case _CDecoyProjectile:
		case _CBaseCSGrenadeProjectile:
		return true;
	}
	return false;
}

bool CBaseEntity::IsFlashGrenade()
{
	ClientClass *clclass = GetClientClass();
	int classid = ReadInt((uintptr_t)&clclass->m_ClassID);

	return classid == _CBaseCSGrenadeProjectile;
}

bool CBaseEntity::IsChicken()
{
	ClientClass *clclass = GetClientClass();
	int classid = ReadInt((uintptr_t)&clclass->m_ClassID);

	return classid == _CChicken;
}

void CBaseEntity::DrawHitboxes(ColorRGBA color, float livetimesecs)
{
	model_t *pmodel = GetModel();
	if (!pmodel)
		return;

	studiohdr_t* pStudioHdr = Interfaces::ModelInfoClient->GetStudioModel(pmodel);

	if (!pStudioHdr)
		return;

	mstudiohitboxset_t *set = pStudioHdr->pHitboxSet(GetHitboxSet());
	if (!set)
		return;

	matrix3x4_t boneMatrixes[MAXSTUDIOBONES];
	SetupBones(boneMatrixes, MAXSTUDIOBONES, BONE_USED_BY_HITBOX, /*GetSimulationTime()*/ Interfaces::Globals->curtime);

	Vector hitBoxVectors[8];

	for (int i = 0; i < set->numhitboxes; i++)
	{
		mstudiobbox_t *pbox = set->pHitbox(i);
		if (pbox)
		{
			if (pbox->radius == -1.0f)
			{
				//Slow DirectX method, but works
#if 0
				Vector points[8] = { Vector(pbox->bbmin.x, pbox->bbmin.y, pbox->bbmin.z),
					Vector(pbox->bbmin.x, pbox->bbmax.y, pbox->bbmin.z),
					Vector(pbox->bbmax.x, pbox->bbmax.y, pbox->bbmin.z),
					Vector(pbox->bbmax.x, pbox->bbmin.y, pbox->bbmin.z),
					Vector(pbox->bbmax.x, pbox->bbmax.y, pbox->bbmax.z),
					Vector(pbox->bbmin.x, pbox->bbmax.y, pbox->bbmax.z),
					Vector(pbox->bbmin.x, pbox->bbmin.y, pbox->bbmax.z),
					Vector(pbox->bbmax.x, pbox->bbmin.y, pbox->bbmax.z) };

				for (int index = 0; index < 8; ++index)
				{
					// scale down the hitbox size a tiny bit (default is a little too big)
#if 0
					points[index].x *= 0.9f;
					points[index].y *= 0.9f;
					points[index].z *= 0.9f;
#endif

					// transform the vector
					VectorTransformZ(points[index], boneMatrixes[pbox->bone], hitBoxVectors[index]);
				}

				DrawHitbox(color, hitBoxVectors);
#else
				Vector position;
				QAngle angles;
				//matrix3x4a_t* scaled = static_cast<matrix3x4a_t*>(boneMatrixes);
				MatrixAngles(boneMatrixes[pbox->bone], angles, position);
				Interfaces::DebugOverlay->AddBoxOverlay(position, pbox->bbmin, pbox->bbmax, angles, color.r, color.g, color.b, 0, livetimesecs);

#endif
			}
			else
			{
				Vector vMin, vMax;
				VectorTransformZ(pbox->bbmin, boneMatrixes[pbox->bone], vMin);
				VectorTransformZ(pbox->bbmax, boneMatrixes[pbox->bone], vMax);

				Interfaces::DebugOverlay->DrawPill(vMin, vMax, pbox->radius, color.r, color.g, color.b, color.a, livetimesecs);
			}
		}
	}
}

void CBaseEntity::DrawHitboxesFromCache(ColorRGBA color, float livetimesecs, matrix3x4_t *matrix)
{
	model_t *pmodel = GetModel();
	if (!pmodel)
		return;

	studiohdr_t* pStudioHdr = Interfaces::ModelInfoClient->GetStudioModel(pmodel);

	if (!pStudioHdr)
		return;

	mstudiohitboxset_t *set = pStudioHdr->pHitboxSet(GetHitboxSet());
	if (!set)
		return;

	Vector hitBoxVectors[8];

	for (int i = 0; i < set->numhitboxes; i++)
	{
		mstudiobbox_t *pbox = set->pHitbox(i);
		if (pbox)
		{
			if (pbox->radius == -1.0f)
			{
				//Slow DirectX method, but works
#if 0
				Vector points[8] = { Vector(pbox->bbmin.x, pbox->bbmin.y, pbox->bbmin.z),
					Vector(pbox->bbmin.x, pbox->bbmax.y, pbox->bbmin.z),
					Vector(pbox->bbmax.x, pbox->bbmax.y, pbox->bbmin.z),
					Vector(pbox->bbmax.x, pbox->bbmin.y, pbox->bbmin.z),
					Vector(pbox->bbmax.x, pbox->bbmax.y, pbox->bbmax.z),
					Vector(pbox->bbmin.x, pbox->bbmax.y, pbox->bbmax.z),
					Vector(pbox->bbmin.x, pbox->bbmin.y, pbox->bbmax.z),
					Vector(pbox->bbmax.x, pbox->bbmin.y, pbox->bbmax.z) };

				for (int index = 0; index < 8; ++index)
				{
					// scale down the hitbox size a tiny bit (default is a little too big)
#if 0
					points[index].x *= 0.9f;
					points[index].y *= 0.9f;
					points[index].z *= 0.9f;
#endif

					// transform the vector
					VectorTransformZ(points[index], boneMatrixes[pbox->bone], hitBoxVectors[index]);
				}

				DrawHitbox(color, hitBoxVectors);
#else
				Vector position;
				QAngle angles;
				//matrix3x4a_t* scaled = static_cast<matrix3x4a_t*>(boneMatrixes);
				MatrixAngles(matrix[pbox->bone], angles, position);
				Interfaces::DebugOverlay->AddBoxOverlay(position, pbox->bbmin, pbox->bbmax, angles, color.r, color.g, color.b, 0, livetimesecs);

#endif
			}
			else
			{
				Vector vMin, vMax;
				VectorTransformZ(pbox->bbmin, matrix[pbox->bone], vMin);
				VectorTransformZ(pbox->bbmax, matrix[pbox->bone], vMax);

				Interfaces::DebugOverlay->DrawPill(vMin, vMax, pbox->radius, color.r, color.g, color.b, color.a, livetimesecs);
			}
		}
	}
}

#include "INetchannelInfo.h"
INetChannelInfo* CBaseEntity::GetNetChannel()
{
	return nullptr;
	//todo: fix me
	//return ((INetChannelInfo*(__thiscall*)(CBaseEntity* me))ReadInt(ReadInt((uintptr_t)this) + m_dwGetNetChannel))(this);
}

void CBaseEntity::DisableInterpolation()
{
	CustomPlayer *pCPlayer = &AllPlayers[index];

	VarMapping_t *map = (VarMapping_t*)((DWORD)this + 0x24); // tf2 = 20
	if (pCPlayer->iOriginalInterpolationEntries == 0)
	{
		pCPlayer->iOriginalInterpolationEntries = map->m_nInterpolatedEntries;
	}
	map->m_nInterpolatedEntries = 0;

	/*for (int i = 0; i < map->m_nInterpolatedEntries; ++i)
	{
	VarMapEntry_t *e = &map->m_Entries[i];

	e->m_bNeedsToInterpolate = false;
	}*/

	//crash city, fuck this shit. phage's just crashes in client.dll
#if 0
	auto var_map_list_count = *(int*)((DWORD)map + 20);

	for (auto index = 0; index < var_map_list_count; index++)
	{
		*(DWORD*)((DWORD)map + (index * 12)) = 0;
	}
#endif
}

void CBaseEntity::EnableInterpolation() 
{
	CustomPlayer *pCPlayer = &AllPlayers[index];
	VarMapping_t *map = (VarMapping_t*)((DWORD)this + 0x24); // tf2 = 20
	if (map->m_nInterpolatedEntries == 0 && pCPlayer->iOriginalInterpolationEntries > 0 && !(GetEffects() & EF_NOINTERP))
	{
		map->m_nInterpolatedEntries = pCPlayer->iOriginalInterpolationEntries;
	}
#if 0
	auto var_map_list_count = *(int*)((DWORD)map + 20);

	for (auto index = 0; index < var_map_list_count; index++)
	{
		*(DWORD*)((DWORD)map + (index * 12)) = 1;
	}
#endif
}

void CBaseEntity::UpdateClientSideAnimation()
{
	typedef void(__thiscall *o_updateClientSideAnimation)(void*);
	GetVFunc<o_updateClientSideAnimation>(this, 218)(this);
}

float CBaseEntity::GetLastClientSideAnimationUpdateTime()
{
	void* animstate = GetPlayerAnimState();
	if (animstate)
	{
		return *(float*)((DWORD)animstate + 0x6C);
	}
	
	return 0.0f;
}

void CBaseEntity::SetLastClientSideAnimationUpdateTime(float time)
{
	void* animstate = GetPlayerAnimState();
	if (animstate)
	{
		*(float*)((DWORD)animstate + 0x6C) = time;
	}
}

int CBaseEntity::GetLastClientSideAnimationUpdateGlobalsFrameCount()
{
	void* animstate = GetPlayerAnimState();
	if (animstate)
	{
		return *(float*)((DWORD)animstate + 0x70);
	}

	return 0;
}

void CBaseEntity::SetLastClientSideAnimationUpdateGlobalsFrameCount(int framecount)
{
	void* animstate = GetPlayerAnimState();
	if (animstate)
	{
		 *(float*)((DWORD)animstate + 0x70) = framecount;
	}
}

float CBaseEntity::FrameAdvance(float flInterval)
{
#if 0
	CustomPlayer *pCPlayer = &AllPlayers[ReadInt((uintptr_t)&this->index)];

	float curtime = ReadFloat((uintptr_t)&Interfaces::Globals->curtime);

	if (flInterval == 0.0f)
	{
		flInterval = (curtime - pCPlayer->flAnimTime);
		if (flInterval <= 0.001f)
		{
			return 0.0f;
		}
	}

	if (!pCPlayer->flAnimTime)
	{
		flInterval = 0.0f;
	}

	pCPlayer->flAnimTime = curtime;

#endif
	return flInterval;
}

int CBaseEntity::GetEffects()
{
	return ReadInt((uintptr_t)this + m_fEffects);
}

void CBaseEntity::SetEffects(int effects)
{
	WriteInt((uintptr_t)this + m_fEffects, effects);
}

int CBaseEntity::GetObserverMode()
{
	return ReadInt((uintptr_t)this + m_iObserverMode);
}

void CBaseEntity::SetObserverMode(int mode)
{
	WriteInt((uintptr_t)this + m_iObserverMode, mode);
}

CUtlVectorSimple* CBaseEntity::GetAnimOverlayStruct() const
{
	return (CUtlVectorSimple*) ((DWORD)this + dwm_nAnimOverlay);
}

C_AnimationLayer* CBaseEntity::GetAnimOverlay(int i)
{
	if (i >= 0 && i < MAX_OVERLAYS)
	{
		CUtlVectorSimple *m_AnimOverlay = GetAnimOverlayStruct();
		return (C_AnimationLayer*)m_AnimOverlay->Retrieve(i, sizeof(C_AnimationLayer));
		//DWORD v19 = 7 * i;
		//DWORD m = *(DWORD*)((DWORD)m_AnimOverlay);
		//C_AnimationLayer* test = (C_AnimationLayer*)(m + 8 * v19);
		//DWORD sequence = *(DWORD *)(m + 8 * v19 + 24);

	}
	return nullptr;
}

int CBaseEntity::GetNumAnimOverlays() const
{
	CUtlVectorSimple *m_AnimOverlay = GetAnimOverlayStruct();
	return m_AnimOverlay->count;
}

void CBaseEntity::CopyAnimLayers(C_AnimationLayer* dest)
{
	int count = GetNumAnimOverlays();
	for (int i = 0; i < count; i++)
	{
		dest[i] = *GetAnimOverlay(i);
	}
}

// Invalidates the abs state of all children
void CBaseEntity::InvalidatePhysicsRecursive(int nChangeFlags)
{
	return ((void(__thiscall*)(CBaseEntity*, int))AdrOf_InvalidatePhysicsRecursive)(this, nChangeFlags);
}

Vector* CBaseEntity::GetAbsOrigin()
{
	return GetVFunc<Vector*(__thiscall*)(void*)>(this, 10)(this);
}

Vector CBaseEntity::GetAbsOriginServer()
{
	if ((*(DWORD *)((DWORD)this + 0x0D0) >> 11) & 1)
		CalcAbsolutePositionServer();
	return *(Vector*)((DWORD)this + 0x1D8);
}

int CBaseEntity::entindex()
{
	return GetVFunc<int(__thiscall*)(void*)>(this, (m_dwEntIndexOffset / 4))(this);
}

int CBaseEntity::entindexServer()
{
#ifdef _DEBUG
	static DWORD m_Network_Adr = NULL;
	if (!m_Network_Adr)
	{
		const char *sig = "8B  4F  1C  85  C9  74  0B  A1  ??  ??  ??  ??  2B  48  60  C1  F9  04";
		m_Network_Adr = FindMemoryPattern(GetModuleHandleA("server.dll"), (char*)sig, strlen(sig));
		if (!m_Network_Adr)
			DebugBreak();

		m_Network_Adr = *(DWORD*)(m_Network_Adr + 8);
	}
	DWORD ecx = *(DWORD*)((DWORD)this + 0x1C);
	if (ecx)
	{
		DWORD eax = *(DWORD*)m_Network_Adr;
		ecx -= *(DWORD*)(eax + 0x60);
		ecx = ecx >> 4;
		return ecx;
	}
#endif
	return -1;
}

float CBaseEntity::GetCurrentFeetYawServer()
{
	void* animstate = GetPlayerAnimStateServer();
	if (animstate)
	{
		return *(float*)((DWORD)animstate + m_flCurrentFeetYawServer);
	}
	return 0.0f;
}

float CBaseEntity::GetCurrentFeetYaw()
{
	void* animstate = GetPlayerAnimState();
	if (animstate)
	{
		return *(float*)((DWORD)animstate + 0x84);
	}
	return 0.0f;
}

void CBaseEntity::SetCurrentFeetYaw(float yaw)
{
	//0x7C = pitch
	//0x74 = some type of radians?
	void* animstate = GetPlayerAnimState();
	if (animstate)
	{
		float* m_flCurrentFeetyaw = (float*)((DWORD)animstate + 0x84);
		*m_flCurrentFeetyaw = yaw;
	}
}

float CBaseEntity::GetGoalFeetYawServer()
{
	void* animstate = GetPlayerAnimStateServer();
	if (animstate)
	{
		return *(float*)((DWORD)animstate + m_flGoalFeetYawServer);
	}
	return 0.0f;
}

float CBaseEntity::GetGoalFeetYaw()
{
	void* animstate = GetPlayerAnimState();
	if (animstate)
	{
		return *(float*)((DWORD)animstate + 0x80);
	}
	return 0.0f;
}

void CBaseEntity::SetGoalFeetYaw(float yaw)
{
	void* animstate = GetPlayerAnimState();
	if (animstate)
	{
		float* m_flGoalFeetYaw = (float*)((DWORD)animstate + 0x80);
		*m_flGoalFeetYaw = yaw;
	}
}

float CBaseEntity::GetFriction()
{
	return *(float*)((DWORD)this + m_flFriction);
}

void CBaseEntity::SetFriction(float friction)
{
	*(float*)((DWORD)this + m_flFriction) = friction;
}

float CBaseEntity::GetStepSize()
{
	return *(float*)((DWORD)this + m_flStepSize);
}

void CBaseEntity::SetStepSize(float stepsize)
{
	*(float*)((DWORD)this + m_flStepSize) = stepsize;
}

float CBaseEntity::GetMaxSpeed()
{
	return *(float*)((DWORD)this + m_flMaxSpeed);
}

void CBaseEntity::SetMaxSpeed(float maxspeed)
{
	*(float*)((DWORD)this + m_flMaxSpeed) = maxspeed;
}

bool CBaseEntity::IsParentChanging() 
{ 
	return *(DWORD*)((DWORD)this + m_hNetworkMoveParent) != *(DWORD*)((DWORD)this + m_pMoveParent); 
}

void CBaseEntity::SetLocalVelocity(const Vector& vecVelocity)
{
	if (GetVelocity() != vecVelocity)
	{
		InvalidatePhysicsRecursive(VELOCITY_CHANGED);
		SetVelocity(vecVelocity);
	}
}