#include "GetViewModelFOV.h"
#include "VTHook.h"

GetViewModelFOVFn oGetViewModelFOV;

float __stdcall GetViewModelFOV() {
	if (ViewModelFOVChangerTxt.flValue != 68.0f)
	{
		return ViewModelFOVChangerTxt.flValue;
	}
	return oGetViewModelFOV();
}