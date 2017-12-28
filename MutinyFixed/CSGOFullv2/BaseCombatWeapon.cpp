#include "BaseCombatWeapon.h"
#include "GameMemory.h"
#include "VTHook.h"


DWORD WeaponDataPtrUnknown = NULL;
DWORD WeaponDataPtrUnknownCall = NULL;
DWORD GetCSWeaponDataAdr = NULL;

int CBaseCombatWeapon::GetWeaponID()
{
	int t = ReadInt((DWORD)this + m_iWeaponID);
	return t; //*(int*)((DWORD)this + m_iWeaponID);
}

float CBaseCombatWeapon::GetNextPrimaryAttack()
{
	return ReadFloat((DWORD)this + m_flNextPrimaryAttack);//*(float*)((DWORD)this + m_flNextPrimaryAttack);
}

float CBaseCombatWeapon::GetNextSecondaryAttack()
{
	return ReadFloat((DWORD)this + m_flNextSecondaryAttack);
}

float CBaseCombatWeapon::GetPostPoneFireReadyTime()
{
	return ReadFloat((DWORD)this + m_flPostponeFireReadyTime);
}

float CBaseCombatWeapon::GetAccuracyPenalty()
{
	return ReadFloat((DWORD)this + m_fAccuracyPenalty);//*(float*)((DWORD)this + m_fAccuracyPenalty);
}

int CBaseCombatWeapon::GetXUIDLow()
{
	return ReadInt((DWORD)this + m_OriginalOwnerXuidLow);//*(int*)((DWORD)this + m_OriginalOwnerXuidLow);
}

int CBaseCombatWeapon::GetXUIDHigh()
{
	return ReadInt((DWORD)this + m_OriginalOwnerXuidHigh);// *(int*)((DWORD)this + m_OriginalOwnerXuidHigh);
}

int CBaseCombatWeapon::GetEntityQuality()
{
	return ReadInt((DWORD)this + m_iEntityQuality);//*(int*)((DWORD)this + m_iEntityQuality);
}

int CBaseCombatWeapon::GetAccountID()
{
	return ReadInt((DWORD)this + m_iAccountID);// *(int*)((DWORD)this + m_iAccountID);
}

int CBaseCombatWeapon::GetItemIDHigh()
{
	return ReadInt((DWORD)this + m_iItemIDHigh);//*(int*)((DWORD)this + m_iItemIDHigh);
}

int CBaseCombatWeapon::GetItemDefinitionIndex()
{
	return ReadInt((DWORD)this + m_iItemDefinitionIndex);// *(int*)((DWORD)this + m_iItemDefinitionIndex);
}

int CBaseCombatWeapon::GetFallbackPaintKit()
{
	return ReadInt((DWORD)this + m_nFallbackPaintKit);;// *(int*)((DWORD)this + m_nFallbackPaintKit);
}

int CBaseCombatWeapon::GetFallbackStatTrak()
{
	return ReadInt((DWORD)this + m_nFallbackStatTrak);// *(int*)((DWORD)this + m_nFallbackStatTrak);
}

float CBaseCombatWeapon::GetFallbackWear()
{
	return ReadFloat((DWORD)this + m_flFallbackWear);// *(float*)((DWORD)this + m_flFallbackWear);
}

WeaponInfo_t* CBaseCombatWeapon::GetCSWpnData()
{
	DWORD thisptr = (DWORD)this;

	if (thisptr)
	{
		DWORD* vmt = *(DWORD**)this;
		auto VMTGetWpnDataAdr = vmt[446];

		if (VMTGetWpnDataAdr == GetCSWeaponDataAdr || *(DWORD*)(thisptr + 0x3350) == 0)
		{
			DWORD ptr2 = thisptr + 0x2DB0;
			if (ptr2)
			{
				DWORD EAX = ((DWORD(__thiscall*)(DWORD))WeaponDataPtrUnknownCall)(ptr2);
				if (EAX)
				{
					DWORD ECX = EAX;

					EAX = *(DWORD*)ECX;

					DWORD ESI = *(DWORD*)WeaponDataPtrUnknown;

					DWORD result = (DWORD)((unsigned __int16(__thiscall*)(DWORD))*(DWORD*)EAX)(ECX);

					result = ((DWORD(__thiscall*)(DWORD, DWORD))*(DWORD*)(ESI + 8))(WeaponDataPtrUnknown, result);

					return (WeaponInfo_t*)result;
				}
			}
		}
	}

	//just return nullptr instead of crashing the game
	return nullptr;

	//typedef WeaponInfo_t*(__thiscall* OriginalFn)(void*);
	//return GetVFunc<OriginalFn>(this, 446)(this);
}

BOOL CBaseCombatWeapon::IsEmpty()
{
	return ReadInt((DWORD)this + m_iClip1) == 0;//*(int*)((DWORD)this + m_iClip1);
}

