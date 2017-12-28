#include "LocalPlayer.h"
#include "VTHook.h"
#include "Netchan.h"
#include "player_lagcompensation.h"
#include "VMProtectDefs.h"
SendDatagramFn oSendDatagram = NULL;


void FuckTheFuckingPitch(INetChannel* chan)
{
	if (!LocalPlayer.bSpecialFiring)
	{
		chan->m_nInSequenceNr += 15000;
		//chan->m_nChokedPackets = 255;
		chan->m_nOutSequenceNr += 50000;
	}
	LocalPlayer.bSpecialFiring = false;
}

int __fastcall Hooks::SendDatagram(void* netchan, void*, void *datagram)
{
	VMP_BEGINMUTILATION("SD")
	INetChannel* chan = (INetChannel*)netchan;
	bf_write* data = (bf_write*)datagram;

	if (DisableAllChk.Checked || !BacktrackExploitChk.Checked || datagram)
	{
		return oSendDatagram(chan, data);
	}

	int instate = chan->m_nInReliableState;
	int insequencenr = chan->m_nInSequenceNr;

	//int choked = chan->m_nChokedPackets;
	//int outsequencenr = chan->m_nOutSequenceNr;
#ifndef FUCK
	gLagCompensation.AddLatencyToNetchan(chan, FAKE_LATENCY_AMOUNT);
#else
	FuckTheFuckingPitch(chan);
#endif

	int ret = oSendDatagram(chan, data);

	//chan->m_nOutSequenceNr = outsequencenr;

	//chan->m_nChokedPackets = choked;

	chan->m_nInReliableState = instate;
	chan->m_nInSequenceNr = insequencenr;

#if 0
	netchan->m_nInSequenceNr = LocalPlayer.Original_m_nInSequenceNr;

	if (LocalPlayer.ModifiedSequenceNr)
	{
		if (ReturnAddress == (void*)SendDatagramCL_MoveReturnAdr)
		{
			netchan->m_nInSequenceNr = LocalPlayer.Original_m_nInSequenceNr;
			LocalPlayer.ModifiedSequenceNr = false;
		}
	}
#endif
	return ret;
	VMP_END
}

#if 0
__declspec (naked) int __stdcall Hooks::SendDatagram(void* netchan, void *datagram)
{
	__asm
	{
		push dword ptr ss:[esp]
		push[esp + 8]
		push ecx
		call hSendDatagram
		retn 4
	}
}
#endif