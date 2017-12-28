// CSGO_HX.cpp : Defines the exported functions for the DLL application.
//

#include "stdafx.h"
#include "CSGO_HX.h"
#include <windows.h>
#include <TlHelp32.h>
#include <iostream>
#include <mmsystem.h>
#pragma comment(lib,"winmm.lib")
#include "Usefuls.h"
#include "XorString.h"


// This is an example of an exported variable
//DLLEXP int nCSGO_HX = 0;

// This is an example of an exported function.
/*DLLEXP int fnCSGO_HX(void)
{
	return 42;
}*/

HFONT DEFAULT_FONT = CreateFontA(18, 0, 0, 0, 600, false, false, false, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DRAFT_QUALITY, DEFAULT_PITCH, "Verdana");
unsigned char (*ReadByte) (intptr_t);
int (*ReadInt) (intptr_t);
float (*ReadFloat) (intptr_t);
double (*ReadDouble) (intptr_t);
void (*WriteByte) (intptr_t, unsigned char);
void (*WriteInt) (intptr_t, int);
void (*WriteFloat) (intptr_t, float);
void (*WriteDouble) (intptr_t, double);
//void (*TraceRay) (Ray_t const&, unsigned int, ITraceFilter *, CGameTrace *);
EngineTrace *p_EngineTrace = nullptr;
HANDLE EngineHandle;
HANDLE ClientHandle;
HMODULE ThisDLLHandle;
HANDLE MatchmakingHandle;
int* InvalidGame;
bool FoundCSGO;
bool EnableESP;

void ReadByteArray(intptr_t adr, char* dest, int size)
{
	for (int i = 0; i < size; i++)
	{
		dest[i] = ReadByte(adr + i);
	}
}

void ReadVector(intptr_t adr, float* dest)
{
	//Vector retvect(ReadFloat(adr), ReadFloat(adr + sizeof(float)), ReadFloat(adr + (sizeof(float) * 2)));
	//dest = retvect;
	dest[0] = ReadFloat(adr);
	dest[1] = ReadFloat(adr + sizeof(float));
	dest[2] = ReadFloat(adr + (sizeof(float) * 2));
}

void DeObfuscateString(char &dest, char* src, int inlen, int destlen)
{
	for (int i = 0; i < inlen; i++)
	{
		char val;
		val = src[i];
		val ^= 50;
		val ^= 72;
		char newchar[2] = { 0 };

		*newchar = static_cast<char>(val);
		strcat_s(&dest, destlen, newchar);
	}
}

HANDLE FindHandle(const char *name)
{
	char string[32] = { 0 };
	DeObfuscateString(*string, (char*)name, strlen(name), sizeof(string));
	HANDLE handle = GetModuleHandleA(string);
	memset(string, 0, sizeof(string));
	return handle;
}

float SanitizeColor(int value)
{
	if (value > 255) value = 255;
	if (value < 0) value = 0;
	return (float)value / 255;
}

DWORD m_dwGlowObject = 0x04F607EC;
DWORD m_iTeamNum = 0x000000F0;
DWORD m_dwEntityList = 0x04A4A894;
DWORD m_bDormant = 0x000000E9;
DWORD m_iGlowIndex = 0x0000A310;
DWORD m_dwLocalPlayer = 0x00A2F4AC;
DWORD m_dwLocalPlayerIndex = 0x00000178;
DWORD m_dwClientState = 0x005B6304;
DWORD m_dwViewAngles = 0x00004D0C;
DWORD m_iCrossHairID = 0x0000AA44; //m_dwInCross = 0x2400;
DWORD m_iHealth = 0x000000FC;
DWORD m_vecOrigin = 0x00000134;
DWORD m_dwViewMatrix = 0x04A3C424;
DWORD m_dwBoneMatrix = 0x00002698;
DWORD m_lifeState = 0x0000025B;
DWORD m_iCompetitiveRanking = 0x00001A44;
DWORD m_iCompetitiveWins = 0x00001B48;
DWORD DT_CSPlayerResource = 0x02E888D0;
DWORD m_bSpotted = 0x00000939;
LONG I_CLICK_UP_TIME = 1; //Time to click up after clicking down
LONG I_TRIGGERBOT_DELAY = 125; //Time to click again after clicking
LONG I_MOUSE_MOVE_DELAY = 0;//35;
int AIMBOT_MAXDIST_FROM_CROSSHAIR = 99999;//100;
int AIMBOT_DELAY_DIVIDER = 1; //4
//#define TRACE_RAY_OFFSET 0x208BB0; //engine + offset