BOOLEAN CBaseCombatWeapon::IsReloading()
{
	return (BOOLEAN)ReadByte((DWORD)this + m_bInReload);// 0x3235); //*(bool*)((DWORD)this + 0x3235);
}

int CBaseCombatWeapon::GetClipOne()
{
	return ReadInt((uintptr_t)this + m_iClip1);
}

void CBaseCombatWeapon::UpdateAccuracyPenalty()
{
	//typedef void(__thiscall *FunctionFn)(void*);
	//reinterpret_cast<FunctionFn>(*reinterpret_cast<PDWORD>(*reinterpret_cast<PDWORD>(this) + 0x748))((void*)this);
	DWORD dwUpdateVMT = (*reinterpret_cast< PDWORD_PTR* >(this))[470]; //461
	return ((void(__thiscall*)(CBaseCombatWeapon*)) dwUpdateVMT)(this);
}

float CBaseCombatWeapon::GetWeaponCone() //GetInaccuracy
{
	typedef float(__thiscall* OriginalFn)(void*);
	return GetVFunc<OriginalFn>(this, 469)(this);
}

float CBaseCombatWeapon::GetWeaponSpread()
{
#ifdef _DEBUG
	static int shit = 439;
#ifdef DEBUG_INDEX
	static int nexttime = 0;
	int time = GetTickCount();
	if (GetAsyncKeyState(VK_HOME) && time >= nexttime)
	{
		nexttime = time + 500;
		shit++;
	}
	else if (GetAsyncKeyState(VK_END) && time >= nexttime)
	{
		nexttime = time + 500;
		shit--;
	}
#endif
#endif
	typedef float(__thiscall* OriginalFn)(void*);
#ifdef _DEBUG
	return GetVFunc<OriginalFn>(this, shit)(this);
#else
	return GetVFunc<OriginalFn>(this, 439)(this);
#endif
}

bool CBaseCombatWeapon::IsGun()
{
	int id = GetItemDefinitionIndex();
	return !IsKnife(id) && !IsGrenade(id) && id != WEAPON_C4;
#if 0
	int id = this->GetWeaponID();

	switch (id)
	{
	case WEAPON_DEAGLE:
	case WEAPON_ELITE:
	case WEAPON_FIVESEVEN:
	case WEAPON_GLOCK:
	case WEAPON_AK47:
	case WEAPON_AUG:
	case WEAPON_AWP:
	case WEAPON_FAMAS:
	case WEAPON_G3SG1:
	case WEAPON_GALILAR:
	case WEAPON_M249:
	case WEAPON_M4A1:
	case WEAPON_MAC10:
	case WEAPON_P90:
	case WEAPON_UMP45:
	case WEAPON_XM1014:
	case WEAPON_BIZON:
	case WEAPON_MAG7:
	case WEAPON_NEGEV:
	case WEAPON_SAWEDOFF:
	case WEAPON_TEC9:
		return true;
	case WEAPON_TASER:
		return false;
	case WEAPON_HKP2000:
	case WEAPON_MP7:
	case WEAPON_MP9:
	case WEAPON_NOVA:
	case WEAPON_P250:
	case WEAPON_SCAR20:
	case WEAPON_SG556:
	case WEAPON_SSG08:
		return true;
	case WEAPON_KNIFE:
	case WEAPON_FLASHBANG:
	case WEAPON_HEGRENADE:
	case WEAPON_SMOKEGRENADE:
	case WEAPON_MOLOTOV:
	case WEAPON_DECOY:
	case WEAPON_INCGRENADE:
	case WEAPON_C4:
	case WEAPON_KNIFE_T:
		return false;
	case WEAPON_M4A1_SILENCER:
	case WEAPON_USP_SILENCER:
	case WEAPON_CZ75A:
	case WEAPON_REVOLVER:
		return true;
	default:
		return false;
	}
#endif
}

