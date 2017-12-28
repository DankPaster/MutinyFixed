#include "GameEventListener.h"
#include "EncryptString.h"
//////////////////////////////////////
//listener cpp file
////////////////////////////////

std::string IGameEvent::GetName(void)
{
	typedef const char* (__thiscall* OriginalFn)(void*);
	return std::string(GetVFunc<OriginalFn>(this, 1)(this));
}

bool IGameEvent::GetBool(std::string strKeyName, bool bDefaultValue)
{
	typedef bool(__thiscall* OriginalFn)(void*, const char*, bool);
	const char* c = strKeyName.c_str();
	return GetVFunc<OriginalFn>(this, 5)(this, c, bDefaultValue);
}

int IGameEvent::GetInt(std::string strKeyName, int iDefaultValue)
{
	typedef int(__thiscall* OriginalFn)(void*, const char*, int);
	const char* c = strKeyName.c_str();
	return GetVFunc<OriginalFn>(this, 6)(this, c, iDefaultValue);
}

float IGameEvent::GetFloat(std::string strKeyName, float fDefaultValue)
{
	typedef float(__thiscall* OriginalFn)(void*, const char*, float);
	const char* c = strKeyName.c_str();
	return GetVFunc<OriginalFn>(this, 8)(this, c, fDefaultValue);
}

std::string IGameEvent::GetString(std::string strKeyName, int iDefaultValue)
{
	typedef const char* (__thiscall* OriginalFn)(void*, const char*, int);
	const char* c = strKeyName.c_str();
	return std::string(GetVFunc<OriginalFn>(this, 9)(this, c, iDefaultValue));
}

void IGameEvent::SetString(std::string strKeyName, std::string strValue)
{
	typedef void(__thiscall* OriginalFn)(void*, const char*, const char*);
	const char* c = strKeyName.c_str();
	const char* c2 = strValue.c_str();
	GetVFunc<OriginalFn>(this, 15)(this, c, c2);
}


bool IGameEventManager2::AddListener(IGameEventListener2* listener, std::string name, bool serverSide)
{
	typedef bool(__thiscall* OriginalFn)(void*, IGameEventListener2*, const char*, bool);
	return GetVFunc<OriginalFn>(this, 3)(this, listener, name.c_str(), serverSide);
}

void IGameEventManager2::RemoveListener(IGameEventListener2* listener)
{
	typedef void(__thiscall* OriginalFn)(void*, IGameEventListener2*);
	GetVFunc<OriginalFn>(this, 5)(this, listener);
}

bool IGameEventManager2::FireEventClientSide(IGameEvent* gameEvent)
{
	typedef bool(__thiscall* OriginalFn)(void*, IGameEvent*);
	return GetVFunc<OriginalFn>(this, 8)(this, gameEvent);
}

CGameEventListener::CGameEventListener(char* eventName, EventFunction gameEventFunction, bool serverSide)
{
	int len = strlen(eventName);
	DecStr(eventName, len);
	char *newstr1 = new char[strlen(eventName) + 1];
	strcpy(newstr1, eventName);
	EncStr(eventName, len);
	this->eventName = newstr1;
	this->gameEventFunction = gameEventFunction;
	Interfaces::GameEventManager->AddListener(this, this->eventName, serverSide);
}