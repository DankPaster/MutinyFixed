#include "GameMemory.h"
#include "Overlay.h"
#ifdef VISTA
#define PSAPI_VERSION 1
#endif
#include <Psapi.h>
#include "CSGO_HX.h"
#include "FixMove.h"
#include "player_lagcompensation.h"
#include "LocalPlayer.h"
#include "Interfaces.h"

int ActualNumPlayers = 0;

unsigned char(*ReadByte) (uintptr_t);
int(*ReadInt) (uintptr_t);
float(*ReadFloat) (uintptr_t);
double(*ReadDouble) (uintptr_t);
short(*ReadShort) (uintptr_t);
void(*WriteByte) (uintptr_t, unsigned char);
void(*WriteInt) (uintptr_t, int);
void(*WriteFloat) (uintptr_t, float);
void(*WriteDouble) (uintptr_t, double);
void(*WriteShort) (uintptr_t, short);

HANDLE EngineHandle;
HANDLE ClientHandle;
HMODULE ThisDLLHandle;
HANDLE MatchmakingHandle;
HANDLE VPhysicsHandle;
HANDLE VSTDLIBHandle;
HANDLE SHADERAPIDX9Handle;
HANDLE DatacacheHandle;

RECT rc; //screen rectangle
HWND tWnd; //target game window
HWND hWnd; //hack window

void ClearAllPlayers()
{
	NumStreamedPlayers = 0;
	NumStreamedEnemies = 0;
	ClearHighlightedPlayer();

	for (int i = 0; i < 65; i++)
	{
		CustomPlayer *pCPlayer = &AllPlayers[i];
		if (pCPlayer->BaseEntity)
		{
			memset(pCPlayer, 0, (DWORD)&pCPlayer->Personalize.m_PlayerLowerBodyRecords - (DWORD)pCPlayer); //hard coded for now..
			gLagCompensation.ClearLagCompensationRecords(pCPlayer);
			//memset(pCPlayer, 0, sizeof(CustomPlayer));
		}
	}
}

int GetGlobalTickCount()
{
	return Interfaces::Globals->tickcount;
}

int GetExactTick(float SimulationTime)
{
	return TIME_TO_TICKS(SimulationTime + gLagCompensation.GetLerpTime());
	//Below was found to work amazingly well for shot backtracking..
	//return TIME_TO_TICKS((SimulationTime - Interfaces::Globals->interval_per_tick) + gLagCompensation.GetLerpTime()) + 1;
}

int GetTicksChoked(CustomPlayer* pCPlayer)
{
	return pCPlayer->TicksChoked;
}

#if 0
char *silver1str = new char[9]{ 41, 19, 22, 12, 31, 8, 90, 51, 0 }; /*Silver I*/
char *silver2str = new char[10]{ 41, 19, 22, 12, 31, 8, 90, 51, 51, 0 }; /*Silver II*/
char *silver3str = new char[11]{ 41, 19, 22, 12, 31, 8, 90, 51, 51, 51, 0 }; /*Silver III*/
char *silver4str = new char[10]{ 41, 19, 22, 12, 31, 8, 90, 51, 44, 0 }; /*Silver IV*/
char *silver5str = new char[13]{ 41, 19, 22, 12, 31, 8, 90, 63, 22, 19, 14, 31, 0 }; /*Silver Elite*/
char *silver6str = new char[20]{ 41, 19, 22, 12, 31, 8, 90, 63, 22, 19, 14, 31, 90, 55, 27, 9, 14, 31, 8, 0 }; /*Silver Elite Master*/
char *nova1str = new char[12]{ 61, 21, 22, 30, 90, 52, 21, 12, 27, 90, 51, 0 }; /*Gold Nova I*/
char *nova2str = new char[13]{ 61, 21, 22, 30, 90, 52, 21, 12, 27, 90, 51, 51, 0 }; /*Gold Nova II*/
char *nova3str = new char[14]{ 61, 21, 22, 30, 90, 52, 21, 12, 27, 90, 51, 51, 51, 0 }; /*Gold Nova III*/
char *nova4str = new char[17]{ 61, 21, 22, 30, 90, 52, 21, 12, 27, 90, 55, 27, 9, 14, 31, 8, 0 }; /*Gold Nova Master*/
char *mg1str = new char[18]{ 55, 27, 9, 14, 31, 8, 90, 61, 15, 27, 8, 30, 19, 27, 20, 90, 51, 0 }; /*Master Guardian I*/
char *mg2str = new char[19]{ 55, 27, 9, 14, 31, 8, 90, 61, 15, 27, 8, 30, 19, 27, 20, 90, 51, 51, 0 }; /*Master Guardian II*/
char *mg3str = new char[22]{ 55, 27, 9, 14, 31, 8, 90, 61, 15, 27, 8, 30, 19, 27, 20, 90, 63, 22, 19, 14, 31, 0 }; /*Master Guardian Elite*/
char *mg4str = new char[30]{ 62, 19, 9, 14, 19, 20, 29, 15, 19, 9, 18, 31, 30, 90, 55, 27, 9, 14, 31, 8, 90, 61, 15, 27, 8, 30, 19, 27, 20, 0 }; /*Distinguished Master Guardian*/
char *lestr = new char[16]{ 54, 31, 29, 31, 20, 30, 27, 8, 3, 90, 63, 27, 29, 22, 31, 0 }; /*Legendary Eagle*/
char *lemstr = new char[23]{ 54, 31, 29, 31, 20, 30, 27, 8, 3, 90, 63, 27, 29, 22, 31, 90, 55, 27, 9, 14, 31, 8, 0 }; /*Legendary Eagle Master*/
char *supremestr = new char[27]{ 41, 15, 10, 8, 31, 23, 31, 90, 55, 27, 9, 14, 31, 8, 90, 60, 19, 8, 9, 14, 90, 57, 22, 27, 9, 9, 0 }; /*Supreme Master First Class*/
char *globalstr = new char[17]{ 46, 18, 31, 90, 61, 22, 21, 24, 27, 22, 90, 63, 22, 19, 14, 31, 0 }; /*The Global Elite*/
#endif

