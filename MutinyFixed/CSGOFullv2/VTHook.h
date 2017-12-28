//Thanks to https://github.com/A5-/Gamerfood_CSGO
#ifndef VTHOOK_H
#define VTHOOK_H
#pragma once
#include <Windows.h>
#include "GameMemory.h"
#include "CViewSetup.h"
#include "IEngineTrace.h"
#include "TE_FireBullets.h"
#include "TE_EffectDispatch.h"
#include "imdlcache.h"

//DWORD HookGameFunc(DWORD** ppClassBase, int IndexToHook, DWORD ReplacementFunction);

class VTHook
{
public:
	VTHook()
	{
		memset(this, 0, sizeof(VTHook));
	}

	VTHook(PDWORD* ppdwClassBase)
	{
		bInitialize(ppdwClassBase);
	}

	~VTHook()
	{
		if (m_ClassBase)
		{
			//*m_ClassBase = m_OldVT;
			WriteInt(ReadInt((DWORD)m_ClassBase), (int)m_OldVT);
		}
	}

	void ClearClassBase()
	{
		m_ClassBase = NULL;
	}

	void /*bool*/ bInitialize(PDWORD* ppdwClassBase)
	{
		m_ClassBase = ppdwClassBase; // Get Pointer to the class base and store in member variable of current object
		//m_OldVT = *ppdwClassBase; // Get Pointer the the Old Virtual Address Table
		m_OldVT = (PDWORD)ReadInt((DWORD)ppdwClassBase);
		m_VTSize = GetVTCount(m_OldVT);//*ppdwClassBase); // Get the number of functions in the Virtual Address Table
		m_NewVT = new DWORD[m_VTSize]; // Create A new virtual Address Table
		//memcpy(m_NewVT, m_OldVT, sizeof(DWORD) * m_VTSize);
		WriteByteArray((char*)m_NewVT, (char*)m_OldVT, sizeof(DWORD) * m_VTSize); //Use driver to write instead of hack
		//*ppdwClassBase = m_NewVT; // replace the old virtual address table pointer with a pointer to the newly created virtual address table above
		WriteInt(ReadInt((DWORD)&ppdwClassBase), (int)m_NewVT);		
		//return true;
	}
	void /*bool*/ bInitialize(PDWORD** pppdwClassBase) // fix for pp
	{
		//return bInitialize(*pppdwClassBase);
		bInitialize(*pppdwClassBase);
	}

	/*
	void ReHook()
	{
		if (m_ClassBase)
		{
			*m_ClassBase = m_NewVT;
		}
	}
	*/
	/*
	int iGetFuncCount()
	{
		return (int)m_VTSize;
	}
	*/
	/*
	DWORD GetFuncAddress(int Index)
	{
		if (Index >= 0 && Index <= (int)m_VTSize && m_OldVT != NULL)
		{
			return m_OldVT[Index];
		}
		return NULL;
	}
	*/

	/*
	PDWORD GetOldVT()
	{
		return m_OldVT;
	}
	*/

	DWORD HookFunction(DWORD dwNewFunc, unsigned int iIndex)
	{
		// Noel: Check if both the New Virtual Address Table and the the Old one have been allocated and exist,
		// also check if the function index into the New Virtual Address Table is within the bounds of the table
		if (m_NewVT && m_OldVT && iIndex <= m_VTSize && iIndex >= 0)
		{
			m_NewVT[iIndex] = dwNewFunc; // Set the new function to be called in the new virtual address table
			return ReadInt((DWORD)m_OldVT + (sizeof(DWORD*) * iIndex));
			//return m_OldVT[iIndex]; // Return the old function to be called in the virtual address table
		}
		// Return null and fail if the new Virtual Address Table or the old one hasn't been allocated with = new,
		// or if the index chosen is outside the bounds of the table.
		return NULL;
	}

private:

