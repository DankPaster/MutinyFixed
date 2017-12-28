#include "PlayerList.h"
#include "GameMemory.h"
#include "Interfaces.h"
#include "ESP.h"
#include "EncryptString.h"
#include "Overlay.h"
#include "ClassIDS.h"
#include "BaseCombatWeapon.h"
#include "AutoGrief.h"
#include "Animation.h"
#include "player_lagcompensation.h"
#include "LocalPlayer.h"

CustomPlayer* pHighlightedPlayer = nullptr;

#define ROW_LEFT 0
#define ROW_RIGHT 1

void OnResolvePitchPressed()
{
	if (pHighlightedPlayer)
	{
		ResolverChk.Checked = true;
		pHighlightedPlayer->Personalize.ResolvePitchMode = (ResolvePitchModes)ResolvePitchDrop.index;
	}
	else
	{
		ClearHighlightedPlayer();
	}
}
char *nopitchstr = new char[20]{ 62, 21, 20, 93, 14, 90, 40, 31, 9, 21, 22, 12, 31, 90, 42, 19, 14, 25, 18, 0 }; /*Don't Resolve Pitch*/
char *automaticpitchstr = new char[16]{ 59, 15, 14, 21, 23, 27, 14, 19, 25, 90, 42, 19, 14, 25, 18, 0 }; /*Automatic Pitch*/
char *pitchupstr = new char[15]{ 60, 21, 8, 25, 31, 90, 42, 19, 14, 25, 18, 90, 47, 10, 0 }; /*Force Pitch Up*/
char *pitchdownstr = new char[17]{ 60, 21, 8, 25, 31, 90, 42, 19, 14, 25, 18, 90, 62, 21, 13, 20, 0 }; /*Force Pitch Down*/
char *custompitchstaticstr = new char[20]{ 57, 15, 9, 14, 21, 23, 90, 42, 19, 14, 25, 18, 90, 41, 14, 27, 14, 19, 25, 0 }; /*Custom Pitch Static*/
char *custompitchadditivestr = new char[22]{ 57, 15, 9, 14, 21, 23, 90, 42, 19, 14, 25, 18, 90, 59, 30, 30, 19, 14, 19, 12, 31, 0 }; /*Custom Pitch Additive*/
char *resolvemodepitchstrings[6] = { nopitchstr, automaticpitchstr, pitchupstr, pitchdownstr, custompitchstaticstr, custompitchadditivestr };
Dropdown ResolvePitchDrop = { Vector2D(420 - OVERLAY_OFFSETX, 60 - OVERLAY_OFFSETY), MENUS::MENU_PLAYERLIST, Centered, &OnResolvePitchPressed, 100, INDEX_ZERO, INDEX_ZERO, 6, &resolvemodepitchstrings, NotPressed, FADE_ZERO, IsNotFaded, IsNotDisabled };

void OnResolveYawPressed()
{
	if (pHighlightedPlayer)
	{
		ResolverChk.Checked = true;
		switch (ResolveYawDrop.index)
		{
			case ResolveYawModes::BloodBodyRealDelta:
			case ResolveYawModes::BloodEyeRealDelta:
			case ResolveYawModes::BloodReal:
				BloodResolverChk.Checked = true;
				break;
		}
		pHighlightedPlayer->Personalize.ResolveYawMode = (ResolveYawModes)ResolveYawDrop.index;
	}
	else
	{
		ClearHighlightedPlayer();
	}
}
char *noyawstr = new char[18]{ 62, 21, 20, 93, 14, 90, 40, 31, 9, 21, 22, 12, 31, 90, 35, 27, 13, 0 }; /*Don't Resolve Yaw*/
char *automaticyawstr = new char[14]{ 59, 15, 14, 21, 23, 27, 14, 19, 25, 90, 35, 27, 13, 0 }; /*Automatic Yaw*/
char *fakespinstr = new char[20]{ 60, 27, 17, 31, 90, 41, 22, 21, 13, 85, 60, 27, 9, 14, 90, 41, 10, 19, 20, 0 }; /*Fake Slow/Fast Spin*/
char *linearfakeyawstr = new char[12]{ 54, 19, 20, 31, 27, 8, 90, 60, 27, 17, 31, 0 }; /*Linear Fake*/
char *randomfakeyawstr = new char[12]{ 40, 27, 20, 30, 21, 23, 90, 60, 27, 17, 31, 0 }; /*Random Fake*/
char *staticantiresolveyawstr = new char[18]{ 59, 20, 14, 19, 87, 40, 31, 9, 21, 22, 12, 31, 90, 60, 27, 17, 31, 0 }; /*Anti-Resolve Fake*/
char *bodyrealdeltastr = new char[16]{ 56, 21, 30, 3, 90, 40, 31, 27, 22, 90, 62, 31, 22, 14, 27, 0 }; /*Body Real Delta*/
char *eyerealdeltastr = new char[15]{ 63, 3, 31, 90, 40, 31, 27, 22, 90, 62, 31, 22, 14, 27, 0 }; /*Eye Real Delta*/
char *lastrealyawstr = new char[14]{ 54, 27, 9, 14, 90, 40, 31, 27, 22, 90, 35, 27, 13, 0 }; /*Last Real Yaw*/
char *attargetstr1 = new char[10]{ 59, 14, 90, 46, 27, 8, 29, 31, 14, 0 }; /*At Target*/
char *inverseattargetstr1 = new char[18]{ 51, 20, 12, 31, 8, 9, 31, 90, 59, 14, 90, 46, 27, 8, 29, 31, 14, 0 }; /*Inverse At Target*/
char *avglowerbodydeltastr = new char[21]{ 59, 12, 29, 90, 54, 21, 13, 31, 8, 90, 56, 21, 30, 3, 90, 62, 31, 22, 14, 27, 0 }; /*Avg Lower Body Delta*/
char *staticdynamicfakestr = new char[20]{ 41, 14, 27, 14, 19, 25, 90, 60, 27, 17, 31, 90, 62, 3, 20, 27, 23, 19, 25, 0 }; /*Static Fake Dynamic*/
char *fakewalkstr2 = new char[10]{ 60, 27, 17, 31, 90, 45, 27, 22, 17, 0 }; /*Fake Walk*/
char *staticfakeyawstr = new char[12]{ 41, 14, 27, 14, 19, 25, 90, 60, 27, 17, 31, 0 }; /*Static Fake*/
char *inverseyawstr = new char[8]{ 51, 20, 12, 31, 8, 9, 31, 0 }; /*Inverse*/
char *yawforceback = new char[11]{ 60, 21, 8, 25, 31, 90, 56, 27, 25, 17, 0 }; /*Force Back*/
char *yawleftstr = new char[11]{ 60, 21, 8, 25, 31, 90, 54, 31, 28, 14, 0 }; /*Force Left*/
char *yawrightstr = new char[12]{ 60, 21, 8, 25, 31, 90, 40, 19, 29, 18, 14, 0 }; /*Force Right*/
char *customyawstaticstr = new char[18]{ 57, 15, 9, 14, 21, 23, 90, 35, 27, 13, 90, 41, 14, 27, 14, 19, 25, 0 }; /*Custom Yaw Static*/
char *customyawadditivestr = new char[20]{ 57, 15, 9, 14, 21, 23, 90, 35, 27, 13, 90, 59, 30, 30, 19, 14, 19, 12, 31, 0 }; /*Custom Yaw Additive*/
char *resolvemodeyawstrings[21] = { noyawstr, automaticyawstr, fakespinstr, linearfakeyawstr, randomfakeyawstr, staticantiresolveyawstr, bodyrealdeltastr, eyerealdeltastr, lastrealyawstr, attargetstr1, inverseattargetstr1, avglowerbodydeltastr, staticdynamicfakestr, fakewalkstr2, staticfakeyawstr, inverseyawstr, yawforceback, yawleftstr, yawrightstr, customyawstaticstr, customyawadditivestr };
Dropdown ResolveYawDrop = { Vector2D(420 - OVERLAY_OFFSETX, 90 - OVERLAY_OFFSETY), MENUS::MENU_PLAYERLIST, Centered, &OnResolveYawPressed, 100, INDEX_ZERO, INDEX_ZERO, 21, &resolvemodeyawstrings, NotPressed, FADE_ZERO, IsNotFaded, IsNotDisabled };