//credits for bits: https://github.com/A5-/Gamerfood_CSGO/blob/38c08cde08d9ac60fb1a84ec10facc74d711bbc9/csgo_internal/csgo_internal/Util.h
#define INRANGE(x,a,b) (x >= a && x <= b) 
#define getBits( x )    (INRANGE((x&(~0x20)),'A','F') ? ((x&(~0x20)) - 'A' + 0xa) : (INRANGE(x,'0','9') ? x - '0' : 0))
#define getByte( x ) (getBits(x[0]) << 4 | getBits(x[1]))

//Dylan's own memory scanner. Requires double spaces and double ?? wildcards
uintptr_t FindMemoryPattern(HANDLE ModuleHandle, char* strpattern, int length)
{
	//double startfilter = QPCTime();
	//Filter out junk and get a clean hex version of the signature
	char *signature = new char[length + 1];
	int signaturelength = 0;
	for (int byteoffset = 0; byteoffset < length - 1; byteoffset += 2)
	{
		char charhex[4]; //4 to keep sscanf happy
		*(short*)charhex = *(short*)&strpattern[byteoffset];
		if (charhex[0] != ' ')
		{
			if (charhex[0] == '?')
			{
				signature[signaturelength] = '?';
			}
			else
			{
				//Convert ascii to hex
				charhex[2] = NULL; //add null terminator
								   //std::string strhex = std::string(charhex);
								   //signature[signaturelength] = (unsigned char)std::stoul(strhex, nullptr, 16);
				sscanf(charhex, "%x", &signature[signaturelength]);
			}
			signaturelength++;
		}
	}
	//double timetakentofilter = QPCTime() - startfilter;


	//Search for the hex signature in memory
	int searchoffset = 0;
	int maxoffset = signaturelength - 1;
	//double timetakentosearch = 0.0;
	//double startsearch = QPCTime();
#if 0
	uintptr_t adr = (uintptr_t)ModuleHandle;
	while (1)
	{
		try {
			unsigned char hextosearch = signature[searchoffset];
			bool skip = hextosearch == '?';
			if (ReadByte(adr) == hextosearch || skip)
			{
				searchoffset++;
				if (searchoffset > maxoffset)
				{
					delete[] signature;
					//timetakentosearch = QPCTime() - startsearch;
					return adr - maxoffset; //FOUND OFFSET!
				}
			}
			else
			{
				searchoffset = 0;
			}
		}
		catch (...) {
			break;
		}
		adr++;
	}
#else
	MODULEINFO dllinfo;
	GetModuleInformation(GetCurrentProcess(), (HMODULE)ModuleHandle, &dllinfo, sizeof(MODULEINFO));
	uintptr_t endadr = (uintptr_t)ModuleHandle + dllinfo.SizeOfImage;
	for (uintptr_t adr = (uintptr_t)ModuleHandle; adr < endadr; adr++)
	{
		unsigned char hextosearch = signature[searchoffset];
		bool skip = hextosearch == '?';
		if (ReadByte(adr) == hextosearch || skip)
		{
			searchoffset++;
			if (searchoffset > maxoffset)
			{
				delete[] signature;
				//timetakentosearch = QPCTime() - startsearch;
				return adr - maxoffset; //FOUND OFFSET!
			}
		}
		else
		{
			searchoffset = 0;
		}
	}
#endif
	//timetakentosearch = QPCTime() - startsearch;
	delete[] signature;
	return NULL; //NOT FOUND!
}


