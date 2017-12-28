// CSGO_HX.cpp : Defines the exported functions for the DLL application.
//
#ifdef LICENSED
#include "Licensing.h"
#endif
#include "CSGO_HX.h"
//#include <TlHelp32.h>

#include "CreateMove.h"
#include "EncryptString.h"
#include "LocalPlayer.h"
#include "VMProtectDefs.h"
#include "player_lagcompensation.h"

LARGE_INTEGER g_PerformanceFrequency;
LARGE_INTEGER g_MSPerformanceFrequency;
LARGE_INTEGER g_ClockStart;
bool LevelIsLoaded = false;
Vector vecZero = Vector(0.0f, 0.0f, 0.0f);
QAngle angZero = QAngle(0.0f, 0.0f, 0.0f);

void InitTime()
{
	if (!g_PerformanceFrequency.QuadPart)
	{
		QueryPerformanceFrequency(&g_PerformanceFrequency);
		g_MSPerformanceFrequency.QuadPart = g_PerformanceFrequency.QuadPart / 1000;
		QueryPerformanceCounter(&g_ClockStart);
	}
}

//Returns curtime in seconds
double QPCTime()
{
	//InitTime();

	LARGE_INTEGER CurrentTime;

	QueryPerformanceCounter(&CurrentTime);

	return (double)(CurrentTime.QuadPart - g_ClockStart.QuadPart) / (double)(g_PerformanceFrequency.QuadPart);
}

__declspec (naked) unsigned int GuidToInt(const char * guidstr)
{
	__asm {
		//Mov [Esp-8], Edi
		//Mov Edi, [Esp+4]
		//Mov Ecx, -1
		//Mov Al, 0
		//Repne Scasb
		//Neg Ecx
		//Sub Ecx, 6
		Mov[Esp - 4], EBX
			Mov[Esp - 8], ECX
			Mov Ebx, [Esp + 4]
			Mov Ecx, 32
			Mov Eax, 0
		HashLoop:
		Xor Eax, [Ebx + Ecx]
			Sub Ecx, 4
			Jns HashLoop
			Mov Ebx, [Esp - 4]
			Mov Ecx, [Esp - 8]
			Retn //4
	}
}

char *regkeystr2 = new char[32]{ 41, 53, 60, 46, 45, 59, 40, 63, 38, 55, 19, 25, 8, 21, 9, 21, 28, 14, 38, 57, 8, 3, 10, 14, 21, 29, 8, 27, 10, 18, 3, 0 }; /*SOFTWARE\Microsoft\Cryptography*/
char *guidstr2 = new char[12]{ 55, 27, 25, 18, 19, 20, 31, 61, 15, 19, 30, 0 }; /*MachineGuid*/
unsigned int GetHWID(void) {
	HKEY hkey;              // Handle to registry key
	DecStr(regkeystr2, 31);
	DecStr(guidstr2, 11);
	if (RegOpenKeyExA(HKEY_LOCAL_MACHINE, regkeystr2, NULL, KEY_QUERY_VALUE | KEY_WOW64_64KEY, &hkey) == ERROR_SUCCESS) {
		char data[256];
		unsigned long datalen = 255;  // data field length(in), data returned length(out) 
		unsigned long datatype; // #defined in winnt.h (predefined types 0-11)
		// Datalen is an in/out field, so reset it for every query
		// I've ignored displays in the event of an error ...
		// The error is most likely to be a misspelled value field name
		if (RegQueryValueExA(hkey, guidstr2, NULL, &datatype, (LPBYTE)data, &datalen) == ERROR_SUCCESS) {
			//printf("GUID:\n%s\n",data);
			if (datalen == 37) {
				EncStr(guidstr2, 11);
				EncStr(regkeystr2, 31);
				return GuidToInt(data);
			}
			//printf("Hash:\n%d", hash);
		}
		// That's all for this key. Close it.
		RegCloseKey(hkey);
	} /*else {
	  GetLastError();
	  }*/
	EncStr(guidstr2, 11);
	EncStr(regkeystr2, 31);
	return 0;
}

//HFONT DEFAULT_FONT = CreateFontA(18, 0, 0, 0, 600, false, false, false, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DRAFT_QUALITY, DEFAULT_PITCH, "Verdana");
//void (*TraceRay) (Ray_t const&, unsigned int, ITraceFilter *, CGameTrace *);
//EngineTrace *p_EngineTrace = nullptr;

