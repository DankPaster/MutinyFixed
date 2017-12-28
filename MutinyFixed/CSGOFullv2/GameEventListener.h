/// listener header file
#pragma once
#include "Interfaces.h"
#include <string>
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

class IGameEvent
{
public:
	std::string GetName(void);
	bool GetBool(std::string strKeyName, bool bDefaultValue = false);
	int GetInt(std::string strKeyName, int iDefaultValue = 0);
	std::string GetString(std::string strKeyName, int iDefaultValue = 0);
	void SetString(std::string strKeyName, std::string strValue);
	float GetFloat(std::string streKeyName, float iDefaultValue = 0);
};

class IGameEventListener2
{
public:
	virtual ~IGameEventListener2(void) {}

	virtual void FireGameEvent(IGameEvent*) = 0;

	virtual int IndicateEventHandling(void) {
		return 0x2A;
	}
};

class IGameEventManager2
{
public:
	bool AddListener(IGameEventListener2* listener, std::string name, bool serverSide);
	void RemoveListener(IGameEventListener2* listener);
	bool FireEventClientSide(IGameEvent* gameEvent);
};


typedef void(*EventFunction)(IGameEvent*);
class CGameEventListener : public IGameEventListener2
{
private:
	char* eventName;
	EventFunction gameEventFunction;
public:
	CGameEventListener(char* eventName, EventFunction gameEventFunction, bool serverSide);

	
	virtual void FireGameEvent(IGameEvent* gameEvent)
	{
		//if (LevelIsLoaded && LocalPlayer && LocalPlayer.Entity->GetAlive() && Interfaces::Engine->IsInGame())
		{
			this->gameEventFunction(gameEvent);
		}
	}
};