#if 1
#include "Interfaces.h"
//char *viewmatrixstr = new char[47]{ 59, 75, 90, 90, 69, 69, 90, 90, 69, 69, 90, 90, 69, 69, 90, 90, 69, 69, 90, 90, 66, 73, 90, 90, 60, 66, 90, 90, 74, 75, 90, 90, 77, 63, 90, 90, 75, 75, 90, 90, 76, 67, 90, 90, 57, 66, 0 }; /*A1  ??  ??  ??  ??  83  F8  01  7E  11  69  C8*/
char *viewmatrixstr = new char[51]{ 56, 67, 90, 90, 69, 69, 90, 90, 69, 69, 90, 90, 69, 69, 90, 90, 69, 69, 90, 90, 59, 75, 90, 90, 69, 69, 90, 90, 69, 69, 90, 90, 69, 69, 90, 90, 69, 69, 90, 90, 60, 60, 90, 90, 76, 74, 90, 90, 73, 66, 0 }; /*B9  ??  ??  ??  ??  A1  ??  ??  ??  ??  FF  60  38*/
DWORD ViewMatrixPtr = 0;
BOOL WorldToScreenCapped(Vector& from, Vector& to)
{
	if (!ViewMatrixPtr)
	{
		DecStr(viewmatrixstr, 50);
		DWORD ViewMatrix = FindMemoryPattern(EngineHandle, viewmatrixstr, 50);
		EncStr(viewmatrixstr, 50);
		if (!ViewMatrix)
		{
			return false;
			//THROW_ERROR(ERR_CANT_FIND_VIEWMATRIX_SIGNATURE);
			//exit(EXIT_SUCCESS);
		}
		ViewMatrixPtr = *(DWORD*)(ViewMatrix + 0x1); //0x11
	}
	//const VMatrix& WorldToScreenMatrix = (const VMatrix&) ((*(PDWORD)(*(PDWORD)ViewMatrixPtr + 0x11)) + 0x3DC);//Interfaces::Engine->WorldToScreenMatrix();
	
	//DWORD dwVMatrix = *(DWORD*)ViewMatrixPtr;
	DWORD dwVMatrix = 2 * 528; //*(DWORD*)(ViewMatrixPtr + 0x0E8) * 528;// 2 * 528;
	DWORD dwVMatrix2 = *(DWORD*)(ViewMatrixPtr + 0x0DC) - 68;
	//DWORD dwResult = dwVMatrix + 2 * 528 - 68;
	DWORD dwResult = dwVMatrix + dwVMatrix2;

#if 0
	__asm {
		mov ecx, ViewMatrixPtr;
		imul edx, dword ptr ds : [ecx + 0x0e8], 0x210;
		mov eax, dword ptr ds : [ecx + 0x0dc];
		sub eax, 68
		add eax, edx;
		mov dwResult, eax
	}
#endif

	const VMatrix& WorldToScreenMatrix = (VMatrix&)(*(DWORD_PTR*)dwResult);

	to.z = 0.0f;
	to.y = WorldToScreenMatrix.m[1][0] * from.x + WorldToScreenMatrix.m[1][1] * from.y + WorldToScreenMatrix.m[1][2] * from.z + WorldToScreenMatrix.m[1][3];
	to.x = WorldToScreenMatrix.m[0][0] * from.x + WorldToScreenMatrix.m[0][1] * from.y + WorldToScreenMatrix.m[0][2] * from.z + WorldToScreenMatrix.m[0][3];

	float w = WorldToScreenMatrix.m[3][0] * from.x + WorldToScreenMatrix.m[3][1] * from.y + WorldToScreenMatrix.m[3][2] * from.z + WorldToScreenMatrix.m[3][3];

	if (w < 0.001f) {
		return FALSE;
	}

	float invw = 1.0f / w;

	int width, height;
	Interfaces::Engine->GetScreenSize(width, height);
	float flwidth = (float)width;
	float flheight = (float)height;

	to.y *= invw;
	to.x *= invw;

	to.y = (flheight / 2.0f) - (to.y * flheight) / 2;
	to.x = (flwidth / 2.0f) + (to.x * flwidth) / 2;

	return TRUE;
}
BOOL WorldToScreen(Vector& from, Vector& to)
{
	if (!ViewMatrixPtr)
	{
		DecStr(viewmatrixstr, 50);
		DWORD ViewMatrix = FindMemoryPattern(EngineHandle, viewmatrixstr, 50);
		EncStr(viewmatrixstr, 50);
		if (!ViewMatrix)
		{
			return false;
			//THROW_ERROR(ERR_CANT_FIND_VIEWMATRIX_SIGNATURE);
			//exit(EXIT_SUCCESS);
		}
		ViewMatrixPtr = *(DWORD*)(ViewMatrix + 0x1); //0x11
	}
	//const VMatrix& WorldToScreenMatrix = (const VMatrix&) ((*(PDWORD)(*(PDWORD)ViewMatrixPtr + 0x11)) + 0x3DC);//Interfaces::Engine->WorldToScreenMatrix();

	//DWORD dwVMatrix = *(DWORD*)ViewMatrixPtr;
	DWORD dwVMatrix = 2 * 528; //*(DWORD*)(ViewMatrixPtr + 0x0E8) * 528;// 2 * 528;
	DWORD dwVMatrix2 = *(DWORD*)(ViewMatrixPtr + 0x0DC) - 68;
	//DWORD dwResult = dwVMatrix + 2 * 528 - 68;
	DWORD dwResult = dwVMatrix + dwVMatrix2;

#if 0
	__asm {
		mov ecx, ViewMatrixPtr;
		imul edx, dword ptr ds : [ecx + 0x0e8], 0x210;
		mov eax, dword ptr ds : [ecx + 0x0dc];
		sub eax, 68
			add eax, edx;
		mov dwResult, eax
	}
#endif
	const VMatrix& WorldToScreenMatrix = (VMatrix&)(*(DWORD_PTR*)dwResult);

	to.z = 0.0f;
	to.y = WorldToScreenMatrix.m[1][0] * from.x + WorldToScreenMatrix.m[1][1] * from.y + WorldToScreenMatrix.m[1][2] * from.z + WorldToScreenMatrix.m[1][3];
	to.x = WorldToScreenMatrix.m[0][0] * from.x + WorldToScreenMatrix.m[0][1] * from.y + WorldToScreenMatrix.m[0][2] * from.z + WorldToScreenMatrix.m[0][3];

	float w = WorldToScreenMatrix.m[3][0] * from.x + WorldToScreenMatrix.m[3][1] * from.y + WorldToScreenMatrix.m[3][2] * from.z + WorldToScreenMatrix.m[3][3];

	if (w < 0.001f && NoAntiAimsAreOn(false)) {
		if (AimbotFOVTxt.flValue < 180.0f)
			return FALSE;
	}

	float invw = 1.0f / w;

	int width, height;
	Interfaces::Engine->GetScreenSize(width, height);
	float flwidth = (float)width;
	float flheight = (float)height;

	to.y *= invw;
	to.x *= invw;

	to.y = (flheight * 0.5f) - (to.y * flheight) * 0.5f;
	to.x = (flwidth * 0.5f) + (to.x * flwidth) * 0.5f;

	return TRUE;
}
#else
BOOL WorldToScreenCapped(Vector& from, Vector& to)
{
	static WorldToScreenMatrix_t WorldToScreenMatrix;

	ReadIntArray((DWORD)ClientHandle + m_dwViewMatrix, (char*)&WorldToScreenMatrix, sizeof(WorldToScreenMatrix_t));

	float w = 0.0f;

	to.y = WorldToScreenMatrix.flMatrix[1][0] * from.x + WorldToScreenMatrix.flMatrix[1][1] * from.y + WorldToScreenMatrix.flMatrix[1][2] * from.z + WorldToScreenMatrix.flMatrix[1][3];
	to.x = WorldToScreenMatrix.flMatrix[0][0] * from.x + WorldToScreenMatrix.flMatrix[0][1] * from.y + WorldToScreenMatrix.flMatrix[0][2] * from.z + WorldToScreenMatrix.flMatrix[0][3];
	w = WorldToScreenMatrix.flMatrix[3][0] * from.x + WorldToScreenMatrix.flMatrix[3][1] * from.y + WorldToScreenMatrix.flMatrix[3][2] * from.z + WorldToScreenMatrix.flMatrix[3][3];

	if (w < 0.01f) {
		return FALSE;
	}

	float width = (float)(rc.right - rc.left);

	float invw = 1.0f / w;
	to.x *= invw;
	to.y *= invw;

	float height = (float)(rc.bottom - rc.top);

	float x = width / 2.0f;
	float y = height / 2.0f;

	y -= 0.5f * to.y * height + 0.5f;
	x += 0.5f * to.x * width + 0.5f;

	to.y = y + (float)rc.top;
	to.x = x + (float)rc.left;

	return TRUE;
}
BOOL WorldToScreen(Vector& from, Vector& to)
{
	static WorldToScreenMatrix_t WorldToScreenMatrix;

	ReadIntArray((DWORD)ClientHandle + m_dwViewMatrix, (char*)&WorldToScreenMatrix, sizeof(WorldToScreenMatrix_t));

	float w = 0.0f;

	to.y = WorldToScreenMatrix.flMatrix[1][0] * from.x + WorldToScreenMatrix.flMatrix[1][1] * from.y + WorldToScreenMatrix.flMatrix[1][2] * from.z + WorldToScreenMatrix.flMatrix[1][3];
	to.x = WorldToScreenMatrix.flMatrix[0][0] * from.x + WorldToScreenMatrix.flMatrix[0][1] * from.y + WorldToScreenMatrix.flMatrix[0][2] * from.z + WorldToScreenMatrix.flMatrix[0][3];
	w = WorldToScreenMatrix.flMatrix[3][0] * from.x + WorldToScreenMatrix.flMatrix[3][1] * from.y + WorldToScreenMatrix.flMatrix[3][2] * from.z + WorldToScreenMatrix.flMatrix[3][3];

	if (w < 0.01f && NoAntiAimsAreOn(false)) {
		if (AimbotMaxDistFromCrosshairTxt.iValue < 6000)
			return FALSE;
	}

	float width = (float)(rc.right - rc.left);

	float invw = 1.0f / w;
	to.x *= invw;
	to.y *= invw;

	float height = (float)(rc.bottom - rc.top);

	float x = width / 2.0f;
	float y = height / 2.0f;

	y -= 0.5f * to.y * height + 0.5f;
	x += 0.5f * to.x * width + 0.5f;

	to.y = y + (float)rc.top;
	to.x = x + (float)rc.left;

	return TRUE;
}
#endif

