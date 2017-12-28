#ifndef OVERLAY_H
#define OVERLAY_H
#pragma once

bool InitMenu();

#include "hDirectX.h"

#include <Windows.h>
#include <iostream>

#include <d3d9.h>
#include <d3dx9.h>
#pragma comment(lib, "d3d9.lib")
#pragma comment(lib, "d3dx9.lib")

#include <dwmapi.h>
#pragma comment(lib, "dwmapi.lib")
#include <vector>
#include "misc.h"

const ColorRGBA greencolor = { 0, 255, 0, 255 };
const ColorRGBA redcolor = { 255, 0, 0, 255 };

extern HINSTANCE hInstance;
extern bool EXTERNAL_WINDOW;
extern int Width;
extern int Height;
extern char lWindowName[256];
extern RECT tSize;
extern MSG Message;
extern bool Debug_Border;
extern float NextThink;

#ifdef _DEBUG
#define OVERLAY_OFFSETX 0
#define OVERLAY_OFFSETY 0
#else
#define OVERLAY_OFFSETX 250
#define OVERLAY_OFFSETY 50
#endif

#define TEXTBOX_HEIGHT 16
#define TEXTBOX_SCROLL_WIDTH 15
#define TEXTBOX_SCROLL_HEIGHT 7
#define CHECKBOX_WIDTH 10
#define CHECKBOX_HEIGHT 10
#define MENU_BUTTON_WIDTH 70
#define MENU_BUTTON_HEIGHT 32

void OverlayThink();

LRESULT CALLBACK WinProc(HWND hWnd, UINT Message, WPARAM wParam, LPARAM lParam);
//int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hSecInstance, LPSTR nCmdLine, INT nCmdShow);


enum TYPES
{
	Float = 0,
	Double,
	Integer,
	Bool,
	Byte,
	Char
};

enum MENUS
{
	MENU_NONE = 0,
	MENU_AIMBOT,
	MENU_TRIGGERBOT,
	MENU_ESP,
	MENU_MISC,
	MENU_PLAYERLIST,
	MENU_ABOUT,
	MENU_LEGITBOT
};

extern int MENU_ACTIVE;
static int currentweapon = 0;

//Vector2D ScreenPos, MENUS Menu, bool OffsetFromCenter, void* OnPressed, int width, int height, std::string Label, Color RGB, bool Highlighted, bool Pressed
struct Button
{
	Vector2D ScreenPos;
	MENUS Menu; //The menu this button is on
	bool OffsetFromCenter;
	void* OnPressed;
	int width;
	int height;
	char* Label;
	bool Highlighted;
	bool Pressed;
	bool Fade;
	bool Disabled;
};

//Vector2D ScreenPos, MENUS Menu, bool OffsetFromCenter, void* OnPressed, bool Checked, const char *Label, Color RGB, bool Pressed
struct Checkbox
{
	Vector2D ScreenPos;
	MENUS Menu;
	bool OffsetFromCenter;
	void* OnPressed;
	//bool WPConfigChecked[10];
	bool Checked;// = WPConfigChecked[currentweapon];
	//if(Menu != MENU_LEGITBOT)
	//	Checked = WPConfigChecked[currentweapon];
	char* Label;
	bool Pressed;
	bool Fade;
	bool Disabled;
};

#define Centered 1
#define NotCentered 0
#define CheckedOff 0
#define CheckedOn 1
#define NotPressed 0
#define IsPressed 1
#define NotHighlighted 0
#define IsHighlighted 1
#define INDEX_ZERO 0
#define FADE_ZERO 0.0f
#define NOT_FADED 0
#define IsFaded 1
#define IsNotFaded 0
#define IsDisabled 1
#define IsNotDisabled 0

//Vector2D ScreenPos, int Menu, Vector2D IncScreenPos, Vector2D DecScreenPos, MENUS Menu, bool OffsetFromCenter, int Width, TYPES Type, std::string Label, double dbMin, double dbMax, char Value[128], float flValue, double dbValue, int iValue, BYTE bValue
struct Textbox
{
	Vector2D ScreenPos;
	Vector2D IncScreenPos;
	Vector2D DecScreenPos;
	MENUS Menu;
	bool OffsetFromCenter;
	void* CallOnSet;
	int Width;
	TYPES Type;
	char* Label;
	double dbMin;
	double dbMax;
	char Value[128];
	float flValue;
	double dbValue;
	int iValue;
	BYTE bValue;
	bool Fade;
	bool Disabled;
};

//Vector2D ScreenPos, MENUS Menu, bool OffsetFromCenter, void* CallOnSet, int Width, int index, int highlightedindex, int count, void *strings, bool Clicked, float AlphaFade;
struct Dropdown
{
	Vector2D ScreenPos;
	MENUS Menu;
	bool OffsetFromCenter;
	void* CallOnSet;
	int Width;
	int index;
	int highlightedindex;
	int count;
	void* strings;
	bool Clicked;
	float AlphaFade;
	bool Fade;
	bool Disabled;
};

