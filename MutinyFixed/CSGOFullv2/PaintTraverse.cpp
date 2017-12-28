#include "VTHook.h"
#include "Interfaces.h"

PaintTraverseFn oPaintTraverse;
char *hudscopestr = new char[8]{ 50, 15, 30, 32, 21, 21, 23, 0 }; /*HudZoom*/

void __fastcall Hooks::PaintTraverse(void* thisptr, void* edx, unsigned int panel, bool forceRepaint, bool allowForce)
{
	if (!DisableAllChk.Checked && NoScopeChk.Checked)
	{
		DecStr(hudscopestr, 7);
		const char *panelname = Interfaces::VPanel->GetName(panel);
		if (!strcmp(panelname, hudscopestr))
		{
			EncStr(hudscopestr, 7);
			return;
		}

		EncStr(hudscopestr, 7);
	}

	oPaintTraverse(thisptr, panel, forceRepaint, allowForce);
}