	//Use standard C++ functions instead
	BOOL CodePointerIsInvalid(DWORD* testptr) {
#if 0
		bool Invalid = false;
		int test;
		try{
			test = ReadInt((DWORD)testptr);
		}catch(...){
			Invalid = true;
		}
		return Invalid;
#else

	return IsBadCodePtr((FARPROC)testptr);
#endif
	}
	DWORD GetVTCount(PDWORD pdwVMT)
	{
		DWORD dwIndex = NULL;

		for (dwIndex = 0; pdwVMT[dwIndex]; dwIndex++)
			if (!pdwVMT[dwIndex])
				break;
		
		/*
		for (dwIndex = 0; pdwVMT[dwIndex] != 0; dwIndex++)
		{
			//if (ShitPointer((DWORD*)pdwVMT[dwIndex]))
			//break;

			if (CodePointerIsInvalid((DWORD*)ReadInt((DWORD)pdwVMT + (sizeof(DWORD*) * dwIndex))))
				break;

			//Original code, uses windows api function

			//if (IsBadCodePtr((FARPROC)pdwVMT[dwIndex]))
			//{
			//break;
			//}
		}
		*/
		return dwIndex;
	}
	PDWORD*	m_ClassBase;
	PDWORD	m_NewVT, m_OldVT;
	DWORD	m_VTSize;
};

extern VTHook*	VPanel;
extern VTHook*	ClientMode;
extern VTHook*	Client;
extern VTHook*  Trace;
extern VTHook*	Prediction;
extern VTHook*  Engine;
extern VTHook* TE_FireBullets;
extern VTHook* HRenderView;
extern VTHook* EngineClientSound;
extern VTHook* DirectX;
extern VTHook* DemoPlayer;
extern VTHook* HNetchan;
extern VTHook* HSurface;
//extern VTHook*	ModelRender;
//extern VTHook*	Surface;
//extern VTHook*  D3D9;
void SetupVMTHooks();
extern float(*RandomFloat) (float from, float to);
extern void (*RandomSeed) (unsigned int seed);

template <typename T>
T GetVFunc(void *vTable, int iIndex) {
	return (*(T**)vTable)[iIndex];
}

using OverrideViewFN = void(__stdcall*)(CViewSetup*);
extern OverrideViewFN oOverrideView;
using CreateMoveFn = bool(__stdcall*)(float, CUserCmd*);
extern CreateMoveFn oCreateMove; //bool(__stdcall *oCreateMove)(float, CUserCmd*);
using FrameStageNotifyFn =  void(__thiscall*)(void*, ClientFrameStage_t);
extern FrameStageNotifyFn oFrameStageNotify;
using IDrawModelExecuteFn = void(__thiscall*)(void*, void*, void*, const ModelRenderInfo_t&, matrix3x4_t*);
extern IDrawModelExecuteFn oIDrawModelExecute;