enum TEAMS {
	TEAM_NONE = 0,
	TEAM_GOTV,
	TEAM_T,
	TEAM_CT
};

inline DWORD GetPlayerInCrosshair(DWORD LocalPlayer)
{
	return ReadInt(LocalPlayer + m_iCrossHairID) - 1;
}

inline DWORD GetLocalPlayer()
{
	return ReadInt((DWORD)ClientHandle + m_dwLocalPlayer);
}

inline DWORD GetLocalPlayerIndex()
{
	return ReadInt((DWORD)ClientHandle + m_dwLocalPlayerIndex);
}

inline TEAMS GetPlayerTeam(DWORD Entity)
{
	return (TEAMS)ReadInt(Entity + m_iTeamNum);
}

inline int GetPlayerHealth(DWORD BasePlayer)
{
	return ReadInt(BasePlayer + m_iHealth);
}

inline DWORD GetBasePlayer(DWORD index)
{
	return ReadInt((DWORD)ClientHandle + m_dwEntityList + (index * 16));
}

inline void GetPlayerOrigin(DWORD Entity, float* dest)
{
	ReadVector(Entity + m_vecOrigin, dest);
}

inline void GetBonePos(DWORD BoneMatrix, int BoneID, float* BonePos)
{
	BonePos[0] = ReadFloat((BoneMatrix + (0x30 * BoneID) + 0xC));//x
	BonePos[1] = ReadFloat((BoneMatrix + (0x30 * BoneID) + 0x1C));//y
	BonePos[2] = ReadFloat((BoneMatrix + (0x30 * BoneID) + 0x2C));//z
}

inline DWORD GetCSPlayerResource()
{
	return ReadInt((DWORD)ClientHandle + DT_CSPlayerResource);
}

const char *Ranks[] =
{
	"W",//"-",
	")Z3",//"Silver I",
	")Z33",//"Silver II",
	")Z333",//"Silver III",
	")Z3,",//"Silver IV",
	")Z?",//"Silver Elite",
	")Z?Z7	",//"Silver Elite Master",

	"=Z4Z3",//"Gold Nova I",
	"=Z4Z33",//"Gold Nova II",
	"=Z4Z333",//"Gold Nova III",
	"=Z4Z7	",//"Gold Nova Master",
	"7	Z=Z3",//"Master Guardian I",
	"7	Z=Z33",//"Master Guardian II",

	"7	Z=Z?",//"Master Guardian Elite",
	">		Z7	Z=",//"Distinguished Master Guardian",
	"6Z?",//"Legendary Eagle",
	"6Z?Z7	",//"Legendary Eagle Master",
	")\nZ7	Z<	Z9		",//"Supreme Master First Class",
	".Z=Z?"//"The Global Elite"
};

inline int GetCompetitiveRank(DWORD CSPlayerResource, DWORD index)
{
	return ReadInt(CSPlayerResource + m_iCompetitiveRanking + index * 4);
}

inline void GetCompetitiveRankString(char* dest, int destlength, int rank)
{
	DeObfuscateString(*dest, (char*)Ranks[rank], strlen(Ranks[rank]), destlength);
}

inline int GetCompetitiveWins(DWORD CSPlayerResource, DWORD index)
{
	return ReadInt(CSPlayerResource + m_iCompetitiveWins + index * 4);
}

inline void SpotPlayer(DWORD Entity)
{
	WriteByte((DWORD)Entity + m_bSpotted, 1);
}

typedef struct
{
	float flMatrix[4][4];
}WorldToScreenMatrix_t;

RECT rc; //screen rectangle
HWND tWnd; //target game window

