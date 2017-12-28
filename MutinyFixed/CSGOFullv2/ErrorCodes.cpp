#ifdef MUTINY
#ifndef NO_MUTINY_HEARTBEAT
#include "framework.h"

void THROW_ERROR(const char* str)
{
	auto Logs = MutinyFrame::WebLoggingApi(Framework->Heartbeat->hKey, Framework->Heartbeat->hIV, Framework->Heartbeat->hUserAgent);
	Logs.LogError(str);
}
#endif
#endif