using TraceRayFn = void(__thiscall *)(void *, Ray_t const &, unsigned int, CTraceFilter *, trace_t &);
extern TraceRayFn oTraceRay;
using RunCommandFn = void (__thiscall *)(void *, CBaseEntity* , CUserCmd*, void*);
extern RunCommandFn oRunCommand;
using GetLastTimeStampFn = float(__thiscall *)(void);
extern GetLastTimeStampFn oGetLastTimeStamp;
using TE_FireBullets_PostDataUpdateFn = void (__thiscall*)(C_TEFireBullets*, DataUpdateType_t);
extern TE_FireBullets_PostDataUpdateFn oTE_FireBullets_PostDataUpdate;
using TE_EffectDispatch_PostDataUpdateFn = void(__thiscall*)(C_TEEffectDispatch*, DataUpdateType_t);
extern TE_EffectDispatch_PostDataUpdateFn oTE_EffectDispatch_PostDataUpdate;
using LookupPoseParameterFn = int(__stdcall*)(CBaseEntity* me, const char* str);
extern LookupPoseParameterFn LookupPoseParameterGame;
using GetPoseParameterRangeFn = bool(__thiscall*)(CBaseEntity* me, int index, float& minValue, float& maxValue);
extern GetPoseParameterRangeFn GetPoseParameterRangeGame;
using SetPoseParameterFn = float (__thiscall*)(CBaseEntity* me, /*CStudioHdr*/ void *pStudioHdr, int iParameter, float flValue);
extern SetPoseParameterFn SetPoseParameterGame;
using GetViewModelFOVFn = float(__stdcall *)();
extern GetViewModelFOVFn oGetViewModelFOV;
using UpdateClientSideAnimationFn = void(__thiscall*)(CBaseEntity* me);
#if 0
using SetViewAnglesFn = void(__stdcall *) (QAngle &va);
extern SetViewAnglesFn oSetViewAngles;
#endif
using IsPausedFn = bool(__stdcall *)();
extern IsPausedFn oIsPaused;
using IsPlayingBackFn = bool(__thiscall *)(DWORD demoplayerptr);
extern IsPlayingBackFn oIsDemoPlayingBack;
using SendDatagramFn = int(__thiscall*)(void*, void *);
extern SendDatagramFn oSendDatagram;
using EmitSoundFn = void(__fastcall *)(void* ecx, void* edx, void* filter, int iEntIndex, int iChannel, const char *pSoundEntry, unsigned int nSoundEntryHash, const char *pSample, float flVolume, float flAttenuation, int nSeed, int iFlags, int iPitch, const Vector *pOrigin, const Vector *pDirection, Vector * pUtlVecOrigins, bool bUpdatePositions, float soundtime, int speakerentity);
extern EmitSoundFn oEmitSound;
using PlaySoundFn = void(__thiscall*)(void*, const char*);
extern PlaySoundFn oPlaySound;
using PaintTraverseFn = void(__thiscall*)(void*, unsigned int, bool, bool);
extern PaintTraverseFn oPaintTraverse;
#ifdef _DEBUG
using FrameUpdatePostEntityThinkFn = void(__fastcall*)(void*);
extern FrameUpdatePostEntityThinkFn oLagCompFrameUpdatePostEntityThink;
#endif

//float SetPoseParameter(CBaseEntity* me, void*pStudioHDR, int iParameter, float flValue, float *flValueResult);


typedef void(__thiscall *RenderViewFn)(void*, CViewSetup &, CViewSetup &, int, int);
extern RenderViewFn oRenderView;

using ShouldDrawLocalPlayerFn = bool(__fastcall*)(void*, void*, CBaseEntity*);
extern ShouldDrawLocalPlayerFn oShouldDrawLocalPlayer;

namespace Hooks
{
	extern bool __stdcall CreateMove(float flInputSampleTime, CUserCmd* cmd);
	extern void __stdcall OverrideView(CViewSetup* setup);
	extern void __fastcall FrameStageNotify(void* ecx, void*, ClientFrameStage_t stage);
	extern void __fastcall DrawModelExecute(void* thisptr, int edx, void* ctx, void* state, const ModelRenderInfo_t &pInfo, matrix3x4_t *pCustomBoneToWorld);
	extern void __fastcall TraceRay(void *ths, void*, Ray_t const &ray, unsigned int mask, CTraceFilter *filter, trace_t &trace);
	extern void __stdcall RunCommand(CBaseEntity* pEntity, CUserCmd* pUserCmd, void* moveHelper);
	extern float __stdcall GetLastTimeStamp(void);
	extern void __stdcall TE_FireBullets_PostDataUpdate(DataUpdateType_t updateType);
	extern void __fastcall TE_EffectDispatch_PostDataUpdate(C_TEEffectDispatch*, void* edx, DataUpdateType_t updateType);
	extern void __fastcall RenderView(void* thisptr, void* edx, CViewSetup &setup, CViewSetup &hudViewSetup, int nClearFlags, int whatToDraw);
	extern bool __fastcall ShouldDrawLocalPlayer(void* ecx, void* edx, CBaseEntity *pPlayer);
	//extern void __stdcall SetViewAngles(QAngle &va);
	extern bool __stdcall IsPaused();
	extern bool __fastcall DemoIsPlayingBack(void* thisptr);
	extern int __fastcall SendDatagram(void* netchan, void*, void *datagram);
	extern void __fastcall EmitSound(void* ecx, void* edx, void* filter, int iEntIndex, int iChannel, const char *pSoundEntry, unsigned int nSoundEntryHash, const char *pSample, float flVolume, float flAttenuation, int nSeed, int iFlags, int iPitch, const Vector *pOrigin, const Vector *pDirection, Vector * pUtlVecOrigins, bool bUpdatePositions, float soundtime, int speakerentity);
	extern void __fastcall HookedPlaySound(void* ecx, void* edx, const char* filename);
	extern void __fastcall PaintTraverse(void* thisptr, void* edx, unsigned int panel, bool forceRepaint, bool allowForce);
#ifdef _DEBUG
	extern void __fastcall LagComp_FrameUpdatePostEntityThink(void* thisptr);
#endif
}