double curtime = 0.0f;
double lasttime = 0.0f;
float Time = 0.0f;
//float DeltaTime = 0.0f;
int* InvalidGame;
bool FoundCSGO = false;
//bool EnableHack;
Vector CenterOfScreen;
bool AllocedConsole = false;

void AllocateConsole()
{
	if (!AllocedConsole)
	{
		AllocConsole();
		FILE* fp;
		freopen_s(&fp, "CONOUT$", "w", stdout);
		AllocedConsole = true;
	}
}

HANDLE FindHandle(std::string name)
{
	return GetModuleHandle(name.c_str());
}

//int AIMBOT_MAXDIST_FROM_CROSSHAIR = INT_MAX;//100;
//#define TRACE_RAY_OFFSET 0x208BB0; //engine + offset

struct CustomPlayer AllPlayers[65] { 0 }; //Entire table from 0 to MAX_PLAYERS, including unstreamed players
struct CustomPlayer* StreamedPlayers[65] { 0 }; //Starts from 0, ends at number of streamed players found, use index of NumStreamedPlayers

int LastTargetIndex = INVALID_PLAYER;
float NextTimeBoneGiveUp = 0.0f;
int LastBoneChecked = 6; //todo safety fix: initialize to 6 for head?
int LastBoneCheckIndex = 0;
Vector TargetMousePosition = vecZero;

//DWORD ClientState; //not needed anymore
CBaseEntity* LocalEntity;
int NumStreamedPlayers;
int NumStreamedEnemies;

void DoWork()
{
#ifndef NO_DIRECTX
	CheckMenuButtons();
#endif
}

char *clientdllstr = new char[11]{ 25, 22, 19, 31, 20, 14, 84, 30, 22, 22, 0 }; /*client.dll*/
char *matchmakingdllstr = new char[16]{ 23, 27, 14, 25, 18, 23, 27, 17, 19, 20, 29, 84, 30, 22, 22, 0 }; /*matchmaking.dll*/
char *vphysicsdllstr = new char[13]{ 12, 10, 18, 3, 9, 19, 25, 9, 84, 30, 22, 22, 0 }; /*vphysics.dll*/
char *vstdlibdllstr = new char[12]{ 12, 9, 14, 30, 22, 19, 24, 84, 30, 22, 22, 0 }; /*vstdlib.dll*/
char *enginedllstr = new char[11]{ 31, 20, 29, 19, 20, 31, 84, 30, 22, 22, 0 }; /*engine.dll*/
char *shaderapistr = new char[17]{ 9, 18, 27, 30, 31, 8, 27, 10, 19, 30, 2, 67, 84, 30, 22, 22, 0 }; /*shaderapidx9.dll*/
char *datacachestr = new char[14]{ 30, 27, 14, 27, 25, 27, 25, 18, 31, 84, 30, 22, 22, 0 }; /*datacache.dll*/

DWORD InitialRenderThread = 0;
float ResetRenderThreadTime = 0.0f;