//Enables all automatic resolves for a specific client
void AutoResolvePlayer(CustomPlayer *pCAPlayer)
{
	pCAPlayer->Personalize.ResolvePitchMode = ResolvePitchModes::AutomaticPitch;
	pCAPlayer->Personalize.ResolveYawMode = ResolveYawModes::AutomaticYaw;
	ResolverChk.Checked = true;
}

void UpdateResolverValues()
{
	ResolvePitchDrop.index = pHighlightedPlayer->Personalize.ResolvePitchMode;
	ResolveYawDrop.index = pHighlightedPlayer->Personalize.ResolveYawMode;
	DontTargetHeadNeckChk.Checked = pHighlightedPlayer->Personalize.DontShootAtHeadNeck;
	ResolvePitchTxt.flValue = pHighlightedPlayer->Personalize.flPitch;
	sprintf(ResolvePitchTxt.Value, "%f", pHighlightedPlayer->Personalize.flPitch);
	ResolveYawTxt.flValue = pHighlightedPlayer->Personalize.flYaw;
	sprintf(ResolveYawTxt.Value, "%f", pHighlightedPlayer->Personalize.flYaw);
	PriorityTargetChk.Checked = pHighlightedPlayer->Personalize.PriorityTarget;
	IgnoreAutoResolveChk.Checked = pHighlightedPlayer->Personalize.IgnoreAutoResolve;
}

void ClearHighlightedPlayer()
{
	pHighlightedPlayer = nullptr;
	ResolvePitchDrop.index = ResolvePitchModes::NoPitch;
	ResolveYawDrop.index = ResolveYawModes::NoYaw;
	PriorityTargetChk.Checked = false;
	IgnoreAutoResolveChk.Checked = false;
	DontTargetHeadNeckChk.Checked = false;
}

char *firewhenshootingstr = new char[24]{ 60, 19, 8, 31, 90, 45, 18, 31, 20, 90, 63, 20, 31, 23, 19, 31, 9, 90, 41, 18, 21, 21, 14, 0 }; /*Fire When Enemies Shoot*/
Checkbox FireWhenEnemiesShootChk = { Vector2D(105 - OVERLAY_OFFSETX, -15 - OVERLAY_OFFSETY), MENU_PLAYERLIST, Centered, nullptr, CheckedOff, firewhenshootingstr, NotPressed, IsNotFaded, IsNotDisabled };

char *autopelvisstr = new char[10]{ 59, 15, 14, 21, 90, 56, 59, 19, 23, 0 }; /*Auto BAim*/
Checkbox AutoPelvisChk = { Vector2D(238 - OVERLAY_OFFSETX, -15 - OVERLAY_OFFSETY), MENU_PLAYERLIST, Centered, nullptr, CheckedOff, autopelvisstr, NotPressed, IsNotFaded, IsNotDisabled };

char *autoresolveallstr = new char[17]{ 59, 15, 14, 21, 90, 40, 31, 9, 21, 22, 12, 31, 90, 59, 22, 22, 0 }; /*Auto Resolve All*/
Checkbox AutoResolveAllChk = { Vector2D(315 - OVERLAY_OFFSETX, -15 - OVERLAY_OFFSETY), MENU_PLAYERLIST, Centered, nullptr, CheckedOff, autoresolveallstr, NotPressed, IsNotFaded, IsNotDisabled };

char *valveresolvestr = new char[14]{ 41, 10, 8, 31, 27, 30, 90, 41, 31, 8, 12, 31, 8, 0 }; /*Spread Server*/
Checkbox ValveResolverChk = { Vector2D(420 - OVERLAY_OFFSETX, -15 - OVERLAY_OFFSETY), MENU_PLAYERLIST, Centered, nullptr, CheckedOn, valveresolvestr, NotPressed, IsNotFaded, IsNotDisabled };


void OnResolvePitch()
{
	if (pHighlightedPlayer)
	{
		pHighlightedPlayer->Personalize.flPitch = ResolvePitchTxt.flValue;
	}
}
char *custompitchstr = new char[13]{ 57, 15, 9, 14, 21, 23, 90, 42, 19, 14, 25, 18, 0 }; /*Custom Pitch*/
Textbox ResolvePitchTxt = { Vector2D(420 - OVERLAY_OFFSETX, 60 + (20 * 3) + 10 - OVERLAY_OFFSETY), Vector2D(475 - OVERLAY_OFFSETX, 60 + (20 * 3) + 5 - OVERLAY_OFFSETY), Vector2D(475 - OVERLAY_OFFSETX, 60 + (20 * 3) + 20 - OVERLAY_OFFSETY), MENU_PLAYERLIST, Centered, &OnResolvePitch, 50, TYPES::Float, custompitchstr, -89.0, 89.0, "", 0.0f, 0.0, 0, 0, IsNotFaded, IsNotDisabled };