std::string CBaseCombatWeapon::GetWeaponName()
{
	return "";
#if 0

	int id = this->GetWeaponID();

	switch (id)
	{
	case WEAPON_DEAGLE:
		return strenc("Desert Eagle");
	case WEAPON_ELITE:
		return strenc("Dual Berettas");
	case WEAPON_FIVESEVEN:
		return strenc("Five-SeveN");
	case WEAPON_GLOCK:
		return strenc("Glock-18");
	case WEAPON_AK47:
		return strenc("AK-47");
	case WEAPON_AUG:
		return strenc("AUG");
	case WEAPON_AWP:
		return strenc("AWP");
	case WEAPON_FAMAS:
		return strenc("FAMAS");
	case WEAPON_G3SG1:
		return strenc("G3SG1");
	case WEAPON_GALILAR:
		return strenc("Galil");
	case WEAPON_M249:
		return strenc("M249");
	case WEAPON_M4A1:
		return strenc("M4A1");
	case WEAPON_MAC10:
		return strenc("MAC-10");
	case WEAPON_P90:
		return strenc("P90");
	case WEAPON_UMP45:
		return strenc("UMP-45");
	case WEAPON_XM1014:
		return strenc("XM1014");
	case WEAPON_BIZON:
		return strenc("PP-Bizon");
	case WEAPON_MAG7:
		return strenc("MAG-7");
	case WEAPON_NEGEV:
		return strenc("Negev");
	case WEAPON_SAWEDOFF:
		return strenc("Sawed-Off");
	case WEAPON_TEC9:
		return strenc("Tec-9");
	case WEAPON_TASER:
		return strenc("Taser");
	case WEAPON_HKP2000:
		return strenc("P2000");
	case WEAPON_MP7:
		return strenc("MP7");
	case WEAPON_MP9:
		return strenc("MP9");
	case WEAPON_NOVA:
		return strenc("Nova");
	case WEAPON_P250:
		return strenc("P250");
	case WEAPON_SCAR20:
		return strenc("SCAR-20");
	case WEAPON_SG556:
		return strenc("SG 553");
	case WEAPON_SSG08:
		return strenc("SSG 08");
	case WEAPON_KNIFE:
		return strenc("Knife");
	case WEAPON_FLASHBANG:
		return strenc("Flashbang");
	case WEAPON_HEGRENADE:
		return strenc("HE Grenade");
	case WEAPON_SMOKEGRENADE:
		return strenc("Smoke Grenade");
	case WEAPON_MOLOTOV:
		return strenc("Molotov");
	case WEAPON_DECOY:
		return strenc("Decoy");
	case WEAPON_INCGRENADE:
		return strenc("Incendiary Grenade");
	case WEAPON_C4:
		return strenc("C4");
	case WEAPON_KNIFE_T:
		return strenc("Knife");
	case WEAPON_M4A1_SILENCER:
		return strenc("M4A1-S");
	case WEAPON_USP_SILENCER:
		return strenc("USP-S");
	case WEAPON_CZ75A:
		return strenc("CZ75-Auto");
	case WEAPON_REVOLVER:
		return strenc("R8 Revolver");
	default:
		return strenc("Knife");
	}

	return "";
#endif
}

bool CBaseCombatWeapon::IsPistol(int OptionalItemDefinitionIndex)
{
	int id = OptionalItemDefinitionIndex ? OptionalItemDefinitionIndex : this->GetItemDefinitionIndex();

	switch (id)
	{
	case WEAPON_GLOCK:
	case WEAPON_P250:
	case WEAPON_USP_SILENCER:
	case WEAPON_DEAGLE:
	case WEAPON_CZ75A:
	case WEAPON_TEC9:
	case WEAPON_ELITE:
	case WEAPON_FIVESEVEN:
	case WEAPON_HKP2000:
	case WEAPON_REVOLVER:
		return true;
	}
	return false;
}

bool CBaseCombatWeapon::IsShotgun(int OptionalItemDefinitionIndex)
{
	int id = OptionalItemDefinitionIndex ? OptionalItemDefinitionIndex : this->GetItemDefinitionIndex();
	if (id == WEAPON_MAG7 || id == WEAPON_SAWEDOFF || id == WEAPON_XM1014 || id == WEAPON_NOVA)
		return true;
	return false;
}


bool CBaseCombatWeapon::IsKnife(int OptionalItemDefinitionIndex)
{
	int id = OptionalItemDefinitionIndex ? OptionalItemDefinitionIndex : this->GetItemDefinitionIndex();

	return id == WEAPON_KNIFE || id == WEAPON_KNIFE_T || id >= 500;
}

bool CBaseCombatWeapon::IsSniper(bool IncludeAutoSnipers, int OptionalItemDefinitionIndex)
{
	int id = OptionalItemDefinitionIndex ? OptionalItemDefinitionIndex : this->GetItemDefinitionIndex();

	if (id == WEAPON_SSG08 || id == WEAPON_AWP)
		return true;

	if (IncludeAutoSnipers)
		return (id == WEAPON_G3SG1 || id == WEAPON_SCAR20);

	return false;
}

bool CBaseCombatWeapon::IsGrenade(int OptionalItemDefinitionIndex)
{
	int id = OptionalItemDefinitionIndex ? OptionalItemDefinitionIndex : this->GetItemDefinitionIndex();

	switch (id)
	{
	case WEAPON_MOLOTOV:
	case WEAPON_INCGRENADE:
	case WEAPON_HEGRENADE:
	case WEAPON_FLASHBANG:
	case WEAPON_SMOKEGRENADE:
	case WEAPON_DECOY:
		return true;
	}
	return false;
}