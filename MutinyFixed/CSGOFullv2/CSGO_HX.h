// The following ifdef block is the standard way of creating macros which make exporting 
// from a DLL simpler. All files within this DLL are compiled with the CSGO_HX_EXPORTS
// symbol defined on the command line. This symbol should not be defined on any project
// that uses this DLL. This way any other project whose source files include this file see 
// CSGO_HX_API functions as being imported from a DLL, whereas this DLL sees symbols
// defined with this macro as being exported.
#ifndef CSGO_HX
#define CSGO_H
#pragma once

#ifdef CSGO_HX_EXPORTS
#define DLLEXP __declspec(dllexport)
#else
#define DLLEXP __declspec(dllimport)
#endif

#include <Windows.h>
#include "misc.h"
#include <string>
#include "BaseEntity.h"
#include "ErrorCodes.h"
#include "Profiler.h"
#include "Targetting.h"

#define MAX_PLAYERS 64

typedef unsigned char uint8_t;
extern Vector vecZero;
extern QAngle angZero;

#define AIMBOT_KEY 0x43
#define TRIGGERBOT_KEY VK_XBUTTON2

/* Glow Object structure in csgo */
/*
struct glow_t
{
	DWORD dwBase;
	float r;
	float g;
	float b;
	float a;
	uint8_t unk1[16];
	bool m_bRenderWhenOccluded;
	bool m_bRenderWhenUnoccluded;
	bool m_bFullBloom;
	uint8_t unk2[10];
};
*/

//WARNING: IF THE SIZE OF THE STRUCT IS CHANGED, MAKE SURE TO CHANGE void ClearAllPlayers()
extern struct CustomPlayer AllPlayers[65]; //Entire table from 0 to 64, including unstreamed players. When looping through the playerlist, use this instead of StreamedPlayers
extern struct CustomPlayer *StreamedPlayers[65]; //Starts from 0, ends at number of streamed players found
extern int NumStreamedPlayers;
extern int NumStreamedEnemies;
extern DWORD InitialRenderThread;
extern float ResetRenderThreadTime;

void InitTime();
HANDLE FindHandle(std::string name);

extern bool CrashingServer;
extern bool LevelIsLoaded;
extern bool GotCSGORect;
double QPCTime();
void DoWork();
void GetCSGORect();

void* Wrap_NVInit(HMODULE handletothisdll, int* invalidadr, void* aReadWord, void* aWriteWord, void* aWriteDouble, void* aReadDouble, void* aWriteFloat, void* aReadFloat, void* aWriteInt, void* aReadInt, void* aWriteByte, void* aReadByte);
extern float Time;
//extern float DeltaTime;
//extern bool EnableHack;
extern int LastTargetIndex;
extern float NextTimeBoneGiveUp;
extern int LastBoneChecked;
extern int LastBoneCheckIndex;
extern Vector TargetMousePosition;
extern Vector CenterOfScreen;
extern bool AllocedConsole;
void AllocateConsole();

#define THINK_SPEED (1.0f / 60.0f)

//TODO: Place this shit in math
#ifdef DEBUG  // stop crashing edit-and-continue
FORCEINLINE float clamp(float val, float minVal, float maxVal)
{
	if (maxVal < minVal)
		return maxVal;
	else if (val < minVal)
		return minVal;
	else if (val > maxVal)
		return maxVal;
	else
		return val;
}
#else // DEBUG
FORCEINLINE float clamp(float val, float minVal, float maxVal)
{
#if defined(__i386__) || defined(_M_IX86)
	_mm_store_ss(&val,
		_mm_min_ss(
			_mm_max_ss(
				_mm_load_ss(&val),
				_mm_load_ss(&minVal)),
			_mm_load_ss(&maxVal)));
#else
	val = fpmax(minVal, val);
	val = fpmin(maxVal, val);
#endif
	return val;
}
#endif // DEBUG

//
// Returns a clamped value in the range [min, max].
//
template< class T >
inline T clamp(T const &val, T const &minVal, T const &maxVal)
{
	if (maxVal < minVal)
		return maxVal;
	else if (val < minVal)
		return minVal;
	else if (val > maxVal)
		return maxVal;
	else
		return val;
}

#endif