void OnResolveYaw()
{
	if (pHighlightedPlayer)
	{
		pHighlightedPlayer->Personalize.flYaw = ResolveYawTxt.flValue;
	}
}
char *customyawstr = new char[11]{ 57, 15, 9, 14, 21, 23, 90, 35, 27, 13, 0 }; /*Custom Yaw*/
Textbox ResolveYawTxt = { Vector2D(420 - OVERLAY_OFFSETX, 60 + (20 * 4) + 10 + 10 - OVERLAY_OFFSETY), Vector2D(475 - OVERLAY_OFFSETX, 60 + (20 * 4) + 5 + 10 - OVERLAY_OFFSETY), Vector2D(475 - OVERLAY_OFFSETX, 60 + (20 * 4) + 20 + 10 - OVERLAY_OFFSETY), MENU_PLAYERLIST, Centered, &OnResolveYaw, 50, TYPES::Float, customyawstr, -360.0, 360.0, "", 0.0f, 0.0, 0, 0, IsNotFaded, IsNotDisabled };

void OnDontTargetHeadNeckClicked()
{
	if (pHighlightedPlayer)
	{
		pHighlightedPlayer->Personalize.DontShootAtHeadNeck = DontTargetHeadNeckChk.Checked;
	}
	else
	{
		DontTargetHeadNeckChk.Checked = false;
	}
}
char *donttargetheadneckstr = new char[23]{ 62, 21, 20, 93, 14, 90, 46, 27, 8, 29, 31, 14, 90, 50, 31, 27, 30, 85, 52, 31, 25, 17, 0 }; /*Don't Target Head/Neck*/
Checkbox DontTargetHeadNeckChk = { Vector2D(420 - OVERLAY_OFFSETX, 60 + ((CHECKBOX_WIDTH * 2) * 7) - OVERLAY_OFFSETY), MENU_PLAYERLIST, Centered, &OnDontTargetHeadNeckClicked, CheckedOff, donttargetheadneckstr, NotPressed, IsNotFaded, IsNotDisabled };

void OnIgnoreClicked()
{
	if (pHighlightedPlayer)
	{
		pHighlightedPlayer->Personalize.IgnoreAutoResolve = IgnoreAutoResolveChk.Checked;
		if (IgnoreAutoResolveChk.Checked)
		{
			pHighlightedPlayer->Personalize.ResolveYawMode = ResolveYawModes::NoYaw;
			pHighlightedPlayer->Personalize.ResolvePitchMode = ResolvePitchModes::NoPitch;
		}
	}
	else
	{
		IgnoreAutoResolveChk.Checked = false;
	}
}
char *donotresolvestr = new char[15]{ 62, 21, 90, 52, 21, 14, 90, 40, 31, 9, 21, 22, 12, 31, 0 }; /*Do Not Resolve*/
Checkbox IgnoreAutoResolveChk = { Vector2D(420 - OVERLAY_OFFSETX, 60 + ((CHECKBOX_WIDTH * 2) * 8) - OVERLAY_OFFSETY), MENU_PLAYERLIST, Centered, &OnIgnoreClicked, CheckedOff, donotresolvestr, NotPressed, IsNotFaded, IsNotDisabled };

void OnPriorityClicked()
{
	if (pHighlightedPlayer)
	{
		pHighlightedPlayer->Personalize.PriorityTarget = PriorityTargetChk.Checked;
	}
	else
	{
		PriorityTargetChk.Checked = false;
	}
}
char *prioritytargetstr = new char[16]{ 42, 8, 19, 21, 8, 19, 14, 3, 90, 46, 27, 8, 29, 31, 14, 0 }; /*Priority Target*/
Checkbox PriorityTargetChk = { Vector2D(420 - OVERLAY_OFFSETX, 60 + ((CHECKBOX_WIDTH * 2) * 9) - OVERLAY_OFFSETY), MENU_PLAYERLIST, Centered, &OnPriorityClicked, CheckedOff, prioritytargetstr, NotPressed, IsNotFaded, IsNotDisabled };

char *playersspectatingyoustr = new char[24]{ 42, 22, 27, 3, 31, 8, 9, 90, 41, 10, 31, 25, 14, 27, 14, 19, 20, 29, 90, 35, 21, 15, 64, 0 }; /*Players Spectating You:*/
char *defusingstr = new char[10]{ 62, 63, 60, 47, 41, 51, 52, 61, 91, 0 }; /*DEFUSING!*/
float DefuseTimeRemaining = 0.0f;
char *realstraa = new char[6]{ 40, 31, 27, 22, 90, 0 }; /*Real */
char *lbystraa= new char[5]{ 54, 56, 35, 90, 0 }; /*LBY */
char *fakestr = new char[6]{ 60, 27, 17, 31, 90, 0 }; /*Fake */
char *airstuckenabledstr = new char[18]{ 59, 19, 8, 90, 41, 14, 15, 25, 17, 90, 63, 20, 27, 24, 22, 31, 30, 0 }; /*Air Stuck Enabled*/

//Strings not drawing correctly....
char *lcstr = new char[12]{ 56, 8, 31, 27, 17, 19, 20, 29, 90, 54, 57, 0 }; /*Breaking LC*/
char *lbystr = new char[13]{ 56, 8, 31, 27, 17, 19, 20, 29, 90, 54, 56, 35, 0 }; /*Breaking LBY*/
extern double difference(double first, double second);

