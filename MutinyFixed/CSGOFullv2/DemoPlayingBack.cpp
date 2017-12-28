#include "VTHook.h"
#include "LocalPlayer.h"
#include "netchan.h"

IsPlayingBackFn oIsDemoPlayingBack;

bool __fastcall Hooks::DemoIsPlayingBack(void* thisptr)
{
#if 0
	if (LocalPlayer.ModifiedSequenceNr)
	{
		if (_ReturnAddress() == (void*)DemoPlayerCreateMoveReturnAdr)
		{
			DWORD ClientState = (DWORD)ReadInt(pClientState);
			INetChannel *netchan = (INetChannel*)ReadInt((uintptr_t)(ClientState + 0x9C));
			//netchan->m_nInSequenceNr = LocalPlayer.Original_m_nInSequenceNr;
			//netchan->last_received = LocalPlayer.Original_last_received;
			LocalPlayer.ModifiedSequenceNr = false;
			return true;
		}
	}
	return oIsDemoPlayingBack((DWORD)thisptr);
#endif
	return oIsDemoPlayingBack((DWORD)thisptr);
}