#include "Overlay.h"
#include "hDirectX.h"
#include "CSGO_HX.h"
#include "GameMemory.h"
#ifdef LICENSED
#include "Licensing2.h"
#endif
#include "ConfigFile.h"
#include "CreateMove.h"
#include "Reporting.h" //For competitive match id
#include "FixMove.h"
#include "SetClanTag.h"
#define PRIu64       "llu" //#include <inttypes.h> //For competitive match id 
#include <ctype.h>
#include "PlayerList.h"
#include "EncryptString.h"
#include "VTHook.h"
#include "ConVar.h"
#include "FixMove.h"
#include "Targetting.h"

HINSTANCE hInstance; //Set by dllmain.cpp
#ifdef MUTINY
bool EXTERNAL_WINDOW = false;
#else
					 //Just set to false for now
bool EXTERNAL_WINDOW = false;
#endif

std::vector<Button*>Buttons; //Global buttons list
std::vector<Checkbox*>Checkboxes; //Global checkbox list
std::vector<Textbox*>Textboxes; //Global textbox list
std::vector<Dropdown*>Dropdowns; //Global dropdown list
bool MENU_IS_OPEN = false;
int MENU_ACTIVE = MENU_AIMBOT;
Button *menubuttonhighlighted = nullptr; //Current menu button highlighted
Textbox *virtualkeytxtboxtoedit = nullptr; //Current textbox we are waiting to input data to from a virtual key
float flTimeRequestedToChangeVirtualKey = 0.0f;

char *olstorestr = new char[8]{ 21, 22, 9, 14, 21, 8, 31, 0 }; /*olstore*/

void AddButton(Button* button)
{
	Buttons.push_back(button);
}

void AddCheckbox(Checkbox* box)
{
	Checkboxes.push_back(box);
}

void AddTextbox(Textbox* box)
{
	Textboxes.push_back(box);
}

void ExitTextbox()
{
	if (txtboxediting != nullptr)
	{
		ModifyTextboxValue(txtboxediting, 3);
		txtboxediting = nullptr;
	}
}

void ExitDropdown()
{
	if (dropdownediting != nullptr)
	{
		dropdownediting->Clicked = false;
		dropdownediting->highlightedindex = -1;
		dropdownediting = nullptr;
	}
}

void AddDropdown(Dropdown* dropdown)
{
	Dropdowns.push_back(dropdown);
}

void OnGenericMenuBtnPressed(Button* MenuButtonToHighlight)
{
	if (menubuttonhighlighted != nullptr)
		menubuttonhighlighted->Highlighted = false;
	menubuttonhighlighted = MenuButtonToHighlight;
	menubuttonhighlighted->Highlighted = true;

	ExitDropdown();
	ExitTextbox();

	ClearHighlightedPlayer();
}

void OnAimbotMenuBtnPressed()
{
	MENU_ACTIVE = MENU_AIMBOT;
	OnGenericMenuBtnPressed(&AimbotMenuBtn);
}
char *aimbotstr = new char[7]{ 59, 19, 23, 24, 21, 14, 0 }; /*Aimbot*/
Button AimbotMenuBtn = { Vector2D(30 - OVERLAY_OFFSETX, 10 - OVERLAY_OFFSETY), MENU_NONE, Centered, &OnAimbotMenuBtnPressed, MENU_BUTTON_WIDTH, MENU_BUTTON_HEIGHT, aimbotstr, IsHighlighted, NotPressed, IsNotFaded, IsNotDisabled };
void OnTriggerbotMenuBtnPressed()
{
	MENU_ACTIVE = MENU_TRIGGERBOT;
	OnGenericMenuBtnPressed(&TriggerbotMenuBtn);
}
char *triggerbotstr = new char[10]{ 59, 15, 14, 21, 87, 60, 19, 8, 31, 0 }; /*Auto-Fire*/
Button TriggerbotMenuBtn = { Vector2D(30 + MENU_BUTTON_WIDTH + (MENU_BUTTON_WIDTH / 3) - OVERLAY_OFFSETX, 10 - OVERLAY_OFFSETY), MENU_NONE, Centered, &OnTriggerbotMenuBtnPressed, MENU_BUTTON_WIDTH, MENU_BUTTON_HEIGHT, triggerbotstr, NotHighlighted, NotPressed, IsNotFaded, IsNotDisabled };
void OnVisualsMenuBtnPressed()
{
	MENU_ACTIVE = MENU_ESP;
	OnGenericMenuBtnPressed(&VisualsMenuBtn);
}
char *visualsstr = new char[8]{ 44, 19, 9, 15, 27, 22, 9, 0 }; /*Visuals*/
Button VisualsMenuBtn = { Vector2D(30 + (MENU_BUTTON_WIDTH * 2) + (((MENU_BUTTON_WIDTH / 3) * 2)) - OVERLAY_OFFSETX, 10 - OVERLAY_OFFSETY), MENU_NONE, Centered, &OnVisualsMenuBtnPressed, MENU_BUTTON_WIDTH, MENU_BUTTON_HEIGHT, visualsstr, NotHighlighted, NotPressed, IsNotFaded, IsNotDisabled };
void OnMiscMenuBtnPressed()
{
	MENU_ACTIVE = MENU_MISC;
	OnGenericMenuBtnPressed(&MiscMenuBtn);
}
char *miscstr = new char[14]{ 55, 19, 9, 25, 31, 22, 22, 27, 20, 31, 21, 15, 9, 0 }; /*Miscellaneous*/
Button MiscMenuBtn = { Vector2D(30 + (MENU_BUTTON_WIDTH * 3) + (((MENU_BUTTON_WIDTH / 3) * 3)) - OVERLAY_OFFSETX, 10 - OVERLAY_OFFSETY), MENU_NONE, Centered, &OnMiscMenuBtnPressed, MENU_BUTTON_WIDTH, MENU_BUTTON_HEIGHT, miscstr, NotHighlighted, NotPressed, IsNotFaded, IsNotDisabled };
void OnPlayerListMenuBtnPressed()
{
	MENU_ACTIVE = MENU_PLAYERLIST;
	OnGenericMenuBtnPressed(&PlayerListMenuBtn);
}
char *playerliststr = new char[12]{ 42, 22, 27, 3, 31, 8, 90, 54, 19, 9, 14, 0 }; /*Player List*/
Button PlayerListMenuBtn = { Vector2D(30 + (MENU_BUTTON_WIDTH * 4) + (((MENU_BUTTON_WIDTH / 3) * 4)) - OVERLAY_OFFSETX, 10 - OVERLAY_OFFSETY), MENU_NONE, Centered, &OnPlayerListMenuBtnPressed, MENU_BUTTON_WIDTH, MENU_BUTTON_HEIGHT, playerliststr, NotHighlighted, NotPressed, IsNotFaded, IsNotDisabled };
void OnAboutMenuBtnPressed()
{
	MENU_ACTIVE = MENU_ABOUT;
	OnGenericMenuBtnPressed(&AboutMenuBtn);
}
char *aboutstr = new char[6]{ 59, 24, 21, 15, 14, 0 }; /*About*/
Button AboutMenuBtn = { Vector2D(30 + (MENU_BUTTON_WIDTH * 5) + (((MENU_BUTTON_WIDTH / 3) * 5)) - OVERLAY_OFFSETX, 10 - OVERLAY_OFFSETY), MENU_NONE, Centered, &OnAboutMenuBtnPressed, MENU_BUTTON_WIDTH, MENU_BUTTON_HEIGHT, aboutstr, NotHighlighted, NotPressed, IsNotFaded, IsNotDisabled };

//ABOUT MENU
char *menuoffsetxstr = new char[22]{ 55, 31, 20, 15, 90, 53, 12, 31, 8, 22, 27, 3, 90, 53, 28, 28, 9, 31, 14, 90, 34, 0 }; /*Menu Overlay Offset X*/
Textbox MenuOffsetXTxt = { Vector2D(40 - OVERLAY_OFFSETX, 100 - OVERLAY_OFFSETY), Vector2D(95 - OVERLAY_OFFSETX, 95 - OVERLAY_OFFSETY), Vector2D(95 - OVERLAY_OFFSETX, 110 - OVERLAY_OFFSETY), MENU_ABOUT, Centered, nullptr, 50, TYPES::Integer, menuoffsetxstr, -10000.0, 10000.0, "0", 0.0f, 0.0, 0, 0, IsNotFaded, IsNotDisabled };
char *menuoffsetystr = new char[22]{ 55, 31, 20, 15, 90, 53, 12, 31, 8, 22, 27, 3, 90, 53, 28, 28, 9, 31, 14, 90, 35, 0 }; /*Menu Overlay Offset Y*/
Textbox MenuOffsetYTxt = { Vector2D(40 - OVERLAY_OFFSETX, 130 - OVERLAY_OFFSETY), Vector2D(95 - OVERLAY_OFFSETX, 125 - OVERLAY_OFFSETY), Vector2D(95 - OVERLAY_OFFSETX, 140 - OVERLAY_OFFSETY), MENU_ABOUT, Centered, nullptr, 50, TYPES::Integer, menuoffsetystr, -10000.0, 10000.0, "0", 0.0f, 0.0, 0, 0, IsNotFaded, IsNotDisabled };
char *menutextredstr = new char[24]{ 46, 31, 2, 14, 90, 27, 20, 30, 90, 54, 19, 20, 31, 90, 57, 21, 22, 21, 8, 90, 40, 31, 30, 0 }; /*Text and Line Color Red*/
Textbox MenuTextLineColorRedTxt = { Vector2D(40 - OVERLAY_OFFSETX, 160 - OVERLAY_OFFSETY), Vector2D(95 - OVERLAY_OFFSETX, 155 - OVERLAY_OFFSETY), Vector2D(95 - OVERLAY_OFFSETX, 170 - OVERLAY_OFFSETY), MENU_ABOUT, Centered, nullptr, 50, TYPES::Float, menutextredstr, 0, 255.0, "255.0", 255.0f, 255.0, 0, 0, IsNotFaded, IsNotDisabled };
char *menutextgreenstr = new char[26]{ 46, 31, 2, 14, 90, 27, 20, 30, 90, 54, 19, 20, 31, 90, 57, 21, 22, 21, 8, 90, 61, 8, 31, 31, 20, 0 }; /*Text and Line Color Green*/
Textbox MenuTextLineColorGreenTxt = { Vector2D(40 - OVERLAY_OFFSETX, 190 - OVERLAY_OFFSETY), Vector2D(95 - OVERLAY_OFFSETX, 185 - OVERLAY_OFFSETY), Vector2D(95 - OVERLAY_OFFSETX, 200 - OVERLAY_OFFSETY), MENU_ABOUT, Centered, nullptr, 50, TYPES::Float, menutextgreenstr, 0.0, 255.0, "255.0", 255.0f, 255.0, 0, 0, IsNotFaded, IsNotDisabled };
char *menutextbluestr = new char[25]{ 46, 31, 2, 14, 90, 27, 20, 30, 90, 54, 19, 20, 31, 90, 57, 21, 22, 21, 8, 90, 56, 22, 15, 31, 0 }; /*Text and Line Color Blue*/
Textbox MenuTextLineColorBlueTxt = { Vector2D(40 - OVERLAY_OFFSETX, 220 - OVERLAY_OFFSETY), Vector2D(95 - OVERLAY_OFFSETX, 215 - OVERLAY_OFFSETY), Vector2D(95 - OVERLAY_OFFSETX, 230 - OVERLAY_OFFSETY), MENU_ABOUT, Centered, nullptr, 50, TYPES::Float, menutextbluestr, 0.0, 255.0, "255.0", 255.0f, 255.0, 0, 0, IsNotFaded, IsNotDisabled };
char *menuredstr = new char[15]{ 55, 31, 20, 15, 90, 57, 21, 22, 21, 8, 90, 40, 31, 30, 0 }; /*Menu Color Red*/
Textbox MenuColorRedTxt = { Vector2D(40 - OVERLAY_OFFSETX, 250 - OVERLAY_OFFSETY), Vector2D(95 - OVERLAY_OFFSETX, 245 - OVERLAY_OFFSETY), Vector2D(95 - OVERLAY_OFFSETX, 260 - OVERLAY_OFFSETY), MENU_ABOUT, Centered, nullptr, 50, TYPES::Float, menuredstr, 0.0, 255.0, "0.0", 0.0f, 0.0, 0, 0, IsNotFaded, IsNotDisabled };
char *menugreenstr = new char[17]{ 55, 31, 20, 15, 90, 57, 21, 22, 21, 8, 90, 61, 8, 31, 31, 20, 0 }; /*Menu Color Green*/
Textbox MenuColorGreenTxt = { Vector2D(40 - OVERLAY_OFFSETX, 280 - OVERLAY_OFFSETY), Vector2D(95 - OVERLAY_OFFSETX, 275 - OVERLAY_OFFSETY), Vector2D(95 - OVERLAY_OFFSETX, 290 - OVERLAY_OFFSETY), MENU_ABOUT, Centered, nullptr, 50, TYPES::Float, menugreenstr, 0.0, 255.0, "150.0", 150.0f, 0.0, 0, 0, IsNotFaded, IsNotDisabled };
char *menubluestr = new char[16]{ 55, 31, 20, 15, 90, 57, 21, 22, 21, 8, 90, 56, 22, 15, 31, 0 }; /*Menu Color Blue*/
Textbox MenuColorBlueTxt = { Vector2D(40 - OVERLAY_OFFSETX, 310 - OVERLAY_OFFSETY), Vector2D(95 - OVERLAY_OFFSETX, 305 - OVERLAY_OFFSETY), Vector2D(95 - OVERLAY_OFFSETX, 320 - OVERLAY_OFFSETY), MENU_ABOUT, Centered,nullptr,  50, TYPES::Float, menubluestr, 0.0, 255.0, "255.0", 255.0f, 0.0, 0, 0, IsNotFaded, IsNotDisabled };

void OnSaveConfigOnePressed()
{
	DecStr(olstorestr, 7);
	WriteConfigFile(olstorestr, Configs::ONE);
	EncStr(olstorestr, 7);
}
char *saveconfig1str = new char[14]{ 41, 27, 12, 31, 90, 57, 21, 20, 28, 19, 29, 90, 75, 0 }; /*Save Config 1*/
Button SaveConfigOneBtn = { Vector2D(440 - OVERLAY_OFFSETX,  64 + ((float)MENU_BUTTON_HEIGHT / 1.5f) + ((float)MENU_BUTTON_HEIGHT / 4.0f) - OVERLAY_OFFSETY), MENU_ABOUT, Centered, &OnSaveConfigOnePressed, MENU_BUTTON_WIDTH, ((float)MENU_BUTTON_HEIGHT / 1.5f), saveconfig1str, NotHighlighted, NotPressed, IsNotFaded, IsNotDisabled };

void ActivateAllMenuOptionsAfterConfigLoad()
{
	//Stop loading config files from disabling resolver
	int MaxEntities = 64;// Interfaces::ClientEntList->GetHighestEntityIndex();
	int NumPlayers = GetClientCount();
	int CountedPlayers = 0;

	if (NumPlayers)
	{
		for (int i = 0; i <= MaxEntities; i++)
		{
			CBaseEntity* Entity = Interfaces::ClientEntList->GetClientEntity(i);
			CustomPlayer* pCPlayer = &AllPlayers[i];

			if (pCPlayer->Connected)
			{
				Personalize *pPersonalize = &pCPlayer->Personalize;

				if (pPersonalize->ResolvePitchMode != NoPitch || pPersonalize->ResolveYawMode != NoYaw)
				{
					ResolverChk.Checked = true;
					break;
				}

				if (++CountedPlayers == NumPlayers)
					break;
			}
		}
	}

	for (std::vector<Checkbox*>::iterator chkbx = Checkboxes.begin(); chkbx != Checkboxes.end(); chkbx++)
	{
		Checkbox* chkbox = *chkbx;
		if (chkbox->OnPressed)
		{
			((void(*)(void))chkbox->OnPressed)();
		}
	}
}

void OnLoadConfigOnePressed()
{
	bool Autoresolvebefore = AutoResolveAllChk.Checked;
	DecStr(olstorestr, 7);
	ReadConfigFile(olstorestr, Configs::ONE);
	EncStr(olstorestr, 7);
	AutoResolveAllChk.Checked = Autoresolvebefore;
	ActivateAllMenuOptionsAfterConfigLoad();
}
char *loadconfig1str = new char[14]{ 54, 21, 27, 30, 90, 57, 21, 20, 28, 19, 29, 90, 75, 0 }; /*Load Config 1*/
Button LoadConfigOneBtn = { Vector2D(350 - OVERLAY_OFFSETX, 64 + ((float)MENU_BUTTON_HEIGHT / 1.5f) + ((float)MENU_BUTTON_HEIGHT / 4.0f) - OVERLAY_OFFSETY), MENU_ABOUT, Centered, &OnLoadConfigOnePressed, MENU_BUTTON_WIDTH, (MENU_BUTTON_HEIGHT / 1.5f), loadconfig1str, NotHighlighted, NotPressed, IsNotFaded, IsNotDisabled };
void OnSaveConfigTwoPressed()
{
	DecStr(olstorestr, 7);
	WriteConfigFile(olstorestr, Configs::TWO);
	EncStr(olstorestr, 7);
}
char *saveconfig2str = new char[14]{ 41, 27, 12, 31, 90, 57, 21, 20, 28, 19, 29, 90, 72, 0 }; /*Save Config 2*/
Button SaveConfigTwoBtn = { Vector2D(440 - OVERLAY_OFFSETX, 64 + (((float)MENU_BUTTON_HEIGHT / 1.5f) * 2) + (((float)MENU_BUTTON_HEIGHT / 4.0f) * 2) - OVERLAY_OFFSETY), MENU_ABOUT, Centered, &OnSaveConfigTwoPressed, MENU_BUTTON_WIDTH, (MENU_BUTTON_HEIGHT / 1.5f), saveconfig2str, NotHighlighted, NotPressed, IsNotFaded, IsNotDisabled };
void OnLoadConfigTwoPressed()
{
	bool Autoresolvebefore = AutoResolveAllChk.Checked;
	DecStr(olstorestr, 7);
	ReadConfigFile(olstorestr, Configs::TWO);
	EncStr(olstorestr, 7);
	AutoResolveAllChk.Checked = Autoresolvebefore;
	ActivateAllMenuOptionsAfterConfigLoad();
}
char *loadconfig2str = new char[14]{ 54, 21, 27, 30, 90, 57, 21, 20, 28, 19, 29, 90, 72, 0 }; /*Load Config 2*/
Button LoadConfigTwoBtn = { Vector2D(350 - OVERLAY_OFFSETX, 64 + (((float)MENU_BUTTON_HEIGHT / 1.5f) * 2) + (((float)MENU_BUTTON_HEIGHT / 4.0f) * 2) - OVERLAY_OFFSETY), MENU_ABOUT, Centered, &OnLoadConfigTwoPressed, MENU_BUTTON_WIDTH, (MENU_BUTTON_HEIGHT / 1.5f), loadconfig2str, NotHighlighted, NotPressed, IsNotFaded, IsNotDisabled };


void OnLoadConfigCustomPressed()
{
	bool Autoresolvebefore = AutoResolveAllChk.Checked;
	ReadConfigFile(CustomConfigTxt.Value, Configs::CUSTOM);
	AutoResolveAllChk.Checked = Autoresolvebefore;
	ActivateAllMenuOptionsAfterConfigLoad();
}
void OnSaveConfigCustomPressed()
{
	WriteConfigFile(CustomConfigTxt.Value, Configs::CUSTOM);
}
void OnSetSave()
{
	if (strlen(CustomConfigTxt.Value) == 0)
	{
		CustomConfigTxt.Value[0] = ' ';
		CustomConfigTxt.Value[1] = 0x0;
	}
	else
	{
		if (strstr(CustomConfigTxt.Value, "\\n"))
		{
			int sizeofarray = sizeof(CustomConfigTxt.Value);
			for (int i = 0; i < sizeofarray; i++)
			{
				if (CustomConfigTxt.Value[i] == 0x0)
					break;
				if (CustomConfigTxt.Value[i] == '\\')
				{
					if (i + 1 < sizeofarray)
					{
						if (CustomConfigTxt.Value[i + 1] == 0x0)
							break;
						if (CustomConfigTxt.Value[i + 1] == 'n')
						{
							CustomConfigTxt.Value[i] = '\n';
							CustomConfigTxt.Value[i + 1] = ' '; 	//todo: fix me: concat the rest of the string!
						}
					}
				}
			}
		}
	}
}

char *customconfignamestr = new char[19]{ 57, 15, 9, 14, 21, 23, 90, 57, 21, 20, 28, 19, 29, 90, 52, 27, 23, 31, 0 }; /*Custom Config Name*/
Textbox CustomConfigTxt = { Vector2D(350 - OVERLAY_OFFSETX, 64 + (((float)MENU_BUTTON_HEIGHT / 1.5f) * 4) + (((float)MENU_BUTTON_HEIGHT / 4.0f) * 4) - OVERLAY_OFFSETY), Vector2D(450 - OVERLAY_OFFSETX, 245 - OVERLAY_OFFSETY), Vector2D(95 - OVERLAY_OFFSETX, 260 - OVERLAY_OFFSETY), MENU_ABOUT, Centered, &OnSetSave, 110, TYPES::Char, customconfignamestr, -10000.0, 10000.0, "", 0.0f, 0.0, 0, 0, IsNotFaded, IsNotDisabled };

char *savecustomconfigstr = new char[12]{ 41, 27, 12, 31, 90, 57, 15, 9, 14, 21, 23, 0 }; /*Save Custom*/
Button SaveCustomConfigBtn = { Vector2D(440 - OVERLAY_OFFSETX, 64 + (((float)MENU_BUTTON_HEIGHT / 1.5f) * 5) + (((float)MENU_BUTTON_HEIGHT / 4.0f) * 5) - OVERLAY_OFFSETY), MENU_ABOUT, Centered, &OnSaveConfigCustomPressed, MENU_BUTTON_WIDTH, (MENU_BUTTON_HEIGHT / 1.5f), savecustomconfigstr, NotHighlighted, NotPressed, IsNotFaded, IsNotDisabled };

char *loadcustomconfigstr = new char[12]{ 54, 21, 27, 30, 90, 57, 15, 9, 14, 21, 23, 0 }; /*Load Custom*/
Button LoadCustomConfigBtn = { Vector2D(350 - OVERLAY_OFFSETX, 64 + (((float)MENU_BUTTON_HEIGHT / 1.5f) * 5) + (((float)MENU_BUTTON_HEIGHT / 4.0f) * 5) - OVERLAY_OFFSETY), MENU_ABOUT, Centered, &OnLoadConfigCustomPressed, MENU_BUTTON_WIDTH, (MENU_BUTTON_HEIGHT / 1.5f), loadcustomconfigstr, NotHighlighted, NotPressed, IsNotFaded, IsNotDisabled };

void OnLoadDefaultConfigOnePressed()
{
	DisableAllChk.Checked = true;

	AimbotFOVTxt.flValue = 30.0f;
	ModifyTextboxValue(&AimbotFOVTxt, 3);
	AimbotSpeedTxt.flValue = 0.07f;
	ModifyTextboxValue(&AimbotSpeedTxt, 3);
	AimbotKeyTxt.iValue = 67;
	ModifyTextboxValue(&AimbotKeyTxt, 3);
	pSilentKeyTxt.iValue = 6;
	ModifyTextboxValue(&pSilentKeyTxt, 3);
	BodyAimBelowThisHealthTxt.flValue = 30.0f;
	ModifyTextboxValue(&BodyAimBelowThisHealthTxt, 3);
	MaxTargetsPerTickTxt.iValue = 3;
	ModifyTextboxValue(&MaxTargetsPerTickTxt, 3);
	TargetFPSTxt.flValue = 80.0f;
	ModifyTextboxValue(&TargetFPSTxt, 3);
	pSilentFOVTxt.flValue = 0.5f;
	ModifyTextboxValue(&pSilentFOVTxt, 3);
	PointScaleTxt.flValue = 1.0f;
	ModifyTextboxValue(&PointScaleTxt, 3);
	AimbotChk.Checked = true;
	AimbotHoldKeyChk.Checked = true;
	AimbotAutoTargetSwitchChk.Checked = false;
	PressToRetargetChk.Checked = true;
	LockOnToHitgroupsOnlyChk.Checked = true;
	AimbotAimXChk.Checked = true;
	AimbotAimYChk.Checked = true;
	AimbotAimHeadNeckChk.Checked = true;
	AimbotAimTorsoChk.Checked = true;
	AimbotAimArmsHandsChk.Checked = true;
	AimbotAimLegsFeetChk.Checked = true;
	DetectPeekersChk.Checked = false;
	HeadshotOnlyPeekersChk.Checked = false;
	SilentAimChk.Checked = false;
	pSilentChk.Checked = false;
	LegitAimStepChk.Checked = true;
	RageAimStepChk.Checked = false;
	AutoBoneBestDamageChk.Checked = true;
	AimbotMultiPointChk.Checked = true;
	AimbotMultiPointHeadNeckChk.Checked = true;
	AimbotMultiPointChestTorsoPelvisChk.Checked = false;
	AimbotMultiPointShoulderArmsHandsChk.Checked = false;
	AimbotMultiPointLegsFeetChk.Checked = false;
	AimbotMultiPointPrioritiesOnlyChk.Checked = false;
	AimbotHitchanceArmsLegsFeetChk.Checked = false;
	BodyAimIfLethalChk.Checked = false;

	//AUTOFIRE
	TriggerBotDelayTxt.iValue = 0.0f;
	ModifyTextboxValue(&TriggerBotDelayTxt, 3);
	TriggerBotTimeToFireTxt.flValue = 0.0f;
	ModifyTextboxValue(&TriggerBotTimeToFireTxt, 3);
	TriggerbotKeyTxt.iValue = 6;
	ModifyTextboxValue(&TriggerbotKeyTxt, 3);
	AimbotHitPercentageHeadTxt.flValue = 95.0f;
	ModifyTextboxValue(&AimbotHitPercentageHeadTxt, 3);
	AimbotHitPercentageBodyTxt.flValue = 90.0f;
	ModifyTextboxValue(&AimbotHitPercentageBodyTxt, 3);
	MinimumDamageBeforeFiringTxt.flValue = 0.0f;
	ModifyTextboxValue(&MinimumDamageBeforeFiringTxt, 3);
	LowerBodyWaitTimeBeforeFiringTxt.flValue = 0.0f;
	ModifyTextboxValue(&LowerBodyWaitTimeBeforeFiringTxt, 3);
	DontShootAfterXMissesTxt.iValue = 0;
	AutoFireChk.Checked = false;
	TriggerbotWhileAimbottingChk.Checked = true;
	TriggerbotDontShootWhileJumpingChk.Checked = false;
	KnifeBotChk.Checked = true;
	AutoStopMovementChk.Checked = false;
	AutoScopeSnipersChk.Checked = false;
	OnlyHitchanceInTriggerbotChk.Checked = true;
	BodyAimIfNoBacktrackTickChk.Checked = false;
	AutoRevolverChk.Checked = false;
	AntiBacktrackChk.Checked = false;

	//VISUALS
	ThirdPersonKeyTxt.iValue = 76;
	ModifyTextboxValue(&ThirdPersonKeyTxt, 3);
	ThirdPersonOffsetChk.Checked = false;
	ShowFakedAnglesChk.Checked = true;
	ShowRealAnglesChk.Checked = false;
	AirStuckKeyTxt.iValue = VK_RSHIFT;
	ModifyTextboxValue(&AirStuckKeyTxt, 3);
	FOVChangerTxt.flValue = 90.0f;
	ModifyTextboxValue(&FOVChangerTxt, 3);
	ViewModelFOVChangerTxt.flValue = 68.0f;
	ModifyTextboxValue(&ViewModelFOVChangerTxt, 3);
	SetClanTagTxt.Value[0] = 0x0;
	GlowChk.Checked = false;
	GlowTargetsChk.Checked = true;
	ESPChk.Checked = true;
	EntityESPChk.Checked = true;
	BoxESPChk.Checked = true;
	DrawBloodResolveChk.Checked = false;
	MiniMapSpottedChk.Checked = false;
	DrawResolveModeChk.Checked = false;
	DrawSpectatorsChk.Checked = true;
	DrawRecoilCrosshairChk.Checked = true;
	DrawDamageChk.Checked = true;
	DrawAimbotBoneChk.Checked = false;
	DisablePostProcessingChk.Checked = true;
	DisableDynamicLightingChk.Checked = true;
	NoFlashChk.Checked = false;
	GrenadeTrajectoryChk.Checked = true;
	DrawHitboxesChk.Checked = false;
	AimESPChk.Checked = false;
	ArrowESPChk.Checked = false;

	//MISC
	FakeLagChokedTxt.iValue = 1;
	ModifyTextboxValue(&FakeLagChokedTxt, 3);
	SpinDegPerTickTxt.flValue = 360.0f;
	ModifyTextboxValue(&SpinDegPerTickTxt, 3);
	NoRecoilXPercentTxt.flValue = 50.0f;
	ModifyTextboxValue(&NoRecoilXPercentTxt, 3);
	NoRecoilYPercentTxt.flValue = 100.0f;
	ModifyTextboxValue(&NoRecoilYPercentTxt, 3);
	WhaleDongTxt.flValue = 0.0f;
	ModifyTextboxValue(&WhaleDongTxt, 3);
	AntiAimPitchDrop.index = AntiAimPitches::PITCH_NONE;
	AntiAimRealDrop.index = AntiAims::ANTIAIM_NONE;
	AntiAimFakeDrop.index = AntiAims::ANTIAIM_NONE;
	AntiAimJitterChk.Checked = false;
	SpinBetweenShotsChk.Checked = true;
	ManualAntiAimChk.Checked = false;
	EdgeAntiAimChk.Checked = false;
	FakeLagDropDown.index = 0;
	AutoGriefChk.Checked = false;
	BhopChk.Checked = true;
	RageBhopChk.Checked = false;
	EnginePredictChk.Checked = true;
	RemoveInterpChk.Checked = true;
	AutoWallChk.Checked = false;
	AutoWallPenetrateTeammatesChk.Checked = false;
	NoRecoilChk.Checked = false;
	NoVisualRecoilChk.Checked = false;
	NoRecoilRageChk.Checked = false;
	NoRecoilPistolsShotgunsSnipersChk.Checked = false;
	TargetTeammatesChk.Checked = false;
	ResolverChk.Checked = true;
	PredictFakeAnglesChk.Checked = true;
	FaceItChk.Checked = false;
	FiringCorrectionsChk.Checked = false;
	BloodResolverChk.Checked = false;
	BacktrackToBloodChk.Checked = false;

	//PLAYERLIST
	FireWhenEnemiesShootChk.Checked = false;
	AutoPelvisChk.Checked = false;
	AutoResolveAllChk.Checked = false;
	ValveResolverChk.Checked = true;

	//ABOUT
	MenuTextLineColorRedTxt.flValue = 255.0f;
	ModifyTextboxValue(&MenuTextLineColorRedTxt, 3);
	MenuTextLineColorGreenTxt.flValue = 255.0f;
	ModifyTextboxValue(&MenuTextLineColorGreenTxt, 3);
	MenuTextLineColorBlueTxt.flValue = 255.0f;
	ModifyTextboxValue(&MenuTextLineColorBlueTxt, 3);
	MenuColorRedTxt.flValue = 0.0f;
	ModifyTextboxValue(&MenuColorRedTxt, 3);
	MenuColorGreenTxt.flValue = 255.0f;
	ModifyTextboxValue(&MenuColorGreenTxt, 3);
	MenuColorBlueTxt.flValue = 0.0f;
	ModifyTextboxValue(&MenuColorBlueTxt, 3);
}
char *loaddefault1str = new char[15]{ 54, 21, 27, 30, 90, 62, 31, 28, 27, 15, 22, 14, 90, 75, 0 }; /*Load Default 1*/
Button LoadDefaultConfigOneBtn = { Vector2D(350 - OVERLAY_OFFSETX, 64 + (((float)MENU_BUTTON_HEIGHT / 1.5f) * 3) + (((float)MENU_BUTTON_HEIGHT / 4.0f) * 3) - OVERLAY_OFFSETY), MENU_ABOUT, Centered, &OnLoadDefaultConfigOnePressed, MENU_BUTTON_WIDTH, (MENU_BUTTON_HEIGHT / 1.5f), loaddefault1str, NotHighlighted, NotPressed, IsNotFaded, IsNotDisabled };