BOOL WorldToScreen(float * from, float * to)
{
	WorldToScreenMatrix_t *WorldToScreenMatrix = new WorldToScreenMatrix_t;
	memset(WorldToScreenMatrix, 0, sizeof(WorldToScreenMatrix_t));
	//char Matrix[sizeof(WorldToScreenMatrix_t)];
	ReadByteArray((DWORD)ClientHandle + /*ViewMatrix_1*/ m_dwViewMatrix, (char*)WorldToScreenMatrix, sizeof(WorldToScreenMatrix_t));
	
	float w = 0.0f;

	to[0] = WorldToScreenMatrix->flMatrix[0][0] * from[0] + WorldToScreenMatrix->flMatrix[0][1] * from[1] + WorldToScreenMatrix->flMatrix[0][2] * from[2] + WorldToScreenMatrix->flMatrix[0][3];
	to[1] = WorldToScreenMatrix->flMatrix[1][0] * from[0] + WorldToScreenMatrix->flMatrix[1][1] * from[1] + WorldToScreenMatrix->flMatrix[1][2] * from[2] + WorldToScreenMatrix->flMatrix[1][3];
	w = WorldToScreenMatrix->flMatrix[3][0] * from[0] + WorldToScreenMatrix->flMatrix[3][1] * from[1] + WorldToScreenMatrix->flMatrix[3][2] * from[2] + WorldToScreenMatrix->flMatrix[3][3];

	if (w < 0.01f) {
		delete WorldToScreenMatrix;
		return FALSE;
	}

	int width = (int)(rc.right - rc.left);

	float invw = 1.0f / w;
	to[0] *= invw;
	to[1] *= invw;

	int height = (int)(rc.bottom - rc.top);

	float x = (float)width / 2.0f;
	float y = (float)height / 2.0f;

	x += 0.5f * to[0] * (float)width + 0.5f;
	y -= 0.5f * to[1] * (float)height + 0.5f;

	to[0] = x + (float)rc.left;
	to[1] = y + (float)rc.top;

	delete WorldToScreenMatrix;

	return TRUE;
}

void DrawTextOverlay(int x, int y, char *text, COLORREF color, HFONT font)
{
	int oldleft, oldtop;
	oldleft = rc.left;
	oldtop = rc.top;

	rc.left = x - rc.left;
	rc.top = y - rc.top;
	LPRECT prect = &rc;
	HWND hwnd = GetForegroundWindow();
	HDC layhdc = GetWindowDC(hwnd);
	HFONT OLD_HFONT = (HFONT)SelectObject(layhdc, font);
	SetTextColor(layhdc, color);
	SetBkMode(layhdc, TRANSPARENT);
	DrawTextA(layhdc, (LPCSTR)text, -1, prect, 0);
	//UpdateWindow(hwnd);
	SelectObject(layhdc, OLD_HFONT);
	ReleaseDC(hwnd, layhdc);
	rc.left = oldleft;
	rc.top = oldtop;
}

namespace InterfaceManager
{
	class Manager
	{
	public:

		template<typename T>
		static T *getIface(DWORD handle, const char *ifaceName)
		{
			T *iface = nullptr;
			char tmpIface[100];
			char CreateInterface[64] = { 0 };
			DeObfuscateString(*CreateInterface, "93", strlen("93"), sizeof(CreateInterface)); //CreateInterface
			CreateInterfaceFn ifaceFn = (CreateInterfaceFn)GetProcAddress((HMODULE)handle, CreateInterface);
			for (int i = 1; i < 100; ++i)
			{
				//%s0%i
				memset(CreateInterface, 0, sizeof(CreateInterface));
				DeObfuscateString(*CreateInterface, "_	J_", strlen("_	J_"), sizeof(CreateInterface));
				sprintf(tmpIface, CreateInterface, ifaceName, i);
				iface = (T*)ifaceFn(tmpIface, 0);

				if (iface != nullptr)
					break;

				//%s00%i
				memset(CreateInterface, 0, sizeof(CreateInterface));
				DeObfuscateString(*CreateInterface, "_	JJ_", strlen("_	JJ_"), sizeof(CreateInterface));
				sprintf(tmpIface, CreateInterface, ifaceName, i);
				iface = (T*)ifaceFn(tmpIface, 0);

				if (iface != nullptr)
					break;
			}
			return iface;
		}

	private:

		using CreateInterfaceFn = void*(*)(const char *name, int *returnC);
	};
}

