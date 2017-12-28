#pragma once
#ifndef CONFIG_H
#define CONFIG_H
//#include "cx_strenc.h"
#include <fstream>
//olstore.con
#ifndef MUTINY
#ifdef ATI
#define CONFIG_FILE_PATH "C:\\ProgramData\\ATI\\" //charenc("C:\\ProgramData\\ATI\\olstore.con")
#else
#define CONFIG_FILE_PATH "C:\\ProgramData\\NVIDIA Corporation\\" //charenc("C:\\ProgramData\\NVIDIA Corporation\\olstore.con")
#endif
#endif

#define CONFIG_FILE_VERSION 2.0f

#ifdef _DEBUG
#ifndef NO_HOOKS
//#define CONFIG_FILE_TYPE 0 //Debug
#define CONFIG_FILE_TYPE 2 //Release
#else
#define CONFIG_FILE_TYPE 1 //Debug with NO HOOKS
#endif
#else
#ifndef NO_HOOKS
#define CONFIG_FILE_TYPE 2 //Release
#else
#define CONFIG_FILE_TYPE 3 //Release with NO HOOKS
#endif
#endif

enum Configs
{
	ONE = 0,
	TWO,
	CUSTOM
};

int ReadConfigFile(char *name, Configs Type);
int WriteConfigFile(char *name, Configs Type);


void ReadWriteConfigFeatures(bool Writing, std::fstream &configFile, char* tempString);

#endif