void OnLoadDefaultConfigTwoPressed()
{
	DisableAllChk.Checked = true;

	AimbotFOVTxt.flValue = 180.0f;
	ModifyTextboxValue(&AimbotFOVTxt, 3);
	AimbotSpeedTxt.flValue = 999.0f;
	ModifyTextboxValue(&AimbotSpeedTxt, 3);
	AimbotKeyTxt.iValue = 67;
	ModifyTextboxValue(&AimbotKeyTxt, 3);
	pSilentKeyTxt.iValue = 88;
	ModifyTextboxValue(&pSilentKeyTxt, 3);
	BodyAimBelowThisHealthTxt.flValue = 30.0f;
	ModifyTextboxValue(&BodyAimBelowThisHealthTxt, 3);
	MaxTargetsPerTickTxt.iValue = 1;
	ModifyTextboxValue(&MaxTargetsPerTickTxt, 3);
	TargetFPSTxt.flValue = 80.0f;
	ModifyTextboxValue(&TargetFPSTxt, 3);
	pSilentFOVTxt.flValue = 180.0f;
	ModifyTextboxValue(&pSilentFOVTxt, 3);
	PointScaleTxt.flValue = 1.0f;
	ModifyTextboxValue(&PointScaleTxt, 3);
	AimbotChk.Checked = true;
	AimbotHoldKeyChk.Checked = false;
	AimbotAutoTargetSwitchChk.Checked = true;
	PressToRetargetChk.Checked = true;
	LockOnToHitgroupsOnlyChk.Checked = true;
	AimbotAimXChk.Checked = true;
	AimbotAimYChk.Checked = true;
	AimbotAimHeadNeckChk.Checked = true;
	AimbotAimTorsoChk.Checked = true;
	AimbotAimArmsHandsChk.Checked = true;
	AimbotAimLegsFeetChk.Checked = true;
	DetectPeekersChk.Checked = true;
	HeadshotOnlyPeekersChk.Checked = true;
	SilentAimChk.Checked = true;
	pSilentChk.Checked = false;
	LegitAimStepChk.Checked = false;
	RageAimStepChk.Checked = true;
	AutoBoneBestDamageChk.Checked = true;
	AimbotMultiPointChk.Checked = true;
	AimbotMultiPointHeadNeckChk.Checked = true;
	AimbotMultiPointChestTorsoPelvisChk.Checked = true;
	AimbotMultiPointShoulderArmsHandsChk.Checked = true;
	AimbotMultiPointLegsFeetChk.Checked = true;
	AimbotMultiPointPrioritiesOnlyChk.Checked = false;
	AimbotHitchanceArmsLegsFeetChk.Checked = false;
	BodyAimIfLethalChk.Checked = true;

	//AUTOFIRE
	TriggerBotDelayTxt.iValue = 0.0f;
	ModifyTextboxValue(&TriggerBotDelayTxt, 3);
	TriggerBotTimeToFireTxt.flValue = 0.0f;
	ModifyTextboxValue(&TriggerBotTimeToFireTxt, 3);
	TriggerbotKeyTxt.iValue = 6;
	ModifyTextboxValue(&TriggerbotKeyTxt, 3);
	AimbotHitPercentageHeadTxt.flValue = 50.0f;
	ModifyTextboxValue(&AimbotHitPercentageHeadTxt, 3);
	AimbotHitPercentageBodyTxt.flValue = 60.0f;
	ModifyTextboxValue(&AimbotHitPercentageBodyTxt, 3);
	MinimumDamageBeforeFiringTxt.flValue = 0.0f;
	ModifyTextboxValue(&MinimumDamageBeforeFiringTxt, 3);
	LowerBodyWaitTimeBeforeFiringTxt.flValue = 0.0f;
	ModifyTextboxValue(&LowerBodyWaitTimeBeforeFiringTxt, 3);
	DontShootAfterXMissesTxt.iValue = 0;
	ModifyTextboxValue(&DontShootAfterXMissesTxt, 3);
	AutoFireChk.Checked = true;
	TriggerbotWhileAimbottingChk.Checked = true;
	TriggerbotDontShootWhileJumpingChk.Checked = false;
	KnifeBotChk.Checked = true;
	AutoStopMovementChk.Checked = false;
	AutoScopeSnipersChk.Checked = false;
	OnlyHitchanceInTriggerbotChk.Checked = false;
	BodyAimIfNoBacktrackTickChk.Checked = false;
	AutoRevolverChk.Checked = true;
	AntiBacktrackChk.Checked = true;

	//VISUALS
	ThirdPersonKeyTxt.iValue = 76;
	ModifyTextboxValue(&ThirdPersonKeyTxt, 3);
	ThirdPersonOffsetChk.Checked = false;
	ShowFakedAnglesChk.Checked = true;
	ShowRealAnglesChk.Checked = false;
	AirStuckKeyTxt.iValue = VK_RSHIFT;
	ModifyTextboxValue(&AirStuckKeyTxt, 3);
	FOVChangerTxt.flValue = 90.0f;
	ModifyTextboxValue(&FOVChangerTxt, 3);
	ViewModelFOVChangerTxt.flValue = 68.0f;
	ModifyTextboxValue(&ViewModelFOVChangerTxt, 3);
	SetClanTagTxt.Value[0] = 0x0;
	GlowChk.Checked = true;
	GlowTargetsChk.Checked = true;
	ESPChk.Checked = true;
	EntityESPChk.Checked = true;
	BoxESPChk.Checked = true;
	DrawBloodResolveChk.Checked = true;
	MiniMapSpottedChk.Checked = true;
	DrawResolveModeChk.Checked = true;
	DrawSpectatorsChk.Checked = true;
	DrawRecoilCrosshairChk.Checked = true;
	DrawDamageChk.Checked = true;
	DrawAimbotBoneChk.Checked = true;
	DisablePostProcessingChk.Checked = true;
	DisableDynamicLightingChk.Checked = true;
	NoFlashChk.Checked = true;
	GrenadeTrajectoryChk.Checked = true;
	DrawHitboxesChk.Checked = false;
	AimESPChk.Checked = false;
	ArrowESPChk.Checked = false;

	//MISC
	FakeLagChokedTxt.iValue = 4;
	ModifyTextboxValue(&FakeLagChokedTxt, 3);
	SpinDegPerTickTxt.flValue = 360.0f;
	ModifyTextboxValue(&SpinDegPerTickTxt, 3);
	NoRecoilXPercentTxt.flValue = 100.0f;
	ModifyTextboxValue(&NoRecoilXPercentTxt, 3);
	NoRecoilYPercentTxt.flValue = 100.0f;
	ModifyTextboxValue(&NoRecoilYPercentTxt, 3);
	WhaleDongTxt.flValue = 0.0f;
	ModifyTextboxValue(&WhaleDongTxt, 3);
	AntiAimPitchDrop.index = AntiAimPitches::PITCH_DOWN;
	AntiAimRealDrop.index = AntiAims::ANTIAIM_EXPERIMENTAL;
	AntiAimFakeDrop.index = ANTIAIM_DEFAULT;
	AntiAimJitterChk.Checked = false;
	SpinBetweenShotsChk.Checked = true;
	ManualAntiAimChk.Checked = true;
	EdgeAntiAimChk.Checked = false;
	FakeLagDropDown.index = 1;
	AutoGriefChk.Checked = false;
	BhopChk.Checked = true;
	RageBhopChk.Checked = true;
	EnginePredictChk.Checked = true;
	RemoveInterpChk.Checked = true;
	AutoWallChk.Checked = true;
	AutoWallPenetrateTeammatesChk.Checked = false;
	NoRecoilChk.Checked = true;
	NoVisualRecoilChk.Checked = true;
	NoRecoilRageChk.Checked = true;
	NoRecoilPistolsShotgunsSnipersChk.Checked = true;
	TargetTeammatesChk.Checked = false;
	ResolverChk.Checked = true;
	PredictFakeAnglesChk.Checked = true;
	FaceItChk.Checked = false;
	FiringCorrectionsChk.Checked = true;
	BloodResolverChk.Checked = true;
	BacktrackToBloodChk.Checked = true;

	//PLAYERLIST
	FireWhenEnemiesShootChk.Checked = false;
	AutoPelvisChk.Checked = false;
	AutoResolveAllChk.Checked = true;
	ValveResolverChk.Checked = true;

	//ABOUT
	MenuTextLineColorRedTxt.flValue = 255.0f;
	ModifyTextboxValue(&MenuTextLineColorRedTxt, 3);
	MenuTextLineColorGreenTxt.flValue = 255.0f;
	ModifyTextboxValue(&MenuTextLineColorGreenTxt, 3);
	MenuTextLineColorBlueTxt.flValue = 255.0f;
	ModifyTextboxValue(&MenuTextLineColorBlueTxt, 3);
	MenuColorRedTxt.flValue = 255.0f;
	ModifyTextboxValue(&MenuColorRedTxt, 3);
	MenuColorGreenTxt.flValue = 0.0f;
	ModifyTextboxValue(&MenuColorGreenTxt, 3);
	MenuColorBlueTxt.flValue = 0.0f;
	ModifyTextboxValue(&MenuColorBlueTxt, 3);
}
char *loaddefault2str = new char[15]{ 54, 21, 27, 30, 90, 62, 31, 28, 27, 15, 22, 14, 90, 72, 0 }; /*Load Default 2*/
Button LoadDefaultConfigTwoBtn = { Vector2D(440 - OVERLAY_OFFSETX, 64 + (((float)MENU_BUTTON_HEIGHT / 1.5f) * 3) + (((float)MENU_BUTTON_HEIGHT / 4.0f) * 3) - OVERLAY_OFFSETY), MENU_ABOUT, Centered, &OnLoadDefaultConfigTwoPressed, MENU_BUTTON_WIDTH, (MENU_BUTTON_HEIGHT / 1.5f), loaddefault2str, NotHighlighted, NotPressed, IsNotFaded, IsNotDisabled };


//TODO: FIX DISABLEALL HARD CODED ORIGIN!!

void OnCopyCompetitiveMatchID()
{
	char id[128];
	sprintf(id, "%" PRIu64 "\n", Helper_GetLastCompetitiveMatchId());
	Helper_GetLastCompetitiveMatchId();
	OpenClipboard(0);
	EmptyClipboard();
	int length = strlen(id) + 1;
	HGLOBAL hg = GlobalAlloc(GMEM_MOVEABLE, length);
	if (!hg) {
		CloseClipboard();
		return;
	}
	memcpy(GlobalLock(hg), id, length);
	GlobalUnlock(hg);
	SetClipboardData(CF_TEXT, hg);
	CloseClipboard();
	GlobalFree(hg);
}
char *copystr = new char[8]{ 57, 21, 10, 3, 90, 51, 62, 0 }; /*Copy ID*/
Button CopyCompetitiveMatchIDBtn = { Vector2D(250 - OVERLAY_OFFSETX, 64 + (((float)MENU_BUTTON_HEIGHT / 1.5f) * 1) + (((float)MENU_BUTTON_HEIGHT / 4.0f) * 1) - OVERLAY_OFFSETY), MENU_ABOUT, Centered, &OnCopyCompetitiveMatchID, MENU_BUTTON_WIDTH, (MENU_BUTTON_HEIGHT / 1.5f), copystr, NotHighlighted, NotPressed, IsNotFaded, IsNotDisabled };


//END OF ABOUT MENU

char *disableallstr = new char[12]{ 62, 19, 9, 27, 24, 22, 31, 90, 59, 22, 22, 0 }; /*Disable All*/
Checkbox DisableAllChk = { Vector2D(20 + OVERLAY_OFFSETX, -17 - OVERLAY_OFFSETY), MENU_NONE, Centered, nullptr, CheckedOn, disableallstr, NotPressed, IsNotFaded, IsNotDisabled }; //Disable Everything

																																												   //AIMBOT MENU
void OnAimbotClicked()
{
	if (!AimbotChk.Checked)
	{
		LastTargetIndex = INVALID_PLAYER; //remove any targets
		if (!NoAntiAimsAreOn(false))
			AimbotChk.Checked = true;
	}
}
char *aimbotenabledstr = new char[15]{ 59, 19, 23, 24, 21, 14, 90, 63, 20, 27, 24, 22, 31, 30, 0 }; /*Aimbot Enabled*/
Checkbox AimbotChk = { Vector2D(280 - OVERLAY_OFFSETX, 60 - OVERLAY_OFFSETY), MENU_AIMBOT, Centered, &OnAimbotClicked, CheckedOff, aimbotenabledstr, NotPressed, IsNotFaded, IsNotDisabled }; //Aimbot Enabled
void OnAimbotHoldKeyPressed()
{
	if (AimbotHoldKeyChk.Checked)
	{
		if (!NoAntiAimsAreOn(false))
			AimbotHoldKeyChk.Checked = false;
	}
}
char *aimholdkeystr = new char[19]{ 50, 21, 22, 30, 90, 49, 31, 3, 90, 46, 21, 90, 59, 19, 23, 24, 21, 14, 0 }; /*Hold Key To Aimbot*/
Checkbox AimbotHoldKeyChk = { Vector2D(280 - OVERLAY_OFFSETX, 60 + (CHECKBOX_WIDTH * 2) - OVERLAY_OFFSETY), MENU_AIMBOT, Centered, &OnAimbotHoldKeyPressed, CheckedOn, aimholdkeystr, NotPressed, IsNotFaded, IsNotDisabled };
char *autotargetstr = new char[19]{ 59, 15, 14, 21, 90, 46, 27, 8, 29, 31, 14, 90, 41, 13, 19, 14, 25, 18, 0 }; /*Auto Target Switch*/
Checkbox AimbotAutoTargetSwitchChk = { Vector2D(280 - OVERLAY_OFFSETX, 60 + ((CHECKBOX_WIDTH * 2) * 2) - OVERLAY_OFFSETY), MENU_AIMBOT, Centered, nullptr, CheckedOn, autotargetstr, NotPressed, IsNotFaded, IsNotDisabled };
char *pressretargetstr = new char[25]{ 42, 8, 31, 9, 9, 90, 59, 19, 23, 49, 31, 3, 90, 46, 21, 90, 40, 31, 14, 27, 8, 29, 31, 14, 0 }; /*Press AimKey To Retarget*/
Checkbox PressToRetargetChk = { Vector2D(280 - OVERLAY_OFFSETX, 60 + ((CHECKBOX_WIDTH * 2) * 3) - OVERLAY_OFFSETY), MENU_AIMBOT, Centered, nullptr, CheckedOn, pressretargetstr, NotPressed, IsNotFaded, IsNotDisabled };
char *lockonhitgroupsonlystr = new char[28]{ 54, 21, 25, 17, 90, 53, 20, 90, 53, 20, 22, 3, 90, 51, 28, 90, 51, 20, 90, 50, 19, 14, 29, 8, 21, 15, 10, 0 }; /*Lock On Only If In Hitgroup*/
void OnLockOnChanged()
{
	//FIX ME, THIS IS FUCKED RIGHT NOW WHEN IT'S OFF!
	LockOnToHitgroupsOnlyChk.Checked = true;
}
Checkbox LockOnToHitgroupsOnlyChk = { Vector2D(280 - OVERLAY_OFFSETX, 60 + ((CHECKBOX_WIDTH * 2) * 4) - OVERLAY_OFFSETY), MENU_AIMBOT, Centered, &OnLockOnChanged, CheckedOn, lockonhitgroupsonlystr, NotPressed, IsNotFaded, IsNotDisabled };
char *aimxstr = new char[17]{ 46, 8, 27, 25, 17, 90, 54, 31, 28, 14, 85, 40, 19, 29, 18, 14, 0 }; /*Track Left/Right*/
Checkbox AimbotAimXChk = { Vector2D(280 - OVERLAY_OFFSETX, 60 + ((CHECKBOX_WIDTH * 2) * 5) - OVERLAY_OFFSETY), MENU_AIMBOT, Centered, nullptr, CheckedOn, aimxstr, NotPressed, IsNotFaded, IsNotDisabled }; //Aimbot Aim Left/Right
char *aimystr = new char[14]{ 46, 8, 27, 25, 17, 90, 47, 10, 85, 62, 21, 13, 20, 0 }; /*Track Up/Down*/
Checkbox AimbotAimYChk = { Vector2D(280 - OVERLAY_OFFSETX, 60 + ((CHECKBOX_WIDTH * 2) * 6) - OVERLAY_OFFSETY), MENU_AIMBOT, Centered, nullptr, CheckedOn, aimystr, NotPressed, IsNotFaded, IsNotDisabled }; //Aimbot Aim Up/Down
void OnSetAimTarget()
{
	MTargetting.UpdateBoneList();
}
char *aimheadneckstr = new char[17]{ 46, 27, 8, 29, 31, 14, 90, 50, 31, 27, 30, 85, 52, 31, 25, 17, 0 }; /*Target Head/Neck*/
Checkbox AimbotAimHeadNeckChk = { Vector2D(280 - OVERLAY_OFFSETX, 60 + ((CHECKBOX_WIDTH * 2) * 7) - OVERLAY_OFFSETY), MENU_AIMBOT, Centered, &OnSetAimTarget, CheckedOn, aimheadneckstr, NotPressed, IsNotFaded, IsNotDisabled };
char *aimtorsostr = new char[26]{ 46, 27, 8, 29, 31, 14, 90, 57, 18, 31, 9, 14, 85, 46, 21, 8, 9, 21, 85, 42, 31, 22, 12, 19, 9, 0 }; /*Target Chest/Torso/Pelvis*/
Checkbox AimbotAimTorsoChk = { Vector2D(280 - OVERLAY_OFFSETX, 60 + ((CHECKBOX_WIDTH * 2) * 8) - OVERLAY_OFFSETY), MENU_AIMBOT, Centered, &OnSetAimTarget, CheckedOn, aimtorsostr, NotPressed, IsNotFaded, IsNotDisabled };
char *aimarmshandsstr = new char[27]{ 46, 27, 8, 29, 31, 14, 90, 41, 18, 21, 15, 22, 30, 31, 8, 85, 59, 8, 23, 9, 85, 50, 27, 20, 30, 9, 0 }; /*Target Shoulder/Arms/Hands*/
Checkbox AimbotAimArmsHandsChk = { Vector2D(280 - OVERLAY_OFFSETX, 60 + ((CHECKBOX_WIDTH * 2) * 9) - OVERLAY_OFFSETY), MENU_AIMBOT, Centered, &OnSetAimTarget, CheckedOn, aimarmshandsstr, NotPressed, IsNotFaded, IsNotDisabled };
char *aimlegsfeetstr = new char[17]{ 46, 27, 8, 29, 31, 14, 90, 54, 31, 29, 9, 85, 60, 31, 31, 14, 0 }; /*Target Legs/Feet*/
Checkbox AimbotAimLegsFeetChk = { Vector2D(280 - OVERLAY_OFFSETX, 60 + ((CHECKBOX_WIDTH * 2) * 10) - OVERLAY_OFFSETY), MENU_AIMBOT, Centered, &OnSetAimTarget, CheckedOn, aimlegsfeetstr, NotPressed, IsNotFaded, IsNotDisabled };
char *detectpeekingplayersstr = new char[23]{ 62, 31, 14, 31, 25, 14, 90, 42, 31, 31, 17, 19, 20, 29, 90, 42, 22, 27, 3, 31, 8, 9, 0 }; /*Detect Peeking Players*/
Checkbox DetectPeekersChk = { Vector2D(280 - OVERLAY_OFFSETX, 60 + ((CHECKBOX_WIDTH * 2) * 11) - OVERLAY_OFFSETY), MENU_AIMBOT, Centered, nullptr, CheckedOff, detectpeekingplayersstr, NotPressed, IsNotFaded, IsNotDisabled };
char *headshotonlypeekersstr = new char[22]{ 50, 31, 27, 30, 9, 18, 21, 14, 90, 53, 20, 22, 3, 90, 42, 31, 31, 17, 31, 8, 9, 0 }; /*Headshot Only Peekers*/
Checkbox HeadshotOnlyPeekersChk = { Vector2D(280 - OVERLAY_OFFSETX, 60 + ((CHECKBOX_WIDTH * 2) * 12) - OVERLAY_OFFSETY), MENU_AIMBOT, Centered, nullptr, CheckedOff, headshotonlypeekersstr, NotPressed, IsNotFaded, IsNotDisabled };

//Right side checkboxes
char *nospreadstr = new char[17]{ 52, 21, 90, 41, 10, 8, 31, 27, 30, 82, 52, 90, 85, 90, 59, 83, 0 }; /*No Spread(N / A)*/
Checkbox NoSpreadChk = { Vector2D(420 - OVERLAY_OFFSETX, 40 - OVERLAY_OFFSETY), MENU_AIMBOT, Centered, nullptr, CheckedOff, nospreadstr, NotPressed, IsNotFaded, IsNotDisabled };

char *legitbotstr = new char[10]{ 54, 31, 29, 19, 14, 90, 56, 21, 14, 0 }; /*Legit Bot*/
void OnLegitBotClicked()
{
	if (LegitBotChk.Checked)
	{
		//Aimbot Tab
		AimbotSpeedTxt.Disabled = true;
		AimbotActivateAfterXShotsTxt.Disabled = false;
		BodyAimBelowThisHealthTxt.Disabled = true;
		SpreadReduceWaitTimeTxt.Disabled = true;
		BodyAimAfterHeadNotVisibleForXSecsTxt.Disabled = true;
		DetectPeekersChk.Fade = true;
		HeadshotOnlyPeekersChk.Fade = true;
		SilentAimChk.Fade = true;
		pSilentChk.Fade = true;
		AimbotAimXChk.Fade = true;
		AimbotAimYChk.Fade = true;
		LegitAimStepChk.Fade = true;
		RageAimStepChk.Fade = true;
		AutoBoneBestDamageChk.Fade = true;
		BodyAimIfLethalChk.Fade = true;
		BodyAimIfNoBacktrackTickChk.Fade = true;
		AimbotSmoothingScaleTxt.Disabled = false;
		AimbotPosRandomScaleTxt.Disabled = false;
		AimbotAngleRandomScaleTxt.Disabled = false;
		pSilentKeyTxt.Fade = true;
		pSilentFOVTxt.Fade = true;
		MaxTargetsPerTickTxt.Fade = true;
		TargetFPSTxt.Fade = true;

		//Auto-Fire Tab
		OnlyHitchanceInTriggerbotChk.Checked = true;
		OnlyHitchanceInTriggerbotChk.Fade = true;
		MinimumDamageBeforeFiringTxt.Fade = true;
		DontShootAfterXMissesTxt.Fade = true;
		AntiBacktrackChk.Checked = false;
		AntiBacktrackChk.Fade = true;


		//Miscellaneous Tab
		RageBhopChk.Checked = false;
		RageBhopChk.Fade = true;
		FakeLagChokedTxt.iValue = 0;
		FakeLagChokedTxt.Fade = true;
		FakeLagDropDown.index = 0;
		FakeLagDropDown.Fade = true;
		FakeLagOnlyInAirChk.Checked = false;
		FakeLagOnlyInAirChk.Fade = true;
		AutoWallChk.Checked = false;
		AutoWallChk.Fade = true;
		AutoWallPenetrateTeammatesChk.Checked = false;
		AutoWallPenetrateTeammatesChk.Fade = true;
		NoVisualRecoilChk.Checked = false;
		NoVisualRecoilChk.Fade = true;
		NoRecoilRageChk.Checked = false;
		NoRecoilRageChk.Fade = true;
		BacktrackExploitChk.Checked = false;
		BacktrackExploitChk.Fade = true;

		AntiAimPitchDrop.index = 0;
		AntiAimPitchDrop.Fade = true;
		AntiAimRealDrop.index = 0;
		AntiAimRealDrop.Fade = true;
		AntiAimFakeDrop.index = 0;
		AntiAimFakeDrop.Fade = true;
		ManualAntiAimChk.Checked = false;
		ManualAntiAimChk.Fade = true;
		EdgeAntiAimChk.Checked = false;
		EdgeAntiAimChk.Fade = true;
		AntiAimJitterChk.Checked = false;
		AntiAimJitterChk.Fade = true;
		CStraffKeyBtn.Fade = true;
		CStraffKeyTxt.Fade = true;
		SpinDegPerTickTxt.Fade = true;

	}
	else
	{
		//Aimbot Tab
		AimbotSpeedTxt.Disabled = false;
		AimbotActivateAfterXShotsTxt.Disabled = true;
		BodyAimBelowThisHealthTxt.Disabled = false;
		SpreadReduceWaitTimeTxt.Disabled = false;
		BodyAimAfterHeadNotVisibleForXSecsTxt.Disabled = false;
		DetectPeekersChk.Fade = false;
		HeadshotOnlyPeekersChk.Fade = false;
		SilentAimChk.Fade = false;
		pSilentChk.Fade = false;
		AimbotAimXChk.Fade = false;
		AimbotAimYChk.Fade = false;
		LegitAimStepChk.Fade = false;
		RageAimStepChk.Fade = false;
		AutoBoneBestDamageChk.Fade = false;
		BodyAimIfLethalChk.Fade = false;
		BodyAimIfNoBacktrackTickChk.Fade = false;
		AimbotSmoothingScaleTxt.Disabled = true;
		AimbotPosRandomScaleTxt.Disabled = true;
		AimbotAngleRandomScaleTxt.Disabled = true;
		pSilentKeyTxt.Fade = false;
		pSilentFOVTxt.Fade = false;
		MaxTargetsPerTickTxt.Fade = false;
		TargetFPSTxt.Fade = false;

		//Auto-Fire Tab
		OnlyHitchanceInTriggerbotChk.Fade = false;
		MinimumDamageBeforeFiringTxt.Fade = false;
		DontShootAfterXMissesTxt.Fade = false;
		AntiBacktrackChk.Fade = false;

		//Miscellaneous Tab
		RageBhopChk.Fade = false;
		FakeLagChokedTxt.Fade = false;
		FakeLagDropDown.Fade = false;
		FakeLagOnlyInAirChk.Fade = false;
		AutoWallChk.Fade = false;
		AutoWallPenetrateTeammatesChk.Fade = false;
		NoVisualRecoilChk.Fade = false;
		NoRecoilRageChk.Fade = false;
		BacktrackExploitChk.Fade = false;

		AntiAimPitchDrop.Fade = false;
		AntiAimRealDrop.Fade = false;
		AntiAimFakeDrop.Fade = false;
		ManualAntiAimChk.Fade = false;
		EdgeAntiAimChk.Fade = false;
		AntiAimJitterChk.Fade = false;
		CStraffKeyBtn.Fade = false;
		CStraffKeyTxt.Fade = false;
		SpinDegPerTickTxt.Fade = false;
	}
}
Checkbox LegitBotChk = { Vector2D(420 - OVERLAY_OFFSETX, 60 - OVERLAY_OFFSETY), MENU_AIMBOT, Centered, &OnLegitBotClicked, CheckedOff, legitbotstr, NotPressed, IsNotFaded, IsNotDisabled };


