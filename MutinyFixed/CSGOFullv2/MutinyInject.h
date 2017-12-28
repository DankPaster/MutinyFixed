#pragma once
#include <Windows.h>
#include <thread>

#include "misc.h"

//So much for classes..
namespace MutinyInjection
{
	extern bool StopThread;
	extern int InvalidGame;
	extern HINSTANCE hfInstance;

	void OnDLLInjected(HMODULE hInstance);
	void ThreadLoop(HMODULE hInstance);
	inline void KillMainThread() { StopThread = true; }

	char ReadByte(uintptr_t adr);
	void WriteByte(uintptr_t adr, char val);
	short ReadShort(uintptr_t adr);
	void WriteShort(uintptr_t adr, short val);
	int ReadInt(uintptr_t adr);
	void WriteInt(uintptr_t adr, int val);
	float ReadFloat(uintptr_t adr);
	void WriteFloat(uintptr_t adr, float val);
	double ReadDouble(uintptr_t adr);
	void WriteDouble(uintptr_t adr, double val);
	void ReadAngle(uintptr_t adr, QAngle& dest);
	void WriteAngle(uintptr_t adr, QAngle val);
	void ReadVector(uintptr_t adr, Vector& dest);
	void WriteVector(uintptr_t adr, Vector val);
};