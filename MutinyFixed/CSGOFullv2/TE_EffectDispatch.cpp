#include "TE_EffectDispatch.h"
#include "VTHook.h"
#include "SpawnBlood.h"
#include "VMProtectDefs.h"

TE_EffectDispatch_PostDataUpdateFn oTE_EffectDispatch_PostDataUpdate;

void __fastcall Hooks::TE_EffectDispatch_PostDataUpdate(C_TEEffectDispatch* thisptr, void* edx, DataUpdateType_t updateType)
{
	VMP_BEGINMUTILATION("EFDPDU")
	if (!DisableAllChk.Checked)
	{ 
		//decimal
		//4 = csblood
		//34 = Impact
		switch (thisptr->m_EffectData.m_iEffectName)
		{
		case 4:
			OnSpawnBlood(thisptr, updateType);
			return;
		default:
			oTE_EffectDispatch_PostDataUpdate(thisptr, updateType);
			return;
		}
	}
	else
	{
		oTE_EffectDispatch_PostDataUpdate(thisptr, updateType);
	}
	VMP_END
}