extern Button AimbotMenuBtn;
extern Button TriggerbotMenuBtn;
extern Button VisualsMenuBtn;
extern Button MiscMenuBtn;
extern Button PlayerListMenuBtn;
extern Button AboutMenuBtn;
extern Button SaveConfigOneBtn;
extern Button LoadConfigOneBtn;
extern Button LoadDefaultConfigOneBtn;
extern Button SaveConfigTwoBtn;
extern Button LoadConfigTwoBtn;
extern Textbox CustomConfigTxt;
extern Button SaveCustomConfigBtn;
extern Button LoadCustomConfigBtn;
extern Button LoadDefaultConfigTwoBtn;
extern Checkbox DisableAllChk;
extern Checkbox AimbotChk;
extern Checkbox AutoFireChk;
extern Checkbox TriggerbotWhileAimbottingChk;
extern Checkbox TriggerbotDontShootWhileJumpingChk;
extern Checkbox KnifeBotChk;
extern Checkbox AutoStopMovementChk;
extern Checkbox AutoScopeSnipersChk;
extern Checkbox OnlyHitchanceInTriggerbotChk;
extern Checkbox AutoRevolverChk;
extern Checkbox AntiBacktrackChk;
extern Checkbox BodyAimIfNoBacktrackTickChk;
extern Checkbox ESPChk;
extern Checkbox EntityESPChk;
extern Checkbox GlowChk;
extern Checkbox GlowTargetsChk;
extern Checkbox AimbotAimXChk;
extern Checkbox AimbotAimYChk;
extern Checkbox MiniMapSpottedChk;
extern Checkbox DrawResolveModeChk;
extern Checkbox BoxESPChk;
extern Checkbox DrawBloodResolveChk;
extern Checkbox DrawSpectatorsChk;
extern Checkbox ManualAntiAimChk;
extern Checkbox BhopChk;
extern Checkbox RageBhopChk;
extern Checkbox EnginePredictChk;
extern Checkbox RemoveInterpChk;
extern Checkbox AntiAimJitterChk;
extern Checkbox AimbotMultiPointChk;
extern Checkbox AimbotMultiPointHeadNeckChk;
extern Checkbox AimbotMultiPointChestTorsoPelvisChk;
extern Checkbox AimbotMultiPointShoulderArmsHandsChk;
extern Checkbox AimbotMultiPointLegsFeetChk;
extern Checkbox AimbotMultiPointPrioritiesOnlyChk;
extern Checkbox PressToRetargetChk;
extern Checkbox LockOnToHitgroupsOnlyChk;
extern Checkbox AutoBoneBestDamageChk;
extern Checkbox AimbotHoldKeyChk;
extern Checkbox NoRecoilChk;
extern Checkbox NoRecoilPistolsShotgunsSnipersChk;
extern Checkbox AimbotAutoTargetSwitchChk;
extern Checkbox pSilentChk;
extern Checkbox TargetTeammatesChk;
extern Checkbox AutoWallChk;
extern Checkbox AutoWallPenetrateTeammatesChk;
extern Checkbox NoSpreadChk;
extern Checkbox LegitBotChk;
extern Checkbox SilentAimChk;
extern Checkbox pSilentChk;
extern Checkbox LegitAimStepChk;
extern Checkbox RageAimStepChk;
extern Checkbox AutoGriefChk;
extern Checkbox FakeLagOnlyInAirChk;

