#include "SetClanTag.h"
void(__fastcall* pSetClanTag) (const char*tag, const char*name);

void SetClanTag(char *tag, char *name)
{
	pSetClanTag((const char*)tag, (const char*)name);
}