void OnSilentAimClicked()
{
	if (!SilentAimChk.Checked)
	{
		//if (!NoAntiAimsAreOn(false) || pSilentChk.Checked)
		//	SilentAimChk.Checked = true;
	}
}
char *silentaimstr = new char[11]{ 41, 19, 22, 31, 20, 14, 90, 59, 19, 23, 0 }; /*Silent Aim*/
Checkbox SilentAimChk = { Vector2D(420 - OVERLAY_OFFSETX, 60 + (CHECKBOX_WIDTH * 2) - OVERLAY_OFFSETY), MENU_AIMBOT, Centered, &OnSilentAimClicked, CheckedOff, silentaimstr, NotPressed, IsNotFaded, IsNotDisabled };
void OnpSilentClicked()
{
	if (pSilentChk.Checked)
	{
		//AntiAimDrop.index = ANTIAIM_NONE;
		//AntiAimXDrop.index = ANTIAIM_NONE;
		//SilentAimChk.Checked = true;
		//ManualAntiAimChk.Checked = false;
		LegitAimStepChk.Checked = false;
		RageAimStepChk.Checked = false;
		//TriggerbotWhileAimbottingChk.Checked = true;
		if (AimbotFOVTxt.flValue >= 180.0f)
		{
			//	strcpy(AimbotMaxDistFromCrosshairTxt.Value, "600");
			//	AimbotMaxDistFromCrosshairTxt.iValue = 600;
			//	AimbotMaxDistFromCrosshairHalf = 300.0f;
		}
	}
}
char *psilentstr = new char[18]{ 10, 41, 19, 22, 31, 20, 14, 90, 59, 22, 13, 27, 3, 9, 90, 53, 20, 0 }; /*pSilent Always On*/
Checkbox pSilentChk = { Vector2D(420 - OVERLAY_OFFSETX, 60 + ((CHECKBOX_WIDTH * 2) * 2) - OVERLAY_OFFSETY), MENU_AIMBOT, Centered, &OnpSilentClicked, CheckedOff, psilentstr, NotPressed, IsNotFaded, IsNotDisabled };
void OnLegitAimStepClicked()
{
	if (LegitAimStepChk.Checked)
	{
		SilentAimChk.Checked = false;
		pSilentChk.Checked = false;
		RageAimStepChk.Checked = false;
		AimbotSpeedTxt.dbMin = 0.0;
		AimbotSpeedTxt.dbMax = 999.0;
		//AimbotDelayDivider.dbMin = 0.0;
		//AimbotDelayDivider.dbMax = 1.0;
		//if (AimbotDelayDivider.flValue > 1.0f || AimbotDelayDivider.flValue < 0.0f)
		{
			//	AimbotDelayDivider.flValue = 1.0f;
			//	strcpy(AimbotDelayDivider.Value, "1.0");
		}
	}
}
char *legitaimstepstr = new char[14]{ 54, 31, 29, 19, 14, 90, 59, 19, 23, 41, 14, 31, 10, 0 }; /*Legit AimStep*/
Checkbox LegitAimStepChk = { Vector2D(420 - OVERLAY_OFFSETX, 60 + ((CHECKBOX_WIDTH * 2) * 3) - OVERLAY_OFFSETY), MENU_AIMBOT, Centered, &OnLegitAimStepClicked, CheckedOff, legitaimstepstr, NotPressed, IsNotFaded, IsNotDisabled };
void OnRageAimStepClicked()
{
	if (RageAimStepChk.Checked)
	{
		pSilentChk.Checked = false;
		LegitAimStepChk.Checked = false;
		AimbotSpeedTxt.dbMin = 0.0;
		AimbotSpeedTxt.dbMax = 999.0;
		//if (AimbotDelayDivider.flValue > 1000.0f || AimbotDelayDivider.flValue < 0.0f)
		{
			//	AimbotDelayDivider.flValue = 30.0f;
			//	strcpy(AimbotDelayDivider.Value, "30.0");
		}
	}
}
char *antivacaimstepstr = new char[21]{ 40, 27, 29, 31, 90, 59, 20, 14, 19, 44, 59, 57, 90, 59, 19, 23, 41, 14, 31, 10, 0 }; /*Rage AntiVAC AimStep*/
Checkbox RageAimStepChk = { Vector2D(420 - OVERLAY_OFFSETX, 60 + ((CHECKBOX_WIDTH * 2) * 4) - OVERLAY_OFFSETY), MENU_AIMBOT, Centered, &OnRageAimStepClicked, CheckedOn, antivacaimstepstr, NotPressed, IsNotFaded, IsNotDisabled };
char *bestdmgstr = new char[22]{ 56, 31, 9, 14, 90, 62, 27, 23, 27, 29, 31, 90, 59, 15, 14, 21, 90, 56, 21, 20, 31, 0 }; /*Best Damage Auto Bone*/
Checkbox AutoBoneBestDamageChk = { Vector2D(420 - OVERLAY_OFFSETX, 60 + ((CHECKBOX_WIDTH * 2) * 5) - OVERLAY_OFFSETY), MENU_AIMBOT, Centered, nullptr, CheckedOn, bestdmgstr, NotPressed, IsNotFaded, IsNotDisabled };
char *multipointstr = new char[23]{ 55, 15, 22, 14, 19, 87, 42, 21, 19, 20, 14, 90, 46, 27, 8, 29, 31, 14, 14, 19, 20, 29, 0 }; /*Multi-Point Targetting*/
Checkbox AimbotMultiPointChk = { Vector2D(420 - OVERLAY_OFFSETX, 60 + ((CHECKBOX_WIDTH * 2) * 6) - OVERLAY_OFFSETY), MENU_AIMBOT, Centered, nullptr, CheckedOn, multipointstr, NotPressed, IsNotFaded, IsNotDisabled };
char *mpheadneckstr = new char[22]{ 55, 15, 22, 14, 19, 87, 42, 21, 19, 20, 14, 90, 50, 31, 27, 30, 85, 52, 31, 25, 17, 0 }; /*Multi-Point Head/Neck*/
Checkbox AimbotMultiPointHeadNeckChk = { Vector2D(420 - OVERLAY_OFFSETX, 60 + ((CHECKBOX_WIDTH * 2) * 7) - OVERLAY_OFFSETY), MENU_AIMBOT, Centered, nullptr, CheckedOn, mpheadneckstr, NotPressed, IsNotFaded, IsNotDisabled };
char *mpchesttorsopelvisstr = new char[31]{ 55, 15, 22, 14, 19, 87, 42, 21, 19, 20, 14, 90, 57, 18, 31, 9, 14, 85, 46, 21, 8, 9, 21, 85, 42, 31, 22, 12, 19, 9, 0 }; /*Multi-Point Chest/Torso/Pelvis*/
Checkbox AimbotMultiPointChestTorsoPelvisChk = { Vector2D(420 - OVERLAY_OFFSETX, 60 + ((CHECKBOX_WIDTH * 2) * 8) - OVERLAY_OFFSETY), MENU_AIMBOT, Centered, nullptr, CheckedOn, mpchesttorsopelvisstr, NotPressed, IsNotFaded, IsNotDisabled };
char *mpshoulderarmshandsstr = new char[32]{ 55, 15, 22, 14, 19, 87, 42, 21, 19, 20, 14, 90, 41, 18, 21, 15, 22, 30, 31, 8, 85, 59, 8, 23, 9, 85, 50, 27, 20, 30, 9, 0 }; /*Multi-Point Shoulder/Arms/Hands*/
Checkbox AimbotMultiPointShoulderArmsHandsChk = { Vector2D(420 - OVERLAY_OFFSETX, 60 + ((CHECKBOX_WIDTH * 2) * 9) - OVERLAY_OFFSETY), MENU_AIMBOT, Centered, nullptr, CheckedOn, mpshoulderarmshandsstr, NotPressed, IsNotFaded, IsNotDisabled };
char *mplegsfeetstr = new char[22]{ 55, 15, 22, 14, 19, 87, 42, 21, 19, 20, 14, 90, 54, 31, 29, 9, 85, 60, 31, 31, 14, 0 }; /*Multi-Point Legs/Feet*/
Checkbox AimbotMultiPointLegsFeetChk = { Vector2D(420 - OVERLAY_OFFSETX, 60 + ((CHECKBOX_WIDTH * 2) * 10) - OVERLAY_OFFSETY), MENU_AIMBOT, Centered, nullptr, CheckedOn, mplegsfeetstr, NotPressed, IsNotFaded, IsNotDisabled };
char *multipointprioritystr = new char[34]{ 55, 15, 22, 14, 19, 87, 42, 21, 19, 20, 14, 90, 53, 20, 22, 3, 90, 42, 8, 19, 21, 8, 19, 14, 3, 90, 46, 27, 8, 29, 31, 14, 9, 0 }; /*Multi-Point Only Priority Targets*/
Checkbox AimbotMultiPointPrioritiesOnlyChk = { Vector2D(420 - OVERLAY_OFFSETX, 60 + ((CHECKBOX_WIDTH * 2) * 11) - OVERLAY_OFFSETY), MENU_AIMBOT, Centered, nullptr, CheckedOff, multipointprioritystr, NotPressed, IsNotFaded, IsNotDisabled };
char *aimbothitchancearmslegsfeetstr = new char[31]{ 63, 2, 10, 31, 20, 9, 19, 12, 31, 90, 59, 8, 23, 9, 85, 54, 31, 29, 9, 85, 60, 31, 31, 14, 90, 57, 27, 22, 25, 9, 0 }; /*Expensive Arms/Legs/Feet Calcs*/
Checkbox AimbotHitchanceArmsLegsFeetChk = { Vector2D(420 - OVERLAY_OFFSETX, 60 + ((CHECKBOX_WIDTH * 2) * 12) - OVERLAY_OFFSETY), MENU_AIMBOT, Centered, nullptr, CheckedOff, aimbothitchancearmslegsfeetstr, NotPressed, IsNotFaded, IsNotDisabled };
char *bodyaimiflethalstr = new char[19]{ 56, 21, 30, 3, 90, 59, 19, 23, 90, 51, 28, 90, 54, 31, 14, 18, 27, 22, 0 }; /*Body Aim If Lethal*/
Checkbox BodyAimIfLethalChk = { Vector2D(420 - OVERLAY_OFFSETX, 60 + ((CHECKBOX_WIDTH * 2) * 13) - OVERLAY_OFFSETY), MENU_AIMBOT, Centered, nullptr, CheckedOff, bodyaimiflethalstr, NotPressed, IsNotFaded, IsNotDisabled };
char *baimifnobacktrackstr = new char[28]{ 56, 21, 30, 3, 90, 59, 19, 23, 90, 51, 28, 90, 57, 27, 20, 93, 14, 90, 56, 27, 25, 17, 14, 8, 27, 25, 17, 0 }; /*Body Aim If Can't Backtrack*/
Checkbox BodyAimIfNoBacktrackTickChk = { Vector2D(420 - OVERLAY_OFFSETX, 60 + ((CHECKBOX_WIDTH * 2) * 14) - OVERLAY_OFFSETY), MENU_AIMBOT, Centered,  nullptr, CheckedOff, baimifnobacktrackstr, NotPressed, IsNotFaded, IsNotDisabled };

//left side Textboxes
char *aimbotfovstr = new char[21]{ 59, 19, 23, 24, 21, 14, 90, 60, 53, 44, 90, 82, 30, 31, 29, 8, 31, 31, 9, 83, 0 }; /*Aimbot FOV (degrees)*/
Textbox AimbotFOVTxt = { Vector2D(40 - OVERLAY_OFFSETX, 70 - OVERLAY_OFFSETY), Vector2D(95 - OVERLAY_OFFSETX, 65 - OVERLAY_OFFSETY), Vector2D(95 - OVERLAY_OFFSETX, 80 - OVERLAY_OFFSETY), MENU_AIMBOT, Centered, nullptr, 50, TYPES::Float, aimbotfovstr, 0.0, 180.0, "30.0", 30.0f, 30.0, 30, 0, IsNotFaded, IsNotDisabled }; //Aimbot Max FOV (0.0 Min) //Max distance from crosshair
char *delaydividerstr = new char[32]{ 59, 19, 23, 24, 21, 14, 90, 41, 10, 31, 31, 30, 90, 82, 40, 31, 11, 15, 19, 8, 31, 9, 90, 59, 19, 23, 41, 14, 31, 10, 83, 0 }; /*Aimbot Speed (Requires AimStep)*/

																																									 //ragebot
Textbox AimbotSpeedTxt = { Vector2D(40 - OVERLAY_OFFSETX, 100 - OVERLAY_OFFSETY), Vector2D(95 - OVERLAY_OFFSETX, 95 - OVERLAY_OFFSETY), Vector2D(95 - OVERLAY_OFFSETX, 110 - OVERLAY_OFFSETY), MENU_AIMBOT, Centered, nullptr, 50, TYPES::Float, delaydividerstr, 0.0, 999.0, "30.0", 30.0f, 30.0, 10, 0, IsNotFaded, IsNotDisabled }; //Aimbot Speed (1.0 Min, 100.0 Rage)
char *aimkeystr = new char[24]{ 59, 19, 23, 24, 21, 14, 90, 49, 31, 3, 90, 82, 44, 19, 8, 14, 15, 27, 22, 49, 31, 3, 83, 0 }; /*Aimbot Key (VirtualKey)*/

																															  //legitbot
char *activateafterxshotsstr = new char[29]{ 59, 25, 14, 19, 12, 27, 14, 31, 90, 59, 28, 14, 31, 8, 90, 34, 90, 41, 18, 21, 14, 9, 90, 60, 19, 8, 31, 30, 0 }; /*Activate After X Shots Fired*/
Textbox AimbotActivateAfterXShotsTxt = { Vector2D(40 - OVERLAY_OFFSETX, 100 - OVERLAY_OFFSETY), Vector2D(95 - OVERLAY_OFFSETX, 95 - OVERLAY_OFFSETY), Vector2D(95 - OVERLAY_OFFSETX, 110 - OVERLAY_OFFSETY), MENU_AIMBOT, Centered, nullptr, 50, TYPES::Integer, activateafterxshotsstr, 0.0, 999.0, "0", 0.0f, 0.0, 0, 0, IsNotFaded, IsDisabled };

Textbox AimbotKeyTxt = { Vector2D(40 - OVERLAY_OFFSETX, 130 - OVERLAY_OFFSETY), Vector2D(95 - OVERLAY_OFFSETX, 125 - OVERLAY_OFFSETY), Vector2D(95 - OVERLAY_OFFSETX, 140 - OVERLAY_OFFSETY), MENU_AIMBOT, Centered, nullptr, 50, TYPES::Integer, aimkeystr, 0.0, 1000.0, "67", 67.0f, 67.0, 67, 0, IsNotFaded, IsNotDisabled };
char *psilentkeystr = new char[26]{ 10, 41, 19, 22, 31, 20, 14, 90, 45, 18, 19, 22, 31, 90, 50, 21, 22, 30, 19, 20, 29, 90, 49, 31, 3, 0 }; /*pSilent While Holding Key*/
Textbox pSilentKeyTxt = { Vector2D(40 - OVERLAY_OFFSETX, 160 - OVERLAY_OFFSETY), Vector2D(95 - OVERLAY_OFFSETX, 155 - OVERLAY_OFFSETY), Vector2D(95 - OVERLAY_OFFSETX, 170 - OVERLAY_OFFSETY), MENU_AIMBOT, Centered, nullptr, 50, TYPES::Integer, psilentkeystr, 0, 1000.0, "88", 88.0f, 88.0, 88, 0, IsNotFaded, IsNotDisabled };

//ragebot
char *baimbelowstr = new char[30]{ 56, 21, 30, 3, 90, 59, 19, 23, 90, 45, 18, 31, 20, 90, 50, 31, 27, 22, 14, 18, 90, 51, 9, 90, 56, 31, 22, 21, 13, 0 }; /*Body Aim When Health Is Below*/
Textbox BodyAimBelowThisHealthTxt = { Vector2D(40 - OVERLAY_OFFSETX, 190 - OVERLAY_OFFSETY), Vector2D(95 - OVERLAY_OFFSETX, 185 - OVERLAY_OFFSETY), Vector2D(95 - OVERLAY_OFFSETX, 200 - OVERLAY_OFFSETY), MENU_AIMBOT, Centered, nullptr, 50, TYPES::Float, baimbelowstr, 0.0, 100.0, "30.0", 30.0f, 30.0, 0, 0, IsNotFaded, IsNotDisabled };

//legitbot
char *legitbotsmoothingscalestr = new char[26]{ 54, 31, 29, 19, 14, 90, 56, 21, 14, 90, 41, 23, 21, 21, 14, 18, 19, 20, 29, 90, 41, 25, 27, 22, 31, 0 }; /*Legit Bot Smoothing Scale*/
Textbox AimbotSmoothingScaleTxt = { Vector2D(40 - OVERLAY_OFFSETX, 190 - OVERLAY_OFFSETY), Vector2D(95 - OVERLAY_OFFSETX, 185 - OVERLAY_OFFSETY), Vector2D(95 - OVERLAY_OFFSETX, 200 - OVERLAY_OFFSETY), MENU_AIMBOT, Centered, nullptr, 50, TYPES::Float, legitbotsmoothingscalestr, 0.0, 100.0, "10.0", 10.0f, 0.0, 0, 0, IsNotFaded, IsDisabled };

char *maxtargetspertickstr = new char[33]{ 55, 27, 2, 90, 46, 27, 8, 29, 31, 14, 9, 90, 42, 31, 8, 90, 46, 19, 25, 17, 90, 82, 28, 10, 9, 90, 24, 21, 21, 9, 14, 83, 0 }; /*Max Targets Per Tick (fps boost)*/
Textbox MaxTargetsPerTickTxt = { Vector2D(40 - OVERLAY_OFFSETX, 220 - OVERLAY_OFFSETY), Vector2D(95 - OVERLAY_OFFSETX, 215 - OVERLAY_OFFSETY), Vector2D(95 - OVERLAY_OFFSETX, 230 - OVERLAY_OFFSETY), MENU_AIMBOT, Centered, nullptr, 50, TYPES::Integer, maxtargetspertickstr, 1.0, 64.0, "2", 2.0f, 2.0, 2, 2, IsNotFaded, IsNotDisabled };
char *targetfpsstr = new char[20]{ 62, 31, 9, 19, 8, 31, 30, 90, 55, 19, 20, 19, 23, 15, 23, 90, 60, 42, 41, 0 }; /*Desired Minimum FPS*/
Textbox TargetFPSTxt = { Vector2D(40 - OVERLAY_OFFSETX, 250 - OVERLAY_OFFSETY), Vector2D(95 - OVERLAY_OFFSETX, 245 - OVERLAY_OFFSETY), Vector2D(95 - OVERLAY_OFFSETX, 260 - OVERLAY_OFFSETY), MENU_AIMBOT, Centered, nullptr, 50, TYPES::Float, targetfpsstr, 1.0, 1024.0, "80.0", 80.0f, 80.0, 3, 3, IsNotFaded, IsNotDisabled };
char *psilentfovstr = new char[22]{ 10, 41, 19, 22, 31, 20, 14, 90, 60, 53, 44, 90, 82, 30, 31, 29, 8, 31, 31, 9, 83, 0 }; /*pSilent FOV (degrees)*/
Textbox pSilentFOVTxt = { Vector2D(40 - OVERLAY_OFFSETX, 280 - OVERLAY_OFFSETY), Vector2D(95 - OVERLAY_OFFSETX, 275 - OVERLAY_OFFSETY), Vector2D(95 - OVERLAY_OFFSETX, 290 - OVERLAY_OFFSETY), MENU_AIMBOT, Centered, nullptr, 50, TYPES::Float, psilentfovstr, 0.0001, 179.999, "0.7", 0.7f, 0.7, 0, 0, IsNotFaded, IsNotDisabled };
char *pointscalestr = new char[12]{ 42, 21, 19, 20, 14, 90, 41, 25, 27, 22, 31, 0 }; /*Point Scale*/
Textbox PointScaleTxt = { Vector2D(40 - OVERLAY_OFFSETX, 310 - OVERLAY_OFFSETY), Vector2D(95 - OVERLAY_OFFSETX, 305 - OVERLAY_OFFSETY), Vector2D(95 - OVERLAY_OFFSETX, 320 - OVERLAY_OFFSETY), MENU_AIMBOT, Centered, nullptr, 50, TYPES::Float, pointscalestr, 0.0, 1.0, "1.0", 1.0f, 1.0, 0, 0, IsNotFaded, IsNotDisabled };

//ragebot
char *spreadwaitsecsstr = new char[53]{ 45, 27, 19, 14, 90, 9, 31, 25, 9, 90, 28, 21, 8, 90, 9, 10, 8, 31, 27, 30, 90, 14, 21, 90, 8, 31, 30, 15, 25, 31, 90, 24, 31, 28, 21, 8, 31, 90, 27, 19, 23, 19, 20, 29, 90, 27, 14, 90, 24, 21, 30, 3, 0 }; /*Wait secs for spread to reduce before aiming at body*/
Textbox SpreadReduceWaitTimeTxt = { Vector2D(40 - OVERLAY_OFFSETX, 340 - OVERLAY_OFFSETY), Vector2D(95 - OVERLAY_OFFSETX, 335 - OVERLAY_OFFSETY), Vector2D(95 - OVERLAY_OFFSETX, 350 - OVERLAY_OFFSETY), MENU_AIMBOT, Centered, nullptr, 50, TYPES::Float, spreadwaitsecsstr, 0.0, 60.0, "0.6", 0.6f, 0.6, 0, 0, IsNotFaded, IsNotDisabled };

//legitbot
char *legitbotposrandomscalestr = new char[32]{ 54, 31, 29, 19, 14, 90, 56, 21, 14, 90, 42, 21, 9, 19, 14, 19, 21, 20, 90, 40, 27, 20, 30, 21, 23, 90, 41, 25, 27, 22, 31, 0 }; /*Legit Bot Position Random Scale*/
Textbox AimbotPosRandomScaleTxt = { Vector2D(40 - OVERLAY_OFFSETX, 340 - OVERLAY_OFFSETY), Vector2D(95 - OVERLAY_OFFSETX, 335 - OVERLAY_OFFSETY), Vector2D(95 - OVERLAY_OFFSETX, 350 - OVERLAY_OFFSETY), MENU_AIMBOT, Centered, nullptr, 50, TYPES::Float, legitbotposrandomscalestr, 0.0, 100.0, "5.0", 5.0f, 0.0, 0, 0, IsNotFaded, IsDisabled };

//ragebot
char *bodyaimafterheadnotvisibleafterxsecsstr = new char[55]{ 53, 20, 22, 3, 90, 14, 8, 3, 90, 24, 21, 30, 3, 90, 27, 19, 23, 19, 20, 29, 90, 27, 28, 14, 31, 8, 90, 18, 31, 27, 30, 90, 20, 21, 14, 90, 12, 19, 9, 19, 24, 22, 31, 90, 28, 21, 8, 90, 2, 90, 9, 31, 25, 9, 0 }; /*Only try body aiming after head not visible for x secs*/
Textbox BodyAimAfterHeadNotVisibleForXSecsTxt = { Vector2D(40 - OVERLAY_OFFSETX, 370 - OVERLAY_OFFSETY), Vector2D(95 - OVERLAY_OFFSETX, 365 - OVERLAY_OFFSETY), Vector2D(95 - OVERLAY_OFFSETX, 380 - OVERLAY_OFFSETY), MENU_AIMBOT, Centered, nullptr, 50, TYPES::Float, bodyaimafterheadnotvisibleafterxsecsstr, 0.0, 60.0, "2.0", 2.0f, 2.0, 0, 0, IsNotFaded, IsNotDisabled };

//legitbot
char *legitbotanglerandomscalestr = new char[29]{ 54, 31, 29, 19, 14, 90, 56, 21, 14, 90, 59, 20, 29, 22, 31, 90, 40, 27, 20, 30, 21, 23, 90, 41, 25, 27, 22, 31, 0 }; /*Legit Bot Angle Random Scale*/
Textbox AimbotAngleRandomScaleTxt = { Vector2D(40 - OVERLAY_OFFSETX, 370 - OVERLAY_OFFSETY), Vector2D(95 - OVERLAY_OFFSETX, 365 - OVERLAY_OFFSETY), Vector2D(95 - OVERLAY_OFFSETX, 380 - OVERLAY_OFFSETY), MENU_AIMBOT, Centered, nullptr, 50, TYPES::Float, legitbotanglerandomscalestr, 0.0, 100.0, "0.2", 0.2f, 0.0, 0, 0, IsNotFaded, IsDisabled };

void OnSetAimbotKeyPressed()
{
	flTimeRequestedToChangeVirtualKey = Interfaces::Globals->realtime;
	virtualkeytxtboxtoedit = &AimbotKeyTxt;
}
char *setaimbotkeystr = new char[4]{ 41, 31, 14, 0 }; /*Set*/
Button SetAimbotKeyBtn = { Vector2D(225 - OVERLAY_OFFSETX, 130 - 5 - OVERLAY_OFFSETY), MENU_AIMBOT, Centered, &OnSetAimbotKeyPressed, 35, 20, setaimbotkeystr, NotHighlighted, NotPressed, IsNotFaded, IsNotDisabled };

void OnSetpSilentKeyPressed()
{
	flTimeRequestedToChangeVirtualKey = Interfaces::Globals->realtime;
	virtualkeytxtboxtoedit = &pSilentKeyTxt;
}
char *setpsilentkeystr = new char[4]{ 41, 31, 14, 0 }; /*Set*/
Button SetpSilentKeyBtn = { Vector2D(225 - OVERLAY_OFFSETX, 160 - 5 - OVERLAY_OFFSETY), MENU_AIMBOT, Centered, &OnSetpSilentKeyPressed, 35, 20, setpsilentkeystr, NotHighlighted, NotPressed, IsNotFaded, IsNotDisabled };



//END OF AIMBOT MENU