extern Checkbox AimbotAimHeadNeckChk;
extern Checkbox AimbotAimTorsoChk;
extern Checkbox AimbotAimArmsHandsChk;
extern Checkbox AimbotAimLegsFeetChk;
extern Checkbox DetectPeekersChk;
extern Checkbox HeadshotOnlyPeekersChk;
extern Checkbox AimbotHitchanceArmsLegsFeetChk;
extern Checkbox BodyAimIfLethalChk;
extern Checkbox SpinBetweenShotsChk;
extern Checkbox NoRecoilRageChk;
extern Checkbox NoVisualRecoilChk;
extern Checkbox ResolverChk;
extern Checkbox PredictFakeAnglesChk;
extern Checkbox FaceItChk;
extern Checkbox FiringCorrectionsChk;
extern Checkbox BloodResolverChk;
extern Checkbox BacktrackToBloodChk;
extern Checkbox BacktrackExploitChk;
extern Checkbox AutoAcceptChk;
extern Checkbox DrawRecoilCrosshairChk;
extern Checkbox DrawDamageChk;
extern Checkbox DrawAimbotBoneChk;
extern Checkbox DisablePostProcessingChk;
extern Checkbox DisableDynamicLightingChk;
extern Checkbox NoFlashChk;
extern Checkbox NoSmokeChk;
extern Checkbox NoScopeChk;
extern Checkbox AnimatedClanTagChk;
extern Checkbox GrenadeTrajectoryChk;
extern Checkbox DrawHitboxesChk;
extern Checkbox AimESPChk;
extern Dropdown FakeLagDropDown;
extern Checkbox ArrowESPChk;
extern Checkbox FootstepESPChk;
extern Checkbox NoFlickerChk;
extern Textbox ThirdPersonKeyTxt;
extern Textbox CStraffKeyTxt;
extern Button SetThirdPersonKeyBtn;
extern Button CStraffKeyBtn;
extern Checkbox ThirdPersonOffsetChk;
extern Checkbox ShowRealAnglesChk;
extern Checkbox ShowFakedAnglesChk;
extern Textbox SetClanTagTxt;
extern Checkbox EdgeAntiAimChk;
extern Textbox TriggerBotDelayTxt;
extern Textbox TriggerBotTimeToFireTxt;
extern Textbox AimbotFOVTxt;
extern Textbox AimbotSpeedTxt;
extern Textbox AimbotActivateAfterXShotsTxt;
extern Textbox AimbotHitPercentageHeadTxt;
extern Textbox AimbotHitPercentageBodyTxt;
extern Textbox MinimumDamageBeforeFiringTxt;
extern Textbox LowerBodyWaitTimeBeforeFiringTxt;
extern Textbox DontShootAfterXMissesTxt;
extern Textbox BodyAimBelowThisHealthTxt;
extern Textbox AimbotSmoothingScaleTxt;
extern Textbox MaxTargetsPerTickTxt;
extern Textbox TargetFPSTxt;
extern Textbox pSilentFOVTxt;
extern Textbox PointScaleTxt;
extern Textbox SpreadReduceWaitTimeTxt;
extern Textbox AimbotPosRandomScaleTxt;
extern Textbox BodyAimAfterHeadNotVisibleForXSecsTxt;
extern Textbox AimbotAngleRandomScaleTxt;
extern Button SetAimbotKeyBtn;
extern Button SetpSilentKeyBtn;
extern Textbox AirStuckKeyTxt;
extern Button SetAirStuckKeyBtn;
extern Textbox AimbotKeyTxt;
extern Textbox TriggerbotKeyTxt;
extern Button SetTriggerbotKeyBtn;
extern Textbox FakeLagChokedTxt;
extern Textbox pSilentKeyTxt;
extern Textbox SpinDegPerTickTxt;
extern Textbox NoRecoilXPercentTxt;
extern Textbox NoRecoilYPercentTxt;
extern Textbox WhaleDongTxt;
extern Textbox FOVChangerTxt;
extern Checkbox spoofConvars;
extern Textbox ViewModelFOVChangerTxt;
extern Textbox MenuOffsetXTxt;
extern Textbox MenuOffsetYTxt;
extern Textbox MenuTextLineColorRedTxt;
extern Textbox MenuTextLineColorGreenTxt;
extern Textbox MenuTextLineColorBlueTxt;
extern Textbox MenuColorRedTxt;
extern Textbox MenuColorGreenTxt;
extern Textbox MenuColorBlueTxt;
extern Dropdown AntiAimRealDrop;
extern Dropdown AntiAimFakeDrop;
extern Dropdown AntiAimPitchDrop;
extern std::vector<Button*>Buttons; //Global buttons list
extern std::vector<Checkbox*>Checkboxes; //Global checkbox list
extern std::vector<Textbox*>Textboxes; //Global textbox list
extern std::vector<Dropdown*>Dropdowns; //Global dropdown list
extern bool MENU_IS_OPEN;

bool shouldweaponconfig(MENUS menu);
void InitMenuOptions();
void AddButton(Button* button);
void AddCheckbox(Checkbox* box);
void AddTextbox(Textbox* box);
void ExitDropdown();
void ExitTextbox();
void DrawMenu();
void DrawFOV();
void ModifyTextboxValue(Textbox* txtbox, unsigned char Modified);
extern Textbox *txtboxediting; //Current textbox we are editing
extern Button *menubuttonhighlighted; //Current menu button highlighted
extern Textbox *virtualkeytxtboxtoedit; //Current textbox we are waiting to input data to from a virtual key
extern Dropdown *dropdownediting; //Current dropdown we have open
extern float flTimeRequestedToChangeVirtualKey;

//Initialized at NVInit
extern short LastButtonsPressed_Txtbox[91]; //List of keys pressed
extern float NextTimeButtonPress_Txtbox[91];  //Last time pressed that key

#define CheckCheckbox(box) box.Checked = true;
#define UnCheckCheckbox(box) box.Checked = false;

void ToggleMenu();
void CheckMenuButtons();

#endif