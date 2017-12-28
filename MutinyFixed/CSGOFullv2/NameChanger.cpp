#include "CreateMove.h"
#include "ConVar.h"
#include "NameChanger.h"
#include "SetClanTag.h"

#ifndef LICENSED
#ifndef MUTINY
//credits: flashyy
#include <ctime>
char *namestr = new char[5]{ 20, 27, 23, 31, 0 }; /*name*/
char *namespam1str = new char[151]{ 119, 119, 119, 119, 119, 119, -1, 31, -1, 31, -1, 31, -1, 31, -1, 31, -1, 31, -1, 31, -1, 31, -1, 31, -1, 31, -1, 31, -1, 31, -1, 31, -1, 31, -1, 31, -1, 31, -1, 31, -1, 31, -1, 31, -1, 31, -1, 31, -1, 31, -1, 31, -1, 31, -1, 31, -1, 31, -1, 31, -1, 31, -1, 31, -1, 31, -1, 31, -1, 31, -1, 31, -1, 31, -1, 31, -1, 31, -1, 31, -1, 31, -1, 31, -1, 31, -1, 31, -1, 31, -1, 31, -1, 31, -1, 31, -1, 31, -1, 31, -1, 31, -1, 31, -1, 31, -1, 31, -1, 31, -1, 31, -1, 31, -1, 31, -1, 31, -1, 31, -1, 31, -1, 31, -1, 31, -1, 31, -1, 31, -1, 31, -1, 31, -1, 31, -1, 31, -1, 31, -1, 31, -1, 31, -1, 31, -1, 31, -1, 31, 0 };
char *namespam2str = new char[148]{ 119, 119, 119, 119, 119, 119, -1, 31, -1, 31, -1, 31, -1, 31, -1, 31, -1, 31, -1, 31, -1, 31, -1, 31, -1, 31, -1, 31, -1, 31, -1, 31, -1, 31, -1, 31, -1, 31, -1, 31, -1, 31, -1, 31, -1, 31, -1, 31, -1, 31, -1, 31, -1, 31, -1, 31, -1, 31, -1, 31, -1, 31, -1, 31, -1, 31, -1, 31, -1, 31, -1, 31, -1, 31, -1, 31, -1, 31, -1, 31, -1, 31, -1, 31, -1, 31, -1, 31, -1, 31, -1, 31, -1, 31, -1, 31, -1, 31, -1, 31, -1, 31, -1, 31, -1, 31, -1, 31, -1, 31, -1, 31, -1, 31, -1, 31, -1, 31, -1, 31, -1, 31, -1, 31, -1, 31, -1, 31, -1, 31, -1, 31, -1, 31, -1, 31, -1, 31, -1, 31, -1, 31, -1, 31, -1, 31, -1, 0 };
ConVar* CVAR_Name = nullptr;
void NameSpamDisperse(int mode)
{
	DecStr(namestr, 4);
	CVAR_Name = Interfaces::Cvar->FindVar(namestr);
	EncStr(namestr, 4);

	if (!CVAR_Name)
	{
		DecStr(namestr, 4);
		CVAR_Name = Interfaces::Cvar->FindVar(namestr);
		EncStr(namestr, 4);
		if (!CVAR_Name)
			return;
		*(int*)((DWORD)&CVAR_Name->fnChangeCallback + 0xC) = NULL;
	}
	*(int*)((DWORD)&CVAR_Name->fnChangeCallback + 0xC) = NULL;
	if (mode == 0)
	{
		static int cmdcnt = 0;
		if (cmdcnt < 100)
		{
			//allow name spam exploit
			CVAR_Name->SetValue("\n­­­");
			cmdcnt++;
		}
		else
		{
			//spam other player names
			std::vector<int>connected;
			for (int i = 0; i <= MAX_PLAYERS; i++)
			{
				CustomPlayer *pCPlayer = &AllPlayers[i];
				if (!pCPlayer->IsLocalPlayer && pCPlayer->BaseEntity && pCPlayer->Connected)
					connected.push_back(i);
			}
			if (connected.size())
			{
				int target = connected.at(rand() % connected.size());
				char name[64];
				wcstombs(name, GetPlayerNameAddress(GetRadar(), target), sizeof(name));
				strcat(name, "\n");
				CVAR_Name->SetValue(name);
			}
		}
	}
	else
	{
		static clock_t start_t = clock();
		double timeSoFar = (double)(clock() - start_t) / CLOCKS_PER_SEC;
		if (timeSoFar < 0.001)
			return;

		//flashyy's mega-spam exploit
		static bool useSpace = true;
		if (useSpace)
		{
			DecStr(namespam1str, 150);
			CVAR_Name->SetValue(namespam1str);//("\x0D\x0D\x0D\x0D\x0D\x0D""…e…e…e…e…e…e…e…e…e…e…e…e…e…e…e…e…e…e…e…e…e…e…e…e…e…e…e…e…e…e…e…e…e…e…e…e…e…e…e…e…e…e…e…e…e…e…e…e…e…e…e…e…e…e…e…e…e…e…e…e…e…e…e…e…e…e…e…e…e…e…e…e");
			EncStr(namespam1str, 150);
			useSpace = !useSpace;
		}
		else
		{
			DecStr(namespam2str, 147);
			CVAR_Name->SetValue(namespam2str);//("\x0D\x0D\x0D\x0D\x0D\x0D""…e…e…e…e…e…e…e…e…e…e…e…e…e…e…e…e…e…e…e…e…e…e…e…e…e…e…e…e…e…e…e…e…e…e…e…e…e…e…e…e…e…e…e…e…e…e…e…e…e…e…e…e…e…e…e…e…e…e…e…e…e…e…e…e…e…e…e…e…e…e…");
			EncStr(namespam2str, 147);
			useSpace = !useSpace;
		}

		start_t = clock();
	}
}
#endif
#endif