//TRIGGERBOT MENU
char *triggerstr = new char[28]{ 59, 15, 14, 21, 87, 60, 19, 8, 31, 90, 45, 19, 14, 18, 21, 15, 14, 90, 49, 31, 3, 90, 42, 8, 31, 9, 9, 0 }; /*Auto-Fire Without Key Press*/
Checkbox AutoFireChk = { Vector2D(280 - OVERLAY_OFFSETX, 60 - OVERLAY_OFFSETY), MENU_TRIGGERBOT, Centered, nullptr, CheckedOff, triggerstr, NotPressed, IsNotFaded, IsNotDisabled };
char *triggeraimstr = new char[27]{ 59, 15, 14, 21, 87, 60, 19, 8, 31, 90, 45, 18, 19, 22, 31, 90, 59, 19, 23, 22, 21, 25, 17, 19, 20, 29, 0 }; /*Auto-Fire While Aimlocking*/
Checkbox TriggerbotWhileAimbottingChk = { Vector2D(280 - OVERLAY_OFFSETX, 60 + (CHECKBOX_WIDTH * 2) - OVERLAY_OFFSETY), MENU_TRIGGERBOT, Centered,  nullptr, CheckedOn, triggeraimstr, NotPressed, IsNotFaded, IsNotDisabled };
char *fireatpeakstr = new char[31]{ 53, 20, 22, 3, 90, 28, 19, 8, 31, 90, 27, 14, 90, 14, 18, 31, 90, 10, 31, 27, 17, 90, 21, 28, 90, 16, 15, 23, 10, 9, 0 }; /*Only fire at the peak of jumps*/
Checkbox TriggerbotDontShootWhileJumpingChk = { Vector2D(280 - OVERLAY_OFFSETX, 60 + (CHECKBOX_WIDTH * 2) * 2 - OVERLAY_OFFSETY), MENU_TRIGGERBOT, Centered,  nullptr, CheckedOff, fireatpeakstr, NotPressed, IsNotFaded, IsNotDisabled };
char *knifebotstr = new char[10]{ 49, 20, 19, 28, 31, 90, 56, 21, 14, 0 }; /*Knife Bot*/
Checkbox KnifeBotChk = { Vector2D(280 - OVERLAY_OFFSETX, 60 + (CHECKBOX_WIDTH * 2) * 3 - OVERLAY_OFFSETY), MENU_TRIGGERBOT, Centered,  nullptr, CheckedOn, knifebotstr, NotPressed, IsNotFaded, IsNotDisabled };
char *autostopmovementstr = new char[19]{ 59, 15, 14, 21, 90, 41, 14, 21, 10, 90, 55, 21, 12, 31, 23, 31, 20, 14, 0 }; /*Auto Stop Movement*/
Checkbox AutoStopMovementChk = { Vector2D(280 - OVERLAY_OFFSETX, 60 + (CHECKBOX_WIDTH * 2) * 4 - OVERLAY_OFFSETY), MENU_TRIGGERBOT, Centered,  nullptr, CheckedOff, autostopmovementstr, NotPressed, IsNotFaded, IsNotDisabled };
char *autoscopesnipersstr = new char[21]{ 59, 15, 14, 21, 90, 41, 25, 21, 10, 31, 90, 82, 41, 20, 19, 10, 31, 8, 9, 83, 0 }; /*Auto Scope (Snipers)*/
Checkbox AutoScopeSnipersChk = { Vector2D(410 - OVERLAY_OFFSETX, 60 + (CHECKBOX_WIDTH * 2) * 4 - OVERLAY_OFFSETY), MENU_TRIGGERBOT, Centered,  nullptr, CheckedOff, autoscopesnipersstr, NotPressed, IsNotFaded, IsNotDisabled };
char *onlyhitchanceautofirestr = new char[53]{ 50, 19, 14, 25, 18, 27, 20, 25, 31, 90, 53, 20, 22, 3, 90, 62, 15, 8, 19, 20, 29, 90, 59, 15, 14, 21, 60, 19, 8, 31, 90, 82, 54, 31, 9, 9, 90, 56, 59, 19, 23, 86, 90, 55, 21, 8, 31, 90, 60, 42, 41, 83, 0 }; /*Hitchance Only During AutoFire (Less BAim, More FPS)*/
void OnHitchanceInTriggerbot()
{
	//OnlyHitchanceInTriggerbotChk.Checked = false;
}
Checkbox OnlyHitchanceInTriggerbotChk = { Vector2D(280 - OVERLAY_OFFSETX, 60 + (CHECKBOX_WIDTH * 2) * 5 - OVERLAY_OFFSETY), MENU_TRIGGERBOT, Centered, &OnHitchanceInTriggerbot, CheckedOff, onlyhitchanceautofirestr, NotPressed, IsNotFaded, IsNotDisabled };
char *autorevolverstr = new char[14]{ 59, 15, 14, 21, 90, 40, 31, 12, 21, 22, 12, 31, 8, 0 }; /*Auto Revolver*/
Checkbox AutoRevolverChk = { Vector2D(280 - OVERLAY_OFFSETX, 60 + (CHECKBOX_WIDTH * 2) * 6 - OVERLAY_OFFSETY), MENU_TRIGGERBOT, Centered, nullptr, CheckedOff, autorevolverstr, NotPressed, IsNotFaded, IsNotDisabled };
char *antibacktrackstr = new char[59]{ 59, 20, 14, 19, 87, 56, 27, 25, 17, 14, 8, 27, 25, 17, 90, 82, 40, 31, 11, 15, 19, 8, 31, 9, 90, 60, 27, 17, 31, 54, 27, 29, 90, 68, 90, 73, 90, 27, 20, 30, 90, 23, 27, 3, 90, 9, 18, 21, 21, 14, 90, 9, 22, 21, 13, 31, 8, 83, 0 }; /*Anti-Backtrack (Requires FakeLag > 3 and may shoot slower)*/
Checkbox AntiBacktrackChk = { Vector2D(280 - OVERLAY_OFFSETX, 60 + (CHECKBOX_WIDTH * 2) * 7 - OVERLAY_OFFSETY), MENU_TRIGGERBOT, Centered, nullptr, CheckedOff, antibacktrackstr, NotPressed, IsNotFaded, IsNotDisabled };

char *triggerdelaystr = new char[24]{ 46, 8, 19, 29, 29, 31, 8, 24, 21, 14, 90, 40, 27, 14, 31, 90, 53, 28, 90, 60, 19, 8, 31, 0 }; /*Triggerbot Rate Of Fire*/
Textbox TriggerBotDelayTxt = { Vector2D(40 - OVERLAY_OFFSETX, 70 - OVERLAY_OFFSETY), Vector2D(95 - OVERLAY_OFFSETX, 65 - OVERLAY_OFFSETY), Vector2D(95 - OVERLAY_OFFSETX, 80 - OVERLAY_OFFSETY), MENU_TRIGGERBOT, Centered, nullptr, 50, TYPES::Integer, triggerdelaystr, 0.0, 5000.0, "2", 2.0f, 2.0, 2, 2, IsNotFaded, IsNotDisabled }; // Triggerbot Rate Of Fire (1 Min) //Time to click again after clicking
char *triggertimetofirestr = new char[29]{ 46, 8, 19, 29, 29, 31, 8, 24, 21, 14, 90, 46, 19, 23, 31, 90, 46, 21, 90, 60, 19, 8, 31, 90, 82, 23, 9, 83, 0 }; /*Triggerbot Time To Fire (ms)*/
Textbox TriggerBotTimeToFireTxt = { Vector2D(40 - OVERLAY_OFFSETX, 100 - OVERLAY_OFFSETY), Vector2D(95 - OVERLAY_OFFSETX, 95 - OVERLAY_OFFSETY), Vector2D(95 - OVERLAY_OFFSETX, 110 - OVERLAY_OFFSETY), MENU_TRIGGERBOT, Centered, nullptr, 50, TYPES::Float, triggertimetofirestr, 0.0, 5000.0, "0.0", 0.0f, 0.0, 0, 0, IsNotFaded, IsNotDisabled };
char *triggerbotkeystr = new char[28]{ 46, 8, 19, 29, 29, 31, 8, 24, 21, 14, 90, 49, 31, 3, 90, 82, 44, 19, 8, 14, 15, 27, 22, 49, 31, 3, 83, 0 }; /*Triggerbot Key (VirtualKey)*/
Textbox TriggerbotKeyTxt = { Vector2D(40 - OVERLAY_OFFSETX, 130 - OVERLAY_OFFSETY), Vector2D(95 - OVERLAY_OFFSETX, 125 - OVERLAY_OFFSETY), Vector2D(95 - OVERLAY_OFFSETX, 140 - OVERLAY_OFFSETY), MENU_TRIGGERBOT, Centered, nullptr, 50, TYPES::Integer, triggerbotkeystr, 0.0, 1000.0, "6", 6.0f, 6.0, 6, 6, IsNotFaded, IsNotDisabled };
char *hitchanceheadstr = new char[25]{ 55, 19, 20, 19, 23, 15, 23, 90, 50, 19, 14, 25, 18, 27, 20, 25, 31, 90, 50, 31, 27, 30, 90, 95, 0 }; /*Minimum Hitchance Head %%*/
Textbox AimbotHitPercentageHeadTxt = { Vector2D(40 - OVERLAY_OFFSETX, 160 - OVERLAY_OFFSETY), Vector2D(95 - OVERLAY_OFFSETX, 155 - OVERLAY_OFFSETY), Vector2D(95 - OVERLAY_OFFSETX, 170 - OVERLAY_OFFSETY), MENU_TRIGGERBOT, Centered, nullptr, 50, TYPES::Float, hitchanceheadstr, 0.0, 100.0, "75.0", 75.0f, 75.0, 75, 0, IsNotFaded, IsNotDisabled };
char *hitchancebodystr = new char[25]{ 55, 19, 20, 19, 23, 15, 23, 90, 50, 19, 14, 25, 18, 27, 20, 25, 31, 90, 56, 21, 30, 3, 90, 95, 0 }; /*Minimum Hitchance Body %%*/
Textbox AimbotHitPercentageBodyTxt = { Vector2D(40 - OVERLAY_OFFSETX, 190 - OVERLAY_OFFSETY), Vector2D(95 - OVERLAY_OFFSETX, 185 - OVERLAY_OFFSETY), Vector2D(95 - OVERLAY_OFFSETX, 200 - OVERLAY_OFFSETY), MENU_TRIGGERBOT, Centered, nullptr, 50, TYPES::Float, hitchancebodystr, 0.0, 100.0, "75.0", 75.0f, 75.0, 75, 0, IsNotFaded, IsNotDisabled };
char *mindmgstr = new char[76]{ 55, 19, 20, 19, 23, 15, 23, 90, 62, 27, 23, 27, 29, 31, 90, 56, 31, 28, 21, 8, 31, 90, 60, 19, 8, 19, 20, 29, 90, 82, 40, 31, 11, 15, 19, 8, 31, 9, 90, 59, 15, 14, 21, 45, 27, 22, 22, 90, 27, 20, 30, 90, 56, 31, 9, 14, 90, 62, 27, 23, 27, 29, 31, 90, 59, 15, 14, 21, 90, 56, 21, 20, 31, 80, 83, 0 }; /*Minimum Damage Before Firing (Requires AutoWall and Best Damage Auto Bone*)*/
Textbox MinimumDamageBeforeFiringTxt = { Vector2D(40 - OVERLAY_OFFSETX, 220 - OVERLAY_OFFSETY), Vector2D(95 - OVERLAY_OFFSETX, 215 - OVERLAY_OFFSETY), Vector2D(95 - OVERLAY_OFFSETX, 230 - OVERLAY_OFFSETY), MENU_TRIGGERBOT, Centered, nullptr, 50, TYPES::Float, mindmgstr, 0.0, 100.0, "0.0", 0.0f, 0.0, 0, 0, IsNotFaded, IsNotDisabled };
char *lowerbodywaitstr = new char[45]{ 45, 27, 19, 14, 90, 14, 19, 23, 31, 90, 28, 21, 8, 90, 8, 31, 9, 21, 22, 12, 31, 30, 90, 22, 21, 13, 31, 8, 90, 24, 21, 30, 3, 90, 3, 27, 13, 90, 82, 9, 31, 25, 9, 83, 0 }; /*Wait time for resolved lower body yaw (secs)*/
Textbox LowerBodyWaitTimeBeforeFiringTxt = { Vector2D(40 - OVERLAY_OFFSETX, 250 - OVERLAY_OFFSETY), Vector2D(95 - OVERLAY_OFFSETX, 245 - OVERLAY_OFFSETY), Vector2D(95 - OVERLAY_OFFSETX, 260 - OVERLAY_OFFSETY), MENU_TRIGGERBOT, Centered, nullptr, 50, TYPES::Float, lowerbodywaitstr, 0.0, 999.0, "0.0", 0.0f, 0.0, 0, 0, IsNotFaded, IsNotDisabled };
char *dontshootafterxmissesstr = new char[27]{ 62, 21, 20, 93, 14, 90, 41, 18, 21, 21, 14, 90, 59, 28, 14, 31, 8, 90, 34, 90, 55, 19, 9, 9, 31, 9, 0 }; /*Don't Shoot After X Misses*/
Textbox DontShootAfterXMissesTxt = { Vector2D(40 - OVERLAY_OFFSETX, 280 - OVERLAY_OFFSETY), Vector2D(95 - OVERLAY_OFFSETX, 275 - OVERLAY_OFFSETY), Vector2D(95 - OVERLAY_OFFSETX, 290 - OVERLAY_OFFSETY), MENU_TRIGGERBOT, Centered, nullptr, 50, TYPES::Integer, dontshootafterxmissesstr, 0.0, 999.0, "0", 0.0f, 0.0, 0, 0, IsNotFaded, IsNotDisabled };

void OnSetTriggerbotKeyPressed()
{
	flTimeRequestedToChangeVirtualKey = Interfaces::Globals->realtime;
	virtualkeytxtboxtoedit = &TriggerbotKeyTxt;
}
char *settriggerbotkeystr = new char[4]{ 41, 31, 14, 0 }; /*Set*/
Button SetTriggerbotKeyBtn = { Vector2D(240 - OVERLAY_OFFSETX, 130 - 5 - OVERLAY_OFFSETY), MENU_TRIGGERBOT, Centered, &OnSetTriggerbotKeyPressed, 35, 20, settriggerbotkeystr, NotHighlighted, NotPressed, IsNotFaded, IsNotDisabled };

//END OF TRIGGERBOT MENU


//VISUALS MENU
char *glowstr = new char[5]{ 61, 22, 21, 13, 0 }; /*Glow*/
Checkbox GlowChk = { Vector2D(280 - OVERLAY_OFFSETX, 60 - OVERLAY_OFFSETY), MENU_ESP, Centered, nullptr, CheckedOff, glowstr, NotPressed, IsNotFaded, IsNotDisabled }; //Glow
char *highlightstr = new char[18]{ 50, 19, 29, 18, 22, 19, 29, 18, 14, 90, 46, 27, 8, 29, 31, 14, 9, 0 }; /*Highlight Targets*/
Checkbox GlowTargetsChk = { Vector2D(340 - OVERLAY_OFFSETX, 60 - OVERLAY_OFFSETY), MENU_ESP, Centered, nullptr, CheckedOn, highlightstr, NotPressed, IsNotFaded, IsNotDisabled }; //Glow
char *espstr = new char[4]{ 63, 41, 42, 0 }; /*ESP*/
Checkbox ESPChk = { Vector2D(280 - OVERLAY_OFFSETX, 60 + (CHECKBOX_WIDTH * 2) - OVERLAY_OFFSETY), MENU_ESP, Centered, nullptr, CheckedOff, espstr, NotPressed, IsNotFaded, IsNotDisabled }; //ESP
char *entityespstr = new char[18]{ 45, 31, 27, 10, 21, 20, 90, 85, 90, 56, 21, 23, 24, 90, 63, 41, 42, 0 }; /*Weapon / Bomb ESP*/
Checkbox EntityESPChk = { Vector2D(340 - OVERLAY_OFFSETX, 60 + (CHECKBOX_WIDTH * 2) - OVERLAY_OFFSETY), MENU_ESP, Centered, nullptr, CheckedOff, entityespstr, NotPressed, IsNotFaded, IsNotDisabled }; //ESP
char *boxespstr = new char[15]{ 42, 22, 27, 3, 31, 8, 90, 56, 21, 2, 90, 63, 41, 42, 0 }; /*Player Box ESP*/
Checkbox BoxESPChk = { Vector2D(280 - OVERLAY_OFFSETX, 60 + ((CHECKBOX_WIDTH * 2) * 2) - OVERLAY_OFFSETY), MENU_ESP, Centered, nullptr, CheckedOff, boxespstr, NotPressed, IsNotFaded, IsNotDisabled }; //Player Boxes
char *drawbloodheadstr = new char[34]{ 62, 8, 27, 13, 90, 50, 31, 27, 30, 90, 28, 8, 21, 23, 90, 40, 31, 9, 21, 22, 12, 31, 30, 90, 56, 21, 30, 3, 90, 50, 19, 14, 9, 0 }; /*Draw Head from Resolved Body Hits*/
Checkbox DrawBloodResolveChk = { Vector2D(370 - OVERLAY_OFFSETX, 60 + ((CHECKBOX_WIDTH * 2) * 2) - OVERLAY_OFFSETY), MENU_ESP, Centered, nullptr, CheckedOff, drawbloodheadstr, NotPressed, IsNotFaded, IsNotDisabled };
char *minimapenemiesspottedstr = new char[24]{ 55, 19, 20, 19, 23, 27, 10, 90, 63, 20, 31, 23, 19, 31, 9, 90, 41, 10, 21, 14, 14, 31, 30, 0 }; /*Minimap Enemies Spotted*/
Checkbox MiniMapSpottedChk = { Vector2D(280 - OVERLAY_OFFSETX, 60 + ((CHECKBOX_WIDTH * 2) * 3) - OVERLAY_OFFSETY), MENU_ESP, Centered, nullptr, CheckedOff, minimapenemiesspottedstr, NotPressed, IsNotFaded, IsNotDisabled }; //Minimap Enemies Spotted
char *drawresolvemodestr = new char[18]{ 62, 8, 27, 13, 90, 40, 31, 9, 21, 22, 12, 31, 90, 55, 21, 30, 31, 0 }; /*Draw Resolve Mode*/
Checkbox DrawResolveModeChk = { Vector2D(425 - OVERLAY_OFFSETX, 60 + ((CHECKBOX_WIDTH * 2) * 3) - OVERLAY_OFFSETY), MENU_ESP, Centered, nullptr, CheckedOff, drawresolvemodestr, NotPressed, IsNotFaded, IsNotDisabled };
char *drawspecstr = new char[28]{ 62, 8, 27, 13, 90, 42, 22, 27, 3, 31, 8, 9, 90, 41, 10, 31, 25, 14, 27, 14, 19, 20, 29, 90, 35, 21, 15, 0 }; /*Draw Players Spectating You*/
Checkbox DrawSpectatorsChk = { Vector2D(280 - OVERLAY_OFFSETX, 60 + ((CHECKBOX_WIDTH * 2) * 4) - OVERLAY_OFFSETY), MENU_ESP, Centered, nullptr, CheckedOff, drawspecstr, NotPressed, IsNotFaded, IsNotDisabled }; //Draw Players Spectating You
char *drawrecoilcrosshairstr = new char[22]{ 62, 8, 27, 13, 90, 40, 31, 25, 21, 19, 22, 90, 57, 8, 21, 9, 9, 18, 27, 19, 8, 0 }; /*Draw Recoil Crosshair*/
Checkbox DrawRecoilCrosshairChk = { Vector2D(280 - OVERLAY_OFFSETX, 60 + ((CHECKBOX_WIDTH * 2) * 5) - OVERLAY_OFFSETY), MENU_ESP, Centered, nullptr, CheckedOff, drawrecoilcrosshairstr, NotPressed, IsNotFaded, IsNotDisabled };
char *drawdamagegivenstr = new char[18]{ 62, 8, 27, 13, 90, 62, 27, 23, 27, 29, 31, 90, 61, 19, 12, 31, 20, 0 }; /*Draw Damage Given*/
Checkbox DrawDamageChk = { Vector2D(280 - OVERLAY_OFFSETX, 60 + ((CHECKBOX_WIDTH * 2) * 6) - OVERLAY_OFFSETY), MENU_ESP, Centered, nullptr, CheckedOff, drawdamagegivenstr, NotPressed, IsNotFaded, IsNotDisabled };
char *drawaimbotbonestr = new char[17]{ 62, 8, 27, 13, 90, 59, 19, 23, 24, 21, 14, 90, 56, 21, 20, 31, 0 }; /*Draw Aimbot Bone*/
Checkbox DrawAimbotBoneChk = { Vector2D(280 - OVERLAY_OFFSETX, 60 + ((CHECKBOX_WIDTH * 2) * 7) - OVERLAY_OFFSETY), MENU_ESP, Centered, nullptr, CheckedOff, drawaimbotbonestr, NotPressed, IsNotFaded, IsNotDisabled };
void OnDisablePostProcessingClicked()
{
	WriteByte((uintptr_t)s_bOverridePostProcessingDisable, DisablePostProcessingChk.Checked);
}
char *disablepostprocessingstr = new char[36]{ 62, 19, 9, 27, 24, 22, 31, 90, 42, 21, 9, 14, 90, 42, 8, 21, 25, 31, 9, 9, 19, 20, 29, 90, 82, 60, 42, 41, 90, 56, 21, 21, 9, 14, 83, 0 }; /*Disable Post Processing (FPS Boost)*/
Checkbox DisablePostProcessingChk = { Vector2D(280 - OVERLAY_OFFSETX, 60 + ((CHECKBOX_WIDTH * 2) * 8) - OVERLAY_OFFSETY), MENU_ESP, Centered, &OnDisablePostProcessingClicked, CheckedOff, disablepostprocessingstr, NotPressed, IsNotFaded, IsNotDisabled };
char *r_dynamicstr = new char[10]{ 8, 37, 30, 3, 20, 27, 23, 19, 25, 0 }; /*r_dynamic*/
void OnDisableDynamicLightingClicked()
{
	static ConVar* r_dynamic = nullptr;
	if (!r_dynamic)
	{
		DecStr(r_dynamicstr, 9);
		r_dynamic = Interfaces::Cvar->FindVar(r_dynamicstr);
		EncStr(r_dynamicstr, 9);
	}
	if (r_dynamic)
	{
		r_dynamic->SetValue(!DisableDynamicLightingChk.Checked);
	}
	else
	{
		DisableDynamicLightingChk.Checked = false;
	}
}
char *disabledynamiclightingstr = new char[37]{ 62, 19, 9, 27, 24, 22, 31, 90, 62, 3, 20, 27, 23, 19, 25, 90, 54, 19, 29, 18, 14, 19, 20, 29, 90, 82, 60, 42, 41, 90, 56, 21, 21, 9, 14, 83, 0 }; /*Disable Dynamic Lighting (FPS Boost)*/
Checkbox DisableDynamicLightingChk = { Vector2D(280 - OVERLAY_OFFSETX, 60 + ((CHECKBOX_WIDTH * 2) * 9) - OVERLAY_OFFSETY), MENU_ESP, Centered, &OnDisableDynamicLightingClicked, CheckedOff, disabledynamiclightingstr, NotPressed, IsNotFaded, IsNotDisabled };
char *noflashstr = new char[9]{ 52, 21, 90, 60, 22, 27, 9, 18, 0 }; /*No Flash*/
Checkbox NoFlashChk = { Vector2D(280 - OVERLAY_OFFSETX, 60 + ((CHECKBOX_WIDTH * 2) * 10) - OVERLAY_OFFSETY), MENU_ESP, Centered, nullptr, CheckedOff, noflashstr, NotPressed, IsNotFaded, IsNotDisabled };
char *nosmokestr = new char[9]{ 52, 21, 90, 41, 23, 21, 17, 31, 0 }; /*No Smoke*/
Checkbox NoSmokeChk = { Vector2D(280 - OVERLAY_OFFSETX, 60 + ((CHECKBOX_WIDTH * 2) * 11) - OVERLAY_OFFSETY), MENU_ESP, Centered, nullptr, CheckedOff, nosmokestr, NotPressed, IsNotFaded, IsNotDisabled };
void OnNoScopePressed()
{
	if (NoScopeChk.Checked)
	{
		if (DisablePostProcessingChk.OnPressed)
		{
			((void(*)(void))DisablePostProcessingChk.OnPressed)();
		}
	}
}
char *noscopestr = new char[9]{ 52, 21, 90, 41, 25, 21, 10, 31, 0 }; /*No Scope*/
Checkbox NoScopeChk = { Vector2D(355 - OVERLAY_OFFSETX, 60 + ((CHECKBOX_WIDTH * 2) * 10) - OVERLAY_OFFSETY), MENU_ESP, Centered, &OnNoScopePressed, CheckedOff, noscopestr, NotPressed, IsNotFaded, IsNotDisabled };
char *grenadetrajectorystr = new char[23]{ 61, 8, 31, 20, 27, 30, 31, 90, 46, 8, 27, 16, 31, 25, 14, 21, 8, 3, 90, 63, 41, 42, 0 }; /*Grenade Trajectory ESP*/
Checkbox GrenadeTrajectoryChk = { Vector2D(280 - OVERLAY_OFFSETX, 60 + ((CHECKBOX_WIDTH * 2) * 11) - OVERLAY_OFFSETY), MENU_ESP, Centered, nullptr, CheckedOff, grenadetrajectorystr, NotPressed, IsNotFaded, IsNotDisabled };
char *hitboxespstr = new char[11]{ 50, 19, 14, 24, 21, 2, 90, 63, 41, 42, 0 }; /*Hitbox ESP*/
Checkbox DrawHitboxesChk = { Vector2D(280 - OVERLAY_OFFSETX, 60 + ((CHECKBOX_WIDTH * 2) * 12) - OVERLAY_OFFSETY), MENU_ESP, Centered, nullptr, CheckedOff, hitboxespstr, NotPressed, IsNotFaded, IsNotDisabled };
char *aimespstr = new char[8]{ 59, 19, 23, 90, 63, 41, 42, 0 }; /*Aim ESP*/
Checkbox AimESPChk = { Vector2D(280 - OVERLAY_OFFSETX, 60 + ((CHECKBOX_WIDTH * 2) * 13) - OVERLAY_OFFSETY), MENU_ESP, Centered, nullptr, CheckedOff, aimespstr, NotPressed, IsNotFaded, IsNotDisabled };
char *arrowespstr = new char[10]{ 59, 8, 8, 21, 13, 90, 63, 41, 42, 0 }; /*Arrow ESP*/
Checkbox ArrowESPChk = { Vector2D(355 - OVERLAY_OFFSETX, 60 + ((CHECKBOX_WIDTH * 2) * 13) - OVERLAY_OFFSETY), MENU_ESP, Centered, nullptr, CheckedOff, arrowespstr, NotPressed, IsNotFaded, IsNotDisabled };
char *footstepespstr = new char[13]{ 60, 21, 21, 14, 9, 14, 31, 10, 90, 63, 41, 42, 0 }; /*Footstep ESP*/
Checkbox FootstepESPChk = { Vector2D(355 - OVERLAY_OFFSETX, 60 + ((CHECKBOX_WIDTH * 2) * 12) - OVERLAY_OFFSETY), MENU_ESP, Centered, nullptr, CheckedOff, footstepespstr, NotPressed, IsNotFaded, IsNotDisabled };
char *noflickerstr = new char[11]{ 52, 21, 90, 60, 22, 19, 25, 17, 31, 8, 0 }; /*No Flicker*/
Checkbox NoFlickerChk = { Vector2D(105 + OVERLAY_OFFSETX, -17 - OVERLAY_OFFSETY), MENU_ESP, Centered, nullptr, CheckedOff, noflickerstr, NotPressed, IsNotFaded, IsNotDisabled };


char *thirdpersontogglekeystr = new char[24]{ 46, 18, 19, 8, 30, 90, 42, 31, 8, 9, 21, 20, 90, 46, 21, 29, 29, 22, 31, 90, 49, 31, 3, 0 }; /*Third Person Toggle Key*/
Textbox ThirdPersonKeyTxt = { Vector2D(40 - OVERLAY_OFFSETX, 70 - OVERLAY_OFFSETY), Vector2D(95 - OVERLAY_OFFSETX, 65 - OVERLAY_OFFSETY), Vector2D(95 - OVERLAY_OFFSETX, 80 - OVERLAY_OFFSETY), MENU_ESP, Centered, nullptr, 50, TYPES::Integer, thirdpersontogglekeystr, 1.0, (double)SHRT_MAX, "-1", -1.0f, -1.0, -1, 0, IsNotFaded, IsNotDisabled };
char *thirdpersonzoffsetstr = new char[22]{ 46, 18, 19, 8, 30, 90, 42, 31, 8, 9, 21, 20, 90, 32, 90, 53, 28, 28, 9, 31, 14, 0 }; /*Third Person Z Offset*/
Checkbox ThirdPersonOffsetChk = { Vector2D(40 - OVERLAY_OFFSETX, 60 + ((CHECKBOX_WIDTH * 2) * 2) - OVERLAY_OFFSETY), MENU_ESP, Centered, nullptr, CheckedOff, thirdpersonzoffsetstr, NotPressed, IsNotFaded, IsNotDisabled };
char *convarspoofstr = new char[26]{ 46, 18, 19, 8, 30, 90, 42, 31, 8, 9, 21, 20, 90, 57, 21, 20, 44, 27, 8, 90, 41, 10, 21, 21, 28, 0 }; /*Third Person ConVar Spoof*/
Checkbox spoofConvars = { Vector2D(150 - OVERLAY_OFFSETX, 60 + ((CHECKBOX_WIDTH * 2) * 2) - OVERLAY_OFFSETY), MENU_ESP, Centered, nullptr, CheckedOff, convarspoofstr, NotPressed, IsNotFaded, IsNotDisabled };

