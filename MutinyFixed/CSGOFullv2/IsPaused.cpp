#include "VTHook.h"
#include "Interfaces.h"
#include <intrin.h> //VS2017 requires this for _ReturnAddress

IsPausedFn oIsPaused;

bool __stdcall Hooks::IsPaused()
{
	//Disallow extrapolation!
	if (!DisableAllChk.Checked && RemoveInterpChk.Checked)
	{
		if (_ReturnAddress() == (void*)AdrOfIsPausedExtrapolate)
			return true;
	}

	return oIsPaused();
}