//When hooking directx, this runs all visuals
void RunVisualHacks_DXHook()
{
#ifdef PRINT_FUNCTION_NAMES
	AllocateConsole();
	printf("RunVisualHacks_DXHook\n");
#endif
	if (DrawHitboxesChk.Checked || DrawBloodResolveChk.Checked || AimESPChk.Checked)
	{
		Interfaces::DebugOverlay->ClearDeadOverlays();
	}

	CBaseEntity* LocalEntity = Interfaces::ClientEntList->GetClientEntity(Interfaces::Engine->GetLocalPlayer());

	if (LocalEntity == nullptr)
		return;

#ifdef _DEBUG
	CBaseCombatWeapon *pWeapon = LocalPlayer.Entity->GetWeapon();
	if (pWeapon)
	{
		//char tst[42];
		//sprintf(tst, "Your Weapon Spread: %f", pWeapon->GetWeaponSpread());
		//DrawStringUnencrypted(tst, Vector2D(480, 480), ColorRGBA(255, 255, 255, 255), pFont);
	}
#endif

#ifdef PROFILE
	//pSprite->Begin(D3DXSPRITE_ALPHABLEND | D3DXSPRITE_DO_NOT_ADDREF_TEXTURE);
	DRAW_PROFILED_FUNCTIONS();
	//pSprite->End();
#endif

	TEAMS LocalTeam = (TEAMS)LocalEntity->GetTeam();

	int NumSpectators = 0;
	
	Vector LocalPlayerOrigin = LocalEntity->GetOrigin();

	//pSprite->Begin(D3DXSPRITE_ALPHABLEND | D3DXSPRITE_DO_NOT_ADDREF_TEXTURE);

	int MaxEntities = 64;// Interfaces::ClientEntList->GetHighestEntityIndex();
	int NumPlayers = GetClientCount();
	int CountedPlayers = 0;

	if (NumPlayers)
	{
		for (int i = 0; i <= MaxEntities; i++)
		{
			if (CountedPlayers == NumPlayers)
				break;

			CBaseEntity* pBaseEntity = Interfaces::ClientEntList->GetClientEntity(i);

			if (pBaseEntity && pBaseEntity->IsPlayer())
			{
				CountedPlayers++;

				CustomPlayer *pCAPlayer = &AllPlayers[i];

				if (pBaseEntity == LocalEntity || pBaseEntity->GetTeam() == TEAM_NONE || pBaseEntity->GetDormant() || !pCAPlayer->BaseEntity || pBaseEntity->GetDormant())
					continue;

				bool GotName = false;

				//DRAW SPECTATORS
				if (DrawSpectatorsChk.Checked)
				{
					HANDLE hObserverTarget = pBaseEntity->GetObserverTarget();// &0xFFF;
					if (hObserverTarget)
					{
						CBaseEntity *pSpectating = Interfaces::ClientEntList->GetClientEntityFromHandle((DWORD)hObserverTarget);
						if (pSpectating == LocalEntity)
						{
							if (NumSpectators == 0)
							{
								char tempstr[64];
								DecStr(playersspectatingyoustr, 23);
								memcpy(tempstr, playersspectatingyoustr, 24);
								EncStr(playersspectatingyoustr, 23);
								DrawStringUnencrypted(tempstr, Vector2D(320, 1 + (20 * NumSpectators)), ColorRGBA(255, 0, 0, 255), pFont);
							}
							wchar_t PlayerName[32];
							DWORD Radar = GetRadar();
							if (Radar)
								GetPlayerName(Radar, i, PlayerName);
							else
								wcscpy(PlayerName, L"Error");

							DrawStringUnencryptedW(PlayerName, Vector2D(320, 1 + (20 * ++NumSpectators)), ColorRGBA(255, 255, 255, 255), pFont);
							GotName = true;
						}
					}
				}

				if (!pCAPlayer->IsSpectating)
				{

					//MINIMAP SPOTTING

					bool IsTarget = MTargetting.IsPlayerAValidTarget(pBaseEntity);

					if (MiniMapSpottedChk.Checked && IsTarget)
					{
						SpotPlayer(pBaseEntity);
					}

					bool IsLockedOn = LastTargetIndex != INVALID_PLAYER && AllPlayers[LastTargetIndex].Index == ReadInt((uintptr_t)&pBaseEntity->index);

#ifdef _DEBUG
					//char txt[64];
					//sprintf(txt, "Sim Time: %.3f\nTicks Choked: %i", Interfaces::Globals->curtime - pCAPlayer->BaseEntity->GetSimulationTime(), pCAPlayer->tickschoked);
					//DrawStringUnencrypted(txt, Vector2D(640, 480), ColorRGBA(255, 0, 0, 255), pFont);
#endif

			//GLOW ESP
					if (IsTarget)
					{
						if (GlowChk.Checked)
						{
							DWORD GlowIndex = pBaseEntity->GetGlowIndex();
							if (GlowIndex != -1)
							{
								DrawGlowESP((TEAMS)pBaseEntity->GetTeam(), GlowIndex, IsLockedOn);
							}
						}
					}

					if (pBaseEntity->IsDefusing())
					{
						DecStr(defusingstr, 9);
						DrawStringUnencrypted(defusingstr, Vector2D(CenterOfScreen.x - 40, CenterOfScreen.y + 30), (DefuseTimeRemaining > 10.0f || (pBaseEntity->HasDefuseKit() && DefuseTimeRemaining > 5.0f)) ? greencolor : redcolor, pFont);
						EncStr(defusingstr, 9);
					}


					//ESP DRAWING
					if ((ESPChk.Checked || BoxESPChk.Checked || FootstepESPChk.Checked || DrawHitboxesChk.Checked || DrawResolveModeChk.Checked) && IsTarget && pBaseEntity->GetAlive())
					{
						DrawESP(LocalPlayer.Entity, pBaseEntity, pCAPlayer, IsLockedOn);
					}
				}
			}
		}
		if (DrawResolveModeChk.Checked) 
		{
			int DrawSpot = CenterOfScreen.y * 2;//Couldnt find screen height but could find this :)
			DrawSpot *= 0.5;

			//You probably store this somewhere can't find it so here it goes.
			Vector LocalPlayerNetwork = LocalEntity->GetNetworkOrigin();
			static Vector LastLocalPlayerNetwork = LocalPlayerNetwork;
			static bool breakingLagComp = false;
			if (CurrentUserCmd.bSendPacket)
			{
				if ((LocalPlayerNetwork - LastLocalPlayerNetwork).LengthSqr() > (64.0f * 64.0f))
					breakingLagComp = true;
				else
					breakingLagComp = false;

				LastLocalPlayerNetwork = LocalPlayerNetwork;
			}
			if (breakingLagComp)
			{
				DecStr(lcstr, 11);
				DrawStringUnencrypted(lcstr, Vector2D(10, DrawSpot), ColorRGBA(255, 0, 0, 255), pFont);
				EncStr(lcstr, 11);
			}
				
				//^ bandaid fix beceuase I like bandaids
			//This doesn't work correctly
			// should include backtrack ticks
			double lbyYaw = LocalEntity->GetLowerBodyYaw();
			QAngle realAngles = LocalPlayer.LastRealEyeAngles;
			float differenceInTimes = Interfaces::Globals->curtime - TICKS_TO_TIME(LocalPlayer.LowerBodyYawUpdateTick);
			if (abs(difference(realAngles.y, lbyYaw)) > 10 && (differenceInTimes > 0.01f))
			{
				//Should add the whole if valid tick if can be backtrack too etc.
				DecStr(lbystr, 12);
				ColorRGBA colour = ColorRGBA(0, 255, 0, 255);
				if ((differenceInTimes) > 0.2f)
					colour = ColorRGBA(255, 0, 0, 255);

				DrawStringUnencrypted(lbystr, Vector2D(10, DrawSpot + 30), colour, pFont);
				EncStr(lbystr, 12);

			}
			
			char str[30];
			DecStr(realstraa, 5);
			DecStr(lbystraa, 4);
			DecStr(fakestr, 5);

			DrawStringUnencrypted(realstraa, Vector2D(10, DrawSpot + 30*2), ColorRGBA(0, 255, 0, 255), pFont);
			sprintf(str,"%d", (int)LocalPlayer.LastRealEyeAngles.y);
			DrawStringUnencrypted(str, Vector2D(70, DrawSpot + 30 * 2), ColorRGBA(0, 255, 0, 255), pFont);

			DrawStringUnencrypted(lbystraa, Vector2D(10, DrawSpot + 30 * 3), ColorRGBA(0, 255, 0, 255), pFont);
			sprintf(str, "%d", (int)LocalPlayer.LowerBodyYaw);
			DrawStringUnencrypted(str, Vector2D(70, DrawSpot + 30 * 3), ColorRGBA(0, 255, 0, 255), pFont);

			DrawStringUnencrypted(fakestr, Vector2D(10, DrawSpot + 30 * 4), ColorRGBA(0, 255, 0, 255), pFont);
			sprintf(str, "%d", (int)LocalPlayer.LastFakeEyeAngles.y);
			DrawStringUnencrypted(str, Vector2D(70, DrawSpot + 30 * 4), ColorRGBA(0, 255, 0, 255), pFont);

			EncStr(realstraa, 5);
			EncStr(lbystraa, 4);
			EncStr(fakestr, 5);

			if (LocalPlayer.airstuck)
			{
				DecStr(airstuckenabledstr, 17);
				DrawStringUnencrypted(airstuckenabledstr, Vector2D(10, DrawSpot + 30 * 5), ColorRGBA(255, 255, 0, 255), pFont);
				EncStr(airstuckenabledstr, 17);
			}
		}

		if (RunningFrame)
		{
			if (AutoGriefChk.Checked && LastGriefingTargetID != INVALID_PLAYER)
			{
				Vector screen;
				if (WorldToScreenCapped(GriefChestPos, screen))
				{
					DecStr(grieftargetstr, 12);
					DrawStringUnencrypted(grieftargetstr, Vector2D(screen.x, screen.y), ColorRGBA(255, 127, 29, 255), pFont);
					EncStr(grieftargetstr, 12);
				}
			}

			if (NoScopeChk.Checked)
			{
				if (GetZoomLevel(LocalPlayer.Entity))
				{
					int width, height;
					Interfaces::Engine->GetScreenSize(width, height);
					int halfwidth = width / 2;
					int halfheight = height / 2;
					DrawLine(Vector2D(0, halfheight), Vector2D(width, halfheight), 0, 0, 0, 255);
					DrawLine(Vector2D(halfwidth, 0), Vector2D(halfwidth, height), 0, 0, 0, 255);
				}
			}

			if (EntityESPChk.Checked)
			{
				DrawEntityESP(LocalPlayer.Entity);
			}

			if (DrawRecoilCrosshairChk.Checked && LocalEntity->GetAlive())
			{
				//SET_FRAME_TIME(false, (1.0f / 256.0f));
				DrawLine(Vector2D(RecoilCrosshairScreen.x - 5, RecoilCrosshairScreen.y), Vector2D(RecoilCrosshairScreen.x + 5, RecoilCrosshairScreen.y), 255, 0, 0, 230);
				DrawLine(Vector2D(RecoilCrosshairScreen.x, RecoilCrosshairScreen.y - 5), Vector2D(RecoilCrosshairScreen.x, RecoilCrosshairScreen.y + 5), 255, 0, 0, 230);
			}
			if (DrawDamageChk.Checked)
			{
				DrawDamageIndicator();
			}
#if 0
			bNewFontCanDraw = false;
			bNewFontSmallCanDraw = false;
			pNewFontSmall->EndDrawing();
			pNewFont->EndDrawing();
#endif
		}
	}

	//pSprite->End();
#ifdef PRINT_FUNCTION_NAMES
	printf("Finished Visuals DX Hook\n");
#endif
}