void OnShowFakeAngles()
{
	if (ShowFakedAnglesChk.Checked)
	{
		ShowRealAnglesChk.Checked = false;
	}
}
char *showfakedanglesstr = new char[36]{ 41, 18, 21, 13, 90, 60, 27, 17, 31, 30, 90, 59, 20, 29, 22, 31, 9, 90, 53, 14, 18, 31, 8, 90, 42, 22, 27, 3, 31, 8, 9, 90, 41, 31, 31, 0 }; /*Show Faked Angles Other Players See*/
Checkbox ShowFakedAnglesChk = { Vector2D(40 - OVERLAY_OFFSETX, 60 + ((CHECKBOX_WIDTH * 2) * 3) - OVERLAY_OFFSETY), MENU_ESP, Centered, &OnShowFakeAngles, CheckedOff, showfakedanglesstr, NotPressed, IsNotFaded, IsNotDisabled };
void OnShowRealAngles()
{
	if (ShowRealAnglesChk.Checked)
	{
		ShowFakedAnglesChk.Checked = false;
	}
}
char *showrealanglesstr = new char[19]{ 41, 18, 21, 13, 90, 59, 25, 14, 15, 27, 22, 90, 59, 20, 29, 22, 31, 9, 0 }; /*Show Actual Angles*/
Checkbox ShowRealAnglesChk = { Vector2D(40 - OVERLAY_OFFSETX, 60 + ((CHECKBOX_WIDTH * 2) * 4) - OVERLAY_OFFSETY), MENU_ESP, Centered, &OnShowRealAngles, CheckedOff, showrealanglesstr, NotPressed, IsNotFaded, IsNotDisabled };

char *airstuckkeystr = new char[13]{ 59, 19, 8, 9, 14, 15, 25, 17, 90, 49, 31, 3, 0 }; /*Airstuck Key*/
Textbox AirStuckKeyTxt = { Vector2D(40 - OVERLAY_OFFSETX, 160 - OVERLAY_OFFSETY), Vector2D(95 - OVERLAY_OFFSETX, 155 - OVERLAY_OFFSETY), Vector2D(95 - OVERLAY_OFFSETX, 170 - OVERLAY_OFFSETY), MENU_ESP, Centered, nullptr, 50, TYPES::Integer, airstuckkeystr, 1.0, (double)SHRT_MAX, "-1", -1.0f, -1.0, 0, 0, IsNotFaded, IsNotDisabled };
char *fovchangestr = new char[12]{ 60, 53, 44, 90, 57, 18, 27, 20, 29, 31, 8, 0 }; /*FOV Changer*/
Textbox FOVChangerTxt = { Vector2D(40 - OVERLAY_OFFSETX, 190 - OVERLAY_OFFSETY), Vector2D(95 - OVERLAY_OFFSETX, 185 - OVERLAY_OFFSETY), Vector2D(95 - OVERLAY_OFFSETX, 200 - OVERLAY_OFFSETY), MENU_ESP, Centered, nullptr, 50, TYPES::Float, fovchangestr, 1.0, 179.0, "90.0", 90.0f, 90.0, 0, 0, IsNotFaded, IsNotDisabled };
char *viewmodelfovchangerstr = new char[23]{ 44, 19, 31, 13, 90, 55, 21, 30, 31, 22, 90, 60, 53, 44, 90, 57, 18, 27, 20, 29, 31, 8, 0 }; /*View Model FOV Changer*/
Textbox ViewModelFOVChangerTxt = { Vector2D(40 - OVERLAY_OFFSETX, 220 - OVERLAY_OFFSETY), Vector2D(95 - OVERLAY_OFFSETX, 215 - OVERLAY_OFFSETY), Vector2D(95 - OVERLAY_OFFSETX, 230 - OVERLAY_OFFSETY), MENU_ESP, Centered, nullptr, 50, TYPES::Float, viewmodelfovchangerstr, 1.0, 179.0, "68.0", 68.0f, 68.0, 0, 0, IsNotFaded, IsNotDisabled };

void OnSetClanTag()
{
	if (strlen(SetClanTagTxt.Value) == 0)
	{
		SetClanTagTxt.Value[0] = ' ';
		SetClanTagTxt.Value[1] = 0x0;
	}
	else
	{
		if (strstr(SetClanTagTxt.Value, "\\n"))
		{
			int sizeofarray = sizeof(SetClanTagTxt.Value);
			for (int i = 0; i < sizeofarray; i++)
			{
				if (SetClanTagTxt.Value[i] == 0x0)
					break;
				if (SetClanTagTxt.Value[i] == '\\')
				{
					if (i + 1 < sizeofarray)
					{
						if (SetClanTagTxt.Value[i + 1] == 0x0)
							break;
						if (SetClanTagTxt.Value[i + 1] == 'n')
						{
							SetClanTagTxt.Value[i] = '\n';
							SetClanTagTxt.Value[i + 1] = ' '; 	//todo: fix me: concat the rest of the string!
						}
					}
				}
			}
		}
	}
	SetClanTag(SetClanTagTxt.Value, SetClanTagTxt.Value);
}
char *setclantagstr = new char[13]{ 41, 31, 14, 90, 57, 22, 27, 20, 90, 46, 27, 29, 0 }; /*Set Clan Tag*/
Textbox SetClanTagTxt = { Vector2D(40 - OVERLAY_OFFSETX, 250 - OVERLAY_OFFSETY), Vector2D(178 - OVERLAY_OFFSETX, 245 - OVERLAY_OFFSETY), Vector2D(95 - OVERLAY_OFFSETX, 260 - OVERLAY_OFFSETY), MENU_ESP, Centered, &OnSetClanTag, 150, TYPES::Char, setclantagstr, -10000.0, 10000.0, "", 0.0f, 0.0, 0, 0, IsNotFaded, IsNotDisabled };

char *animatedtagstr = new char[13]{ 59, 20, 19, 23, 27, 14, 31, 30, 90, 46, 27, 29, 0 }; /*Animated Tag*/
Checkbox AnimatedClanTagChk = { Vector2D(40 - OVERLAY_OFFSETX, 280 - OVERLAY_OFFSETY), MENU_ESP, Centered, nullptr, CheckedOff, animatedtagstr, NotPressed, IsNotFaded, IsNotDisabled };

char *usemutinytagstr = new char[15]{ 47, 9, 31, 90, 55, 15, 14, 19, 20, 3, 90, 46, 27, 29, 0 }; /*Use Mutiny Tag*/
Checkbox UseMutinyTagChk = { Vector2D(120 - OVERLAY_OFFSETX, 280 - OVERLAY_OFFSETY), MENU_ESP, Centered, nullptr, CheckedOff, usemutinytagstr, NotPressed, IsNotFaded, IsNotDisabled };


void OnSetCStrafeKeyPressed()
{
	flTimeRequestedToChangeVirtualKey = Interfaces::Globals->realtime;
	virtualkeytxtboxtoedit = &CStraffKeyTxt;
}

void OnSetThirdPersonKeyPressed()
{
	flTimeRequestedToChangeVirtualKey = Interfaces::Globals->realtime;
	virtualkeytxtboxtoedit = &ThirdPersonKeyTxt;
}
char *setthirdpersonkeystr = new char[4]{ 41, 31, 14, 0 }; /*Set*/
Button SetThirdPersonKeyBtn = { Vector2D(225 - OVERLAY_OFFSETX, 70 - 5 - OVERLAY_OFFSETY), MENU_ESP, Centered, &OnSetThirdPersonKeyPressed, 35, 20, setthirdpersonkeystr, NotHighlighted, NotPressed, IsNotFaded, IsNotDisabled };

void OnSetAirStuckKeyPressed()
{
	flTimeRequestedToChangeVirtualKey = Interfaces::Globals->realtime;
	virtualkeytxtboxtoedit = &AirStuckKeyTxt;
}
char *setairstuckkeystr = new char[4]{ 41, 31, 14, 0 }; /*Set*/
Button SetAirStuckKeyBtn = { Vector2D(225 - OVERLAY_OFFSETX, 160 - 5 - OVERLAY_OFFSETY), MENU_ESP, Centered, &OnSetAirStuckKeyPressed, 35, 20, setairstuckkeystr, NotHighlighted, NotPressed, IsNotFaded, IsNotDisabled };


//END OF VISUALS MENU




//MISCELLANEOUS MENU

char *offstr = new char[13]{ 60, 27, 17, 31, 90, 54, 27, 29, 90, 53, 28, 28, 0 }; /*Fake Lag Off*/
char *LagCompstr = new char[8]{ 54, 27, 29, 57, 21, 23, 10, 0 }; /*LagComp*/
char *Stepstr = new char[5]{ 41, 14, 31, 10, 0 }; /*Step*/
char *randomstr = new char[7]{ 40, 27, 20, 30, 21, 23, 0 }; /*Random*/
char *staticfakelagstr = new char[15]{ 41, 14, 27, 14, 19, 25, 90, 60, 27, 17, 31, 54, 27, 29, 0 }; /*Static FakeLag*/


void OnFakeLagChanged()
{
	if (FakeLagDropDown.index != 0 && FakeLagChokedTxt.iValue == 0)
	{
		FakeLagChokedTxt.iValue = 1;
		ModifyTextboxValue(&FakeLagChokedTxt, 3);
	}
	else if (FakeLagDropDown.index == 0 && FakeLagChokedTxt.iValue != 0)
	{
		FakeLagChokedTxt.iValue = 0;
		ModifyTextboxValue(&FakeLagChokedTxt, 3);
	}
}
char *fakeLag[5] = { offstr,staticfakelagstr,LagCompstr,Stepstr,randomstr };
Dropdown FakeLagDropDown = { Vector2D(280 - OVERLAY_OFFSETX, 60 - OVERLAY_OFFSETY), MENU_MISC, Centered, &OnFakeLagChanged, 70, INDEX_ZERO, INDEX_ZERO, 5, &fakeLag, FADE_ZERO, IsNotFaded, IsNotDisabled };

char *flagonlyinairstr = new char[17]{ 60, 54, 27, 29, 90, 21, 20, 22, 3, 90, 19, 20, 90, 59, 19, 8, 0 }; /*FLag only in Air*/
Checkbox FakeLagOnlyInAirChk = { Vector2D(385 - OVERLAY_OFFSETX, 60 - OVERLAY_OFFSETY), MENU_MISC, Centered, nullptr, CheckedOff, flagonlyinairstr, NotPressed, IsNotFaded, IsNotDisabled };

char *autogriefstr = new char[11]{ 59, 15, 14, 21, 90, 61, 8, 19, 31, 28, 0 }; /*Auto Grief*/
Checkbox AutoGriefChk = { Vector2D(475 - OVERLAY_OFFSETX, 60 - OVERLAY_OFFSETY), MENU_MISC, Centered, nullptr, CheckedOff, autogriefstr, NotPressed, IsNotFaded, IsNotDisabled };

char *cstrafekeystr = new char[12]{ 57, 41, 14, 8, 27, 28, 31, 90, 49, 31, 3, 0 }; /*CStrafe Key*/
Button CStraffKeyBtn = { Vector2D(545 - OVERLAY_OFFSETX, 80 - 5 - OVERLAY_OFFSETY), MENU_MISC, Centered, &OnSetCStrafeKeyPressed, 35, 20, setthirdpersonkeystr, NotHighlighted, NotPressed, IsNotFaded, IsNotDisabled };
Textbox CStraffKeyTxt = { Vector2D(440 - OVERLAY_OFFSETX, 80 - OVERLAY_OFFSETY), Vector2D(475 - OVERLAY_OFFSETX, 80 - OVERLAY_OFFSETY), Vector2D(475 - OVERLAY_OFFSETX, 90 - OVERLAY_OFFSETY), MENU_MISC, Centered, nullptr, 50, TYPES::Integer, cstrafekeystr, -1.0, (double)SHRT_MAX, "-1",  -1.0f, -1.0, -1, 0, IsNotFaded, IsNotDisabled };


char *bhopstr = new char[9]{ 56, 15, 20, 20, 3, 18, 21, 10, 0 }; /*Bunnyhop*/
Checkbox BhopChk = { Vector2D(280 - OVERLAY_OFFSETX, 60 + ((CHECKBOX_WIDTH * 2) * 1) - OVERLAY_OFFSETY), MENU_MISC, Centered, nullptr, CheckedOff, bhopstr, NotPressed, IsNotFaded, IsNotDisabled }; //Bunnyhop	
void OnRageBhopPressed()
{
}
char *ragebhopstr = new char[14]{ 40, 27, 29, 31, 90, 56, 15, 20, 20, 3, 18, 21, 10, 0 }; /*Rage Bunnyhop*/
Checkbox RageBhopChk = { Vector2D(350 - OVERLAY_OFFSETX,  60 + ((CHECKBOX_WIDTH * 2) * 1) - OVERLAY_OFFSETY), MENU_MISC, Centered, &OnRageBhopPressed, CheckedOff, ragebhopstr, NotPressed, IsNotFaded, IsNotDisabled }; //Rage Bunnyhop
char *enginepredstr = new char[18]{ 63, 20, 29, 19, 20, 31, 90, 42, 8, 31, 30, 19, 25, 14, 19, 21, 20, 0 }; /*Engine Prediction*/
Checkbox EnginePredictChk = { Vector2D(280 - OVERLAY_OFFSETX, 60 + ((CHECKBOX_WIDTH * 2) * 2) - OVERLAY_OFFSETY), MENU_MISC, Centered, nullptr, CheckedOn, enginepredstr, NotPressed, IsNotFaded, IsNotDisabled }; //Engine Prediction
char *removeinterpstr = new char[37]{ 40, 31, 23, 21, 12, 31, 90, 51, 20, 14, 31, 8, 10, 21, 22, 27, 14, 19, 21, 20, 90, 92, 90, 63, 2, 14, 8, 27, 10, 21, 22, 27, 14, 19, 21, 20, 0 }; /*Remove Interpolation & Extrapolation*/
Checkbox RemoveInterpChk = { Vector2D(385 - OVERLAY_OFFSETX, 60 + ((CHECKBOX_WIDTH * 2) * 2) - OVERLAY_OFFSETY), MENU_MISC, Centered, nullptr, CheckedOn, removeinterpstr, NotPressed, IsNotFaded, IsNotDisabled }; //Remove Interpolation
char *autowallstr = new char[10]{ 59, 15, 14, 21, 90, 45, 27, 22, 22, 0 }; /*Auto Wall*/
Checkbox AutoWallChk = { Vector2D(280 - OVERLAY_OFFSETX, 60 + ((CHECKBOX_WIDTH * 2) * 3) - OVERLAY_OFFSETY), MENU_MISC, Centered, nullptr, CheckedOff, autowallstr, NotPressed, IsNotFaded, IsNotDisabled };
char *autowallteammatesstr = new char[35]{ 42, 31, 20, 31, 14, 8, 27, 14, 31, 90, 46, 31, 27, 23, 23, 27, 14, 31, 9, 90, 14, 21, 90, 18, 19, 14, 90, 31, 20, 31, 23, 19, 31, 9, 0 }; /*Penetrate Teammates to hit enemies*/
Checkbox AutoWallPenetrateTeammatesChk = { Vector2D(350 - OVERLAY_OFFSETX, 60 + ((CHECKBOX_WIDTH * 2) * 3) - OVERLAY_OFFSETY), MENU_MISC, Centered, nullptr, CheckedOff, autowallteammatesstr, NotPressed, IsNotFaded, IsNotDisabled };
char *norecoilstr = new char[10]{ 52, 21, 90, 40, 31, 25, 21, 19, 22, 0 }; /*No Recoil*/
Checkbox NoRecoilChk = { Vector2D(280 - OVERLAY_OFFSETX, 60 + ((CHECKBOX_WIDTH * 2) * 4) - OVERLAY_OFFSETY), MENU_MISC, Centered, nullptr, CheckedOff, norecoilstr, NotPressed, IsNotFaded, IsNotDisabled };
char *novisualrecoilstr = new char[17]{ 52, 21, 90, 44, 19, 9, 15, 27, 22, 90, 40, 31, 25, 21, 19, 22, 0 }; /*No Visual Recoil*/
Checkbox NoVisualRecoilChk = { Vector2D(350 - OVERLAY_OFFSETX, 60 + ((CHECKBOX_WIDTH * 2) * 4) - OVERLAY_OFFSETY), MENU_MISC, Centered, nullptr, CheckedOff, novisualrecoilstr, NotPressed, IsNotFaded, IsNotDisabled };
char *norecoilragestr = new char[15]{ 40, 27, 29, 31, 90, 52, 21, 90, 40, 31, 25, 21, 19, 22, 0 }; /*Rage No Recoil*/
Checkbox NoRecoilRageChk = { Vector2D(460 - OVERLAY_OFFSETX, 60 + ((CHECKBOX_WIDTH * 2) * 4) - OVERLAY_OFFSETY), MENU_MISC, Centered, nullptr, CheckedOff, norecoilragestr, NotPressed, IsNotFaded, IsNotDisabled };
char *norecoilothersstr = new char[43]{ 59, 22, 22, 21, 13, 90, 52, 21, 90, 40, 31, 25, 21, 19, 22, 90, 21, 20, 90, 42, 19, 9, 14, 21, 22, 9, 85, 41, 18, 21, 14, 29, 15, 20, 9, 85, 41, 20, 19, 10, 31, 8, 0 }; /*Allow No Recoil on Pistols/Shotguns/Sniper*/
Checkbox NoRecoilPistolsShotgunsSnipersChk = { Vector2D(280 - OVERLAY_OFFSETX, 60 + ((CHECKBOX_WIDTH * 2) * 5) - OVERLAY_OFFSETY), MENU_MISC, Centered, nullptr, CheckedOff, norecoilothersstr, NotPressed, IsNotFaded, IsNotDisabled };
char *targetteammatesstr = new char[17]{ 46, 27, 8, 29, 31, 14, 90, 46, 31, 27, 23, 23, 27, 14, 31, 9, 0 }; /*Target Teammates*/
Checkbox TargetTeammatesChk = { Vector2D(280 - OVERLAY_OFFSETX, 60 + ((CHECKBOX_WIDTH * 2) * 6) - OVERLAY_OFFSETY), MENU_MISC, Centered, nullptr, CheckedOff, targetteammatesstr, NotPressed, IsNotFaded, IsNotDisabled };
char *resolveenabledstr = new char[31]{ 59, 20, 14, 19, 87, 59, 20, 14, 19, 90, 59, 19, 23, 90, 40, 31, 9, 21, 22, 12, 31, 8, 90, 63, 20, 27, 24, 22, 31, 30, 0 }; /*Anti-Anti Aim Resolver Enabled*/
Checkbox ResolverChk = { Vector2D(280 - OVERLAY_OFFSETX, 60 + ((CHECKBOX_WIDTH * 2) * 7) - OVERLAY_OFFSETY), MENU_MISC, Centered, nullptr, CheckedOff, resolveenabledstr, NotPressed, IsNotFaded, IsNotDisabled };
char *predictfakeanglestr = new char[34]{ 42, 8, 31, 30, 19, 25, 14, 90, 28, 27, 17, 31, 90, 27, 20, 29, 22, 31, 9, 90, 27, 15, 14, 21, 23, 27, 14, 19, 25, 27, 22, 22, 3, 0 }; /*Predict fake angles automatically*/
Checkbox PredictFakeAnglesChk = { Vector2D(280 - OVERLAY_OFFSETX, 60 + ((CHECKBOX_WIDTH * 2) * 8) - OVERLAY_OFFSETY), MENU_MISC, Centered, nullptr, CheckedOn, predictfakeanglestr , NotPressed, IsNotFaded, IsNotDisabled };
char *faceitstr = new char[52]{ 60, 27, 25, 31, 51, 14, 90, 57, 21, 8, 8, 31, 25, 14, 19, 21, 20, 9, 90, 82, 62, 53, 63, 41, 90, 52, 53, 46, 90, 55, 59, 49, 63, 90, 51, 46, 90, 60, 59, 57, 63, 51, 46, 87, 42, 40, 53, 53, 60, 91, 83, 0 }; /*FaceIt Corrections (DOES NOT MAKE IT FACEIT-PROOF!)*/
Checkbox FaceItChk = { Vector2D(280 - OVERLAY_OFFSETX, 60 + ((CHECKBOX_WIDTH * 2) * 9) - OVERLAY_OFFSETY), MENU_MISC, Centered, nullptr, CheckedOff, faceitstr , NotPressed, IsNotFaded, IsNotDisabled };
char *firingcorrectionsstr = new char[50]{ 46, 8, 3, 90, 57, 21, 8, 8, 31, 25, 14, 19, 20, 29, 90, 10, 41, 19, 22, 31, 20, 14, 85, 60, 27, 17, 31, 54, 27, 29, 90, 45, 18, 31, 20, 90, 42, 22, 27, 3, 31, 8, 9, 90, 41, 18, 21, 21, 14, 0 }; /*Try Correcting pSilent/FakeLag When Players Shoot*/
Checkbox FiringCorrectionsChk = { Vector2D(280 - OVERLAY_OFFSETX, 60 + ((CHECKBOX_WIDTH * 2) * 10) - OVERLAY_OFFSETY), MENU_MISC, Centered, nullptr, CheckedOn, firingcorrectionsstr , NotPressed, IsNotFaded, IsNotDisabled };
void OnBloodResolverPressed()
{
	if (!BloodResolverChk.Checked)
	{
		//Stop blood resolving any players

		BacktrackToBloodChk.Checked = false;

		int MaxEntities = 64;// Interfaces::ClientEntList->GetHighestEntityIndex();
		int NumPlayers = GetClientCount();
		int CountedPlayers = 0;

		if (NumPlayers)
		{
			for (int i = 0; i <= MaxEntities; i++)
			{
				CBaseEntity* Entity = Interfaces::ClientEntList->GetClientEntity(i);
				CustomPlayer* pCPlayer = &AllPlayers[i];

				if (pCPlayer->Connected)
				{
					switch (pCPlayer->Personalize.EstimateYawMode)
					{
					case ResolveYawModes::BloodBodyRealDelta:
					case ResolveYawModes::BloodEyeRealDelta:
					case ResolveYawModes::BloodReal:
						pCPlayer->Personalize.EstimateYawMode = ResolveYawModes::StaticFakeDynamic;
						break;
					}
					switch (pCPlayer->Personalize.EstimateYawModeInAir)
					{
					case ResolveYawModes::BloodBodyRealDelta:
					case ResolveYawModes::BloodEyeRealDelta:
					case ResolveYawModes::BloodReal:
						pCPlayer->Personalize.EstimateYawModeInAir = ResolveYawModes::StaticFakeDynamic;
						break;
					}
					switch (pCPlayer->Personalize.ResolveYawMode)
					{
					case ResolveYawModes::BloodBodyRealDelta:
					case ResolveYawModes::BloodEyeRealDelta:
					case ResolveYawModes::BloodReal:
						pCPlayer->Personalize.ResolveYawMode = ResolveYawModes::AutomaticYaw;
						break;
					}

					if (++CountedPlayers == NumPlayers)
						break;
				}
			}
		}
	}
}
char *bloodresolverstr = new char[54]{ 40, 31, 9, 21, 22, 12, 31, 90, 18, 31, 27, 30, 90, 28, 8, 21, 23, 90, 24, 21, 30, 3, 90, 18, 19, 14, 9, 90, 27, 20, 30, 90, 15, 9, 31, 90, 19, 14, 90, 28, 21, 8, 90, 10, 8, 31, 30, 19, 25, 14, 19, 21, 20, 0 }; /*Resolve head from body hits and use it for prediction*/
Checkbox BloodResolverChk = { Vector2D(280 - OVERLAY_OFFSETX, 60 + ((CHECKBOX_WIDTH * 2) * 11) - OVERLAY_OFFSETY), MENU_MISC, Centered, &OnBloodResolverPressed, CheckedOn, bloodresolverstr , NotPressed, IsNotFaded, IsNotDisabled };
char *backtracktobodyhitticksstr = new char[42]{ 56, 27, 25, 17, 14, 8, 27, 25, 17, 90, 14, 21, 90, 18, 31, 27, 30, 90, 8, 31, 9, 21, 22, 12, 31, 30, 90, 28, 8, 21, 23, 90, 24, 21, 30, 3, 90, 18, 19, 14, 9, 0 }; /*Backtrack to head resolved from body hits*/
void OnBacktrackToBloodPressed()
{
	if (BacktrackToBloodChk.Checked)
	{
		BloodResolverChk.Checked = true;
	}
}
Checkbox BacktrackToBloodChk = { Vector2D(280 - OVERLAY_OFFSETX, 60 + ((CHECKBOX_WIDTH * 2) * 12) - OVERLAY_OFFSETY), MENU_MISC, Centered, &OnBacktrackToBloodPressed, CheckedOff, backtracktobodyhitticksstr , NotPressed, IsNotFaded, IsNotDisabled };
char *backtrackexploitstr = new char[18]{ 56, 27, 25, 17, 14, 8, 27, 25, 17, 90, 63, 2, 10, 22, 21, 19, 14, 0 }; /*Backtrack Exploit*/
Checkbox BacktrackExploitChk = { Vector2D(280 - OVERLAY_OFFSETX, 60 + ((CHECKBOX_WIDTH * 2) * 13) - OVERLAY_OFFSETY), MENU_MISC, Centered, nullptr, CheckedOff, backtrackexploitstr , NotPressed, IsNotFaded, IsNotDisabled };
char *autoacceptstr = new char[12]{ 59, 15, 14, 21, 90, 59, 25, 25, 31, 10, 14, 0 }; /*Auto Accept*/
Checkbox AutoAcceptChk = { Vector2D(375 - OVERLAY_OFFSETX, 60 + ((CHECKBOX_WIDTH * 2) * 13) - OVERLAY_OFFSETY), MENU_MISC, Centered, nullptr, CheckedOff, autoacceptstr , NotPressed, IsNotFaded, IsNotDisabled };

void OnFakeLagChokedChanged()
{
	if (FakeLagChokedTxt.iValue == 0)
	{
		FakeLagDropDown.index = 0;
	}
	else if (FakeLagDropDown.index == 0)
	{
		FakeLagDropDown.index = 1;
	}
}
char *fakelagchokedstr = new char[24]{ 60, 27, 17, 31, 90, 54, 27, 29, 90, 57, 18, 21, 17, 31, 30, 90, 42, 27, 25, 17, 31, 14, 9, 0 }; /*Fake Lag Choked Packets*/
Textbox FakeLagChokedTxt = { Vector2D(40 - OVERLAY_OFFSETX, 70 - OVERLAY_OFFSETY), Vector2D(95 - OVERLAY_OFFSETX, 65 - OVERLAY_OFFSETY), Vector2D(95 - OVERLAY_OFFSETX, 80 - OVERLAY_OFFSETY), MENU_MISC, Centered, &OnFakeLagChokedChanged, 50, TYPES::Integer, fakelagchokedstr, 0.0, 1000.0, "3", 3.0f, 3.0, 3, 3, IsNotFaded, IsNotDisabled };
char *spindegpertickstr = new char[31]{ 41, 10, 19, 20, 85, 59, 59, 90, 59, 20, 29, 22, 31, 90, 62, 31, 29, 8, 31, 31, 9, 90, 42, 31, 8, 90, 46, 19, 25, 17, 0 }; /*Spin/AA Angle Degrees Per Tick*/
Textbox SpinDegPerTickTxt = { Vector2D(40 - OVERLAY_OFFSETX, 100 - OVERLAY_OFFSETY), Vector2D(95 - OVERLAY_OFFSETX, 95 - OVERLAY_OFFSETY), Vector2D(95 - OVERLAY_OFFSETX, 110 - OVERLAY_OFFSETY), MENU_MISC, Centered, nullptr, 50, TYPES::Float, spindegpertickstr, 0.0, 360.0, "360.0", 360.0f, 360.0, 0, 0, IsNotFaded, IsNotDisabled };
char *norecoilxstr = new char[20]{ 52, 21, 90, 40, 31, 25, 21, 19, 22, 90, 34, 90, 42, 31, 8, 25, 31, 20, 14, 0 }; /*No Recoil X Percent*/
Textbox NoRecoilXPercentTxt = { Vector2D(40 - OVERLAY_OFFSETX, 130 - OVERLAY_OFFSETY), Vector2D(95 - OVERLAY_OFFSETX, 125 - OVERLAY_OFFSETY), Vector2D(95 - OVERLAY_OFFSETX, 140 - OVERLAY_OFFSETY), MENU_MISC, Centered, nullptr, 50, TYPES::Float, norecoilxstr, 0.0, 100.0, "100.0", 100.0f, 0.0, 0, 0, IsNotFaded, IsNotDisabled };
char *norecoilystr = new char[20]{ 52, 21, 90, 40, 31, 25, 21, 19, 22, 90, 35, 90, 42, 31, 8, 25, 31, 20, 14, 0 }; /*No Recoil Y Percent*/
Textbox NoRecoilYPercentTxt = { Vector2D(40 - OVERLAY_OFFSETX, 160 - OVERLAY_OFFSETY), Vector2D(95 - OVERLAY_OFFSETX, 155 - OVERLAY_OFFSETY), Vector2D(95 - OVERLAY_OFFSETX, 170 - OVERLAY_OFFSETY), MENU_MISC, Centered, nullptr, 50, TYPES::Float, norecoilystr, 0.0, 100.0, "100.0", 100.0f, 0.0, 0, 0, IsNotFaded, IsNotDisabled };