//Function the driver will call instead of making a thread
#ifdef ATI
void __stdcall ATIThink(void)
#else
void __stdcall NVThink(void)
#endif
{
	VMP_BEGINMUTILATION("NVT")
	auto stat = START_PROFILING("NVThink/ATIThink");
#if 0
	static bool called = false;
	if (!called)
	{
		called = true;
		Sleep(5000);
	}
#endif
	static bool FirstCall = true;
	if (FirstCall)
	{
		char* cmdline = GetCommandLineA();
		EXTERNAL_WINDOW = strstr(cmdline, "-benchmark") != nullptr;
		FirstCall = false;
	}
	//Whenever we call DirectX functions, a different thread calls this function, so don't allow any other thread.

	//if (EXTERNAL_WINDOW)
	{
		DWORD currentthread = GetCurrentThreadId();
		if (InitialRenderThread == 0)
		{
			InitialRenderThread = currentthread;
		}
		else if (InitialRenderThread == -1)
		{
			if (QPCTime() - ResetRenderThreadTime > 5.0f)
			{
				InitialRenderThread = currentthread;
			}
		}
		else
		{
			if (currentthread != InitialRenderThread)
			{
				END_PROFILING(stat);
				return;
			}
		}
	}
	curtime = QPCTime();
	double realdelta = curtime - lasttime;
	float dt = (float)fmin(realdelta, 0.5);
	Time += dt;
	lasttime = curtime;
	//DeltaTime = dt;

	//TODO: Clean up this code below
	if (!FoundCSGO)
	{
		static float starttime = 0.0f;
		if (starttime == 0.0f)
			starttime = Time;
		if (Time - starttime < 120.0f)
		{
			if (!ClientHandle)
			{
				DecStr(clientdllstr, 10);
				ClientHandle = FindHandle(clientdllstr);
				EncStr(clientdllstr, 10);
			}
			if (!MatchmakingHandle)
			{
				DecStr(matchmakingdllstr, 15);
				MatchmakingHandle = FindHandle(matchmakingdllstr);
				EncStr(matchmakingdllstr, 15);
			}
			if (!VPhysicsHandle)
			{
				DecStr(vphysicsdllstr, 12);
				VPhysicsHandle = FindHandle(vphysicsdllstr);
				EncStr(vphysicsdllstr, 12);
			}
			if (!VSTDLIBHandle)
			{
				DecStr(vstdlibdllstr, 11);
				VSTDLIBHandle = FindHandle(vstdlibdllstr);
				EncStr(vstdlibdllstr, 11);
			}
			if (!SHADERAPIDX9Handle)
			{
				DecStr(shaderapistr, 16);
				SHADERAPIDX9Handle = FindHandle(shaderapistr);
				EncStr(shaderapistr, 16);
			}
			if (!DatacacheHandle)
			{
				DecStr(datacachestr, 13);
				DatacacheHandle = FindHandle(datacachestr);
				EncStr(datacachestr, 13);
			}
			if (!ClientHandle || !MatchmakingHandle || !VPhysicsHandle || !VSTDLIBHandle || !SHADERAPIDX9Handle || !DatacacheHandle)
			{
				END_PROFILING(stat);
				return;
			}
			else
			{
				static float TimeFoundCSGO = Time;
#ifdef _DEBUG
				if (Time - TimeFoundCSGO > 1.0f)
#else
#ifdef MUTINY
				if (Time - TimeFoundCSGO > 2.5f)
#else
				if (Time - TimeFoundCSGO > 8.0f)
#endif
#endif
				{
					SetupVMTHooks();
					delete[]clientdllstr;
					delete[]enginedllstr;
					delete[]matchmakingdllstr;
					delete[]vphysicsdllstr;
					delete[]vstdlibdllstr;
					delete[]shaderapistr;
					delete[]datacachestr;
					FoundCSGO = true;
				}
			}
		}
		else
		{
			//Failed to find engine.dll or matchmaking.dll in a reasonable amount of time, abort and assume it's some other game!
			*InvalidGame = -1;
			FoundCSGO = true;
			if (ThisDLLHandle)
			{
				delete[]clientdllstr;
				delete[]enginedllstr;
				delete[]matchmakingdllstr;
				delete[]vphysicsdllstr;
				delete[]vstdlibdllstr;
				delete[]shaderapistr;
				delete[]datacachestr;
				FreeLibrary(ThisDLLHandle);
			}
		}
	}
	else
	{
		static float NextPanicButtonTime = 0.0f;
		//Global panic key, instead of having to enter the menu
		if (GetAsyncKeyState(VK_INSERT))
		{
			if (Time >= NextPanicButtonTime)
			{
				DisableAllChk.Checked = !DisableAllChk.Checked;
				NextPanicButtonTime = Time + 0.3f;
			}
		}
		if (GotCSGORect)
		{
			//Totally done initializing.. Do the actual Think now

			//We are converting to ULONG so compiler optimizes the division by bit shifting
			GetWindowRect(tWnd, &rc);
			CenterOfScreen = { (float)((ULONG)rc.right / 2), (float)((ULONG)rc.bottom / 2), 0.0f };

			const char *LevelName = Interfaces::Engine->GetLevelName();
			if (!LevelName || (LevelName && (*(char*)LevelName == 0x0)))
			{
				flTimeDamageWasGiven = 0.0f;
				LocalPlayer.Entity = nullptr;
				LevelIsLoaded = false;
				ClearAllPlayers();
				SetLastAngleFirstTime = false;
				RecoilCrosshairScreen = { CenterOfScreen.x, CenterOfScreen.y };
				gLagCompensation.ClearIncomingSequences();
			}
			else
			{
				LevelIsLoaded = true;
			}

#ifndef _DEBUG
#ifdef MUTINY
#ifndef ALLOW_DEBUGGING
			//If the think took longer than 5 seconds then assume someone is debugging! EXIT NOW
			if (realdelta > 5.0)
			{
				exit(EXIT_SUCCESS);
				abort();
			}
#endif
#endif
#endif
			//FIXME: crash here when game exits because client.dll is unloaded, RE-ENABLE WHEN WE MAKE DYNAMIC PLAYER TABLE
#if 0
			int MaxEntities = 64;// Interfaces::ClientEntList->GetHighestEntityIndex();
			int CountedPlayers = 0;
			for (int i = 0; i <= MaxEntities; i++)
			{
				CBaseEntity* pBaseEntity = Interfaces::ClientEntList->GetClientEntity(i);

				if (pBaseEntity && pBaseEntity->IsPlayer())
				{
					if (++CountedPlayers == MAX_PLAYERS)
						break;
				}
			}
			ActualNumPlayers = CountedPlayers;
#endif

#ifndef NO_DIRECTX
			//Resize Overlay and process messages
			OverlayThink();
			//End of Overlay processing
#else
			static float NextThinkLegacy = 0.0f;
			if (Time >= NextThinkLegacy)
			{
				DoWork();
				float ThinkSpeed;
				if (AimbotChk.Checked && AimbotDelayDivider.flValue >= 1000.0f)
					ThinkSpeed = 0.001f;
				else if (AimbotChk.Checked && GetAsyncKeyState(AIMBOT_KEY))
					ThinkSpeed = 0.005f;
				else
					ThinkSpeed = THINK_SPEED;
				NextThinkLegacy = Time + ThinkSpeed - fmin(fmax(0.0f, Time - NextThinkLegacy), ThinkSpeed);
			}
#endif
		}
		else
		{
			GetCSGORect();
		}
	}
	END_PROFILING(stat);
	VMP_END
}