void DoWork(LONG Time)
{
	static bool GotCSGORect;
	if (!GotCSGORect)
	{
		HWND cWnd = GetForegroundWindow();
		char Str[MAX_PATH] = { 0 };
		DeObfuscateString(*Str, (char*)"9W)@Z=Z5	", strlen("9W)@Z=Z5	"), sizeof(Str));
		tWnd = FindWindowA(NULL, Str);
		if (cWnd == tWnd)
		{
			GetWindowRect(tWnd, &rc);
			GotCSGORect = true;
		}
		else
		{
			return;
		}
	}
	static LONG NextThink;
	if (Time >= NextThink)
	{
		int MoveMouseFlags = 0;
		static LONG NextTimeClickDown = INT_MAX;
		static LONG NextTimeClickUp = INT_MAX;
		static LONG LastTimeClickedDown;
		POINT NewCursorPosition = {0, 0};

		float PlayersOnScreenXY[64][2] = { 0.0f, 0.0f };

		if (GetForegroundWindow() == tWnd)
		{
			GetWindowRect(tWnd, &rc);
			DWORD LocalPlayer = GetLocalPlayer();
			DWORD glow_Pointer = ReadInt((DWORD)ClientHandle + m_dwGlowObject);
			TEAMS MyTeamID = GetPlayerTeam(LocalPlayer);
			DWORD PlayerInCrosshair = GetPlayerInCrosshair(LocalPlayer);
			float GlowTerroristRed = 1.0f;
			float GlowTerroristGreen = 0.6f;
			float GlowTerroristBlue = 0.3f;
			float GlowTerroristAlpha = 0.8f;

			float GlowCounterTerroristRed = 0.0f;
			float GlowCounterTerroristGreen = 0.5f;
			float GlowCounterTerroristBlue = 0.7f;
			float GlowCounterTerroristAlpha = 0.8f;


			for (int i = 0; i < 64; i++)
			{
				int TargetEntity = ReadInt((DWORD)ClientHandle + m_dwEntityList + (i * 16));

				if (TargetEntity)
				{
					TEAMS TargetPlayerTeamID = (TEAMS)ReadInt(TargetEntity + m_iTeamNum);

					SpotPlayer(TargetEntity);

					if (ReadByte(TargetEntity + m_bDormant))
					{
						continue;
					}

					if (MyTeamID != TargetPlayerTeamID)
					{
						float TargetPlayerOrigin[3];
						GetPlayerOrigin(TargetEntity, TargetPlayerOrigin);
						DWORD BoneMatrix = ReadInt(TargetEntity + m_dwBoneMatrix);
						float HeadPos[3];
						GetBonePos(BoneMatrix, 6, HeadPos);

						float W2S[3] = { 0.0f, 0.0f, 0.0f };
						float W2S_Head[3] = { 0.0f, 0.0f, 0.0f };
						if (WorldToScreen(TargetPlayerOrigin, W2S) == TRUE && WorldToScreen(HeadPos, W2S_Head) == TRUE)
						{
							//Get the height of our box by taking the W2S'd feet position minus the W2S'd head position
							int height = (int)(abs(W2S[1] - W2S_Head[1]));//abs = absolute http://www.cplusplus.com/reference/cstdlib/abs/
							int width = height / 2; //You can change this value to anything you would liek
							if (ReadInt(TargetEntity + m_lifeState) == 0)
							{
								//If alive
								int TargetPlayerHealth = GetPlayerHealth(TargetEntity);
								char str[256];
								DWORD CSPlayerResource = GetCSPlayerResource();
								char deobfuscatedstring[256] = { 0 };
								DeObfuscateString(*deobfuscatedstring, "2@Z_p-	@Z_p(@Z", strlen("2@Z_p-	@Z_p(@Z"), sizeof(deobfuscatedstring));
								sprintf(str, deobfuscatedstring, TargetPlayerHealth, GetCompetitiveWins(CSPlayerResource, i));
								char rank[128] = { 0 };
								GetCompetitiveRankString(rank, sizeof(rank), GetCompetitiveRank(CSPlayerResource, i));
								strcat_s(str, sizeof(str), rank);
								//DrawTextOverlay(W2S[0]/* - (width / 2)*/ + 3, W2S[1]/* - height*/ + 3, str, RGB(0, 0, 0), DEFAULT_FONT);
								//DrawTextOverlay(W2S[0]/* - (width / 2)*/, W2S[1]/* - height*/, str, RGB(0, 255, 64), DEFAULT_FONT);

								//ray.Init(gBaseHook.m_pMyEngine->EyePositionPrediction(), pBaseEntity->GetEyePos());
								PlayersOnScreenXY[i][0] = W2S_Head[0];
								PlayersOnScreenXY[i][1] = W2S_Head[1];
							}
						}

						//Triggerbot
						if (PlayerInCrosshair && PlayerInCrosshair == i)
						{
							if (Time - LastTimeClickedDown >= I_TRIGGERBOT_DELAY)
							{
								NextTimeClickDown = Time;
							}
						}
						//End Of Triggerbot
					}

					int PlayerGlowIndex = ReadInt(TargetEntity + m_iGlowIndex);

					switch (TargetPlayerTeamID)	// 1 GoTV; 2 T; 3 CT
					{
					case TEAM_T:
						WriteFloat((glow_Pointer + ((PlayerGlowIndex * 0x38) + 0x4)), GlowTerroristRed);
						WriteFloat((glow_Pointer + ((PlayerGlowIndex * 0x38) + 0x8)), GlowTerroristGreen);
						WriteFloat((glow_Pointer + ((PlayerGlowIndex * 0x38) + 0xC)), GlowTerroristBlue);
						if (!EnableESP)
							WriteFloat((glow_Pointer + ((PlayerGlowIndex * 0x38) + 0x10)), 0.0f);
						else
							WriteFloat((glow_Pointer + ((PlayerGlowIndex * 0x38) + 0x10)), GlowTerroristAlpha);
						WriteByte((glow_Pointer + ((PlayerGlowIndex * 0x38) + 0x24)), true);
						WriteByte((glow_Pointer + ((PlayerGlowIndex * 0x38) + 0x25)), false);
						break;
					case TEAM_CT:
						WriteFloat((glow_Pointer + ((PlayerGlowIndex * 0x38) + 0x4)), GlowCounterTerroristRed);
						WriteFloat((glow_Pointer + ((PlayerGlowIndex * 0x38) + 0x8)), GlowCounterTerroristGreen);
						WriteFloat((glow_Pointer + ((PlayerGlowIndex * 0x38) + 0xC)), GlowCounterTerroristBlue);
						if (!EnableESP)
							WriteFloat((glow_Pointer + ((PlayerGlowIndex * 0x38) + 0x10)), 0.0f);
						else
							WriteFloat((glow_Pointer + ((PlayerGlowIndex * 0x38) + 0x10)), GlowCounterTerroristAlpha);
						WriteByte((glow_Pointer + ((PlayerGlowIndex * 0x38) + 0x24)), true);
						WriteByte((glow_Pointer + ((PlayerGlowIndex * 0x38) + 0x25)), false);
						break;
					}
				}
			}

			float centerx = (float)(rc.right * 0.5);
			float centery = (float)(rc.bottom * 0.5);
			float dist[64];
			for (int fuck = 0; fuck < 64; fuck++)
			{
				dist[fuck] = -99999.0f;
			}
			float leaddist = -99999.0f;
			int leadid = 0;
			bool movemouse = false;
			
			for (int f = 0; f < 64; f++)
			{
				//float PlayersOnScreenXY[64][2]
				if (PlayersOnScreenXY[f][0] != 0.0f || PlayersOnScreenXY[f][1] != 0.0f)
				{
					float dx = PlayersOnScreenXY[f][0] - centerx;
					float dy = PlayersOnScreenXY[f][1] - centery;
					dist[f] = sqrt(dx*dx + dy*dy);
				}
			}
			for (int g = 0; g < 64; g ++)
			{
				if (dist[g] != -99999.0f)
				{
					if (dist[g] < leaddist || leaddist == -99999.0f)
					{
						if (dist[g] <= AIMBOT_MAXDIST_FROM_CROSSHAIR)
						{
							leaddist = dist[g];
							leadid = g;
							movemouse = true;
						}
					}
				}
			}
			static LONG NextTime_MoveMouse;
			if (GetAsyncKeyState(VK_XBUTTON2) && movemouse && Time >= NextTime_MoveMouse)
			{
				POINT pos;
				GetCursorPos(&pos);
				NewCursorPosition.x = /*pos.x - */-((pos.x - (LONG)PlayersOnScreenXY[leadid][0]) / AIMBOT_DELAY_DIVIDER);
				NewCursorPosition.y = /*pos.y - */-((pos.y - (LONG)PlayersOnScreenXY[leadid][1]) / AIMBOT_DELAY_DIVIDER);
				MoveMouseFlags |= (MOUSEEVENTF_MOVE/* | MOUSEEVENTF_ABSOLUTE*/);
				NextTime_MoveMouse = Time + I_MOUSE_MOVE_DELAY;
			}
			if (Time >= NextTimeClickDown)
			{
				MoveMouseFlags |= MOUSEEVENTF_LEFTDOWN;
				NextTimeClickUp = Time + I_CLICK_UP_TIME;
				NextTimeClickDown = INT_MAX;
				LastTimeClickedDown = Time;
			}
		} //Foreground window only above

		if (Time >= NextTimeClickUp)
		{
			MoveMouseFlags |= MOUSEEVENTF_LEFTUP;
			NextTimeClickUp = INT_MAX;
		}

		if (MoveMouseFlags)
		{
			//Click or move the mouse
			mouse_event(MoveMouseFlags, NewCursorPosition.x, NewCursorPosition.y, 0, 0);
		}
		NextThink = Time + 10;
	}
}
/*
getKillsLeader() {
	new highest, highestid = -1;
	for (new i = 0; i<MAX_PLAYERS; i++) {
		if (kills[i] != 0) {
			if (kills[i] > highest) {
				highest = kills[i];
				highestid = i;
			}
		}
	}
	return highestid;
}
*/
//Function the driver will call instead of making a thread
void __stdcall NVThink(void)
{
	if (!FoundCSGO)
	{
		static LONG starttime;
		LONG time = timeGetTime();
		if (starttime == 0)
			starttime = time;
		if (time - starttime < 7000)
		{
			if (!ClientHandle)
			{
				ClientHandle = FindHandle("T"); //Don't get confused by the T, it's 'engine.dll' encrypted
			}
			if (!MatchmakingHandle)
			{
				MatchmakingHandle = FindHandle("T"); //Don't get confused by the T, it's 'matchmaking.dll' encrypted
			}
			if (!ClientHandle && !MatchmakingHandle)
			{
				return;
			}
			//GetOffsets();
			FoundCSGO = true;
		}
		else
		{
			*InvalidGame = -1;
			FoundCSGO = true;
			if (ThisDLLHandle)
			{
				FreeLibrary(ThisDLLHandle);
			}
		}
	}
	else
	{
		LONG Time = timeGetTime();
		static LONG LastPress;
		if (GetAsyncKeyState(VK_XBUTTON1)) 
		{
			LONG Time = timeGetTime();
			if (Time - LastPress >= 150)
			{
				EnableESP = !EnableESP;
				LastPress = Time;
				DoWork(Time);
			}
		}
		if (EnableESP)
		{
			DoWork(Time);
		}
	}
}