//Fills the AllPlayers[i] struct with data
void FillCustomPlayerStruct(CustomPlayer* const pCAPlayer, CBaseEntity*const pBaseEntity, int i)
{
#ifdef PRINT_FUNCTION_NAMES
	AllocateConsole();
	printf("FillCustomPlayerStruct\n");
#endif
	if (pBaseEntity && pBaseEntity->IsPlayer())
	{
		//pCAPlayer->Team = (TEAMS)pBaseEntity->GetTeam();
		TEAMS Team = (TEAMS)pBaseEntity->GetTeam();

		if (pBaseEntity == LocalPlayer.Entity)
		{
			pCAPlayer->BaseEntity = pBaseEntity;
			pCAPlayer->IsLocalPlayer = true;
			pCAPlayer->Active = true;
			pCAPlayer->Connected = true;
			pCAPlayer->Dormant = true;
			pCAPlayer->IsTarget = false;
			pCAPlayer->IsSpectating = false;
			pCAPlayer->Index = i;
			StreamedPlayers[i] = nullptr;
			return;
		}

		if (Team == TEAM_NONE || Team == TEAM_GOTV)
		{
			pCAPlayer->BaseEntity = pBaseEntity;
			pCAPlayer->IsLocalPlayer = false;
			pCAPlayer->Active = false;
			pCAPlayer->Connected = true;
			pCAPlayer->Dormant = true;
			pCAPlayer->IsTarget = false;
			pCAPlayer->IsSpectating = true;
			pCAPlayer->Index = i;
			StreamedPlayers[i] = nullptr;
			return;
		}

		//If player slot changed, then delete the old personalization data
		player_info_t info;
		pBaseEntity->GetPlayerInfo(&info);
		DecStr(pCAPlayer->steamid, 32);
		if (strncmp(pCAPlayer->steamid, info.guid, 33))
		{
			//A completely different player joined the server using this slot, clear old persistent data
#if 0
			if (pCAPlayer->bHookedBaseEntity)
			{
				DeleteEntityHook(pCAPlayer->HookedBaseEntity->hook);
				delete pCAPlayer->HookedBaseEntity;
				pCAPlayer->HookedBaseEntity = nullptr;
				pCAPlayer->bHookedBaseEntity = false;
			}
#endif
			Personalize* pPersonalize = &pCAPlayer->Personalize;
			memset(&pCAPlayer->Personalize, 0, (DWORD)&pPersonalize->m_PlayerLowerBodyRecords - (DWORD)pPersonalize); //hard coded for now..
			memcpy(pCAPlayer->steamid, info.guid, 33);
			gLagCompensation.ClearLagCompensationRecords(pCAPlayer);
			gLagCompensation.ClearBloodRecords(pCAPlayer);
		}
		EncStr(pCAPlayer->steamid, 32);

#if 0
		if (!pCAPlayer->bHookedBaseEntity)
		{
			pCAPlayer->HookedBaseEntity = new HookedEntity;
			pCAPlayer->HookedBaseEntity->entity = pBaseEntity;
			pCAPlayer->HookedBaseEntity->index = pBaseEntity->index;
			pCAPlayer->HookedBaseEntity->hook = new VTHook((PDWORD*)pBaseEntity);
			pCAPlayer->HookedBaseEntity->OriginalHookedSub1 = pCAPlayer->HookedBaseEntity->hook->HookFunction((DWORD)&HookedEntityShouldInterpolate, ShouldInterpolateIndex);
			pCAPlayer->bHookedBaseEntity = true;
			HookedEntities.push_back(pCAPlayer->HookedBaseEntity);
		}
#endif

		pCAPlayer->Index = i;
		pCAPlayer->BaseEntity = pBaseEntity;
		pCAPlayer->Connected = true;
		pCAPlayer->Active = true;
		pCAPlayer->IsLocalPlayer = false;
		pCAPlayer->Dormant = pBaseEntity->GetDormant();

		HANDLE hObserverTarget = pBaseEntity->GetObserverTarget();
		if (hObserverTarget)
		{
			CBaseEntity *pSpectating = Interfaces::ClientEntList->GetClientEntityFromHandle((DWORD)hObserverTarget);
			pCAPlayer->IsSpectating = pSpectating && pSpectating != pBaseEntity;
		}
		else
			pCAPlayer->IsSpectating = false;

		if (pCAPlayer->Dormant)
		{
			//Player is connected, but not streamed
			StreamedPlayers[i] = nullptr;
			pCAPlayer->IsTarget = false;
			//pCAPlayer->tickschoked = 0;
			return;
		}

		//Player is streamed
		if (AutoResolveAllChk.Checked)
		{
			if (!pCAPlayer->Personalize.IgnoreAutoResolve && pCAPlayer->Personalize.ResolvePitchMode == NoPitch && pCAPlayer->Personalize.ResolveYawMode == NoYaw)
			{
				AutoResolvePlayer(pCAPlayer);
			}
		}

		pCAPlayer->LifeState = GetPlayerLifeState(pBaseEntity);
		//pCAPlayer->BoneMatrix = GetBoneMatrix(pBaseEntity);
		pCAPlayer->IsTarget = MTargetting.IsPlayerAValidTarget(pCAPlayer);
		if (pCAPlayer->IsTarget)
		{
			NumStreamedEnemies++;
			if (!LocalPlayer.EnemyIsWithinKnifeRange)
			{
				if ((LocalPlayer.Entity->GetOrigin() - pBaseEntity->GetOrigin()).Length() < 100.0f)
				{
					LocalPlayer.EnemyIsWithinKnifeRange = true;
				}
			}
		}
		StreamedPlayers[NumStreamedPlayers++] = pCAPlayer;
	}
	else
	{
		if (pHighlightedPlayer != nullptr && pHighlightedPlayer->Index == i)
		{
			ClearHighlightedPlayer();
		}

		//Invalid player, clear data

		//Temporarily store per-player personalization info in case entity cache was flushed
		Personalize TemporaryPersonalizeBuffer;

		if (pCAPlayer->BaseEntity != nullptr)
		{
#if 0
			if (pCAPlayer->bHookedBaseEntity)
			{
				DeleteEntityHook(pCAPlayer->HookedBaseEntity->hook);
				delete pCAPlayer->HookedBaseEntity;
				pCAPlayer->HookedBaseEntity = nullptr;
				pCAPlayer->bHookedBaseEntity = false;
			}
#endif
			TemporaryPersonalizeBuffer = pCAPlayer->Personalize;
			char steamid[33];
			memcpy(steamid, pCAPlayer->steamid, 33);
			memset(pCAPlayer, 0, (DWORD)&pCAPlayer->Personalize.m_PlayerLowerBodyRecords - (DWORD)pCAPlayer); //hard coded for now..
			pCAPlayer->Personalize = TemporaryPersonalizeBuffer;
			memcpy(pCAPlayer->steamid, steamid, 33);
		}
		StreamedPlayers[i] = nullptr;
	}
}