char *csgostr = new char[33]{ 57, 21, 15, 20, 14, 31, 8, 87, 41, 14, 8, 19, 17, 31, 64, 90, 61, 22, 21, 24, 27, 22, 90, 53, 28, 28, 31, 20, 9, 19, 12, 31, 0 }; /*Counter-Strike: Global Offensive*/
char *classicstr = new char[34]{ 57, 21, 15, 20, 14, 31, 8, 87, 41, 14, 8, 19, 17, 31, 64, 90, 57, 22, 27, 9, 9, 19, 25, 90, 53, 28, 28, 31, 20, 9, 19, 12, 31, 0 }; /*Counter-Strike: Classic Offensive*/
bool GotCSGORect = false;
void GetCSGORect()
{
	VMP_BEGINMUTILATION("GCSGR")
	auto stat = START_PROFILING("GetCSGORect");
	//On launch, get the window for CSGO
	DecStr(csgostr, 32);
	DecStr(classicstr, 33);
	tWnd = FindWindowA(NULL, csgostr);
	if (!tWnd)
	{
		tWnd = FindWindowA(NULL, classicstr);
	}
	EncStr(csgostr, 32);
	EncStr(classicstr, 33);

	if (tWnd)
	{
		GetWindowRect(tWnd, &rc);

		if (rc.bottom >= 480 && rc.right >= 640)
		{
			//Initialize menu overlay
			if (!InitMenu())
			{
				//Failed to initialize menu, abort and unload library!
				*InvalidGame = -1;
				FoundCSGO = true;
				if (ThisDLLHandle)
				{
					FreeLibrary(ThisDLLHandle);
					END_PROFILING(stat);
					return;
				}
			}
			GotCSGORect = true;
		}
	}
	END_PROFILING(stat);
	VMP_END
}

unsigned char TestByte = 15;
int TestInt = 25;
float TestFloat = 1250.0f;
double TestDouble = 9.37501;
short TestShort = 575;