char *WDresolverstr = new char[12]{ 45, 62, 90, 40, 31, 9, 21, 22, 12, 31, 8, 0 }; /*WD Resolver*/
Textbox WhaleDongTxt = { Vector2D(40 - OVERLAY_OFFSETX, 190 - OVERLAY_OFFSETY), Vector2D(95 - OVERLAY_OFFSETX, 185 - OVERLAY_OFFSETY), Vector2D(95 - OVERLAY_OFFSETX, 200 - OVERLAY_OFFSETY), MENU_MISC, Centered, nullptr, 50, TYPES::Integer, WDresolverstr, 0.0, 64.0, "0", 0.0f, 0.0, 0, 0, IsFaded, IsNotDisabled };

char *antiaimdownbackstr = new char[18]{ 59, 20, 14, 19, 59, 19, 23, 90, 56, 27, 25, 17, 13, 27, 8, 30, 9, 0 }; /*AntiAim Backwards*/
char *antiaimfakestr = new char[17]{ 59, 20, 14, 19, 59, 19, 23, 90, 41, 19, 30, 31, 13, 27, 3, 9, 0 }; /*AntiAim Sideways*/
char *antiaimlegitstr = new char[14]{ 59, 20, 14, 19, 59, 19, 23, 90, 54, 31, 29, 19, 14, 0 }; /*AntiAim Legit*/
char *antiaimskatestr = new char[14]{ 59, 20, 14, 19, 59, 19, 23, 90, 41, 17, 27, 14, 31, 0 }; /*AntiAim Skate*/
char *antiaimballerinastr = new char[18]{ 59, 20, 14, 19, 59, 19, 23, 90, 56, 27, 22, 22, 31, 8, 19, 20, 27, 0 }; /*AntiAim Ballerina*/
char *slowspinfakestr = new char[18]{ 59, 20, 14, 19, 59, 19, 23, 90, 41, 22, 21, 13, 90, 41, 10, 19, 20, 0 }; /*AntiAim Slow Spin*/
char *spinstr = new char[13]{ 59, 20, 14, 19, 59, 19, 23, 90, 41, 10, 19, 20, 0 }; /*AntiAim Spin*/
char *antiaimexperimentalstr = new char[21]{ 59, 20, 14, 19, 59, 19, 23, 90, 63, 2, 10, 31, 8, 19, 23, 31, 20, 14, 27, 22, 0 }; /*AntiAim Experimental*/
char *antiaimexperimental2str = new char[23]{ 59, 20, 14, 19, 59, 19, 23, 90, 63, 2, 10, 31, 8, 19, 23, 31, 20, 14, 27, 22, 90, 72, 0 }; /*AntiAim Experimental 2*/
char *experimentalforwardsstr = new char[22]{ 63, 2, 10, 31, 8, 19, 23, 31, 20, 14, 27, 22, 90, 60, 21, 8, 13, 27, 8, 30, 9, 0 }; /*Experimental Forwards*/
char *experimentalforwardsinvertedstr = new char[31]{ 63, 2, 10, 31, 8, 19, 23, 31, 20, 14, 27, 22, 90, 60, 21, 8, 13, 27, 8, 30, 9, 90, 51, 20, 12, 31, 8, 14, 31, 30, 0 }; /*Experimental Forwards Inverted*/
char *testingstr = new char[21]{ 59, 20, 14, 19, 59, 19, 23, 90, 54, 56, 35, 90, 51, 20, 12, 31, 8, 14, 31, 30, 0 }; /*AntiAim LBY Inverted*/
char *testing2str = new char[19]{ 59, 20, 14, 19, 59, 19, 23, 90, 40, 27, 20, 30, 21, 23, 90, 35, 27, 13, 0 }; /*AntiAim Random Yaw*/
char *norealyawstr = new char[25]{ 62, 19, 9, 27, 24, 22, 31, 90, 40, 31, 27, 22, 90, 35, 27, 13, 90, 59, 20, 14, 19, 59, 19, 23, 0 }; /*Disable Real Yaw AntiAim*/
char *nofakeyawstr = new char[25]{ 62, 19, 9, 27, 24, 22, 31, 90, 60, 27, 17, 31, 90, 35, 27, 13, 90, 59, 20, 14, 19, 59, 19, 23, 0 }; /*Disable Fake Yaw AntiAim*/
char *defaultstr = new char[29]{ 62, 31, 28, 27, 15, 22, 14, 90, 85, 90, 59, 15, 14, 21, 23, 27, 14, 19, 25, 90, 60, 27, 17, 31, 90, 35, 27, 13, 0 }; /*Default / Automatic Fake Yaw*/
char *Classical = new char[10]{ 57, 22, 27, 9, 9, 19, 25, 27, 22, 0 }; /*Classical*/
char *Classical180 = new char[13]{ 57, 22, 27, 9, 9, 19, 25, 27, 22, 75, 66, 74, 0 }; /*Classical180*/
char *Classical90 = new char[12]{ 57, 22, 27, 9, 9, 19, 25, 27, 22, 67, 74, 0 }; /*Classical90*/
char *sidewaysjitterstr = new char[16]{ 41, 19, 30, 31, 13, 27, 3, 9, 90, 48, 19, 14, 14, 31, 8, 0 }; /*Sideways Jitter*/
char *inverseswitchstr = new char[15]{ 51, 20, 12, 31, 8, 9, 31, 90, 41, 13, 19, 14, 25, 18, 0 }; /*Inverse Switch*/

void OnAntiAimRealChanged()
{
	AntiAims antiaim = (AntiAims)AntiAimRealDrop.index;

	switch (antiaim)
	{
	case ANTIAIM_NONE:
		return;
	case ANTIAIM_LEGIT:
		AntiAimFakeDrop.index = ANTIAIM_DEFAULT;
		AntiAimPitchDrop.index = PITCH_NONE;
		ManualAntiAimChk.Checked = false;
		return;
	}

	pSilentChk.Checked = false;
	AimbotChk.Checked = true;
	AimbotHoldKeyChk.Checked = false;
	TriggerbotWhileAimbottingChk.Checked = true;
	SilentAimChk.Checked = true;
}

char *antiaimstrings[19] = { norealyawstr, antiaimdownbackstr, antiaimfakestr, antiaimlegitstr, antiaimskatestr, antiaimballerinastr,
slowspinfakestr, spinstr, antiaimexperimentalstr, antiaimexperimental2str, experimentalforwardsstr, experimentalforwardsinvertedstr, testingstr,
testing2str, Classical,Classical180, Classical90, inverseswitchstr , sidewaysjitterstr };
Dropdown AntiAimRealDrop = { Vector2D(40 - OVERLAY_OFFSETX, 245 - OVERLAY_OFFSETY), MENU_MISC, Centered, &OnAntiAimRealChanged, 150, INDEX_ZERO, INDEX_ZERO, 19, &antiaimstrings, FADE_ZERO, IsNotFaded, IsNotDisabled };

void OnAntiAimFakeChanged()
{
	if (AntiAimRealDrop.index == ANTIAIM_LEGIT)
	{
		AntiAimFakeDrop.index = ANTIAIM_DEFAULT;
		AntiAimPitchDrop.index = PITCH_NONE;
		return;
	}

	AntiAims antiaim = (AntiAims)AntiAimFakeDrop.index;

	if (antiaim == ANTIAIM_NONE || antiaim == ANTIAIM_DEFAULT)
		return;

	antiaim = (AntiAims)((int)antiaim + 1); //increase index by 1 since antiaim fake has one extra option

	switch (antiaim)
	{
	case ANTIAIM_LEGIT:
		AntiAimFakeDrop.index = ANTIAIM_DEFAULT;
		AntiAimRealDrop.index = ANTIAIM_LEGIT;
		AntiAimPitchDrop.index = PITCH_NONE;
		ManualAntiAimChk.Checked = false;
		return;
	}

	pSilentChk.Checked = false;
	AimbotChk.Checked = true;
	AimbotHoldKeyChk.Checked = false;
	TriggerbotWhileAimbottingChk.Checked = true;
	SilentAimChk.Checked = true;
}

char *antiaimstringsFake[20] = { nofakeyawstr, defaultstr, antiaimdownbackstr, antiaimfakestr, antiaimlegitstr, antiaimskatestr, antiaimballerinastr, slowspinfakestr, spinstr, antiaimexperimentalstr, antiaimexperimental2str, experimentalforwardsstr, experimentalforwardsinvertedstr, testingstr, testing2str,  Classical,Classical180, Classical90, inverseswitchstr, sidewaysjitterstr };
Dropdown AntiAimFakeDrop = { Vector2D(40 - OVERLAY_OFFSETX, 270 - OVERLAY_OFFSETY), MENU_MISC, Centered, &OnAntiAimFakeChanged, 150, INDEX_ZERO, INDEX_ZERO, 20, &antiaimstringsFake, FADE_ZERO, IsNotFaded, IsNotDisabled };

char *nopitchaastr = new char[22]{ 62, 19, 9, 27, 24, 22, 31, 90, 42, 19, 14, 25, 18, 90, 59, 20, 14, 19, 59, 19, 23, 0 }; /*Disable Pitch AntiAim*/
																														   //char *emotoinstr = new char[8]{ 63, 23, 21, 14, 19, 21, 20, 0 }; /*Emotion*/
char *upstr = new char[3]{ 47, 10, 0 }; /*Up*/
char *downstr = new char[5]{ 62, 21, 13, 20, 0 }; /*Down*/
												  //char *fakedownstr = new char[9]{ 60, 27, 17, 31, 62, 21, 13, 20, 0 }; /*FakeDown*/
												  //char *fakedownnstr = new char[10]{ 60, 27, 17, 31, 62, 21, 13, 20, 20, 0 }; /*FakeDown*/
												  //char *fakeupstr = new char[7]{ 60, 27, 17, 31, 47, 10, 0 }; /*FakeUp*/
char *jitterdownstr = new char[12]{ 48, 19, 14, 14, 31, 8, 90, 62, 21, 13, 20, 0 }; /*Jitter Down*/
char *jitterupdownstr = new char[15]{ 48, 19, 14, 14, 31, 8, 90, 47, 10, 85, 62, 21, 13, 20, 0 }; /*Jitter Up/Down*/
char *lisp1str = new char[18]{ 54, 19, 9, 10, 90, 75, 90, 82, 62, 63, 46, 63, 57, 46, 63, 62, 83, 0 }; /*Lisp 1 (DETECTED)*/
char *lisp2str = new char[18]{ 54, 19, 9, 10, 90, 72, 90, 82, 62, 63, 46, 63, 57, 46, 63, 62, 83, 0 }; /*Lisp 2 (DETECTED)*/

void OnAntiAimPitchChanged()
{
	if (AntiAimRealDrop.index == ANTIAIM_LEGIT)
	{
		AntiAimPitchDrop.index = PITCH_NONE;
		return;
	}

	AntiAimPitches pitch = (AntiAimPitches)AntiAimPitchDrop.index;

	if (pitch == PITCH_NONE)
		return;

	pSilentChk.Checked = false;
	AimbotChk.Checked = true;
	AimbotHoldKeyChk.Checked = false;
	TriggerbotWhileAimbottingChk.Checked = true;
	SilentAimChk.Checked = true;
}

char *TrueDownstr = new char[22]{ 46, 8, 15, 31, 90, 62, 21, 13, 20, 90, 90, 82, 62, 63, 46, 63, 57, 46, 63, 62, 83, 0 }; /*True Down  (DETECTED)*/
char *FakeDownstr = new char[22]{ 60, 27, 17, 31, 90, 62, 21, 13, 20, 90, 90, 82, 62, 63, 46, 63, 57, 46, 63, 62, 83, 0 }; /*Fake Down  (DETECTED)*/

char *antiaimstringsX[9] = { nopitchaastr, upstr, downstr, jitterdownstr , jitterupdownstr, lisp1str, lisp2str , TrueDownstr , FakeDownstr };
Dropdown AntiAimPitchDrop = { Vector2D(40 - OVERLAY_OFFSETX, 220 - OVERLAY_OFFSETY), MENU_MISC, Centered, &OnAntiAimPitchChanged, 150, INDEX_ZERO, INDEX_ZERO, 9, &antiaimstringsX, FADE_ZERO, IsNotFaded, IsNotDisabled };

char *aajitterstr = new char[16]{ 59, 20, 14, 19, 90, 59, 19, 23, 90, 48, 19, 14, 14, 31, 8, 0 }; /*Anti Aim Jitter*/
Checkbox AntiAimJitterChk = { Vector2D(40 - OVERLAY_OFFSETX, 280 + ((CHECKBOX_WIDTH * 2) * 1) - OVERLAY_OFFSETY), MENU_MISC, Centered, nullptr, CheckedOff, aajitterstr, NotPressed, IsNotFaded, IsNotDisabled };

char *nolockbetweenshotsstr = new char[28]{ 41, 10, 19, 20, 85, 59, 20, 14, 19, 90, 59, 19, 23, 90, 56, 31, 14, 13, 31, 31, 20, 90, 41, 18, 21, 14, 9, 0 }; /*Spin/Anti Aim Between Shots*/
Checkbox SpinBetweenShotsChk = { Vector2D(130 - OVERLAY_OFFSETX, 280 + ((CHECKBOX_WIDTH * 2) * 1) - OVERLAY_OFFSETY), MENU_MISC, Centered, nullptr, CheckedOn, nolockbetweenshotsstr, NotPressed, IsNotFaded, IsNotDisabled };


void OnManualAntiAimClicked()
{
	if (ManualAntiAimChk.Checked)
	{
		//pSilentChk.Checked = false;
		AimbotChk.Checked = true;
		AimbotHoldKeyChk.Checked = false;
		TriggerbotWhileAimbottingChk.Checked = true;
		//SilentAimChk.Checked = true;
	}
}
char *manualantiaimstr = new char[28]{ 55, 27, 20, 15, 27, 22, 90, 59, 20, 14, 19, 59, 19, 23, 90, 82, 59, 8, 8, 21, 13, 90, 49, 31, 3, 9, 83, 0 }; /*Manual AntiAim (Arrow Keys)*/
Checkbox ManualAntiAimChk = { Vector2D(40 - OVERLAY_OFFSETX, 280 + ((CHECKBOX_WIDTH * 2) * 2) - OVERLAY_OFFSETY), MENU_MISC, Centered, &OnManualAntiAimClicked, CheckedOff, manualantiaimstr, NotPressed, IsNotFaded, IsNotDisabled };
char *edgeaastr = new char[19]{ 45, 27, 22, 22, 90, 63, 30, 29, 31, 90, 59, 20, 14, 19, 90, 59, 19, 23, 0 }; /*Wall Edge Anti Aim*/
void OnEdgeAA()
{
	//EdgeAntiAimChk.Checked = false;
}
Checkbox EdgeAntiAimChk = { Vector2D(180 - OVERLAY_OFFSETX, 280 + ((CHECKBOX_WIDTH * 2) * 2) - OVERLAY_OFFSETY), MENU_MISC, Centered, &OnEdgeAA, CheckedOff, edgeaastr, NotPressed, IsNotFaded, IsNotDisabled };
//END OF MISCELLANEOUS MENU

//NOTE: ADD NEW CHECKBOXES AND TEXTBOXES AFTER THE EXISTING ONES OR YOU WILL BREAK THE LINEAR CONFIG FILE ORDER
void InitMenuOptions()
{
	menubuttonhighlighted = &AimbotMenuBtn;
	AddButton(&AimbotMenuBtn);
	AddButton(&TriggerbotMenuBtn);
	AddButton(&VisualsMenuBtn);
	AddButton(&MiscMenuBtn);
	AddButton(&AboutMenuBtn);
	AddButton(&SaveConfigOneBtn);
	AddButton(&LoadConfigOneBtn);
	AddButton(&SaveConfigTwoBtn);
	AddButton(&LoadConfigTwoBtn);
	AddButton(&PlayerListMenuBtn);
	AddButton(&CopyCompetitiveMatchIDBtn);
	AddButton(&SetAimbotKeyBtn);
	AddButton(&SetpSilentKeyBtn);
	AddButton(&SetTriggerbotKeyBtn);
	AddButton(&SetThirdPersonKeyBtn);
	AddButton(&CStraffKeyBtn);
	AddButton(&SetAirStuckKeyBtn);
	AddButton(&LoadDefaultConfigOneBtn);
	AddButton(&LoadDefaultConfigTwoBtn);
	AddButton(&SaveCustomConfigBtn);
	AddButton(&LoadCustomConfigBtn);

	AddCheckbox(&DisableAllChk);
	AddCheckbox(&AimbotChk);
	AddCheckbox(&AutoFireChk);
	AddCheckbox(&TriggerbotWhileAimbottingChk);
	AddCheckbox(&ESPChk);
	AddCheckbox(&GlowChk);
	AddCheckbox(&AimbotAimXChk);
	AddCheckbox(&AimbotAimYChk);
	AddCheckbox(&MiniMapSpottedChk);
	AddCheckbox(&BoxESPChk);
	AddCheckbox(&BhopChk);
	//AddCheckbox(&RageBhopChk); //READD WHEN MOUSEDX AND MOUSEDY ARE SET PROPERLY
	AddCheckbox(&EnginePredictChk);
	AddCheckbox(&AimbotAimHeadNeckChk);
	AddCheckbox(&AimbotAimTorsoChk);
	AddCheckbox(&AimbotAimArmsHandsChk);
	AddCheckbox(&AimbotAimLegsFeetChk);
	AddCheckbox(&AimbotHitchanceArmsLegsFeetChk);
	AddCheckbox(&AimbotHoldKeyChk);
	AddCheckbox(&NoRecoilChk);
	AddCheckbox(&AimbotAutoTargetSwitchChk);
	AddCheckbox(&TargetTeammatesChk);
	AddCheckbox(&AutoWallChk);
	//AddCheckbox(&NoSpreadChk);
	AddCheckbox(&SilentAimChk);
	AddCheckbox(&pSilentChk);
	AddCheckbox(&LegitAimStepChk);
	AddCheckbox(&RageAimStepChk);
	AddCheckbox(&FakeLagOnlyInAirChk);
	AddCheckbox(&AntiAimJitterChk);
	AddCheckbox(&SpinBetweenShotsChk);
	AddCheckbox(&NoVisualRecoilChk);
	AddCheckbox(&ResolverChk);
	AddCheckbox(&AimbotMultiPointChk);
	AddCheckbox(&EdgeAntiAimChk);
	AddCheckbox(&PressToRetargetChk);
	AddCheckbox(&LockOnToHitgroupsOnlyChk);
	AddCheckbox(&GlowTargetsChk);
	AddCheckbox(&AutoBoneBestDamageChk);
	AddCheckbox(&AimbotMultiPointHeadNeckChk);
	AddCheckbox(&AimbotMultiPointChestTorsoPelvisChk);
	AddCheckbox(&AimbotMultiPointShoulderArmsHandsChk);
	AddCheckbox(&AimbotMultiPointLegsFeetChk);
	AddCheckbox(&TriggerbotDontShootWhileJumpingChk);
	AddCheckbox(&KnifeBotChk);
	AddCheckbox(&RageBhopChk);
	AddCheckbox(&AutoWallPenetrateTeammatesChk);
	AddCheckbox(&DrawSpectatorsChk);
	AddCheckbox(&NoRecoilPistolsShotgunsSnipersChk);
	AddCheckbox(&DontTargetHeadNeckChk);
	AddCheckbox(&NoRecoilRageChk);
	AddCheckbox(&ValveResolverChk);
	AddCheckbox(&DrawRecoilCrosshairChk);
	AddCheckbox(&RemoveInterpChk);
	AddCheckbox(&AutoStopMovementChk);
	AddCheckbox(&DrawDamageChk);
	AddCheckbox(&AutoResolveAllChk);
	AddCheckbox(&IgnoreAutoResolveChk);
	AddCheckbox(&DrawAimbotBoneChk);
	AddCheckbox(&AutoPelvisChk);
	AddCheckbox(&FireWhenEnemiesShootChk);
	AddCheckbox(&DisablePostProcessingChk);
	AddCheckbox(&DisableDynamicLightingChk);
	AddCheckbox(&EntityESPChk);
	AddCheckbox(&NoFlashChk);
	AddCheckbox(&AutoGriefChk);
	AddCheckbox(&DrawHitboxesChk);
	AddCheckbox(&GrenadeTrajectoryChk);
	AddCheckbox(&ManualAntiAimChk);
	AddCheckbox(&PredictFakeAnglesChk);
	AddCheckbox(&FaceItChk);
	AddCheckbox(&PriorityTargetChk);
	AddCheckbox(&AimbotMultiPointPrioritiesOnlyChk);
	AddCheckbox(&ThirdPersonOffsetChk);
	AddCheckbox(&ShowFakedAnglesChk);
	AddCheckbox(&ShowRealAnglesChk);
	AddCheckbox(&AutoScopeSnipersChk);
	AddCheckbox(&OnlyHitchanceInTriggerbotChk);
	AddCheckbox(&FiringCorrectionsChk);
	AddCheckbox(&DetectPeekersChk);
	AddCheckbox(&HeadshotOnlyPeekersChk);
	AddCheckbox(&BloodResolverChk);
	AddCheckbox(&DrawBloodResolveChk);
	AddCheckbox(&BodyAimIfNoBacktrackTickChk);
	AddCheckbox(&BacktrackToBloodChk);
	AddCheckbox(&AimESPChk);
	AddCheckbox(&ArrowESPChk);
	AddCheckbox(&BodyAimIfLethalChk);
	AddCheckbox(&DrawResolveModeChk);
	AddCheckbox(&spoofConvars);
	AddCheckbox(&NoFlickerChk);
	AddCheckbox(&BacktrackExploitChk);
	AddCheckbox(&FootstepESPChk);
	AddCheckbox(&AutoAcceptChk);
	AddCheckbox(&NoScopeChk);
	AddCheckbox(&AnimatedClanTagChk);
	AddCheckbox(&UseMutinyTagChk);
	AddCheckbox(&AutoRevolverChk);
	AddCheckbox(&LegitBotChk);
	AddCheckbox(&AntiBacktrackChk);
	//AddCheckbox(&NoSmokeChk);
	//MUST ADD NEW ONES AFTER THE LAST


	AddTextbox(&TriggerBotDelayTxt);
	//AddTextbox(&AimbotMouseMoveDelayTxt);
	AddTextbox(&AimbotFOVTxt);
	AddTextbox(&AimbotSpeedTxt);
	AddTextbox(&AimbotHitPercentageHeadTxt);
	AddTextbox(&AimbotKeyTxt);
	AddTextbox(&TriggerbotKeyTxt);
	AddTextbox(&FakeLagChokedTxt);
	AddTextbox(&SpinDegPerTickTxt);
	AddTextbox(&FOVChangerTxt);
	AddTextbox(&MenuOffsetXTxt);
	AddTextbox(&MenuOffsetYTxt);
	AddTextbox(&MinimumDamageBeforeFiringTxt);
	AddTextbox(&MenuTextLineColorRedTxt);
	AddTextbox(&MenuTextLineColorGreenTxt);
	AddTextbox(&MenuTextLineColorBlueTxt);
	AddTextbox(&MenuColorRedTxt);
	AddTextbox(&MenuColorGreenTxt);
	AddTextbox(&MenuColorBlueTxt);
	AddTextbox(&pSilentKeyTxt);
	AddTextbox(&SetClanTagTxt);
	AddTextbox(&CStraffKeyTxt);
	AddTextbox(&ResolvePitchTxt);
	AddTextbox(&ResolveYawTxt);
	AddTextbox(&TriggerBotTimeToFireTxt);
	AddTextbox(&LowerBodyWaitTimeBeforeFiringTxt);
	AddTextbox(&ViewModelFOVChangerTxt);
	AddTextbox(&BodyAimBelowThisHealthTxt);
	AddTextbox(&MaxTargetsPerTickTxt);
	AddTextbox(&ThirdPersonKeyTxt);
	AddTextbox(&AimbotHitPercentageBodyTxt);
	AddTextbox(&TargetFPSTxt);
	AddTextbox(&AirStuckKeyTxt);
	AddTextbox(&NoRecoilXPercentTxt);
	AddTextbox(&NoRecoilYPercentTxt);
	AddTextbox(&DontShootAfterXMissesTxt);
	AddTextbox(&pSilentFOVTxt);
	AddTextbox(&PointScaleTxt);
	AddTextbox(&WhaleDongTxt);
	AddTextbox(&CustomConfigTxt);
	AddTextbox(&SpreadReduceWaitTimeTxt);
	AddTextbox(&BodyAimAfterHeadNotVisibleForXSecsTxt);
	AddTextbox(&AimbotActivateAfterXShotsTxt);
	AddTextbox(&AimbotSmoothingScaleTxt);
	AddTextbox(&AimbotPosRandomScaleTxt);
	AddTextbox(&AimbotAngleRandomScaleTxt);
	//MUST ADD NEW ONES AFTER THE LAST

	AddDropdown(&AntiAimPitchDrop);
	AddDropdown(&AntiAimRealDrop);
	AddDropdown(&AntiAimFakeDrop);
	AddDropdown(&ResolvePitchDrop);
	AddDropdown(&ResolveYawDrop);
	AddDropdown(&FakeLagDropDown);
	//MUST ADD NEW ONES AFTER THE L6AST
}

void DrawButton(Button* button)
{
	//pSprite->Begin(D3DXSPRITE_ALPHABLEND | D3DXSPRITE_DO_NOT_ADDREF_TEXTURE);
	Vector2D posonscreen = button->ScreenPos;
	posonscreen.x += MenuOffsetXTxt.iValue;
	posonscreen.y -= MenuOffsetYTxt.iValue;
	if (button->OffsetFromCenter)
	{
		posonscreen.x += CenterOfScreen.x;
		posonscreen.y += CenterOfScreen.y;
		//todo: window border ^
	}
	int fillR = MenuColorRedTxt.flValue / 1.875f;
	int fillG = MenuColorGreenTxt.flValue / 1.875f;
	int fillB = MenuColorBlueTxt.flValue / 1.875f;
	int fillA = 255;
	if (button->Pressed)
	{
		fillR = max(0, fillR - 25);
		fillG = max(0, fillG - 25);
		fillB = max(0, fillB - 25);
	}

	float flAlpha = !button->Fade ? 255.0f : 150.0f;

	//DrawGUIBox(posonscreen, button->width, button->height, MenuColorRedTxt.flValue, MenuColorGreenTxt.flValue, MenuColorBlueTxt.flValue, 255, fillR, fillG, fillB, fillA);
	DrawBox(posonscreen, button->width, button->height, 1, MenuColorRedTxt.flValue * 0.75f, MenuColorGreenTxt.flValue * 0.75f, MenuColorBlueTxt.flValue * 0.75f, flAlpha);
	GradientFunc(Vector2D(posonscreen.x + 1, posonscreen.y), button->width - 1, button->height - 1, fillR, fillG, fillB, fillA, (button->height / 2.0f));

	if (button->Highlighted)
	{
		DrawBox(Vector2D(posonscreen.x - 3, posonscreen.y - 3), button->width + 5, button->height + 5, 1, MenuTextLineColorRedTxt.flValue, MenuTextLineColorGreenTxt.flValue, MenuTextLineColorBlueTxt.flValue, flAlpha);
	}

	int labellen = strlen(button->Label);

	if (labellen > 0)
	{
		//float strxpos = button->width / 8;
		float strypos = button->height / 3;
		DecStr(button->Label, labellen);
		DrawStringUnencrypted(button->Label, Vector2D(posonscreen.x + ((button->width - 1) / 2) - ((labellen + 1) * 2), posonscreen.y + (button->Pressed ? strypos + 1 : strypos)), ColorRGBA(MenuTextLineColorRedTxt.flValue, MenuTextLineColorGreenTxt.flValue, MenuTextLineColorBlueTxt.flValue, flAlpha), pFontSmall);
		EncStr(button->Label, labellen);
	}
	//pSprite->End();
}

void DrawCheckbox(Checkbox* chkbox)
{
	//pSprite->Begin(D3DXSPRITE_ALPHABLEND | D3DXSPRITE_DO_NOT_ADDREF_TEXTURE);
	Vector2D posonscreen = chkbox->ScreenPos;
	posonscreen.x += MenuOffsetXTxt.iValue;
	posonscreen.y -= MenuOffsetYTxt.iValue;
	if (chkbox->OffsetFromCenter)
	{
		posonscreen.x += CenterOfScreen.x;
		posonscreen.y += CenterOfScreen.y;
		//todo: window border ^
	}

	float flAlpha = !chkbox->Fade ? 255.0f : 150.0f;

	DrawGUIBox(posonscreen, CHECKBOX_WIDTH, CHECKBOX_HEIGHT, MenuColorRedTxt.flValue, MenuColorGreenTxt.flValue, MenuColorBlueTxt.flValue, 255, MenuColorRedTxt.flValue / 1.875f, MenuColorGreenTxt.flValue / 1.875f, MenuColorBlueTxt.flValue / 1.875f, flAlpha);
	if (chkbox->Checked)
	{
		//todo: scale based on width/height
		Vector2D linepos = { posonscreen.x + 2, posonscreen.y + 5 };
		Vector2D linepos2 = { posonscreen.x + 4, posonscreen.y + 9 };
		DrawLine(linepos, linepos2, MenuTextLineColorRedTxt.flValue, MenuTextLineColorGreenTxt.flValue, MenuTextLineColorBlueTxt.flValue, flAlpha);
		linepos = { posonscreen.x + 8, posonscreen.y };
		DrawLine(linepos2, linepos, MenuTextLineColorRedTxt.flValue, MenuTextLineColorGreenTxt.flValue, MenuTextLineColorBlueTxt.flValue, flAlpha);
	}
	int labellen = strlen(chkbox->Label);
	if (labellen > 0)
	{
		DecStr(chkbox->Label, labellen);
		DrawStringUnencrypted(chkbox->Label, Vector2D(posonscreen.x + 15, posonscreen.y - 1), ColorRGBA(MenuTextLineColorRedTxt.flValue, MenuTextLineColorGreenTxt.flValue, MenuTextLineColorBlueTxt.flValue, flAlpha), pFontSmall);
		EncStr(chkbox->Label, labellen);
	}
	//pSprite->End();
}

