#include "ConfigFile.h"
#include <iostream>
#include "CharSplit.h"
#include "Overlay.h"
#include "CreateMove.h"
#include "EncryptString.h"
#include "VMProtectDefs.h"
//Must be called after all menu options have been initialized
#ifdef MUTINY
char *mutinydirstr = new char[16]{ 25, 64, 38, 38, 23, 15, 14, 19, 20, 3, 37, 10, 13, 38, 38, 0 }; /*c:\\mutiny_pw\\*/
#endif

int ReadConfigFile(char *name, Configs Type)
{
	VMP_BEGINULTRA("RCF")

	char *tempString = new char[1024];
	char *file = new char[MAX_PATH];
#ifdef MUTINY
	DecStr(mutinydirstr, 15);
	strcpy(file, mutinydirstr);
	EncStr(mutinydirstr, 15);
#else
	strcpy(file, CONFIG_FILE_PATH);
#endif

	strcat(file, name);

	if (Type != Configs::CUSTOM && Type == Configs::TWO)
		strcat(file, "2");
#ifdef _DEBUG
	//strcat(file, "_dbg");
#endif
	strcat(file, ".con");

	std::fstream configFile(file, std::ios::in);

	if (!configFile.is_open())
	{
		//printf("WARNING: could not open %s\n", CONFIG_FILE_NAME);
		delete[] tempString;
		delete[] file;
		return 0;
	}

	float ConfigVersion = CONFIG_FILE_VERSION;
	int ConfigFileType = CONFIG_FILE_TYPE;
	configFile.getline(tempString, 1024);
	if (strstr(tempString, "Version"))
	{
		StringList *list = SplitChar(tempString, " ");
		ConfigVersion = (float)atof(list->Get(1));
		delete list;
		if (ConfigVersion < 2.0f || ConfigVersion > CONFIG_FILE_VERSION)
		{
			//printf("Error: Spark.cfg is newer (%f) than this Spark revision supports (%f)!\n", globals.ConfigVersion, CONFIG_FILE_VERSION);
			delete[] tempString;
			configFile.close();
			delete[] file;
			return 0;
		}
	}
	else
	{
		//printf("ERROR: config file is corrupt!\n");
		delete[] tempString;
		configFile.close();
		delete[] file;
		return 0;
	}

	int stringlength = 0;

	configFile.getline(tempString, 1024);
	stringlength = strlen(tempString);
	if (stringlength != 0)
	{
		ConfigFileType = atoi(tempString);

		if (ConfigFileType == CONFIG_FILE_TYPE)
		{
			ReadWriteConfigFeatures(false, configFile, tempString);
		}
		else
		{
			//config file not for this build
		}
	}

	configFile.close();
	MTargetting.UpdateBoneList();
	delete[] tempString;
	delete[] file;
	return 1;
	VMP_END
}

int WriteConfigFile(char *name, Configs Type)
{
	VMP_BEGINULTRA("WCF")
	char *file = new char[MAX_PATH];
#ifdef MUTINY
	DecStr(mutinydirstr, 15);
	strcpy(file, mutinydirstr);
	CreateDirectory(mutinydirstr, NULL);
	EncStr(mutinydirstr, 15);
#else
	strcpy(file, CONFIG_FILE_PATH);
	CreateDirectory(CONFIG_FILE_PATH, NULL);
#endif
	strcat(file, name);
	if (Type != Configs::CUSTOM && Type == Configs::TWO)
		strcat(file, "2");
#ifdef _DEBUG
	//strcat(file, "_dbg");
#endif
	strcat(file, ".con");
	std::fstream configFile(file, std::ios::out | std::ios::trunc);
	if (!configFile.is_open())
	{
		//printf("ERROR: could not create %s\n", CONFIG_FILE_NAME);
		delete[] file;
		return 0;
	}

	configFile << "Version " << CONFIG_FILE_VERSION << std::endl;
	configFile << CONFIG_FILE_TYPE << std::endl;
	ReadWriteConfigFeatures(true, configFile, nullptr);
	configFile.close();
	delete[] file;
	return 1;
	VMP_END
}

//todo: return success
void ReadWriteConfigFeatures(bool Writing, std::fstream &configFile, char* tempString)
{
	VMP_BEGINULTRA("RWCF")
	int stringlength = 0;
	bool FoundTxtBoxBeforeEnd = false;
	bool FoundDropDownBeforeEnd = false;

	for (std::vector<Checkbox*>::iterator chkbx = Checkboxes.begin(); chkbx != Checkboxes.end(); chkbx++)
	{
		Checkbox* chkbox = *chkbx;
		if (Writing)
		{
			configFile << "cb\n";
			configFile << chkbox->Checked << "\n";
		}
		else
		{	
			configFile.getline(tempString, 1024);
			stringlength = strlen(tempString);
			if (stringlength == 0)
				break;
			if (strcmp(tempString, "cb"))
			{
				if (!strcmp(tempString, "tb"))
					FoundTxtBoxBeforeEnd = true;
				break;
			}
			configFile.getline(tempString, 1024);
			stringlength = strlen(tempString);
			if (stringlength == 0)
				break;
			chkbox->Checked = atoi(tempString) >= 1;
		}
	}

	for (std::vector<Textbox*>::iterator txtbx = Textboxes.begin(); txtbx != Textboxes.end(); txtbx++)
	{
		Textbox* txtbox = *txtbx;
		if (Writing)
		{
			configFile << "tb\n";
			configFile << txtbox->Value << "\n";
		}
		else
		{
			if (stringlength == 0)
				break;
			if (!FoundTxtBoxBeforeEnd)
			{
				configFile.getline(tempString, 1024);
				stringlength = strlen(tempString);
				if (stringlength == 0)
					break;
				else if (!strcmp(tempString, "dd"))
				{
					FoundDropDownBeforeEnd = true;
					break;
				}
				else if (strcmp(tempString, "tb"))
					break;
			}
			else
			{
				FoundTxtBoxBeforeEnd = false;
			}

			configFile.getline(tempString, 1024);
			stringlength = strlen(tempString);
//			if (stringlength == 0)
//				break;
			if (stringlength > sizeof(txtbox->Value))
				continue;
			if (stringlength != 0)
			{
				strcpy(txtbox->Value, tempString);
				ModifyTextboxValue(txtbox, 4);
			}
			else
			{
				stringlength = 1;
			}
		}
	}

	for (std::vector<Dropdown*>::iterator drpdn = Dropdowns.begin(); drpdn != Dropdowns.end(); drpdn++)
	{
		Dropdown* dropdown = *drpdn;
		if (Writing)
		{
			configFile << "dd\n";
			configFile << dropdown->index << "\n";
		}
		else
		{
			if (stringlength == 0)
				break;
			if (!FoundDropDownBeforeEnd)
			{
				configFile.getline(tempString, 1024);
				stringlength = strlen(tempString);
				if (stringlength == 0 || strcmp(tempString, "dd"))
					break;
			}
			else
			{
				FoundDropDownBeforeEnd = false;
			}

			configFile.getline(tempString, 1024);
			stringlength = strlen(tempString);
			if (stringlength != 0)
				dropdown->index = atoi(tempString);
			else
				stringlength = 1;
		}
	}
	VMP_END
}