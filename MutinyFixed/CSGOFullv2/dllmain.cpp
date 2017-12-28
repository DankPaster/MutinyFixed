// dllmain.cpp : Defines the entry point for the DLL application.
#include "VMProtectDefs.h"


#ifdef MUTINY
#include "framework.h"
#ifndef NO_MUTINY_HEARTBEAT
#include "C:/ClientFramework/Framework/Include/Frame-Include.h"
#pragma comment(lib,"cryptlib.lib")
#include "EncryptString.h"
#endif
#include "MutinyInject.h"

#ifndef NO_MUTINY_HEARTBEAT
void __stdcall HeartbeatThread()
{
	Framework->Heartbeat->Heartbeat_wrapper(nullptr);
}
#endif
#else
#include <Windows.h>
#endif

extern HINSTANCE hInstance; //Inside Overlay.h

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
					 )
{
	VMP_BEGINULTRA("DLLMAIN")
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
	{
		hInstance = (HINSTANCE)hModule;

		MessageBoxA(NULL, "DLL INJECTED", "", MB_OK);
#ifdef MUTINY

#ifndef LOAD_LIBRARY_INJECTABLE
		Framework = new MutinyFrame::Backend();

		const char *UserAgentGameName = VMP_ENCDECSTRINGA("GameCSGO ");
		const char *UserAgentKeyOne = VMP_ENCDECSTRINGA("DdHre5HnhFnHb50kAatPLGuwv");
		const char *UserAgentKeyTwo = VMP_ENCDECSTRINGA("8PjOOlhSTyWO70VBnsDisNIs8");

		Framework->Heartbeat = new MutinyFrame::CHeartBeat(UserAgentKeyOne, UserAgentKeyTwo, UserAgentGameName);

#ifndef NO_MUTINY_HEARTBEAT
		//Framework->Security = new MutinyFrame::Security();

#ifndef ALLOW_DEBUGGING
		Framework->Security->Protect(hModule, false, true, true, true);
#endif

		unsigned int HeartbeatThreadID;
		HANDLE hbthread = (HANDLE)_beginthreadex(nullptr, 0, (_beginthreadex_proc_type)&HeartbeatThread, nullptr, 0, &HeartbeatThreadID);
		//_BEGINTHREAD(HeartbeatThread);
#endif
#endif

		MutinyInjection::OnDLLInjected(hModule);
#endif
	}
	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
	case DLL_PROCESS_DETACH:
		break;
	}
	return TRUE;
	VMP_END
}