ServerRankRevealAllFn ServerRankRevealAllEx;

void ServerRankRevealAll()
{
	static float fArray[3] = { 0.f, 0.f, 0.f };
	ServerRankRevealAllEx(fArray);
}

wchar_t* GetPlayerNameAddress(DWORD RadarAdr, int index)
{
	return (wchar_t*)(RadarAdr + (0x1E0 * index) + 0x24);
}

void GetPlayerName(DWORD RadarAdr, int index, wchar_t* dest)
{
	//radarsize = 1e0, radarname = 24, radarpointer = 50
	//index++;
#if 0
	//Dylan's method to read, would be better with doubles possibly
	* (int*)(dest) = ReadInt(RadarAdr + (0x1E0 * index) + 0x24); //Get first four bytes

	for (int stroffset = 4; stroffset < 64; stroffset += 4)
	{//1B8, 1E0
	 //Get the rest of the bytes in the name
		*(int*)(dest + (stroffset / 2)) = ReadInt(RadarAdr + (0x1E0 * index) + 0x24 + stroffset);
	}
#else
	memcpy(dest, (void*)(RadarAdr + (0x1E0 * index) + 0x24), 64);
	//wcsncpy(dest, (wchar_t*)(RadarAdr + (0x1E0 * index) + 0x24), 32);
#endif

#if 0
	//Now filter the player name from garbage
	for (int nameind = 0; nameind < 64; nameind++)
	{
		unsigned char *val = (unsigned char*)(dest + nameind);
		if (*val == '\n' || *val == '\t')
			*val = ' ';
	}
#endif
}

void StoredNetvars::GetLocalPlayerVars()
{
	if (LocalPlayer.Entity)
	{
		LocalVars.LocalEyePos = LocalPlayer.Entity->GetEyePosition();
		LocalVars.networkorigin = LocalPlayer.Entity->GetNetworkOrigin();
		LocalVars.origin = LocalPlayer.Entity->GetOrigin();
		LocalVars.viewoffset = LocalPlayer.Entity->GetViewOffset();
	}
}