extern uintptr_t pMoveHelperServerPP;
extern DWORD g_pGameRules;
extern char GameRulesShouldCollideOffset;
extern UINT(__cdecl* MD5_PseudoRandom)(UINT);
extern DWORD AdrOfSetLastTimeStampInterpolate;
extern DWORD AdrOfIsPausedExtrapolate;
extern DWORD AdrOfSetLastTimeStampFSN;
extern DWORD AdrOfs_bInterpolate;
extern DWORD AdrOfInvalidateBoneCache;
extern DWORD AdrOf_m_iDidCheckForOcclusion;
extern DWORD AdrOf_m_nWritableBones;
extern DWORD AdrOf_m_dwOcclusionArray;
extern DWORD AdrOf_StandardFilterRulesCallOne;
extern DWORD AdrOf_StandardFilterRulesMemoryOne;
extern DWORD AdrOf_StandardFilterRulesMemoryTwo;
extern DWORD AdrOf_SetAbsOrigin;
extern DWORD AdrOf_SetAbsAngles;
extern DWORD AdrOf_SetAbsVelocity;
extern DWORD AdrOf_DataCacheSetPoseParmaeter;
extern bool *s_bOverridePostProcessingDisable;
extern DWORD OffsetOf_UpdateClientSideAnimation;
extern DWORD AdrOf_InvalidatePhysicsRecursive;
extern DWORD AdrOf_Frametime1;
extern DWORD AdrOf_Frametime2;
extern DWORD AdrOf_Frametime3;
extern DWORD* ClientCount;
extern DWORD ClientList;
extern DWORD AdrOf_NET_SendPacket;
extern DWORD AdrOf_CNET_SendData;
extern DWORD AdrOf_CNET_SendDatagram;
extern unsigned long GlowObjectManagerAdr;
extern unsigned long RadarBaseAdr;
extern DWORD DemoPlayerVTable;
extern DWORD DemoPlayerCreateMoveReturnAdr;
extern DWORD SendDatagramCL_MoveReturnAdr;
extern DWORD IsEntityBreakable_FirstCall_Arg1;
extern DWORD IsEntityBreakable_FirstCall_Arg2;
extern DWORD IsEntityBreakable_SecondCall_Arg1;
extern DWORD IsEntityBreakable_SecondCall_Arg2;
extern DWORD AdrOf_IsEntityBreakableCall;

inline DWORD GetServerClientEntity(int i)
{
	DWORD EAX = *(DWORD*)ClientList;
	DWORD EBX = *(DWORD*)((i * 4) + EAX);

#if 1
	if (!EBX)
		return NULL;

	return (EBX + 4);
#endif

#if 0
	DWORD val = *(DWORD*)(*(DWORD*)ClientList + (4 * i));
	if (!val)
		return NULL;

	return (val + 4);
#endif
}

//CBaseEntity::Instance(edict_t*)
inline CBaseEntity* ServerClientToEntity(DWORD cl)
{
	cl -= 4;

	DWORD edict = *(DWORD*)(cl + 0x48C);

	DWORD ecx = *(DWORD*)(edict + 0x0C);
	if (ecx)
	{
		DWORD EAX = *(DWORD*)ecx;
		return ((CBaseEntity*(__thiscall*)(DWORD))( *(DWORD*)(EAX + 0x14)))(ecx);
	}

	return nullptr;
}

inline int GetServerClientCount()
{
	return *ClientCount;
}

inline int GetClientCount()
{
	return 64;
	//return ActualNumPlayers;
}

#endif