//Driver DLL will call this function to tell us the read and write functions, and we will tell it the think function to call in this DLL
extern "C" DLLEXP void* NVInit(HMODULE handletothisdll, int* invalidadr, void* aWriteDouble, void* aReadDouble, void* aWriteFloat, void* aReadFloat, void* aWriteInt, void* aReadInt, void* aWriteByte, void* aReadByte)
{
	ThisDLLHandle = handletothisdll;
	InvalidGame = invalidadr;
	ReadByte = (unsigned char(*)(intptr_t))aReadByte;
	ReadInt = (int(*)(intptr_t))aReadInt;
	ReadFloat = (float(*)(intptr_t))aReadFloat;
	ReadDouble = (double(*)(intptr_t))aReadDouble;
	WriteByte = (void(*)(intptr_t, unsigned char))aWriteByte;
	WriteInt = (void(*)(intptr_t, int))aWriteInt;
	WriteFloat = (void(*)(intptr_t, float))aWriteFloat;
	WriteDouble = (void(*)(intptr_t, double))aWriteDouble;
	EngineHandle = FindHandle("T"); //engine.dll

	char EngineTraceClient[64] = { 0 };
	DeObfuscateString(*EngineTraceClient, "?.9", strlen("?.9"), sizeof(EngineTraceClient));
	//ClientHandle = GetModuleHandleA(string); //client.dll
	//memset(string, 0, sizeof(string));
	//DeObfuscateString(*string, "T", sizeof("T") - 1, sizeof(string)); //matchmaking.dll
	if (EngineHandle)
	{
		p_EngineTrace = InterfaceManager::Manager::getIface<EngineTrace>((DWORD)EngineHandle, EngineTraceClient/*XorStr("EngineTraceClient")*/);
		//DWORD TraceRateFunction = (DWORD)EngineHandle + TRACE_RAY_OFFSET;
		//TraceRay = (void(*)(Ray_t const&, unsigned int, ITraceFilter *, CGameTrace *))TraceRateFunction;
		return &NVThink;
	}
	return nullptr;
}

// This is the constructor of a class that has been exported.
// see CSGO_HX.h for the class definition
/*
CCSGO_HX::CCSGO_HX()
{
	return;
}
*/

#if 0
#include <psapi.h> //GetModuleInformation
void GetOffsets(void)
{
	modClient.dwBase = (DWORD)ClientHandle;
	MODULEINFO modinfo = { NULL, };
	GetModuleInformation(GetCurrentProcess(), (HMODULE)ClientHandle, &modinfo, sizeof(modinfo));
	modClient.dwSize = modinfo.SizeOfImage;
}
#endif