void DrawTextbox(Textbox* txtbox, Textbox* editingtextbox)
{
	//pSprite->Begin(D3DXSPRITE_ALPHABLEND | D3DXSPRITE_DO_NOT_ADDREF_TEXTURE);
	Vector2D posonscreen = txtbox->ScreenPos;
	posonscreen.x += MenuOffsetXTxt.iValue;
	posonscreen.y -= MenuOffsetYTxt.iValue;
	Vector2D incposonscreen = txtbox->IncScreenPos;
	incposonscreen.x += MenuOffsetXTxt.iValue;
	incposonscreen.y -= MenuOffsetYTxt.iValue;
	Vector2D decposonscreen = txtbox->DecScreenPos;
	decposonscreen.x += MenuOffsetXTxt.iValue;
	decposonscreen.y -= MenuOffsetYTxt.iValue;
	if (txtbox->OffsetFromCenter)
	{
		//todo: window border ^
		posonscreen.x += CenterOfScreen.x;
		posonscreen.y += CenterOfScreen.y;
		incposonscreen.x += CenterOfScreen.x;
		incposonscreen.y += CenterOfScreen.y;
		decposonscreen.x += CenterOfScreen.x;
		decposonscreen.y += CenterOfScreen.y;
	}

	float flAlpha = !txtbox->Fade ? 255.0f : 150.0f;

	DrawBox(posonscreen, txtbox->Width, TEXTBOX_HEIGHT, 1, MenuColorRedTxt.flValue, MenuColorGreenTxt.flValue, MenuColorBlueTxt.flValue, flAlpha);
	if (editingtextbox == txtbox)
		FillRGB(posonscreen, txtbox->Width, TEXTBOX_HEIGHT, 136, 239, 228, 176); //Draw it slightly yellow so we see we are editing that box
	else
		FillRGB(posonscreen, txtbox->Width, TEXTBOX_HEIGHT, MenuColorRedTxt.flValue / 1.875f, MenuColorGreenTxt.flValue / 1.875f, MenuColorBlueTxt.flValue / 1.875f, flAlpha);

	if (strlen(txtbox->Value) > 0)
	{
		DrawStringUnencrypted(txtbox->Value, Vector2D(posonscreen.x + 2, posonscreen.y + 3), ColorRGBA(MenuTextLineColorRedTxt.flValue, MenuTextLineColorGreenTxt.flValue, MenuTextLineColorBlueTxt.flValue, flAlpha), pFontSmall);
	}

	if (txtbox->Type != TYPES::Char)
	{
		//todo: scale based on width/height
		//DrawGUIBox(incposonscreen, TEXTBOX_SCROLL_WIDTH, TEXTBOX_SCROLL_HEIGHT, MenuColorRedTxt.flValue, MenuColorGreenTxt.flValue, MenuColorBlueTxt.flValue, 255, MenuColorRedTxt.flValue / 1.875f, MenuColorGreenTxt.flValue / 1.875f, MenuColorBlueTxt.flValue / 1.875f, 255);
		DrawBox(incposonscreen, TEXTBOX_SCROLL_WIDTH, TEXTBOX_SCROLL_HEIGHT, 1, MenuColorRedTxt.flValue, MenuColorGreenTxt.flValue, MenuColorBlueTxt.flValue, flAlpha);
		GradientFunc(incposonscreen, TEXTBOX_SCROLL_WIDTH, TEXTBOX_SCROLL_HEIGHT, MenuColorRedTxt.flValue / 1.875f, MenuColorGreenTxt.flValue / 1.875f, MenuColorBlueTxt.flValue / 1.875f, flAlpha, 0);
		Vector2D linepos = { incposonscreen.x + 2, incposonscreen.y + 4 }; //bottom left
		Vector2D linepos2 = { incposonscreen.x + 7, incposonscreen.y }; //center top
		DrawLine(linepos, linepos2, MenuTextLineColorRedTxt.flValue, MenuTextLineColorGreenTxt.flValue, MenuTextLineColorBlueTxt.flValue, flAlpha);
		linepos = { incposonscreen.x + 14, incposonscreen.y + 5 }; //bottom right
		DrawLine(linepos2, linepos, MenuTextLineColorRedTxt.flValue, MenuTextLineColorGreenTxt.flValue, MenuTextLineColorBlueTxt.flValue, flAlpha);

		//DrawGUIBox(decposonscreen, TEXTBOX_SCROLL_WIDTH, TEXTBOX_SCROLL_HEIGHT, MenuColorRedTxt.flValue, MenuColorGreenTxt.flValue, MenuColorBlueTxt.flValue, 255, MenuColorRedTxt.flValue / 1.875f, MenuColorGreenTxt.flValue / 1.875f, MenuColorBlueTxt.flValue / 1.875f, 255);
		DrawBox(decposonscreen, TEXTBOX_SCROLL_WIDTH, TEXTBOX_SCROLL_HEIGHT, 1, MenuColorRedTxt.flValue, MenuColorGreenTxt.flValue, MenuColorBlueTxt.flValue, flAlpha);
		GradientFunc(decposonscreen, TEXTBOX_SCROLL_WIDTH, TEXTBOX_SCROLL_HEIGHT, MenuColorRedTxt.flValue / 1.875f, MenuColorGreenTxt.flValue / 1.875f, MenuColorBlueTxt.flValue / 1.875f, flAlpha, 0);
		linepos = { decposonscreen.x + 2, decposonscreen.y + 1 }; //top left
		linepos2 = { decposonscreen.x + 7, decposonscreen.y + 5 }; //center top
		DrawLine(linepos, linepos2, MenuTextLineColorRedTxt.flValue, MenuTextLineColorGreenTxt.flValue, MenuTextLineColorBlueTxt.flValue, flAlpha);
		linepos = { decposonscreen.x + 14, decposonscreen.y }; //top right
		DrawLine(linepos2, linepos, MenuTextLineColorRedTxt.flValue, MenuTextLineColorGreenTxt.flValue, MenuTextLineColorBlueTxt.flValue, flAlpha);
	}
	int len = strlen(txtbox->Label);
	DecStr(txtbox->Label, len);
	DrawStringUnencrypted(txtbox->Label, Vector2D(incposonscreen.x + 20, posonscreen.y + 1), ColorRGBA(MenuTextLineColorRedTxt.flValue, MenuTextLineColorGreenTxt.flValue, MenuTextLineColorBlueTxt.flValue, flAlpha), pFontSmall);
	EncStr(txtbox->Label, len);
	//pSprite->End();
}

Textbox *txtboxediting = nullptr; //Current text box we are editing

								  //0 = not, 1 = increased, 2 = decreased, 3 = reset, 4 = set manual value
void ModifyTextboxValue(Textbox* txtbox, unsigned char Modified)
{
	switch (txtbox->Type)
	{
	case TYPES::Float:
	{
		if (Modified == 4)
		{
			//manually set value
			txtbox->flValue = clamp((float)atof(txtbox->Value), (float)txtbox->dbMin, (float)txtbox->dbMax);
		}
		else if (Modified != 3) //3 is reset value
		{
			txtbox->flValue += Modified == 1 ? 0.05f : -0.05f;
			txtbox->flValue = clamp(txtbox->flValue, (float)txtbox->dbMin, (float)txtbox->dbMax);
		}
		sprintf(txtbox->Value, "%.6f", txtbox->flValue);
		break;
	}
	case TYPES::Double:
	{
		if (Modified == 4)
		{
			//manually set value
			txtbox->dbValue = clamp(atof(txtbox->Value), txtbox->dbMin, txtbox->dbMax);
		}
		else if (Modified != 3) //3 is reset value
		{
			txtbox->dbValue += Modified == 1 ? 0.05 : -0.05;
			txtbox->dbValue = clamp(txtbox->dbValue, txtbox->dbMin, txtbox->dbMax);
		}
		sprintf(txtbox->Value, "%.6f", txtbox->dbValue);
		break;
	}
	case TYPES::Integer:
	{
		if (Modified == 4)
		{
			//manually set value
			txtbox->iValue = clamp(atoi(txtbox->Value), (int)txtbox->dbMin, (int)txtbox->dbMax);
		}
		else if (Modified != 3) //3 is reset value
		{
			txtbox->iValue += Modified == 1 ? 1 : -1;
			txtbox->iValue = clamp(txtbox->iValue, (int)txtbox->dbMin, (int)txtbox->dbMax);
		}
		sprintf(txtbox->Value, "%i", txtbox->iValue);
		break;
	}
	case TYPES::Byte:
	{
		if (Modified == 4)
		{
			//manually set value
			txtbox->bValue = clamp((BYTE)atoi(txtbox->Value), (BYTE)txtbox->dbMin, (BYTE)txtbox->dbMax);
		}
		else if (Modified != 3) //3 is reset value
		{
			txtbox->bValue += Modified == 1 ? 1 : -1;
			txtbox->bValue = clamp(txtbox->bValue, (BYTE)txtbox->dbMin, (BYTE)txtbox->dbMax);
		}
		sprintf(txtbox->Value, "%u", txtbox->bValue);
		break;
	}
	case TYPES::Bool:
	{
		if (Modified == 4)
		{
			//manually set value
			txtbox->bValue = clamp((BYTE)atoi(txtbox->Value), 0, 1);
		}
		else if (Modified != 3) //3 is reset value
		{
			txtbox->bValue ^= 1;
		}
		sprintf(txtbox->Value, "%u", txtbox->bValue);
		break;
	}
	}
}

Dropdown *dropdownediting = nullptr;

void DrawDropdown(Dropdown* dropdown)
{
	pSprite->End();
	pSprite->Begin(D3DXSPRITE_ALPHABLEND | D3DXSPRITE_DO_NOT_ADDREF_TEXTURE);
	Vector2D posonscreen = dropdown->ScreenPos;
	posonscreen.x += MenuOffsetXTxt.iValue;
	posonscreen.y -= MenuOffsetYTxt.iValue;
	if (dropdown->OffsetFromCenter)
	{
		//todo: window border ^
		posonscreen.x += CenterOfScreen.x;
		posonscreen.y += CenterOfScreen.y;
	}

	float flAlpha = !dropdown->Fade ? 255.0f : 150.0f;

	DrawBox(posonscreen, dropdown->Width, TEXTBOX_HEIGHT, 1, MenuColorRedTxt.flValue, MenuColorGreenTxt.flValue, MenuColorBlueTxt.flValue, flAlpha);
	FillRGB(posonscreen, dropdown->Width, TEXTBOX_HEIGHT, MenuColorRedTxt.flValue / 1.875f, MenuColorGreenTxt.flValue / 1.875f, MenuColorBlueTxt.flValue / 1.875f, flAlpha);
	float xpos = posonscreen.x + dropdown->Width + 1;
	DrawGUIBox(Vector2D(xpos, posonscreen.y), TEXTBOX_HEIGHT, TEXTBOX_HEIGHT, MenuColorRedTxt.flValue, MenuColorGreenTxt.flValue, MenuColorBlueTxt.flValue, 255, MenuColorRedTxt.flValue / 1.875f, MenuColorGreenTxt.flValue / 1.875f, MenuColorBlueTxt.flValue / 1.875f, flAlpha);
	//todo: scale based on width/height
	Vector2D linepos = { xpos + 4, posonscreen.y + 4 };
	Vector2D linepos2 = { xpos + (TEXTBOX_HEIGHT / 2), posonscreen.y + TEXTBOX_HEIGHT - 4 };
	DrawLine(linepos, linepos2, MenuTextLineColorRedTxt.flValue, MenuTextLineColorGreenTxt.flValue, MenuTextLineColorBlueTxt.flValue, flAlpha);
	linepos = { xpos + TEXTBOX_HEIGHT - 4, posonscreen.y + 3 };
	DrawLine(linepos2, linepos, MenuTextLineColorRedTxt.flValue, MenuTextLineColorGreenTxt.flValue, MenuTextLineColorBlueTxt.flValue, flAlpha);

	char **strings = (char**)dropdown->strings;

	char* ddstring = strings[dropdown->index];
	int len = strlen(ddstring);

	DecStr(ddstring, len);

	DrawStringUnencrypted(ddstring, Vector2D(posonscreen.x + 5, posonscreen.y + 2), ColorRGBA(MenuTextLineColorRedTxt.flValue, MenuTextLineColorGreenTxt.flValue, MenuTextLineColorBlueTxt.flValue, flAlpha), pFontSmall);

	EncStr(ddstring, len);

	if (dropdown->Clicked)
	{
		if (dropdown->AlphaFade < 255.0f)
		{
			float ft = ReadFloat((uintptr_t)&Interfaces::Globals->absoluteframetime);
			if (ft < (1.0f / 60.0f))
				ft *= 2500.0f;
			else
				ft *= 500.0f;
			dropdown->AlphaFade = min(dropdown->AlphaFade + ft, 255.0f);
		}
		for (int i = 0; i < dropdown->count; i++)
		{
			Vector2D posofindex = { posonscreen.x, posonscreen.y + (TEXTBOX_HEIGHT + 1) * (i + 1) };
			if (i != dropdown->highlightedindex)
			{
				DrawBox(posofindex, dropdown->Width, TEXTBOX_HEIGHT, 1, MenuColorRedTxt.flValue, MenuColorGreenTxt.flValue, MenuColorBlueTxt.flValue, (int)dropdown->AlphaFade);
				FillRGB(posofindex, dropdown->Width, TEXTBOX_HEIGHT, MenuColorRedTxt.flValue / 2.575f, MenuColorGreenTxt.flValue / 2.5975f, MenuColorBlueTxt.flValue / 2.575f, (int)dropdown->AlphaFade);
			}
			else
			{
				DrawBox(posofindex, dropdown->Width, TEXTBOX_HEIGHT, 1, MenuColorRedTxt.flValue, MenuColorGreenTxt.flValue, MenuColorBlueTxt.flValue, (int)dropdown->AlphaFade);
				FillRGB(posofindex, dropdown->Width, TEXTBOX_HEIGHT, 66, 169, 158, (int)dropdown->AlphaFade < 255 ? (int)dropdown->AlphaFade : 255);
			}

			char *txt = strings[i];
			int len = strlen(txt);
			DecStr(strings[i], len);
			DrawStringUnencrypted(txt, Vector2D(posofindex.x + 5, posofindex.y + 2), ColorRGBA(MenuTextLineColorRedTxt.flValue, MenuTextLineColorGreenTxt.flValue, MenuTextLineColorBlueTxt.flValue, (int)dropdown->AlphaFade), pFontSmall);
			EncStr(strings[i], len);
		}
	}
	else
	{
		dropdown->AlphaFade = 1.0f; //TODO: MOVE ME
	}
	pSprite->End();
	pSprite->Begin(D3DXSPRITE_ALPHABLEND | D3DXSPRITE_DO_NOT_ADDREF_TEXTURE);
}

//Initialized in NVInit
short LastButtonsPressed_Txtbox[91]; //List of keys pressed
float NextTimeButtonPress_Txtbox[91];  //Next time you can press keys
char *companystr = new char[14]{ 13, 13, 13, 84, 55, 15, 14, 19, 20, 3, 84, 10, 13, 0 }; /*www.Mutiny.pw*/
#ifndef MUTINY
char *licexpires1str = new char[34]{ 54, 19, 25, 31, 20, 9, 31, 90, 63, 2, 10, 19, 8, 31, 9, 90, 82, 35, 85, 55, 85, 62, 83, 64, 90, 95, 19, 85, 95, 19, 85, 95, 19, 0 }; /*License Expires (Y/M/D): %i/%i/%i*/
char *licexpires2str = new char[31]{ 54, 19, 25, 31, 20, 9, 31, 90, 63, 2, 10, 19, 8, 31, 9, 90, 82, 35, 85, 55, 85, 62, 83, 64, 90, 52, 63, 44, 63, 40, 0 }; /*License Expires (Y/M/D): NEVER*/
#endif
char *compiledonstr = new char[14]{ 57, 21, 23, 10, 19, 22, 31, 30, 90, 53, 20, 64, 90, 0 }; /*Compiled On: */
char *competitivematchidstr = new char[23]{ 57, 21, 23, 10, 31, 14, 19, 14, 19, 12, 31, 90, 55, 27, 14, 25, 18, 90, 51, 62, 64, 90, 0 }; /*Competitive Match ID: */
char *virtualkeyhelperstr = new char[53]{ 42, 8, 31, 9, 9, 90, 14, 18, 31, 90, 17, 31, 3, 90, 3, 21, 15, 90, 13, 19, 9, 18, 90, 14, 21, 90, 15, 9, 31, 90, 21, 8, 90, 10, 8, 31, 9, 9, 90, 63, 41, 57, 90, 14, 21, 90, 25, 27, 20, 25, 31, 22, 0 }; /*Press the key you wish to use or press ESC to cancel*/

#ifdef SERVER_CRASHER
char *devbuildstr = new char[16]{ 62, 63, 44, 63, 54, 53, 42, 63, 40, 90, 56, 47, 51, 54, 62, 0 }; /*DEVELOPER BUILD*/
#else
#ifndef MUTINY
char *privatebuildstr = new char[15]{ 90, 42, 40, 51, 44, 59, 46, 63, 90, 56, 47, 51, 54, 62, 0 }; /* PRIVATE BUILD*/
#endif
#endif


