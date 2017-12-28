#include "Trace.h"
#include "Interfaces.h"
#include "VTHook.h"

bool CGameTrace::DidHitWorld() const
{
	return m_pEnt == Interfaces::ClientEntList->GetClientEntity(0);
}

CTraceFilterSimple::CTraceFilterSimple(IHandleEntity *passentity, int collisionGroup)
{
	m_pPassEnt = passentity;
	m_collisionGroup = collisionGroup;
}

CTraceFilterSkipTwoEntities::CTraceFilterSkipTwoEntities(IHandleEntity *pPassEnt1, IHandleEntity *pPassEnt2, int collisionGroup)
{
	pSkip = pPassEnt1;
	m_pPassEnt2 = pPassEnt2;
	m_icollisionGroup = collisionGroup;
}

bool __fastcall StandardFilterRules(IHandleEntity* pHandleEntity, int fContentsMask)
{
	CBaseEntity* pCollide = (CBaseEntity*)pHandleEntity; //(*(CBaseEntity*(**)(IHandleEntity*))(*(DWORD *)pHandleEntity + 0x1C))(pHandleEntity);

	// Static prop case...
	if (!pCollide)
		return true;

	SolidType_t solid = pCollide->GetSolid();

	const model_t *pModel = pCollide->GetModel();

	if ((Interfaces::ModelInfoClient->GetModelType(pModel) != mod_brush) || (solid != SOLID_BSP && solid != SOLID_VPHYSICS))
	{
		if ((fContentsMask & CONTENTS_MONSTER) == 0)
			return false;
	}
	// This code is used to cull out tests against see-thru entities
	if (!(fContentsMask & CONTENTS_WINDOW))
	{
		//if (pCollide->IsTransparent())
			//return false;

		//easy to read format
		//register DWORD EAX = *(DWORD*)((DWORD)pCollide + 4);
		//register DWORD ECX = ((DWORD)pCollide + 4);
		//unsigned short v10 = *((unsigned short*(__thiscall *)(DWORD)) *(DWORD*)(EAX + 0x1C))(ECX);
		unsigned short v10 = *((unsigned short*(__thiscall *)(DWORD)) *(DWORD*)(*(DWORD*)((DWORD)pCollide + 4) + 0x1C))(((DWORD)pCollide + 4));
			
		if (v10 == (unsigned short)(-1))
		{
			DWORD v9 = ((int(__thiscall*)(CBaseEntity*))AdrOf_StandardFilterRulesCallOne)(pCollide);
			if (v9)
			{
				//easy to read format
				//ECX = *(DWORD*)(v9 + 4);
				//EAX = *(DWORD*)ECX;
				//v10 = *((unsigned short*(__thiscall *)(DWORD)) *(DWORD*)(EAX + 0x1C))(ECX);
				DWORD ECX = *(DWORD*)(v9 + 4);
				v10 = *((unsigned short*(__thiscall *)(DWORD)) *(DWORD*)(*(DWORD*)ECX + 0x1C))(ECX);
			}
		}
		//easy to read format
		//EAX = *(DWORD*)AdrOf_StandardFilterRulesMemoryOne;
		//BOOL transparent = ((bool(__thiscall *)(DWORD, DWORD)) *(DWORD*)(EAX + 0x80)) (AdrOf_StandardFilterRulesMemoryOne, (DWORD)v10);
		BOOLEAN transparent = ((BOOLEAN(__thiscall *)(DWORD, DWORD)) *(DWORD*)(*(DWORD*)AdrOf_StandardFilterRulesMemoryOne + 0x80)) (AdrOf_StandardFilterRulesMemoryOne, (DWORD)v10);
		if (transparent)
			return false;
	}

	// FIXME: this is to skip BSP models that are entities that can be 
	// potentially moved/deleted, similar to a monster but doors don't seem to 
	// be flagged as monsters
	// FIXME: the FL_WORLDBRUSH looked promising, but it needs to be set on 
	// everything that's actually a worldbrush and it currently isn't
	if (!(fContentsMask & CONTENTS_MOVEABLE) && (pCollide->GetMoveType() == MOVETYPE_PUSH))// !(touch->flags & FL_WORLDBRUSH) )
		return false;

	return true;
}

bool PassServerEntityFilter(const IHandleEntity *pTouch, const IHandleEntity *pPass)
{
	if (!pPass)
		return true;

	if (pTouch == pPass)
		return false;

	CBaseEntity *pEntTouch = (CBaseEntity*)pTouch;//EntityFromEntityHandle(pTouch);
	CBaseEntity *pEntPass = (CBaseEntity*)pPass;//EntityFromEntityHandle(pPass);
	if (!pEntTouch || !pEntPass)
		return true;

	// don't clip against own missiles
	if (pEntTouch->GetOwner() == pEntPass)
		return false;

	// don't clip against owner
	if (pEntPass->GetOwner() == pEntTouch)
		return false;


	return true;
}

bool BaseShouldHitEntity(IHandleEntity* pSkip, IHandleEntity *pHandleEntity, int m_collisionGroup, int contentsMask)
{
	if (!StandardFilterRules(pHandleEntity, contentsMask))
		return false;

	if (pSkip)
	{
		if (!PassServerEntityFilter(pHandleEntity, pSkip))
		{
			return false;
		}
	}

	// Don't test if the game code tells us we should ignore this collision...
	CBaseEntity *pEntity = (CBaseEntity*)pHandleEntity;//EntityFromEntityHandle(pHandleEntity);
	if (!pEntity)
		return false;
	if (!pEntity->ShouldCollide(m_collisionGroup, contentsMask))
		return false;
	if (/*pEntity && */!GameRulesShouldCollide(m_collisionGroup, pEntity->GetCollisionGroup()))
		return false;

	return true;
}

BOOLEAN GameRulesShouldCollide(int collisionGroup0, int collisionGroup1)
{
#if 1
	__asm {
		push collisionGroup1
		push collisionGroup0
		mov ecx, g_pGameRules
		mov ecx, [ecx]
		mov eax, [ecx]
		mov eax, [eax + 0x70]
		call eax
	}
#endif

	//return ((BOOLEAN(__stdcall*)(int, int))(ReadInt(ReadInt(ReadInt(g_pGameRules)) + GameRulesShouldCollideOffset)))(collisionGroup0, collisionGroup1);
}