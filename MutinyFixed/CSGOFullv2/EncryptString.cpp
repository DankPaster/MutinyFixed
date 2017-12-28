#include "EncryptString.h"
#include <string>

void DecStr(char *adr, const unsigned len)
{
	for (unsigned i = 0; i < len; i++)
	{
		adr[i] ^= 50;
		adr[i] ^= 72;
	}
}

void EncStr(char *adr, const unsigned len)
{
	for (unsigned i = 0; i < len; i++)
	{
		adr[i] ^= 72;
		adr[i] ^= 50;
	}
}

void DecStr(char *adr)
{
	int len = strlen(adr);
	for (unsigned i = 0; i < len; i++)
	{
		adr[i] ^= 50;
		adr[i] ^= 72;
	}
}

void EncStr(char *adr)
{
	int len = strlen(adr);
	for (unsigned i = 0; i < len; i++)
	{
		adr[i] ^= 72;
		adr[i] ^= 50;
	}
}