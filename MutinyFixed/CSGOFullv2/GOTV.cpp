#include "GOTV.h"
#include "CreateMove.h"
#include "Netchan.h"

AntiGOTV gAntiGOTV;

//char *gotvsignaturestr = new char[63]{ 56, 63, 90, 90, 69, 69, 90, 90, 69, 69, 90, 90, 69, 69, 90, 90, 69, 69, 90, 90, 66, 56, 90, 90, 74, 63, 90, 90, 66, 79, 90, 90, 57, 67, 90, 90, 77, 78, 90, 90, 75, 63, 90, 90, 62, 78, 90, 90, 66, 67, 90, 90, 74, 72, 90, 90, 66, 56, 90, 90, 74, 75, 0 }; /*BE  ??  ??  ??  ??  8B  0E  85  C9  74  1E  D4  89  02  8B  01*/
//char *gotvsignature2str = new char[43]{ 56, 63, 90, 90, 69, 69, 90, 90, 69, 69, 90, 90, 69, 69, 90, 90, 69, 69, 90, 90, 66, 56, 90, 90, 74, 63, 90, 90, 66, 79, 90, 90, 57, 67, 90, 90, 77, 78, 90, 90, 75, 63, 0 }; /*BE  ??  ??  ??  ??  8B  0E  85  C9  74  1E*/

//Source: https://github.com/imfrickingvexed/UndefeatedV2/blob/6bdb78d942369224334647438697167d48321bb0/csgocheat/Tools/Hacks/Aimbot.h
bool AntiGOTV::GOTVIsRecording(int tick)
{
	return false;

#if 0
	static bool wasactivegotv = false;

	static DWORD dwHLTVServer;
#if 1
	static GOTV::CHLTVServer** hltv;
	if (!hltv || !*hltv)
	{
		DecStr(gotvsignature2str, 42);
		dwHLTVServer = FindMemoryPattern(EngineHandle, gotvsignature2str);
		if (!dwHLTVServer)
		{
			THROW_ERROR(ERR_CANT_FIND_GOTV_SIGNATURE);
			exit(EXIT_SUCCESS);
		}
		EncStr(gotvsignature2str, 42);
		dwHLTVServer += 1;
		hltv = *(GOTV::CHLTVServer***)dwHLTVServer;
	}

	if (hltv && *hltv)
	{

		DWORD ClientState = (DWORD)ReadInt(pClientState);

		int clienttickcount = *(DWORD*)(ClientState + 0x160);
		int servertickcount = *(DWORD*)(ClientState + 0x164);
		tick = servertickcount;
		DWORD* m_nDeltaTick = (DWORD*)(ClientState + 0x16C);
		int dif2 = max(*m_nDeltaTick, servertickcount) - min(*m_nDeltaTick, servertickcount);
		int dif3 = max(*m_nDeltaTick, CurrentUserCmd.cmd->tick_count) - min(*m_nDeltaTick, CurrentUserCmd.cmd->tick_count);

		int Dif = max((*hltv)->m_DemoRecorder.m_nDeltaTick, tick) - min((*hltv)->m_DemoRecorder.m_nDeltaTick, tick);
		Dif = servertickcount;
		if (Dif % 16 == 0 || Dif == 0)
		{
			return true;
		}
	}
#else
	DWORD ClientState = (DWORD)ReadInt(pClientState);
	INetChannel *netchan = (INetChannel*)ReadInt((uintptr_t)(ClientState + 0x9C));

	DWORD off = (DWORD)&netchan->m_pDemoRecorder;
	DWORD real = (DWORD)((DWORD)netchan + 0x4294);
	GOTV::CHLTVDemoRecorder* recorder = (GOTV::CHLTVDemoRecorder*)netchan->m_pDemoRecorder;
	DWORD* m_nDeltaTick = (DWORD*)(ClientState + 0x16C);
	if (recorder)
	{
		int Dif = max(*m_nDeltaTick, tick) - min(*m_nDeltaTick, tick); //max(recorder->m_nDeltaTick, tick) - min(recorder->m_nDeltaTick, tick);
		if (Dif % 16 == 0 || Dif == 0)
		{
			return true;
		}
	}
#endif
	return false;
#endif
}