//Runs visual data when not using the directx hook
void RunPerPlayerHacks()
{
	if (!LocalPlayer.Entity)
		return;

	//pSprite->Begin(D3DXSPRITE_ALPHABLEND | D3DXSPRITE_DO_NOT_ADDREF_TEXTURE);

#if 0
#ifdef _DEBUG
	if (EXTERNAL_WINDOW)
	{
		char test[256];
		sprintf(test, "Your LBY: %f\nYourBaseEnt: %02X\nYour Real Yaw: %f\nYour Fake Yaw: %f", LocalPlayer.Entity->GetLowerBodyYaw(), (DWORD)LocalPlayer.Entity, LocalPlayer.LastRealEyeAngles.y, LocalPlayer.LastFakeEyeAngles.y);
		DrawStringUnencrypted(test, Vector2D(CenterOfScreen.x, CenterOfScreen.y + 15), ColorRGBA(255, 255, 255, 255), pFont);
	}
#endif
#endif

#ifdef PROFILE
	if (EXTERNAL_WINDOW)
	{
		//char test[64];
		//sprintf(test, "Your LBY: %f\nYourBaseEnt: %02X", LocalPlayer.Entity->GetLowerBodyYaw(), (DWORD)LocalPlayer.Entity);
		//DrawStringUnencrypted(test, Vector2D(CenterOfScreen.x, CenterOfScreen.y + 15), ColorRGBA(255, 255, 255, 255), pFont);
		//pSprite->Begin(D3DXSPRITE_ALPHABLEND | D3DXSPRITE_DO_NOT_ADDREF_TEXTURE);
		DRAW_PROFILED_FUNCTIONS();
		//pSprite->End();
	}
#endif


	int NumSpectators = 0;

	if (DrawHitboxesChk.Checked || DrawBloodResolveChk.Checked || AimESPChk.Checked)
	{
		Interfaces::DebugOverlay->ClearDeadOverlays();
	}

	int MaxEntities = 64;// Interfaces::ClientEntList->GetHighestEntityIndex();
	int NumPlayers = GetClientCount();
	int CountedPlayers = 0;

	if (NumPlayers)
	{
		for (int i = 0; i <= MaxEntities; i++)
		{
			if (CountedPlayers == NumPlayers)
				break;

			CBaseEntity* pBaseEntity = Interfaces::ClientEntList->GetClientEntity(i);

			if (pBaseEntity)
			{
				CustomPlayer* pCAPlayer = &AllPlayers[i];
				BOOLEAN IsPlayer = pBaseEntity->IsPlayer();

				if (IsPlayer)
					CountedPlayers++;

				//Multithreaded
				if (pBaseEntity == LocalPlayer.Entity || !IsPlayer || pBaseEntity->GetTeam() == TEAM_NONE || pBaseEntity->GetDormant() || !pCAPlayer->BaseEntity || pCAPlayer->Dormant)
					continue;

				bool GotName = false;

				//DRAW SPECTATORS
				if (DrawSpectatorsChk.Checked)
				{
					HANDLE hObserverTarget = pBaseEntity->GetObserverTarget();// &0xFFF;
					if (hObserverTarget)
					{
						CBaseEntity *pSpectating = Interfaces::ClientEntList->GetClientEntityFromHandle((DWORD)hObserverTarget);
						if (pSpectating == LocalPlayer.Entity)
						{
							//SET_FRAME_TIME(false, (1.0f / 256.0f));
							if (NumSpectators == 0)
							{
								char tempstr[64];
								DecStr(playersspectatingyoustr, 23);
								memcpy(tempstr, playersspectatingyoustr, 24);
								EncStr(playersspectatingyoustr, 23);
								DrawStringUnencrypted(tempstr, Vector2D(320, 1 + (20 * NumSpectators)), ColorRGBA(255, 0, 0, 255), pFont);
							}
							wchar_t PlayerName[32];
							DWORD Radar = GetRadar();
							if (Radar)
								GetPlayerName(Radar, i, PlayerName);
							else
								wcscpy(PlayerName, L"Error");

							DrawStringUnencryptedW(PlayerName, Vector2D(320, 1 + (20 * ++NumSpectators)), ColorRGBA(255, 255, 255, 255), pFont);
							GotName = true;
						}
					}
				}

				if (!pCAPlayer->IsSpectating)
				{

					//MINIMAP SPOTTING
					if (MiniMapSpottedChk.Checked && pCAPlayer->IsTarget && pCAPlayer->LifeState == LIFE_ALIVE)
					{
						SpotPlayer(pBaseEntity);
					}

					bool IsLockedOn = LastTargetIndex != INVALID_PLAYER && AllPlayers[LastTargetIndex].Index == pCAPlayer->Index;

#ifdef _DEBUG
					//char txt[64];
					//sprintf(txt, "Sim Time: %.3f\nTicks Choked: %i", Interfaces::Globals->curtime - pCAPlayer->BaseEntity->GetSimulationTime(), pCAPlayer->tickschoked);
					//DrawStringUnencrypted(txt, Vector2D(640, 480), ColorRGBA(255, 0, 0, 255), pFont);
#endif

			//GLOW ESP
					if (pCAPlayer->IsTarget && pCAPlayer->LifeState == LIFE_ALIVE)
					{
						DWORD GlowIndex;
						if (GlowChk.Checked)
						{
							GlowIndex = pBaseEntity->GetGlowIndex();
							if (GlowIndex != -1)
							{
								DrawGlowESP((TEAMS)pBaseEntity->GetTeam(), GlowIndex, IsLockedOn);
							}
						}
					}

					if (pBaseEntity->IsDefusing())
					{
						DecStr(defusingstr, 9);
						DrawStringUnencrypted(defusingstr, Vector2D(CenterOfScreen.x - 40, CenterOfScreen.y + 30), (DefuseTimeRemaining > 10.0f || (pBaseEntity->HasDefuseKit() && DefuseTimeRemaining > 5.0f)) ? greencolor : redcolor, pFont);
						EncStr(defusingstr, 9);
					}

					//ESP DRAWING
					if ((ESPChk.Checked || BoxESPChk.Checked || DrawHitboxesChk.Checked || DrawResolveModeChk.Checked) && pCAPlayer->IsTarget && pCAPlayer->LifeState == LIFE_ALIVE /*&& Time >= pCAPlayer->NextTimeDrawESP*/)
					{
						//SET_FRAME_TIME(false, (1.0f / 256.0f));
						//pCAPlayer->Health = GetPlayerHealth(pBaseEntity);
						//ESP
						DrawESP(LocalPlayer.Entity, pBaseEntity, pCAPlayer, IsLockedOn);
						//pCAPlayer->NextTimeDrawESP = Time + (0.05f) - fmax(0.0f, Time - pCAPlayer->NextTimeDrawESP);
					}
				}
			}
		}


		if (RunningFrame)
		{
			if (AutoGriefChk.Checked && LastGriefingTargetID != INVALID_PLAYER)
			{
				Vector screen;
				if (WorldToScreenCapped(GriefChestPos, screen))
				{
					DecStr(grieftargetstr, 12);
					DrawStringUnencrypted(grieftargetstr, Vector2D(screen.x, screen.y), ColorRGBA(255, 127, 29, 255), pFont);
					EncStr(grieftargetstr, 12);
				}
			}

			if (EntityESPChk.Checked)
			{
				DrawEntityESP(LocalPlayer.Entity);
			}

			if (DrawRecoilCrosshairChk.Checked && LocalPlayer.Entity->GetAlive())
			{
				//SET_FRAME_TIME(false, (1.0f / 256.0f));
				DrawLine(Vector2D(RecoilCrosshairScreen.x - 5, RecoilCrosshairScreen.y), Vector2D(RecoilCrosshairScreen.x + 5, RecoilCrosshairScreen.y), 255, 0, 0, 230);
				DrawLine(Vector2D(RecoilCrosshairScreen.x, RecoilCrosshairScreen.y - 5), Vector2D(RecoilCrosshairScreen.x, RecoilCrosshairScreen.y + 5), 255, 0, 0, 230);
			}
			if (DrawDamageChk.Checked)
			{
				DrawDamageIndicator();
			}

			//pSprite->End();
		}
	}
}


