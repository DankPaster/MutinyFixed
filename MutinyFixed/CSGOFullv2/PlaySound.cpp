#include "VTHook.h"
#include "Interfaces.h"

PlaySoundFn oPlaySound;

typedef void(__cdecl* IsReadyFn)();

IsReadyFn IsReady = NULL;

char *isreadysig = new char[227]{ 66, 75, 90, 90, 76, 76, 90, 90, 77, 74, 90, 90, 74, 74, 90, 90, 74, 74, 90, 90, 60, 74, 90, 90, 60, 60, 90, 90, 57, 77, 90, 90, 78, 76, 90, 90, 76, 57, 90, 90, 74, 74, 90, 90, 74, 74, 90, 90, 74, 74, 90, 90, 74, 74, 90, 90, 57, 77, 90, 90, 78, 76, 90, 90, 77, 78, 90, 90, 74, 74, 90, 90, 74, 74, 90, 90, 74, 74, 90, 90, 74, 74, 90, 90, 79, 63, 90, 90, 63, 67, 90, 90, 69, 69, 90, 90, 69, 69, 90, 90, 69, 69, 90, 90, 69, 69, 90, 90, 57, 57, 90, 90, 57, 57, 90, 90, 57, 57, 90, 90, 57, 57, 90, 90, 57, 57, 90, 90, 57, 57, 90, 90, 57, 57, 90, 90, 57, 57, 90, 90, 57, 57, 90, 90, 57, 57, 90, 90, 57, 57, 90, 90, 57, 57, 90, 90, 57, 57, 90, 90, 79, 79, 90, 90, 66, 56, 90, 90, 63, 57, 90, 90, 66, 73, 90, 90, 63, 78, 90, 90, 60, 66, 90, 90, 66, 73, 90, 90, 63, 57, 90, 90, 74, 66, 90, 90, 79, 76, 90, 90, 66, 56, 90, 90, 73, 79, 90, 90, 69, 69, 90, 90, 69, 69, 90, 90, 69, 69, 90, 90, 69, 69, 90, 90, 79, 77, 0 }; /*81  66  70  00  00  F0  FF  C7  46  6C  00  00  00  00  C7  46  74  00  00  00  00  5E  E9  ??  ??  ??  ??  CC  CC  CC  CC  CC  CC  CC  CC  CC  CC  CC  CC  CC  55  8B  EC  83  E4  F8  83  EC  08  56  8B  35  ??  ??  ??  ??  57*/
char *inpositionstr = new char[32]{ 91, 47, 51, 85, 25, 21, 23, 10, 31, 14, 19, 14, 19, 12, 31, 37, 27, 25, 25, 31, 10, 14, 37, 24, 31, 31, 10, 84, 13, 27, 12, 0 }; /*!UI/competitive_accept_beep.wav*/

void __fastcall Hooks::HookedPlaySound(void* ecx, void* edx, const char* filename)
{
	oPlaySound(ecx, filename);

	if (AutoAcceptChk.Checked)
	{
		if (!IsReady)
		{
			DecStr(inpositionstr, 31);
			DecStr(isreadysig, 226);
			IsReady = (IsReadyFn)(FindMemoryPattern(ClientHandle, isreadysig, 74) + 0x28);
			EncStr(isreadysig, 226);
			delete[] isreadysig;
			if (!IsReady)
			{
				THROW_ERROR(ERR_CANT_FIND_ISREADY_SIG);
				exit(EXIT_SUCCESS);
			}
		}

		if (!Interfaces::Engine->IsInGame() && !strcmp(filename, inpositionstr))
		{
			IsReady();
		}
	}
}