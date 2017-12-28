#include "CreateMove.h"
#include "VTHook.h"
TraceRayFn oTraceRay;

//What is this faggotry
#if 0
class CAutoWallFilter : public CTraceFilter
{
public:

	bool ShouldHitEntity(CBaseEntity *ent, unsigned int mask)
	{

		return ent != me && ent != mywep;

	}

public:
	CBaseEntity *me, *mywep;
};
#endif

bool NoTraceRayEffects = false;

void __fastcall Hooks::TraceRay(void *ths, void*, Ray_t const &ray, unsigned int mask, CTraceFilter *filter, trace_t &trace)
{
	//CAutoWallFilter newfilter;
	//newfilter.me = Interfaces::ClientEntList->GetClientEntity(Interfaces::Engine->GetLocalPlayer());
	//newfilter.mywep = (CBaseEntity*)newfilter.me->GetWeapon();
	oTraceRay(ths, ray, mask, filter, trace);
	if (NoTraceRayEffects)
		trace.surface.flags |= 4;
}