void DrawMenu()
{
	auto stat = START_PROFILING("DrawMenu");
	POINT cpos;
	GetCursorPos(&cpos);
	Vector2D cursorpos = { (float)cpos.x, (float)cpos.y };
	int Clicking = (GetAsyncKeyState(VK_LBUTTON) & (1 << 16));
	static float NextTimeClickOnMenuIsAvailable = 0.0f;
	float CenterX = CenterOfScreen.x;
	float CenterY = CenterOfScreen.y;

#if 0
	pNewFont->BeginDrawing();
	pNewFontSmall->BeginDrawing();
	bNewFontCanDraw = true;
	bNewFontSmallCanDraw = true;
#endif

	const int menuwidth = 589;
	int menuheight = 384;

	if (MENU_ACTIVE == MENU_AIMBOT)
	{
		menuheight = 449;
		DrawFOV();
	}

#ifdef _DEBUG
	double starttime = QPCTime();
#endif

#ifndef MUTINY
#if defined(SERVER_CRASHER)
	DecStr(devbuildstr, 15);
#else
	DecStr(privatebuildstr, 14);
#endif
	FillRGB(Vector2D(CenterX + (200 - OVERLAY_OFFSETX) + MenuOffsetXTxt.iValue, CenterY - (63 + OVERLAY_OFFSETY) - MenuOffsetYTxt.iValue), 162, 25, MenuColorRedTxt.flValue / 4.25f, MenuColorGreenTxt.flValue / 4.25f, MenuColorBlueTxt.flValue / 4.25f, 240);
	DrawBox(Vector2D(CenterX + (200 - OVERLAY_OFFSETX) + MenuOffsetXTxt.iValue, CenterY - (63 + OVERLAY_OFFSETY) - MenuOffsetYTxt.iValue), 162, 25, 10, MenuColorRedTxt.flValue / 2.2f, MenuColorGreenTxt.flValue / 2.2f, MenuColorBlueTxt.flValue / 2.2f, 240);
#if defined(SERVER_CRASHER)
	DrawStringUnencrypted(devbuildstr, Vector2D(CenterX + (207 - OVERLAY_OFFSETX) + MenuOffsetXTxt.iValue, CenterY - (60 + OVERLAY_OFFSETY) - MenuOffsetYTxt.iValue), ColorRGBA(MenuTextLineColorRedTxt.flValue, MenuTextLineColorGreenTxt.flValue, MenuTextLineColorBlueTxt.flValue, 255), pFont);
	EncStr(devbuildstr, 15);
#else
	DrawStringUnencrypted(privatebuildstr, Vector2D(CenterX + (207 - OVERLAY_OFFSETX) + MenuOffsetXTxt.iValue, CenterY - (60 + OVERLAY_OFFSETY) - MenuOffsetYTxt.iValue), ColorRGBA(MenuTextLineColorRedTxt.flValue, MenuTextLineColorGreenTxt.flValue, MenuTextLineColorBlueTxt.flValue, 255), pFont);
	EncStr(privatebuildstr, 14);
#endif
#endif

	//pSprite->Begin(D3DXSPRITE_ALPHABLEND | D3DXSPRITE_DO_NOT_ADDREF_TEXTURE);
	DrawBox(Vector2D(CenterX + (1 - OVERLAY_OFFSETX) + MenuOffsetXTxt.iValue, CenterY - (34 + OVERLAY_OFFSETY) - MenuOffsetYTxt.iValue), menuwidth + 9, menuheight, 5, MenuColorRedTxt.flValue / 2.2f, MenuColorGreenTxt.flValue / 2.2f, MenuColorBlueTxt.flValue / 2.2f, 240);
	//DrawGUIBox(Vector2D(CenterX + (5 - OVERLAY_OFFSETX) + MenuOffsetXTxt.iValue, CenterY - (30 + OVERLAY_OFFSETY) - MenuOffsetYTxt.iValue), 580, 375, MenuColorRedTxt.flValue / 4.25f, MenuColorGreenTxt.flValue / 4.25f, MenuColorBlueTxt.flValue / 4.25f, 240, MenuColorRedTxt.flValue / 4.25f, MenuColorGreenTxt.flValue / 4.25f, MenuColorBlueTxt.flValue / 4.25f, 240);
	FillRGB(Vector2D(CenterX + (5 - OVERLAY_OFFSETX) + MenuOffsetXTxt.iValue, CenterY - (30 + OVERLAY_OFFSETY) - MenuOffsetYTxt.iValue), menuwidth, menuheight - 9, MenuColorRedTxt.flValue / 4.25f, MenuColorGreenTxt.flValue / 4.25f, MenuColorBlueTxt.flValue / 4.25f, 240);
	//GradientFunc(Vector2D(CenterX + (5 - OVERLAY_OFFSETX) + MenuOffsetXTxt.iValue, CenterY - (30 + OVERLAY_OFFSETY) - MenuOffsetYTxt.iValue), 580, 375, MenuColorRedTxt.flValue / 4.25f, MenuColorGreenTxt.flValue / 4.25f, MenuColorBlueTxt.flValue / 4.25f, 240, 220);
	//pSprite->End();

#ifdef _DEBUG
	double endtime = QPCTime();

	char speedstr[48];
	sprintf(speedstr, "Menu background draw time in ms: %.4f", (endtime - starttime) * 1000.0f);
	//pSprite->Begin(D3DXSPRITE_ALPHABLEND | D3DXSPRITE_DO_NOT_ADDREF_TEXTURE);
	DrawStringUnencrypted(speedstr, Vector2D(2, 2), ColorRGBA(0, 255, 0, 255), pFontSmall);
	//pSprite->End();
#endif

	//See if we are waiting on the user to press a button to input to a textbox
	if (virtualkeytxtboxtoedit)
	{
		if (Interfaces::Globals->realtime - flTimeRequestedToChangeVirtualKey > 0.25f)
		{
			for (int i = 1; i <= VK_OEM_CLEAR; i++)
			{
				if (GetAsyncKeyState(i) & (1 << 16))
				{
					if (i != VK_ESCAPE)
					{
						virtualkeytxtboxtoedit->iValue = i;
						itoa(i, virtualkeytxtboxtoedit->Value, 10);
					}

					virtualkeytxtboxtoedit = nullptr;
					break;
				}
			}
		}

		//pSprite->Begin(D3DXSPRITE_ALPHABLEND | D3DXSPRITE_DO_NOT_ADDREF_TEXTURE);
		DecStr(virtualkeyhelperstr, 52);
		DrawStringUnencrypted(virtualkeyhelperstr, Vector2D(CenterX + (125 - OVERLAY_OFFSETX) + MenuOffsetXTxt.iValue, CenterY + 150 + OVERLAY_OFFSETY - MenuOffsetYTxt.iValue), ColorRGBA(MenuTextLineColorRedTxt.flValue, MenuTextLineColorGreenTxt.flValue, MenuTextLineColorBlueTxt.flValue, 255), pFont);
		EncStr(virtualkeyhelperstr, 52);
		//pSprite->End();

		END_PROFILING(stat);
		return;
	}

	if (MENU_ACTIVE == MENU_ABOUT)
	{
		//pSprite->Begin(D3DXSPRITE_ALPHABLEND | D3DXSPRITE_DO_NOT_ADDREF_TEXTURE);
		DecStr(companystr, 13);
		DrawStringUnencrypted(companystr, Vector2D(CenterX + (225 - OVERLAY_OFFSETX) + MenuOffsetXTxt.iValue, CenterY - (20 + OVERLAY_OFFSETY) - MenuOffsetYTxt.iValue), ColorRGBA(MenuTextLineColorRedTxt.flValue, MenuTextLineColorGreenTxt.flValue, MenuTextLineColorBlueTxt.flValue, 255), pFont);
		EncStr(companystr, 13);
		char LicStr[256];
		char CompiledStr[256] = { 0 };
#ifndef MUTINY
#ifdef LICENSED
		DecStr(licexpires1str, 33);
		std::string format = std::string(licexpires1str);
		EncStr(licexpires1str, 33);
		strcpy(LicStr, format.c_str());
		sprintf(LicStr, format.c_str(), LicenseExpiresYear / 4, LicenseExpiresMonth / 4, LicenseExpiresDay / 4);
#else
		DecStr(licexpires2str, 30);
		strcpy(LicStr, licexpires2str);
		EncStr(licexpires2str, 30);
#endif
#endif
		DecStr(compiledonstr, 13);
		memcpy(CompiledStr, compiledonstr, 14);
		EncStr(compiledonstr, 13);
		strcat(CompiledStr, __DATE__);
#ifndef MUTINY
		DrawStringUnencrypted(LicStr, Vector2D(CenterX + (40 - OVERLAY_OFFSETX) + MenuOffsetXTxt.iValue, CenterY - (-60 + OVERLAY_OFFSETY) - MenuOffsetYTxt.iValue), ColorRGBA(MenuTextLineColorRedTxt.flValue, MenuTextLineColorGreenTxt.flValue, MenuTextLineColorBlueTxt.flValue, 255), pFontSmall);
		DrawStringUnencrypted(CompiledStr, Vector2D(CenterX + (40 - OVERLAY_OFFSETX) + MenuOffsetXTxt.iValue, CenterY - (-75 + OVERLAY_OFFSETY) - MenuOffsetYTxt.iValue), ColorRGBA(MenuTextLineColorRedTxt.flValue, MenuTextLineColorGreenTxt.flValue, MenuTextLineColorBlueTxt.flValue, 255), pFontSmall);
#else
		DrawStringUnencrypted(CompiledStr, Vector2D(CenterX + (40 - OVERLAY_OFFSETX) + MenuOffsetXTxt.iValue, CenterY - (-60 + OVERLAY_OFFSETY) - MenuOffsetYTxt.iValue), ColorRGBA(MenuTextLineColorRedTxt.flValue, MenuTextLineColorGreenTxt.flValue, MenuTextLineColorBlueTxt.flValue, 255), pFontSmall);
#endif
		char matchidstr[240];
		DecStr(competitivematchidstr, 22);
		memcpy(matchidstr, competitivematchidstr, 23);
		EncStr(competitivematchidstr, 22);
		char id[128];
		sprintf(id, "%" PRIu64 "\n", Helper_GetLastCompetitiveMatchId());
		strcat(matchidstr, id);
		DrawStringUnencrypted(matchidstr, Vector2D(CenterX + (225 - OVERLAY_OFFSETX) + MenuOffsetXTxt.iValue, CenterY - (-60 + OVERLAY_OFFSETY) - MenuOffsetYTxt.iValue), ColorRGBA(MenuTextLineColorRedTxt.flValue, MenuTextLineColorGreenTxt.flValue, MenuTextLineColorBlueTxt.flValue, 255), pFontSmall);
		//pSprite->End();
		//memset(LicStr, 0, sizeof(LicStr));
	}
	else if (MENU_ACTIVE == MENU_PLAYERLIST)
	{
		DrawPlayerList();
	}

	bool NothingClickedOn = true; //We clicked on nothing, blank space

								  //pSprite->Begin(D3DXSPRITE_ALPHABLEND | D3DXSPRITE_DO_NOT_ADDREF_TEXTURE);
								  //Draw all buttons
	for (std::vector<Button*>::iterator btnp = Buttons.begin(); btnp != Buttons.end(); btnp++)
	{
		Button* btn = *btnp;
		if (!btn->Disabled && (btn->Menu == MENU_ACTIVE || btn->Menu == MENU_NONE))
		{
			Vector2D centeroffset = btn->OffsetFromCenter ? Vector2D(CenterX, CenterY) : Vector2D(0.0f, 0.0f);
			Vector2D posonscreen = Vector2D(btn->ScreenPos.x + centeroffset.x + MenuOffsetXTxt.iValue, btn->ScreenPos.y + centeroffset.y - MenuOffsetYTxt.iValue);//Vector2D(chkbox->ScreenPos.x + 5 + centeroffset.x, chkbox->ScreenPos.y + 5 + centeroffset.y);
			if (!btn->Fade && Clicking && PointIsInsideBox(cursorpos, posonscreen, btn->width, btn->height))//if (DistanceBetweenPoints(cursorpos, posonscreen) <= 12.0f)
			{
				NothingClickedOn = false;
				if (!btn->Pressed)
				{
					if (btn->OnPressed != nullptr)
						((void(*)(void))btn->OnPressed)();
					btn->Pressed = true;
					//Reset text box value since we clicked outside the text box
					ExitTextbox();
				}
			}
			else if (btn->Pressed)
			{
				btn->Pressed = false;
			}
			DrawButton(btn);
		}
	}
	//pSprite->End();

	//Draw all checkboxes
	for (std::vector<Checkbox*>::iterator chkbx = Checkboxes.begin(); chkbx != Checkboxes.end(); chkbx++)
	{
		Checkbox* chkbox = *chkbx;
		if (!chkbox->Disabled && (chkbox->Menu == MENU_ACTIVE || chkbox->Menu == MENU_NONE))
		{
			Vector2D centeroffset = chkbox->OffsetFromCenter ? Vector2D(CenterX, CenterY) : Vector2D(0.0f, 0.0f);
			Vector2D posonscreen = Vector2D(chkbox->ScreenPos.x + centeroffset.x + MenuOffsetXTxt.iValue, chkbox->ScreenPos.y + centeroffset.y - MenuOffsetYTxt.iValue);//Vector2D(chkbox->ScreenPos.x + 5 + centeroffset.x, chkbox->ScreenPos.y + 5 + centeroffset.y);
			if (!chkbox->Fade && Clicking && PointIsInsideBox(cursorpos, posonscreen, CHECKBOX_WIDTH, CHECKBOX_HEIGHT))//if (DistanceBetweenPoints(cursorpos, posonscreen) <= 12.0f)
			{
				NothingClickedOn = false;
				if (!chkbox->Pressed)
				{
					chkbox->Checked ^= true;
					//Reset text box value since we clicked outside the text box
					ExitTextbox();
					if (chkbox->OnPressed != nullptr)
						((void(*)(void))chkbox->OnPressed)();
					chkbox->Pressed = true;
				}
			}
			else if (chkbox->Pressed)
			{
				chkbox->Pressed = false;
			}
			DrawCheckbox(*chkbx);
		}
	}

	//Draw all textboxes
	for (std::vector<Textbox*>::iterator txtbx = Textboxes.begin(); txtbx != Textboxes.end(); txtbx++)
	{
		Textbox* txtbox = *txtbx;
		if (!txtbox->Disabled && (txtbox->Menu == MENU_ACTIVE || txtbox->Menu == MENU_NONE))
		{
			if (Time >= NextTimeClickOnMenuIsAvailable)
			{
				unsigned char Modified = 0; //0 = not, 1 = increased, 2 = decreased, 3 = reset, 4 = set manual value
				if (!txtbox->Fade && Clicking)
				{
					//todo: window border ^
					Vector2D centeroffset = txtbox->OffsetFromCenter ? Vector2D(CenterX, CenterY) : Vector2D(0.0f, 0.0f);
					Vector2D posonscreen = Vector2D(txtbox->ScreenPos.x + centeroffset.x + MenuOffsetXTxt.iValue, txtbox->ScreenPos.y + centeroffset.y - MenuOffsetYTxt.iValue);//Vector2D(txtbox->ScreenPos.x + 5 + centeroffset.x, txtbox->ScreenPos.y + 5 + centeroffset.y);
					Vector2D incposonscreen = Vector2D(txtbox->IncScreenPos.x + centeroffset.x + MenuOffsetXTxt.iValue, txtbox->IncScreenPos.y + centeroffset.y - MenuOffsetYTxt.iValue);//Vector2D(txtbox->IncScreenPos.x + 8 + centeroffset.x, txtbox->IncScreenPos.y + 5 + centeroffset.y);
					Vector2D decposonscreen = Vector2D(txtbox->DecScreenPos.x + centeroffset.x + MenuOffsetXTxt.iValue, txtbox->DecScreenPos.y + centeroffset.y - MenuOffsetYTxt.iValue);//Vector2D(txtbox->DecScreenPos.x + 8 + centeroffset.x, txtbox->DecScreenPos.y + 5 + centeroffset.y);

					if (PointIsInsideBox(cursorpos, posonscreen, txtbox->Width, TEXTBOX_HEIGHT))//DistanceBetweenPoints(cursorpos, posonscreen) <= txtbox->Width / 1.5)
					{
						//Clicked on text box itself
						NextTimeClickOnMenuIsAvailable = Time + 0.1f - clamp(Time - NextTimeClickOnMenuIsAvailable, 0.0f, (1.0f / 64.0f));
						if (txtboxediting)
							ModifyTextboxValue(txtboxediting, 3); //We clicked on another text box while already editing one, reset the old one
						txtboxediting = txtbox;
						memset(txtbox->Value, 0, sizeof(txtbox->Value));
						NothingClickedOn = false;
					}
					else if (txtbox->Type != TYPES::Char)
					{
						if (PointIsInsideBox(cursorpos, incposonscreen, TEXTBOX_SCROLL_WIDTH, TEXTBOX_SCROLL_HEIGHT))//(DistanceBetweenPoints(cursorpos, incposonscreen) <= 6.0f)
						{
							Modified = 1;
							NothingClickedOn = false;
						}
						else if (PointIsInsideBox(cursorpos, decposonscreen, TEXTBOX_SCROLL_WIDTH, TEXTBOX_SCROLL_HEIGHT))//(DistanceBetweenPoints(cursorpos, decposonscreen) <= 6.0f)
						{
							Modified = 2;
							NothingClickedOn = false;
						}
					}
				}
				static float NextTimePressDecimal_Txtbox = 0.0f;
				static float NextTimePressMinus_Txtbox = 0.0f;
				static float NextTimePressBackslash_Txtbox = 0.0f;
				if (txtbox == txtboxediting)
				{
					//Handle button presses to change text box data

					if (txtbox->Fade || (GetAsyncKeyState(VK_ESCAPE) >> 8))
					{
						//Cancel the editing
						Modified = 3;
						txtboxediting = nullptr;
					}
					else if (GetAsyncKeyState(VK_RETURN) >> 8)
					{
						Modified = 4;
						//stop editing
						txtboxediting = nullptr;
					}
					else if (txtbox->dbMin < 0.0 && (txtbox->Type == TYPES::Float || txtbox->Type == TYPES::Double) && GetAsyncKeyState(VK_OEM_MINUS) >> 8)
					{
						int stringlength = strlen(txtbox->Value);
						if (stringlength + 1 < sizeof(txtbox->Value))
						{
							if (Time >= NextTimePressMinus_Txtbox)
							{
								//If we are pressing the same key as last time, only allow a keypress after 0.2 secs
								strcat(txtbox->Value, "-"); //Append txt
								txtbox->Value[stringlength + 1] = 0; //Null terminator
								NextTimePressMinus_Txtbox = Time + 0.2f - clamp(Time - NextTimePressMinus_Txtbox, 0.0f, (1.0f / 64.0f)); //Set time we pressed this key
							}
						}
					}
					else if ((txtbox->Type == TYPES::Float || txtbox->Type == TYPES::Double) && GetAsyncKeyState(VK_OEM_PERIOD) >> 8)
					{
						int stringlength = strlen(txtbox->Value);
						if (stringlength + 1 < sizeof(txtbox->Value))
						{
							if (Time >= NextTimePressDecimal_Txtbox)
							{
								//If we are pressing the same key as last time, only allow a keypress after 0.2 secs
								strcat(txtbox->Value, "."); //Append txt
								txtbox->Value[stringlength + 1] = 0; //Null terminator
								NextTimePressDecimal_Txtbox = Time + 0.2f - clamp(Time - NextTimePressDecimal_Txtbox, 0.0f, (1.0f / 64.0f)); //Set time we pressed this key
							}
						}
					}
					else if (txtbox->Type == TYPES::Char && GetAsyncKeyState(VK_OEM_5)) // backslash
					{
						int stringlength = strlen(txtbox->Value);
						if (stringlength + 1 < sizeof(txtbox->Value))
						{
							if (Time >= NextTimePressBackslash_Txtbox)
							{
								//If we are pressing the same key as last time, only allow a keypress after 0.2 secs
								strcat(txtbox->Value, "\\"); //Append txt
								txtbox->Value[stringlength + 1] = 0; //Null terminator
								NextTimePressBackslash_Txtbox = Time + 0.2f - clamp(Time - NextTimePressBackslash_Txtbox, 0.0f, (1.0f / 64.0f)); //Set time we pressed this key
							}
						}
					}
					else
					{
						static float NextTimePressBackspace_Txtbox = 0.0f;
						int stringlength = strlen(txtbox->Value);
						if (GetAsyncKeyState(VK_BACK) >> 8)
						{
							//Pressed Backspace
							if (stringlength > 0 && Time >= NextTimePressBackspace_Txtbox)
							{
								txtbox->Value[stringlength] = 0;
								txtbox->Value[stringlength - 1] = 0;
								NextTimePressBackspace_Txtbox = Time + 0.2f - clamp(Time - NextTimePressBackspace_Txtbox, 0.0f, (1.0f / 64.0f));
							}
						}
						else
						{
							//Check other keys
							short keys[91] = { 0 };
							short x;
							int count = txtbox->Type == TYPES::Char ? 90 : 57;
							for (x = 48; x <= count; x++) //<= 90 for letters too
							{
								keys[x] = (GetAsyncKeyState(x) >> 8);
								if (keys[x])
								{
									char actualkey[2];
									if (x < 65) {
										//Number keys match exactly
										actualkey[0] = (char)x;
									}
									else {
										//Letters need + 32
										actualkey[0] = (char)x + 32;
										if (GetAsyncKeyState(VK_SHIFT))
										{
											actualkey[0] = toupper(actualkey[0]);
										}
									}
									actualkey[1] = 0;
									if (stringlength + 1 < sizeof(txtbox->Value))
									{
										if (!LastButtonsPressed_Txtbox[x] || Time >= NextTimeButtonPress_Txtbox[x])
										{
											//If we are pressing the same key as last time, only allow a keypress after 0.2 secs
											strcat(txtbox->Value, actualkey); //Append txt
											txtbox->Value[stringlength + 1] = 0; //Null terminator
											NextTimeButtonPress_Txtbox[x] = Time + 0.2f - clamp(Time - NextTimeButtonPress_Txtbox[x], 0.0f, (1.0f / 64.0f)); //Set time we pressed this key
											NextTimePressBackspace_Txtbox = 0.0f; //Let us press backspace immediately after this key
											NextTimePressDecimal_Txtbox = 0.0f; //Let us press decimal immediately after this key
											NextTimePressMinus_Txtbox = 0.0f; //Let us press minus immediately after this key
											NextTimePressBackslash_Txtbox = 0.0f; //Let us press backslash immediately after this key
										}
									}
								}
								else
								{
									//Not pressing this key, reset the last button state time
									NextTimeButtonPress_Txtbox[x] = 0.0f;
								}
								//Update last button states
								LastButtonsPressed_Txtbox[x] = keys[x];
							}
						}
					}
				}
				if (Modified)
				{
					ModifyTextboxValue(txtbox, Modified);
					if (txtbox->CallOnSet != nullptr)
					{
						((void(*)(void))txtbox->CallOnSet)();
					}
					NextTimeClickOnMenuIsAvailable = Time + 0.1f - clamp(Time - NextTimeClickOnMenuIsAvailable, 0.0f, (1.0f / 64.0f));
				}
				if (txtboxediting && Clicking && NothingClickedOn)
				{
					ModifyTextboxValue(txtboxediting, 3);
					txtboxediting = nullptr;
				}
			}
			DrawTextbox(*txtbx, txtboxediting);
		}
	}

	//Draw all dropdowns
	for (std::vector<Dropdown*>::iterator drpdn = Dropdowns.begin(); drpdn != Dropdowns.end(); drpdn++)
	{
		Dropdown* dropdown = *drpdn;
		if (!dropdown->Disabled && (dropdown->Menu == MENU_ACTIVE || dropdown->Menu == MENU_NONE))
		{
			//draw the currently in use dropdown last so other ones don't overlap
			if (dropdown != dropdownediting)
			{
				Vector2D centeroffset = dropdown->OffsetFromCenter ? Vector2D(CenterX, CenterY) : Vector2D(0.0f, 0.0f);
				Vector2D posonscreen = Vector2D(dropdown->ScreenPos.x + centeroffset.x + MenuOffsetXTxt.iValue, dropdown->ScreenPos.y + centeroffset.y - MenuOffsetYTxt.iValue);
				//Check if we are clicking inside the label/downarrow
				if (!dropdown->Fade && !dropdownediting && Clicking && PointIsInsideBox(cursorpos, posonscreen, dropdown->Width + TEXTBOX_HEIGHT, TEXTBOX_HEIGHT))
				{
					NothingClickedOn = false;
					if (Time >= NextTimeClickOnMenuIsAvailable)
					{
						if (dropdownediting)
						{
							dropdownediting->Clicked = false;
							DrawDropdown(dropdownediting); //Draw the dropdown now so it's not invisible this frame
						}

						dropdown->Clicked = true;
						dropdownediting = dropdown;
						NextTimeClickOnMenuIsAvailable = Time + 0.3f;
						Clicking = false; //Stop any attempt from clicking in indexes on first frame
						continue; //draw last
					}
				}
				DrawDropdown(dropdown);
			}
		}
	}

	//Draw currently in use dropdowns last
	if (dropdownediting && !dropdownediting->Disabled)
	{
		Dropdown *currentdropdown = dropdownediting;
		Vector2D centeroffset = dropdownediting->OffsetFromCenter ? Vector2D(CenterX, CenterY) : Vector2D(0.0f, 0.0f);
		Vector2D posonscreen = Vector2D(dropdownediting->ScreenPos.x + centeroffset.x + MenuOffsetXTxt.iValue, dropdownediting->ScreenPos.y + centeroffset.y - MenuOffsetYTxt.iValue);
		//Check if we are clicking inside the 'current selected dropdown box'
		if (!currentdropdown->Fade)
		{
			if (Clicking && PointIsInsideBox(cursorpos, posonscreen, dropdownediting->Width + TEXTBOX_HEIGHT, TEXTBOX_HEIGHT))
			{
				NothingClickedOn = false;
				if (Time >= NextTimeClickOnMenuIsAvailable)
				{
					dropdownediting->Clicked = false;
					dropdownediting = nullptr;
					NextTimeClickOnMenuIsAvailable = Time + 0.3f;
				}
			}
			//check to see if we are clicking any of its indexes
			else
			{
				bool Highlight = false;
				for (int i = 0; i < dropdownediting->count; i++)
				{
					Vector2D posofindex = { posonscreen.x, posonscreen.y + (TEXTBOX_HEIGHT + 1) * (i + 1) };
					if (PointIsInsideBox(cursorpos, posofindex, dropdownediting->Width, TEXTBOX_HEIGHT))
					{
						if (Clicking)
						{
							NothingClickedOn = false;
							dropdownediting->index = i;
							dropdownediting->Clicked = false;
							if (dropdownediting->CallOnSet != nullptr)
								((void(*)(void))dropdownediting->CallOnSet)();
							break;
						}
						else
						{
							dropdownediting->highlightedindex = i;
							Highlight = true;
						}
					}
				}
				if (!Highlight)
				{
					dropdownediting->highlightedindex = -1;
				}
				if (Clicking)
				{
					NothingClickedOn = false;
					dropdownediting->Clicked = false;
					dropdownediting = nullptr;
					NextTimeClickOnMenuIsAvailable = Time + 0.3f;
				}
			}
		}
		DrawDropdown(currentdropdown);
	}

	//Handle moving the menu around

	static Vector2D LastCursorPos = { 0, 0 };
	static bool HoldClick = false;

	if (!Clicking)
	{
		HoldClick = false;
	}
	else if (!NothingClickedOn)
	{
		HoldClick = true;
	}

	if (Clicking && NothingClickedOn && !HoldClick)
	{
		Vector2D backgroundpos{ CenterX + (5 - OVERLAY_OFFSETX) + MenuOffsetXTxt.iValue, CenterY - (30 + OVERLAY_OFFSETY) - MenuOffsetYTxt.iValue };
		if (PointIsInsideBox(cursorpos, backgroundpos, 580, 375))
		{
			MenuOffsetXTxt.iValue += cursorpos.x - LastCursorPos.x;
			MenuOffsetYTxt.iValue -= cursorpos.y - LastCursorPos.y;
		}
	}
	LastCursorPos = cursorpos;
#if 0
	pNewFont->EndDrawing();
	pNewFontSmall->EndDrawing();
	bNewFontCanDraw = false;
	bNewFontSmallCanDraw = false;
#endif

	//Draw mouse cursor
	//pSprite->Begin(D3DXSPRITE_ALPHABLEND | D3DXSPRITE_DO_NOT_ADDREF_TEXTURE);
	DrawLine(Vector2D(cursorpos.x, cursorpos.y), Vector2D(cursorpos.x, cursorpos.y + 18), 255, 255, 255, 255);
	DrawLine(Vector2D(cursorpos.x, cursorpos.y), Vector2D(cursorpos.x + 14, cursorpos.y + 14), 255, 255, 255, 255);
	DrawLine(Vector2D(cursorpos.x, cursorpos.y + 18), Vector2D(cursorpos.x + 4, cursorpos.y + 9), 255, 255, 255, 255);
	DrawLine(Vector2D(cursorpos.x + 4, cursorpos.y + 9), Vector2D(cursorpos.x + 14, cursorpos.y + 14), 255, 255, 255, 255);

	//pSprite->End();

	END_PROFILING(stat);
}

//char *aimbotfovstr = new char[11]{ 59, 19, 23, 24, 21, 14, 90, 60, 53, 44, 0 }; /*Aimbot FOV*/
void DrawFOV()
{
	//FIXME FOR DEGREE FOV NOT PIXEL FOV
#if 0
	//pSprite->Begin(D3DXSPRITE_ALPHABLEND | D3DXSPRITE_DO_NOT_ADDREF_TEXTURE);
	int size = AimbotFOV.iValue;
	DrawGUIBox(Vector2D(2, 2), size, size, MenuColorRedTxt.flValue / 1.4f, MenuColorGreenTxt.flValue / 1.4f, MenuColorBlueTxt.flValue / 1.4f, 255, MenuColorRedTxt.flValue / 4.25f, MenuColorGreenTxt.flValue / 4.25f, MenuColorBlueTxt.flValue / 4.25f, 230);

	if (size >= 100)
	{
		DecStr(aimbotfovstr, 10);
		DrawStringUnencrypted(aimbotfovstr, Vector2D((float)size * 0.5 - 32, (float)size * 0.5 - 10), ColorRGBA(MenuTextLineColorRedTxt.flValue, MenuTextLineColorGreenTxt.flValue, MenuTextLineColorBlueTxt.flValue, 255), pFont);
		EncStr(aimbotfovstr, 10);
	}
	//pSprite->End();
#endif
}

//char *externalstr = new char[10]{ 87, 31, 2, 14, 31, 8, 20, 27, 22, 0 }; /*-external*/
bool InitMenu()
{
	static bool Initialized = false;
	if (Initialized)
		return true;
	Initialized = true;

	if (EXTERNAL_WINDOW)
	{
		WNDCLASSEX wClass;
		wClass.cbWndExtra = NULL;
		wClass.style = CS_VREDRAW | CS_HREDRAW;
		wClass.hbrBackground = (HBRUSH)CreateSolidBrush(RGB(0, 0, 0));
		wClass.hCursor = LoadCursor(0, IDC_ARROW);
		wClass.cbClsExtra = NULL;
		wClass.cbSize = sizeof(WNDCLASSEX);
		wClass.hInstance = hInstance;
		wClass.lpfnWndProc = WinProc;
		wClass.lpszClassName = lWindowName;
		wClass.lpszMenuName = lWindowName;
		wClass.hIcon = LoadIcon(0, IDI_APPLICATION);
		wClass.hIconSm = LoadIcon(0, IDI_APPLICATION);

		if (!RegisterClassEx(&wClass))
			return false;

		GetWindowRect(tWnd, &tSize);
		Width = tSize.right - tSize.left;
		Height = tSize.bottom - tSize.top;
		hWnd = CreateWindowEx(WS_EX_NOPARENTNOTIFY | WS_EX_TOPMOST | WS_EX_TRANSPARENT | WS_EX_LAYERED, lWindowName, lWindowName, WS_POPUP, 1, 1, Width, Height, 0, 0, hInstance, 0);
		SetLayeredWindowAttributes(hWnd, 0, 1.0f, LWA_ALPHA);
		SetLayeredWindowAttributes(hWnd, 0, RGB(0, 0, 0), LWA_COLORKEY);
		ShowWindow(hWnd, SW_SHOW);

		if (!DirectXInit(hWnd))
		{
			return false;
		}
	}
	InitMenuOptions();
	return true;
}

int Width = 1920;
int Height = 1080;

const MARGINS Margin = { 0, 0, Width, Height };

#ifdef MUTINY
char lWindowName[256] = " ";
#else
#ifdef ATI
char lWindowName[256] = "ATI Frame Benchmarking Overlay";
#else
char lWindowName[256] = "NVIDIA Frame Benchmarking Overlay";
#endif
#endif

//char tWindowName[256] = "Counter-Strike: Global Offensive"; /* tWindowName ? Target Window Name */
RECT tSize;

MSG Message;

float NextThink = 0.0f;

void OverlayThink()
{
	/*
	static float NextOverlayReset = 0.0f;
	if (Time >= NextOverlayReset)
	{
	GetWindowRect(tWnd, &tSize);
	Width = tSize.right - tSize.left;
	Height = tSize.bottom - tSize.top;
	DWORD dwStyle = GetWindowLong(tWnd, GWL_STYLE);
	if (dwStyle & WS_BORDER)
	{
	tSize.top += 23;
	Height -= 23;
	}
	MoveWindow(hWnd, tSize.left, tSize.top, Width, Height, true);
	NextOverlayReset = Time + 0.1f;
	}
	*/
	/*
	RECT newsize;
	GetWindowRect(tWnd, &newsize);
	if (newsize.left != tSize.left || newsize.top != tSize.top)
	{
	tSize = newsize;
	Width = tSize.right - tSize.left;
	Height = tSize.bottom - tSize.top;
	DWORD dwStyle = GetWindowLong(tWnd, GWL_STYLE);
	if (dwStyle & WS_BORDER)
	{
	tSize.top += 23;
	Height -= 23;
	}
	MoveWindow(hWnd, tSize.left, tSize.top, Width, Height, true);
	}
	*/

	if (EXTERNAL_WINDOW)
	{
		if (PeekMessage(&Message, NULL, 0, 0, PM_NOREMOVE))
		{
			MESSAGE_PUMP_EMPTY = false;
			if (GetMessage(&Message, NULL, 0, 0))
			{
				TranslateMessage(&Message);
				DispatchMessage(&Message);
			}
		}
		else
		{
			MESSAGE_PUMP_EMPTY = true;
		}
	}

	DoWork();
}

LRESULT CALLBACK WinProc(HWND hWnd, UINT Message, WPARAM wParam, LPARAM lParam)
{
	PAINTSTRUCT ps;
	switch (Message)
	{
	case WM_PAINT:
	{
		BeginPaint(hWnd, &ps);
		//DoWork();
		//NextThink = Time + THINK_SPEED - fmin(fmax(0.0f, Time - NextThink), THINK_SPEED);
		EndPaint(hWnd, &ps);
		ValidateRect(hWnd, NULL);
		break;
	}
	case WM_CREATE:
	{
		__asm nop
		__asm nop
		DwmExtendFrameIntoClientArea(hWnd, &Margin);
		break;
	}
	case WM_DESTROY:
	{
		PostQuitMessage(1);
		return 0;
	}
	/*
	case WM_KEYUP:
	{
	if (MENU_IS_OPEN)
	{
	switch (wParam)
	{
	case VK_LEFT:
	{
	break;
	}
	case VK_RIGHT:
	{
	break;
	}
	case VK_UP:
	{
	break;
	}
	case VK_DOWN:
	{
	break;
	}
	case VK_RETURN:
	{
	break;
	}
	case VK_ESCAPE:
	{
	MENU_IS_OPEN = false;
	break;
	}
	case VK_DECIMAL:
	{
	break;
	}
	case VK_DELETE:
	{
	MENU_IS_OPEN = !MENU_IS_OPEN;
	break;
	}
	default:
	{
	break;
	}
	}
	}
	else
	{
	switch (wParam)
	{
	case VK_DELETE:
	{
	MENU_IS_OPEN = !MENU_IS_OPEN;
	break;
	}
	}
	}
	}
	*/
	default:
	{
		{
			return DefWindowProc(hWnd, Message, wParam, lParam);
			break;
		}
	}
	}
	return 0;
}

char *mouseenablestr = new char[15]{ 25, 22, 37, 23, 21, 15, 9, 31, 31, 20, 27, 24, 22, 31, 0 }; /*cl_mouseenable*/
ConVar *cl_mouseenable = nullptr;

void ToggleMenu()
{
	MENU_IS_OPEN = !MENU_IS_OPEN;

	if (!cl_mouseenable)
	{
		DecStr(mouseenablestr, 14);
		cl_mouseenable = Interfaces::Cvar->FindVar(mouseenablestr);
		EncStr(mouseenablestr, 14);
	}

	if (EXTERNAL_WINDOW)
	{
		int extendedStyle = GetWindowLong(hWnd, GWL_EXSTYLE);
		if (MENU_IS_OPEN)
		{
#ifndef _DEBUG
			SetWindowLong(hWnd, GWL_EXSTYLE, extendedStyle & ~WS_EX_TRANSPARENT);
			SetLayeredWindowAttributes(hWnd, 0, 255.0f, LWA_ALPHA);
#endif
			//SetCapture(hWnd);
			//ShowCursor(true);
			//#ifndef ATI
			SetForegroundWindow(hWnd);
			//#endif
		}
		else
		{
			SetWindowLong(hWnd, GWL_EXSTYLE, extendedStyle | WS_EX_TRANSPARENT);
			//SetLayeredWindowAttributes(hWnd, 0, 1.0f, LWA_ALPHA);
			//SetCapture(tWnd);
			//ShowCursor(false);
			//#ifndef ATI
			SetForegroundWindow(tWnd);
			//#endif
			//DirectXClearScreen();
		}
	}

	if (cl_mouseenable)
		cl_mouseenable->SetValue((int)!MENU_IS_OPEN);

	ExitTextbox();
	ExitDropdown();
	virtualkeytxtboxtoedit = nullptr;
	if (pHighlightedPlayer)
	{
		ClearHighlightedPlayer();
	}
}

#ifdef SERVER_CRASHER
extern char crashingserverstr[17];
#endif

void CheckMenuButtons()
{
	static float NextTimeToggleMenu = 0.0f;
	static float NextMenuDraw = 0.0f;
	if (GetAsyncKeyState(VK_DELETE) && Time >= NextTimeToggleMenu)
	{
		ToggleMenu();
		NextTimeToggleMenu = Time + 0.3f;
		if (EXTERNAL_WINDOW && !LevelIsLoaded && !MENU_IS_OPEN)
		{
			DirectXClearScreen();
		}
	}
	if (EXTERNAL_WINDOW && !LevelIsLoaded)
	{
		if (MENU_IS_OPEN)
		{
			if (Time >= NextMenuDraw)
			{
				//SET_FRAME_TIME(true, (1.0f / 8.0f)); // (1.0f / ((txtboxediting || pHighlightedPlayer) ? 32.0f : 16.0f)));
				if (DirectXBeginFrame())
				{
					DrawMenu();
#ifdef SERVER_CRASHER
					if (CrashingServer)
					{
						DecStr(crashingserverstr, 16);
						DrawStringUnencrypted(crashingserverstr, Vector2D(CenterOfScreen.x - 25, CenterOfScreen.y - 30), ColorRGBA(255, 0, 0, 255), pFont);
						EncStr(crashingserverstr, 16);
					}
#endif
					DirectXEndFrame();
					//pSprite->End();
					NextMenuDraw = Time + (1.0f / 32.0f);
				}
			}
		}
	}
}