void DrawPlayerList()
{
	if (!LocalPlayer.Get(&LocalPlayer))
		return;

	if (EXTERNAL_WINDOW)
	{
		RunPerPlayerHacks();
	}

	int NumPlayersDrawn = 0;
	int HighlightedPlayerRowIndex = ROW_LEFT;
	int RowIndex = ROW_LEFT;
	static bool HoldingLeftMouseButton = false;
	bool HighlightedPlayerIsStillConnected = false;
	const int NAME_BOX_WIDTH = 127;
	const int NAME_BOX_HEIGHT = 21;
	int pIndexPlayer[4096] { 0 };
	int NumPlayersRowLeft = 0;
	int NumPlayersRowRight = 0;
	int LocalTeam = LocalPlayer.Entity->GetTeam();

	//pSprite->Begin(D3DXSPRITE_ALPHABLEND | D3DXSPRITE_DO_NOT_ADDREF_TEXTURE);

	int MaxEntities = 64;// Interfaces::ClientEntList->GetHighestEntityIndex();
	int NumPlayers = GetClientCount();
	int CountedPlayers = 0;

	if (NumPlayers)
	{
		for (int i = 0; i <= MaxEntities; i++)
		{
			CBaseEntity* Entity = Interfaces::ClientEntList->GetClientEntity(i);

			if (Entity && Entity->IsPlayer())
			{
				CustomPlayer* pCAPlayer = &AllPlayers[i];
				if (Entity != LocalPlayer.Entity && pCAPlayer->BaseEntity && pCAPlayer->Active)
				{
					wchar_t PlayerName[32];
					DWORD Radar = GetRadar();
					if (Radar)
					{
						GetPlayerName(Radar, pCAPlayer->Index, PlayerName);
						if (wcslen(PlayerName) == 0 || PlayerName[0] == L'\n' || PlayerName[0] == 0x0A)
						{
							wprintf(PlayerName, L"UID %i", pCAPlayer->BaseEntity->GetUserID());
						}
					}
					else
					{
						wcsncpy(PlayerName, L"Error", 5);
					}

					Vector2D posonscreen = Vector2D((RowIndex * (NAME_BOX_WIDTH * 1.2f)) + 20 - OVERLAY_OFFSETX + CenterOfScreen.x + MenuOffsetXTxt.iValue, 60 + (20 * (RowIndex == ROW_LEFT ? NumPlayersRowLeft : NumPlayersRowRight)) - OVERLAY_OFFSETY + CenterOfScreen.y - MenuOffsetYTxt.iValue);

					if (pHighlightedPlayer && pCAPlayer->BaseEntity == pHighlightedPlayer->BaseEntity)
					{
						HighlightedPlayerRowIndex = RowIndex;
						DrawGUIBox(Vector2D(posonscreen.x - 2, posonscreen.y - 1), NAME_BOX_WIDTH, NAME_BOX_HEIGHT, 136, 239, 228, 176, 136, 239, 228, 176);
						HighlightedPlayerIsStillConnected = true;
					}
					pIndexPlayer[NumPlayersDrawn] = i;
					const ColorRGBA enemycolor = ColorRGBA(255, 155, 115, 255);
					DrawStringUnencryptedW(PlayerName, posonscreen, (TargetTeammatesChk.Checked || Entity->GetTeam() != LocalTeam) ? enemycolor : ColorRGBA(MenuTextLineColorRedTxt.flValue, MenuTextLineColorGreenTxt.flValue, MenuTextLineColorBlueTxt.flValue, 255), pFont);

					if (RowIndex == ROW_LEFT)
					{
						NumPlayersRowLeft++;
						RowIndex = ROW_RIGHT;
					}
					else
					{
						NumPlayersRowRight++;
						RowIndex = ROW_LEFT;
					}
					NumPlayersDrawn++;
				}
				if (++CountedPlayers == NumPlayers)
					break;
			}
		}
	}

	//pSprite->End();

	if (pHighlightedPlayer && !HighlightedPlayerIsStillConnected)
	{
		ClearHighlightedPlayer();
	}

	if (GetAsyncKeyState(VK_LBUTTON) && !HoldingLeftMouseButton)
	{
		//See if we are clicking on a player in the menu
		RowIndex = ROW_LEFT;
		NumPlayersRowLeft = 0;
		NumPlayersRowRight = 0;
		for (int i = 0; i < NumPlayersDrawn; i++)
		{
			Vector2D posonscreen = Vector2D((RowIndex * (NAME_BOX_WIDTH * 1.2f)) + 18 - OVERLAY_OFFSETX + CenterOfScreen.x + MenuOffsetXTxt.iValue, 58 + (20 * (RowIndex == ROW_LEFT ? NumPlayersRowLeft : NumPlayersRowRight)) - OVERLAY_OFFSETY + CenterOfScreen.y - MenuOffsetYTxt.iValue);
			POINT cursorpos;
			GetCursorPos(&cursorpos);
			if (PointIsInsideBox(Vector2D(cursorpos.x, cursorpos.y), posonscreen, Vector2D(posonscreen.x + NAME_BOX_WIDTH, posonscreen.y + NAME_BOX_HEIGHT)))
			{
				pHighlightedPlayer = &AllPlayers[pIndexPlayer[i]];
				//Todo: a for loop to automate these and future additions would be nice..
				//Set the values of the menu checkboxes and textboxes to the values stored in the player struct
				UpdateResolverValues();
			}

			if (RowIndex == ROW_LEFT)
			{
				NumPlayersRowLeft++;
				RowIndex = ROW_RIGHT;
			}
			else
			{
				NumPlayersRowRight++;
				RowIndex = ROW_LEFT;
			}
		}
		HoldingLeftMouseButton = true;
	}
	else
	{
		HoldingLeftMouseButton = false;
	}
}