#ifndef LICENSED
#ifndef MUTINY
//DO NOT PUT IN PUBLIC MUTINY RELEASE
void RunNameChanger()
{
	static bool namespam = false;
	static float nextspam = 0;
	static int mode = 0;
	if (Interfaces::Globals->realtime >= nextspam && (GetAsyncKeyState(VK_CONTROL) & (1 << 16)) && (GetAsyncKeyState(VK_SHIFT) & (1 << 16)) && (GetAsyncKeyState(VK_CAPITAL) & (1 << 16)))
	{
		if ((GetAsyncKeyState(VK_UP) & (1 << 16)))
		{
			mode = 0;
			namespam = !namespam;
			nextspam = Interfaces::Globals->realtime + 1.0f;
		}
		else if ((GetAsyncKeyState(VK_END) & (1 << 16)))
		{
			mode = 1;
			namespam = !namespam;
			nextspam = Interfaces::Globals->realtime + 1.0f;
		}
	}

	if (namespam)
		NameSpamDisperse(mode);
}
#endif
#endif

char *mutinytagstr = new char[10]{ 55, 15, 14, 19, 20, 3, 84, 10, 13, 0 }; /*Mutiny.pw*/

void RunPublicNameChanger()
{
	static float nextupdatetime = 0.0f;

	if (Interfaces::Globals->realtime >= nextupdatetime)
	{
		std::string draw = "";
		const char *text;
		static char lastname[128]{ 0 };
		bool UseMutinyTagChk = false;
		static bool LastWasUseMutinyTagChk = false;
		bool changed = false;
		if (UseMutinyTagChk)
		{
			DecStr(mutinytagstr, 9);
			text = mutinytagstr;
			if (!LastWasUseMutinyTagChk)
			{
				changed = true;
			}
		}
		else
		{
			text = SetClanTagTxt.Value;

			if (LastWasUseMutinyTagChk || strcmp(text, lastname))
			{
				changed = true;
				strcpy(lastname, text);
			}
		}

		LastWasUseMutinyTagChk = UseMutinyTagChk;

		static int charsdrawn = 0;
		int len = strlen(text);
		if (len > 0)
		{
			int padding = len < 9 ? 11 : len + 2;
			static int indextoplacenewchar = padding;

			if (changed)
			{
				charsdrawn = 0;
				indextoplacenewchar = padding;
			}

			if (charsdrawn == len)
			{
				nextupdatetime = Interfaces::Globals->realtime + 1.5f;
				indextoplacenewchar = 1;
				charsdrawn++;
			}
			else if (charsdrawn == len + 1)
			{
				const char *partialstr = &text[indextoplacenewchar];
				SetClanTag((char*)partialstr, "");

				indextoplacenewchar++;

				if (indextoplacenewchar == len + 1)
				{
					indextoplacenewchar = padding;
					charsdrawn = 0;
				}

				nextupdatetime = Interfaces::Globals->realtime + 0.1f;
			}
			else
			{
				for (int i = 0; i < charsdrawn; i++)
				{
					draw.push_back(text[i]);
				}

				for (int i = 0; i < (indextoplacenewchar - 1); i++)
				{
					draw.push_back(' ');
				}

				draw.push_back(text[charsdrawn]);

				indextoplacenewchar--;
				if (indextoplacenewchar < 0)
				{
					indextoplacenewchar = padding - charsdrawn;
					charsdrawn++;
					nextupdatetime = Interfaces::Globals->realtime + 0.1f;
				}
				else
				{
					nextupdatetime = Interfaces::Globals->realtime + 0.1f;
				}

				SetClanTag((char*)draw.c_str(), "");
			}
			if (UseMutinyTagChk)
				EncStr(mutinytagstr, 9);
		}
	}
}