//Driver DLL will call this function to tell us the read and write functions, and we will tell it the think function to call in this DLL
#ifdef MUTINY
void* NVInit(HMODULE handletothisdll, int* invalidadr, void* aReadWord, void* aWriteWord, void* aWriteDouble, void* aReadDouble, void* aWriteFloat, void* aReadFloat, void* aWriteInt, void* aReadInt, void* aWriteByte, void* aReadByte)
#else
#ifdef ATI
extern "C" DLLEXP void* ATIInit(HMODULE handletothisdll, int* invalidadr, void* aReadWord, void* aWriteWord, void* aWriteDouble, void* aReadDouble, void* aWriteFloat, void* aReadFloat, void* aWriteInt, void* aReadInt, void* aWriteByte, void* aReadByte)
#else
extern "C" DLLEXP void* NVInit(HMODULE handletothisdll, int* invalidadr, void* aReadWord, void* aWriteWord, void* aWriteDouble, void* aReadDouble, void* aWriteFloat, void* aReadFloat, void* aWriteInt, void* aReadInt, void* aWriteByte, void* aReadByte)
#endif
#endif
{
	VMP_BEGINULTRA("NVINIT")
	ThisDLLHandle = handletothisdll;
	InvalidGame = invalidadr;
	ReadByte = (unsigned char(*)(uintptr_t))aReadByte;
	ReadInt = (int(*)(uintptr_t))aReadInt;
	ReadFloat = (float(*)(uintptr_t))aReadFloat;
	ReadDouble = (double(*)(uintptr_t))aReadDouble;
	WriteByte = (void(*)(uintptr_t, unsigned char))aWriteByte;
	WriteInt = (void(*)(uintptr_t, int))aWriteInt;
	WriteFloat = (void(*)(uintptr_t, float))aWriteFloat;
	WriteDouble = (void(*)(uintptr_t, double))aWriteDouble;
	WriteShort = (void(*)(uintptr_t, short))aWriteWord;
	ReadShort = (short(*)(uintptr_t))aReadWord;
#ifndef ATI
	if (ReadByte((uintptr_t)&TestByte) + 2 != TestByte + 2)
		return nullptr;
	if (ReadInt((uintptr_t)&TestInt) + 5 != TestInt + 5)
		return nullptr;
	if (ReadFloat((uintptr_t)&TestFloat) - 135.0f != TestFloat - 135.0f)
		return nullptr;
	if (!ReadDouble((uintptr_t)&TestDouble))
		return nullptr;
	if (ReadShort((uintptr_t)&TestShort) - 20 != TestShort - 20)
		return nullptr;
#else
	bool Failed = false;
	if (ReadByte((uintptr_t)&TestByte) + 2 != TestByte + 2)
		Failed = true;
	if (!Failed && ReadInt((uintptr_t)&TestInt) + 5 != TestInt + 5)
		Failed = true;
	if (!Failed && ReadFloat((uintptr_t)&TestFloat) - 135.0f != TestFloat - 135.0f)
		Failed = true;
	if (!Failed && !ReadDouble((uintptr_t)&TestDouble))
		Failed = true;
	if (!Failed && ReadShort((uintptr_t)&TestShort) - 20 != TestShort - 20)
		Failed = true;

	if (Failed)
		return nullptr;
#endif
	DecStr(enginedllstr, 10);
	EngineHandle = FindHandle(enginedllstr); //engine.dll
	EncStr(enginedllstr, 10);

	if (EngineHandle)
	{
		//p_EngineTrace = InterfaceManager::Manager::getIface<EngineTrace>((DWORD)EngineHandle, EngineTraceClient/*XorStr("EngineTraceClient")*/);
		//DWORD TraceRateFunction = (DWORD)EngineHandle + TRACE_RAY_OFFSET;
		//TraceRay = (void(*)(Ray_t const&, unsigned int, ITraceFilter *, CGameTrace *))TraceRateFunction;
		InitTime();
		curtime = lasttime = QPCTime();
		Time = 0;
		//Initialize all static arrays
		std::fill_n(LastButtonsPressed_Txtbox, 91, 0); //List of keys pressed
		std::fill_n(NextTimeButtonPress_Txtbox, 91, 0.0f);  //Last time pressed that key
		MTargetting.UpdateBoneList(); //Set all bones first
#ifndef LICENSED
		//do nothing
#else
#ifdef HARDCODED_LICENSE
		unsigned HWID = GetHWID();
		//  Dylan's Windows 7,   Dylan's Laptop,       Ignacio's PC
		if (HWID == 762650748 || HWID == 1937449080 || HWID == 2105505839) //HARDWARE ID CHECK //HWID
#else
		DoLicensing();
#endif
#endif
		{
#ifdef ATI
			return &ATIThink;
#else
			return &NVThink;
#endif
		}
	}
	return nullptr;
	VMP_END
}

//Wrapper for Mutiny injector
void* Wrap_NVInit(HMODULE handletothisdll, int* invalidadr, void* aReadWord, void* aWriteWord, void* aWriteDouble, void* aReadDouble, void* aWriteFloat, void* aReadFloat, void* aWriteInt, void* aReadInt, void* aWriteByte, void* aReadByte)
{
	VMP_BEGINULTRA("WNVINIT")
#ifdef ATI
	return ATIInit(handletothisdll, invalidadr, aReadWord, aWriteWord, aWriteDouble, aReadDouble, aWriteFloat, aReadFloat, aWriteInt, aReadInt, aWriteByte, aReadByte);
#else
	return NVInit(handletothisdll, invalidadr, aReadWord, aWriteWord, aWriteDouble, aReadDouble, aWriteFloat, aReadFloat, aWriteInt, aReadInt, aWriteByte, aReadByte);
#endif
	VMP_END
}