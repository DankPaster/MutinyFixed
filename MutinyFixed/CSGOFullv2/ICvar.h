#pragma once

class ConVar;

enum InitReturnVal_t
{
	INIT_FAILED = 0,
	INIT_OK,

	INIT_LAST_VAL,
};

enum AppSystemTier_t
{
	APP_SYSTEM_TIER0 = 0,
	APP_SYSTEM_TIER1,
	APP_SYSTEM_TIER2,
	APP_SYSTEM_TIER3,

	APP_SYSTEM_TIER_OTHER,
};

struct AppSystemInfo_t
{
	const char* m_pModuleName;
	const char* m_pInterfaceName;
};

typedef void* (*CreateInterfaceFn)(const char* szName, int iReturn);

class IAppSystem
{
public:
#if 1
	virtual void func0() = 0;
	virtual void func1() = 0;
	virtual void func2() = 0;
	virtual void func3() = 0;
	virtual void func4() = 0;
	virtual void func5() = 0;
	virtual void func6() = 0;
	virtual void func7() = 0;
	virtual void func8() = 0;
	virtual void func9() = 0;
#else
	// Here's where the app systems get to learn about each other 
	virtual bool Connect(CreateInterfaceFn factory) = 0;  // 0
	virtual void Disconnect() = 0;  // 1

									// Here's where systems can access other interfaces implemented by this object
									// Returns NULL if it doesn't implement the requested interface
	virtual void* QueryInterface(const char* pInterfaceName) = 0;  // 2

																   // Init, shutdown
	virtual InitReturnVal_t Init() = 0;  // 3
	virtual void Shutdown() = 0;  // 4

								  // Returns all dependent libraries
	virtual const AppSystemInfo_t* GetDependencies() = 0;  // 5

														   // Returns the tier
	virtual AppSystemTier_t GetTier() = 0;  // 6

											// Reconnect to a particular interface
	virtual void Reconnect(CreateInterfaceFn factory, const char* pInterfaceName) = 0;  // 7

																						// New virtual function added in the 26 MAY 2015 update
	virtual void UnkFunc() = 0;  // 8
	//virtual void func9() = 0;
#endif
};

struct CVarDLLIdentifier_t;

class ICVar : public IAppSystem
{
public:
	//virtual void			func10() = 0;
	virtual void			RegisterConCommand(ConVar *pCommandBase) = 0;
	virtual void			UnregisterConCommand(ConVar *pCommandBase) = 0;
	virtual void			func13() = 0;
	virtual void Dylan() = 0;
	virtual ConVar			*FindVar(const char *var_name) = 0;
	virtual void			func15() = 0;
	virtual void			func16() = 0;
	virtual void			func17() = 0;
	virtual void			func18() = 0;
	virtual void			func19() = 0;
	virtual void			func20() = 0;
	//void RegisterConCommandNew(ConVar *pCommandBase)
	//{
		//return GetVFunc<void(__thiscall*)(void*, ConVar*)>(this, 9)(this, pCommandBase);
	//}
};