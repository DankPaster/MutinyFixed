#ifdef NewMenu


#include "C:/ClientFramework/Framework/Include/Frame-Include.h"
#include "C:/ClientFramework/Framework/Menu/Fonts.h"
#include "framework.h"
const char* Keys[] =
{
	"",
	"Mouse 1",
	"Mouse 2",
	"Cancel",
	"Middle Mouse",
	"Mouse 4",
	"Mouse 5",
	"",
	"Backspace",
	"Tab",
	"",
	"",
	"Clear",
	"Enter",
	"",
	"",
	"Shift",
	"Control",
	"Alt",
	"Pause",
	"Caps",
	"",
	"",
	"",
	"",
	"",
	"",
	"Escape",
	"",
	"",
	"",
	"",
	"Space",
	"Page Up",
	"Page Down",
	"End",
	"Home",
	"Left",
	"Up",
	"Right",
	"Down",
	"",
	"",
	"",
	"Print",
	"Insert",
	"Delete",
	"",
	"0",
	"1",
	"2",
	"3",
	"4",
	"5",
	"6",
	"7",
	"8",
	"9",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"A",
	"B",
	"C",
	"D",
	"E",
	"F",
	"G",
	"H",
	"I",
	"J",
	"K",
	"L",
	"M",
	"N",
	"O",
	"P",
	"Q",
	"R",
	"S",
	"T",
	"U",
	"V",
	"W",
	"X",
	"Y",
	"Z",
	"",
	"",
	"",
	"",
	"",
	"Numpad 0",
	"Numpad 1",
	"Numpad 2",
	"Numpad 3",
	"Numpad 4",
	"Numpad 5",
	"Numpad 6",
	"Numpad 7",
	"Numpad 8",
	"Numpad 9",
	"Multiply",
	"Add",
	"",
	"Subtract",
	"Decimal",
	"Divide",
	"F1",
	"F2",
	"F3",
	"F4",
	"F5",
	"F6",
	"F7",
	"F8",
	"F9",
	"F10",
	"F11",
	"F12"

};
#ifdef FRAME_DX9
MutinyFrame::UserInterface::UserInterface()
{
}

MutinyFrame::UserInterface::UserInterface(HWND Window, IDirect3DDevice9* IDevice)
{
	ImGui_ImplDX9_Init(Window, IDevice);
	ImGuiStyle * style = &ImGui::GetStyle();
	style->WindowTitleAlign = ImVec2(0.5, 0.5);
	style->WindowPadding = ImVec2(15, 15);
	style->WindowRounding = 5.0f;
	style->FramePadding = ImVec2(5, 5);
	style->FrameRounding = 4.0f;
	style->ItemSpacing = ImVec2(12, 8);
	style->ItemInnerSpacing = ImVec2(8, 6);
	style->IndentSpacing = 25.0f;
	style->ScrollbarSize = 15.0f;
	style->ScrollbarRounding = 9.0f;
	style->GrabMinSize = 5.0f;
	style->GrabRounding = 3.0f;

	style->Colors[ImGuiCol_Text] = ImVec4(0.80f, 0.80f, 0.83f, 1.00f);
	style->Colors[ImGuiCol_TextDisabled] = ImVec4(0.24f, 0.23f, 0.29f, 1.00f);
	style->Colors[ImGuiCol_WindowBg] = ImVec4(0.06f, 0.05f, 0.07f, 1.00f);
	style->Colors[ImGuiCol_ChildWindowBg] = ImVec4(0.07f, 0.07f, 0.09f, 1.00f);
	style->Colors[ImGuiCol_PopupBg] = ImVec4(0.07f, 0.07f, 0.09f, 1.00f);
	style->Colors[ImGuiCol_Border] = ImVec4(0.80f, 0.80f, 0.83f, 0.88f);
	style->Colors[ImGuiCol_BorderShadow] = ImVec4(0.92f, 0.91f, 0.88f, 0.00f);
	style->Colors[ImGuiCol_FrameBg] = ImVec4(0.10f, 0.09f, 0.12f, 1.00f);
	style->Colors[ImGuiCol_FrameBgHovered] = ImVec4(0.24f, 0.23f, 0.29f, 1.00f);
	style->Colors[ImGuiCol_FrameBgActive] = ImVec4(0.56f, 0.56f, 0.58f, 1.00f);
	style->Colors[ImGuiCol_TitleBg] = ImVec4(0.10f, 0.09f, 0.12f, 1.00f);
	style->Colors[ImGuiCol_TitleBgCollapsed] = ImVec4(1.00f, 0.98f, 0.95f, 0.75f);
	style->Colors[ImGuiCol_TitleBgActive] = ImVec4(0.07f, 0.07f, 0.09f, 1.00f);
	style->Colors[ImGuiCol_MenuBarBg] = ImVec4(0.10f, 0.09f, 0.12f, 1.00f);
	style->Colors[ImGuiCol_ScrollbarBg] = ImVec4(0.10f, 0.09f, 0.12f, 1.00f);
	style->Colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.80f, 0.80f, 0.83f, 0.31f);
	style->Colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.56f, 0.56f, 0.58f, 1.00f);
	style->Colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.06f, 0.05f, 0.07f, 1.00f);
	style->Colors[ImGuiCol_ComboBg] = ImVec4(0.19f, 0.18f, 0.21f, 1.00f);
	style->Colors[ImGuiCol_CheckMark] = ImVec4(0.80f, 0.80f, 0.83f, 0.31f);
	style->Colors[ImGuiCol_SliderGrab] = ImVec4(0.80f, 0.80f, 0.83f, 0.31f);
	style->Colors[ImGuiCol_SliderGrabActive] = ImVec4(0.06f, 0.05f, 0.07f, 1.00f);
	style->Colors[ImGuiCol_Button] = ImVec4(0.10f, 0.09f, 0.12f, 1.00f);
	style->Colors[ImGuiCol_ButtonHovered] = ImVec4(0.24f, 0.23f, 0.29f, 1.00f);
	style->Colors[ImGuiCol_ButtonActive] = ImVec4(0.56f, 0.56f, 0.58f, 1.00f);
	style->Colors[ImGuiCol_Header] = ImVec4(0.10f, 0.09f, 0.12f, 1.00f);
	style->Colors[ImGuiCol_HeaderHovered] = ImVec4(0.56f, 0.56f, 0.58f, 1.00f);
	style->Colors[ImGuiCol_HeaderActive] = ImVec4(0.06f, 0.05f, 0.07f, 1.00f);
	style->Colors[ImGuiCol_Column] = ImVec4(0.56f, 0.56f, 0.58f, 1.00f);
	style->Colors[ImGuiCol_ColumnHovered] = ImVec4(0.24f, 0.23f, 0.29f, 1.00f);
	style->Colors[ImGuiCol_ColumnActive] = ImVec4(0.56f, 0.56f, 0.58f, 1.00f);
	style->Colors[ImGuiCol_ResizeGrip] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
	style->Colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.56f, 0.56f, 0.58f, 1.00f);
	style->Colors[ImGuiCol_ResizeGripActive] = ImVec4(0.06f, 0.05f, 0.07f, 1.00f);
	style->Colors[ImGuiCol_CloseButton] = ImVec4(0.40f, 0.39f, 0.38f, 0.16f);
	style->Colors[ImGuiCol_CloseButtonHovered] = ImVec4(0.40f, 0.39f, 0.38f, 0.39f);
	style->Colors[ImGuiCol_CloseButtonActive] = ImVec4(0.40f, 0.39f, 0.38f, 1.00f);
	style->Colors[ImGuiCol_PlotLines] = ImVec4(0.40f, 0.39f, 0.38f, 0.63f);
	style->Colors[ImGuiCol_PlotLinesHovered] = ImVec4(0.25f, 1.00f, 0.00f, 1.00f);
	style->Colors[ImGuiCol_PlotHistogram] = ImVec4(0.40f, 0.39f, 0.38f, 0.63f);
	style->Colors[ImGuiCol_PlotHistogramHovered] = ImVec4(0.25f, 1.00f, 0.00f, 1.00f);
	style->Colors[ImGuiCol_TextSelectedBg] = ImVec4(0.25f, 1.00f, 0.00f, 0.43f);
	style->Colors[ImGuiCol_ModalWindowDarkening] = ImVec4(1.00f, 0.98f, 0.95f, 0.73f);
	ImGui::GetIO().Fonts->AddFontFromMemoryCompressedTTF(RudaCompressed, RudaSize, 12);
	ImGui::GetIO().Fonts->AddFontFromMemoryCompressedTTF(RudaCompressed, RudaSize, 10);
	ImGui::GetIO().Fonts->AddFontFromMemoryCompressedTTF(RudaCompressed, RudaSize, 14);
	ImGui::GetIO().Fonts->AddFontFromMemoryCompressedTTF(RudaCompressed, RudaSize, 18);
	this->Tabs = ImGui::GetIO().Fonts->AddFontFromMemoryCompressedTTF(RudaCompressed, RudaSize, 20);
	this->Initialized = true;
}

void MutinyFrame::UserInterface::Draw()
{
	if (!this->Initialized)
		return;
	MenuOpen = (GetAsyncKeyState(VK_INSERT) & (1 << 16));

	ImGui::GetIO().MouseDrawCursor = MenuOpen;

	ImGui_ImplDX9_NewFrame();
	static int CurrentTab = 0;
	if (MenuOpen)
	{
		if (ImGui::Begin(("Mutanity by Clen lol"), &MenuOpen, ImVec2(800, 400), 1.f, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoClose))
		{
			const char* Tabs[] = {
				"Ragebot",
				"Legitbot (coming soon)",
				"Visuals",
				"Misc",
				"Skin Changer (coming soon)",
				"Colours",
				"Configs",
#ifdef ADMIN
				"Admin",
#endif

			};
			static const char* Configs[] = {
				"default",
				"MM",
				"Rage",
				"No Spread",
				"Legit",
				"League",
				"1",
				"2",
				"3",
				"4",

			};
			static const char* WeaponConfigs =
				"Global\0Rifles\0Auto\0Scout\0AWP\0Deagle\0R8\0Heavy\0ShotGuns\0Pistols\0SMGs\0";

		

			ImGui::Columns(2, "Tabs", false);
			{
				ImGui::SetColumnOffset(1, 130);
				for (auto i = 0; i < ARRAYSIZE(Tabs); i++)
				{
					ImGui::PushFont(this->Tabs);
					if (ImGui::Button(Tabs[i], ImVec2(120, 40)))
					{
						CurrentTab = i;
					}

					ImGui::PopFont();
				}

			}

			ImGui::NextColumn();
			ImGui::SetColumnOffset(2, ImGui::GetWindowWidth());
			if (CurrentTab == 0)
			{
				//	ImGui::SameLine();
				static int Item = 0;
				ImGui::Combo("Option", &Item, "Aimbot\0AutoShoot\0Resolver\0AntiAim\0");
				static int CustomantiAimOption = 0;
				static int antiAimOption = 0;
				if (Item == 3)
				{
					this->Combo("AntiAim Type", "AATYPE", "Off\0Simple\0Custom (coming soon)\0");

				}
				if (ImGui::BeginChild("Aim", ImVec2(600, 300), true))
				{
					ImGui::Columns(3, "Aimbot", true);
					float Offsets = ImGui::GetWindowWidth() / 3;
					ImGui::SetColumnOffset(1, Offsets);


					switch (Item)
					{
					case 1:
					{
						{
							this->SlideFloat("Triggerbot Rate Of Fire", "TriggerbotRateOfFireTxt",0, 1000);
							this->SlideFloat("Triggerbot Time To Fire(ms)", "TriggernotTimeToFireTxt", 0, 1000);
							this->Combo("Triggerbot Key(virtualKey)", "TriggerbotKeyTxt", Keys, ARRAYSIZE(Keys));
							this->SlideFloat("Wait time for resolved lower body yaw (secs)", "WaittimeforresolvedlowerbodyyawTxt", 0, 1);
							this->SlideFloat("Don't Shoot After X Misses", "DontShootAfterXMissesTxt", 0, 30);
							this->Checkbox("Auto Scope (Snipers)", "auto_scope");

						}
						ImGui::NextColumn();
						ImGui::SetColumnOffset(2, Offsets * 2);
						{
							this->Checkbox("Auto-Fire Without Key Press", "auto_fire_without_key_press");
							this->Checkbox("Auto-Fire While Aimlocking", "auto_fire_while_aimlocking");
							this->Checkbox("Only Fire at the peak of jumps", "only_fire_at_the_peak_of_jump");
							this->Checkbox("Knife Bot", "knife_bot");
							this->Checkbox("Auto Stop Movement", "auto_stop_movement");

						}

						ImGui::NextColumn();
						ImGui::SetColumnOffset(3, Offsets * 3);
						{
							
							static int Weapon = 0;

							ImGui::Combo("Option", &Weapon, WeaponConfigs);//"Default\0Rifle\0Auto\0Scout\0AWP\0Deagle\0R8\0Heavy\0ShotGuns\0Pistols\0SMGS\0";

							switch (Weapon)
							{
							case 1:
							{
								this->Checkbox("Use Custom Group", "RifleCustom");
								this->SlideFloat("Hitchance Head %", "RifleMinimumHitchanceHeadTxt",0, 100);
								this->SlideFloat("Minimun Hitchance Body %", "RifleMinimumHitchanceBodyTxt", 0, 100);
								this->SlideFloat("Damage Bofore Firing", "RifleMinimumDamageBeforeFiringTxt", 0, 100);
								this->Checkbox("Hitchance Only During Autofire", "Riflehitchance_only_during_autofire");
								this->SlideFloat("Point Scale", "RiflePointScaleTxt", 0, 1);
								break;
							}
							case 2:
							{
								this->Checkbox("Use Custom Group", "AutoCustom");
								this->SlideFloat("Minimum Hitchance Head %", "AutoMinimumHitchanceHeadTxt", 0, 100);
								this->SlideFloat("Minimun Hitchance Body %", "AutoMinimumHitchanceBodyTxt", 0, 100);
								this->SlideFloat("Minimum Damage Bofore Firing(Requires AutoWall and Best Damage Auto Bone*)", "AutoMinimumDamageBeforeFiringTxt", 0, 100);
								this->Checkbox("Hitchance Only During Autofire (Less BAim, More FPS)", "Autohitchance_only_during_autofire");
								this->SlideFloat("Point Scale", "AutoPointScaleTxt", 0, 1);
								break;
							}
							case 3:
							{
								this->Checkbox("Use Custom Group", "ScoutCustom");
								this->SlideFloat("Minimum Hitchance Head %", "ScoutMinimumHitchanceHeadTxt", 0, 100);
								this->SlideFloat("Minimun Hitchance Body %", "ScoutMinimumHitchanceBodyTxt", 0, 100);
								this->SlideFloat("Minimum Damage Bofore Firing(Requires AutoWall and Best Damage Auto Bone*)", "ScoutMinimumDamageBeforeFiringTxt", 0, 100);
								this->Checkbox("Hitchance Only During Autofire (Less BAim, More FPS)", "Scouthitchance_only_during_autofire");
								this->SlideFloat("Point Scale", "ScoutPointScaleTxt", 0, 1);
								break;
							}
							case 4:
							{
								this->Checkbox("Use Custom Group", "AWPCustom");
								this->SlideFloat("Minimum Hitchance Head %", "AWPMinimumHitchanceHeadTxt", 0, 100);
								this->SlideFloat("Minimun Hitchance Body %", "AWPMinimumHitchanceBodyTxt", 0, 100);
								this->SlideFloat("Minimum Damage Bofore Firing(Requires AutoWall and Best Damage Auto Bone*)", "AWPMinimumDamageBeforeFiringTxt", 0, 100);
								this->Checkbox("Hitchance Only During Autofire (Less BAim, More FPS)", "AWPhitchance_only_during_autofire");
								this->SlideFloat("Point Scale", "AWPPointScaleTxt", 0, 1);
								break;
							}
							case 5:
							{
								this->Checkbox("Use Custom Group", "DeagleCustom");
								this->SlideFloat("Minimum Hitchance Head %", "DeagleMinimumHitchanceHeadTxt", 0, 100);
								this->SlideFloat("Minimun Hitchance Body %", "DeagleMinimumHitchanceBodyTxt", 0, 100);
								this->SlideFloat("Minimum Damage Bofore Firing(Requires AutoWall and Best Damage Auto Bone*)", "DeagleMinimumDamageBeforeFiringTxt", 0, 100);
								this->Checkbox("Hitchance Only During Autofire (Less BAim, More FPS)", "Deaglehitchance_only_during_autofire");
								this->SlideFloat("Point Scale", "DeaglePointScaleTxt", 0, 1);
								break;
							}
							case 6:
							{
								this->Checkbox("Use Custom Group", "R8Custom");
								this->SlideFloat("Minimum Hitchance Head %", "R8MinimumHitchanceHeadTxt", 0, 100);
								this->SlideFloat("Minimun Hitchance Body %", "R8MinimumHitchanceBodyTxt", 0, 100);
								this->SlideFloat("Minimum Damage Bofore Firing(Requires AutoWall and Best Damage Auto Bone*)", "R8MinimumDamageBeforeFiringTxt", 0, 100);
								this->Checkbox("Hitchance Only During Autofire (Less BAim, More FPS)", "R8hitchance_only_during_autofire");
								this->SlideFloat("Point Scale", "R8PointScaleTxt", 0, 1);
								break;

							}
							case 7:
							{
								this->Checkbox("Use Custom Group", "HeavyCustom");
								this->SlideFloat("Minimum Hitchance Head %", "HeavyMinimumHitchanceHeadTxt", 0, 100);
								this->SlideFloat("Minimun Hitchance Body %", "HeavyMinimumHitchanceBodyTxt", 0, 100);
								this->SlideFloat("Minimum Damage Bofore Firing(Requires AutoWall and Best Damage Auto Bone*)", "HeavyMinimumDamageBeforeFiringTxt", 0, 100);
								this->Checkbox("Hitchance Only During Autofire (Less BAim, More FPS)", "Heavyhitchance_only_during_autofire");
								this->SlideFloat("Point Scale", "HeavyPointScaleTxt", 0, 1);
								break;

							}
							case 8:
							{
								this->Checkbox("Use Custom Group", "ShotGunsCustom");
								this->SlideFloat("Minimum Hitchance Head %", "ShotGunsMinimumHitchanceHeadTxt", 0, 100);
								this->SlideFloat("Minimun Hitchance Body %", "ShotGunsMinimumHitchanceBodyTxt", 0, 100);
								this->SlideFloat("Minimum Damage Bofore Firing(Requires AutoWall and Best Damage Auto Bone*)", "ShotGunsMinimumDamageBeforeFiringTxt", 0, 100);
								this->Checkbox("Hitchance Only During Autofire (Less BAim, More FPS)", "ShotGunshitchance_only_during_autofire");
								this->SlideFloat("Point Scale", "ShotGunsPointScaleTxt", 0, 1);
								break;

							}
							case 9:
							{
								this->Checkbox("Use Custom Group", "PistolsCustom");
								this->SlideFloat("Minimum Hitchance Head %", "PistolsMinimumHitchanceHeadTxt", 0, 100);
								this->SlideFloat("Minimun Hitchance Body %", "PistolsMinimumHitchanceBodyTxt", 0, 100);
								this->SlideFloat("Minimum Damage Bofore Firing(Requires AutoWall and Best Damage Auto Bone*)", "PistolsMinimumDamageBeforeFiringTxt", 0, 100);
								this->Checkbox("Hitchance Only During Autofire (Less BAim, More FPS)", "Pistolshitchance_only_during_autofire");
								this->SlideFloat("Point Scale", "PistolsPointScaleTxt", 0, 1);
								break;

							}
							case 10:
							{
								this->Checkbox("Use Custom Group", "SMGSCustom");
								this->SlideFloat("Minimum Hitchance Head %", "SMGSMinimumHitchanceHeadTxt", 0, 100);
								this->SlideFloat("Minimun Hitchance Body %", "SMGSMinimumHitchanceBodyTxt", 0, 100);
								this->SlideFloat("Minimum Damage Bofore Firing(Requires AutoWall and Best Damage Auto Bone*)", "SMGSMinimumDamageBeforeFiringTxt", 0, 100);
								this->Checkbox("Hitchance Only During Autofire (Less BAim, More FPS)", "SMGShitchance_only_during_autofire");
								this->SlideFloat("Point Scale", "SMGSPointScaleTxt", 0, 1);
								break;

							}
						
							default:
							{
								this->SlideFloat("Minimum Hitchance Head %", "MinimumHitchanceHeadTxt", 0, 100);
								this->SlideFloat("Minimun Hitchance Body %", "MinimumHitchanceBodyTxt", 0, 100);
								this->SlideFloat("Minimum Damage Bofore Firing(Requires AutoWall and Best Damage Auto Bone*)", "MinimumDamageBeforeFiringTxt", 0, 100);
								this->Checkbox("Hitchance Only During Autofire (Less BAim, More FPS)", "hitchance_only_during_autofire");
								this->SlideFloat("Point Scale", "PointScaleTxt", 0, 1);
								break;

							}
							}


						}
					}
					break;
					case 3: 
					{

						if (antiAimOption == 2)
						{
							ImGui::Combo("Condition", &CustomantiAimOption, "Default\0On Ground\0On Ground Moving\0On Ground Shooting\0In Air\0In Air Shooting\0");
						}


						switch (antiAimOption)
						{
						case 2:
						{
							static const char* Pitches = "Off\0Static\0Lisp'd\0";
							static const char* Yaw = "Off\0From View\0Static\0At Target\0Lby Delta\0";

							switch (CustomantiAimOption)
							{
							case 1:
							{

								{
									//Used for pitches
									this->Checkbox("Use this Pitch", "useOnGroundPitchAngles");
									this->Checkbox("Safe Angles", "ClampAnglesOnGround");
									this->Combo("Pitch", "PitchOnGround", Pitches);
									this->SlideFloat("Angle", "pitchAngleOnGround", -180, 180);
									this->SlideFloat("Jitter", "pitchJitterOnGround", -180, 180);
									this->SlideFloat("Randomiser", "pitchRandomiserOnGround", -180, 180);

								}
								ImGui::NextColumn();
								ImGui::SetColumnOffset(2, Offsets * 2);
								{
									//Real Yaw
									this->Checkbox("Use this Real Yaw", "useOnGroundRealYawAngles");
									this->Combo("Real Yaw", "RealYawOnGround", Yaw);
									this->SlideFloat("Angle", "RealYawAngleOnGround", -180, 180);
									this->Checkbox("Use Second Yaw", "secondRealYawOnGround");
									this->SlideFloat("Second Yaw", "secondRealYawAngleOnGround", -180, 180);
									this->SlideFloat("Jitter", "RealYawJitterOnGround", -180, 180);
									this->SlideFloat("Spin", "RealYawSpinOnGround", -180, 180);
									this->SlideFloat("Randomiser", "RealYawRandomiseOnGround", -180, 180);
									this->Checkbox("Switch On Lby Update", "LBYUPdateRealYawOnGround");
									this->Combo("Real Yaw LBY", "LBYUPRealYawOnGround", Yaw);
									this->SlideFloat("Angle LBY", "LBYUPRealYawAngleOnGround", -180, 180);
									this->Checkbox("Use Second Yaw LBY", "LBYUPsecondRealYawOnGround");
									this->SlideFloat("Second Yaw LBY", "LBYUPsecondRealYawAngleOnGround", -180, 180);
									this->SlideFloat("Jitter LBY", "LBYUPRealYawJitterOnGround", -180, 180);
									this->SlideFloat("Spin LBY", "LBYUPRealYawSpinOnGround", -180, 180);
									this->SlideFloat("Randomiser LBY", "LBYUPRealYawRandomiseOnGround", -180, 180);

								}

								ImGui::NextColumn();
								ImGui::SetColumnOffset(3, Offsets * 3);
								{
									//Fake yaw
									this->Checkbox("Use this Fake Yaw", "useOnGroundFakeYawAngles");
									this->Combo("Fake Yaw", "FakeYawOnGround", Yaw);
									this->SlideFloat("Angle", "FakeYawAngleOnGround", -180, 180);
									this->Checkbox("Use Second Yaw", "secondFakeYawOnGround");
									this->SlideFloat("Second Yaw", "secondFakeYawAngleOnGround", -180, 180);
									this->SlideFloat("Jitter", "FakeYawJitterOnGround", -180, 180);
									this->SlideFloat("Spin", "FakeYawSpinOnGround", -180, 180);
									this->SlideFloat("Randomiser", "FakeYawRandomiseOnGround", -180, 180);
									this->Checkbox("Switch On Lby Update", "LBYUPdateFakeYawOnGround");
									this->Combo("Fake Yaw LBY", "LBYUPFakeYawOnGround", Yaw);
									this->SlideFloat("Angle LBY", "LBYUPFakeYawAngleOnGround", -180, 180);
									this->Checkbox("Use Second Yaw LBY", "LBYUPsecondFakeYawOnGround");
									this->SlideFloat("Second Yaw LBY", "LBYUPsecondFakeYawAngleOnGround", -180, 180);
									this->SlideFloat("Jitter LBY", "LBYUPFakeYawJitterOnGround", -180, 180);
									this->SlideFloat("Spin LBY", "LBYUPFakeYawSpinOnGround", -180, 180);
									this->SlideFloat("Randomiser LBY", "LBYUPFakeYawRandomiseOnGround", -180, 180);
								}
							}
							break;
							case 2:
							{

								{
									//Used for pitches
									this->Checkbox("Use this Pitch", "useOnGroundMovingPitchAngles");
									this->Checkbox("Safe Angles", "ClampAnglesOnGroundMoving");
									this->Combo("Pitch", "PitchOnGroundMoving", Pitches);
									this->SlideFloat("Angle", "pitchAngleOnGroundMoving", -180, 180);
									this->SlideFloat("Jitter", "pitchJitterOnGroundMoving", -180, 180);
									this->SlideFloat("Randomiser", "pitchRandomiserOnGroundMoving", -180, 180);

								}
								ImGui::NextColumn();
								ImGui::SetColumnOffset(2, Offsets * 2);
								{
									//Real Yaw
									this->Checkbox("Use this Real Yaw", "useOnGroundMovingRealYawAngles");
									this->Combo("Real Yaw", "RealYawOnGroundMoving", Yaw);
									this->SlideFloat("Angle", "RealYawAngleOnGroundMoving", -180, 180);
									this->Checkbox("Use Second Yaw", "secondRealYawOnGroundMoving");
									this->SlideFloat("Second Yaw", "secondRealYawAngleOnGroundMoving", -180, 180);
									this->SlideFloat("Jitter", "RealYawJitterOnGroundMoving", -180, 180);
									this->SlideFloat("Spin", "RealYawSpinOnGroundMoving", -180, 180);
									this->SlideFloat("Randomiser", "RealYawRandomiseOnGroundMoving", -180, 180);

								}

								ImGui::NextColumn();
								ImGui::SetColumnOffset(3, Offsets * 3);
								{
									//Fake yaw
									this->Checkbox("Use this Fake Yaw", "useOnGroundMovingFakeYawAngles");
									this->Combo("Fake Yaw", "FakeYawOnGroundMoving", Yaw);
									this->SlideFloat("Angle", "FakeYawAngleOnGroundMoving", -180, 180);
									this->Checkbox("Use Second Yaw", "secondFakeYawOnGroundMoving");
									this->SlideFloat("Second Yaw", "secondFakeYawAngleOnGroundMoving", -180, 180);
									this->SlideFloat("Jitter", "FakeYawJitterOnGroundMoving", -180, 180);
									this->SlideFloat("Spin", "FakeYawSpinOnGroundMoving", -180, 180);
									this->SlideFloat("Randomiser", "FakeYawRandomiseOnGroundMoving", -180, 180);
								}
							}
							break;
							case 3:
							{

								{
									//Used for pitches
									this->Checkbox("Use this Pitch", "useOnGroundShootingPitchAngles");
									this->Checkbox("Safe Angles", "ClampAnglesOnGroundShooting");
									this->Combo("Pitch", "PitchOnGroundShooting", Pitches);
									this->SlideFloat("Angle", "pitchAngleOnGroundShooting", -180, 180);
									this->SlideFloat("Jitter", "pitchJitterOnGroundShooting", -180, 180);
									this->SlideFloat("Randomiser", "pitchRandomiserOnGroundShooting", -180, 180);

								}
								ImGui::NextColumn();
								ImGui::SetColumnOffset(2, Offsets * 2);
								{
									//Real Yaw
									this->Checkbox("Use this Real Yaw", "useOnGroundShootingRealYawAngles");
									this->Combo("Real Yaw", "RealYawOnGroundShooting", Yaw);
									this->SlideFloat("Angle", "RealYawAngleOnGroundShooting", -180, 180);
									this->Checkbox("Use Second Yaw", "secondRealYawOnGroundShooting");
									this->SlideFloat("Second Yaw", "secondRealYawAngleOnGroundShooting", -180, 180);
									this->SlideFloat("Jitter", "RealYawJitterOnGroundShooting", -180, 180);
									this->SlideFloat("Spin", "RealYawSpinOnGroundShooting", -180, 180);
									this->SlideFloat("Randomiser", "RealYawRandomiseOnGroundShooting", -180, 180);

								}

								ImGui::NextColumn();
								ImGui::SetColumnOffset(3, Offsets * 3);
								{
									//Fake yaw
									this->Checkbox("Use this Fake Yaw", "useOnGroundShootingFakeYawAngles");
									this->Combo("Fake Yaw", "FakeYawOnGroundShooting", Yaw);
									this->SlideFloat("Angle", "FakeYawAngleOnGroundShooting", -180, 180);
									this->Checkbox("Use Second Yaw", "secondFakeYawOnGroundShooting");
									this->SlideFloat("Second Yaw", "secondFakeYawAngleOnGroundShooting", -180, 180);
									this->SlideFloat("Jitter", "FakeYawJitterOnGroundShooting", -180, 180);
									this->SlideFloat("Spin", "FakeYawSpinOnGroundShooting", -180, 180);
									this->SlideFloat("Randomiser", "FakeYawRandomiseOnGroundShooting", -180, 180);
								}
							}
							break;
							case 4:
							{

								{
									//Used for pitches
									this->Checkbox("Use this Pitch", "useInAirPitchAngles");
									this->Checkbox("Safe Angles", "ClampAnglesInAir");
									this->Combo("Pitch", "PitchInAir", Pitches);
									this->SlideFloat("Angle", "pitchAngleInAir", -180, 180);
									this->SlideFloat("Jitter", "pitchJitterInAir", -180, 180);
									this->SlideFloat("Randomiser", "pitchRandomiserInAir", -180, 180);

								}
								ImGui::NextColumn();
								ImGui::SetColumnOffset(2, Offsets * 2);
								{
									//Real Yaw
									this->Checkbox("Use this Real Yaw", "useInAirRealYawAngles");
									this->Combo("Real Yaw", "RealYawInAir", Yaw);
									this->SlideFloat("Angle", "RealYawAngleInAir", -180, 180);
									this->Checkbox("Use Second Yaw", "secondRealYawInAir");
									this->SlideFloat("Second Yaw", "secondRealYawAngleInAir", -180, 180);
									this->SlideFloat("Jitter", "RealYawJitterInAir", -180, 180);
									this->SlideFloat("Spin", "RealYawSpinInAir", -180, 180);
									this->SlideFloat("Randomiser", "RealYawRandomiseInAir", -180, 180);

								}

								ImGui::NextColumn();
								ImGui::SetColumnOffset(3, Offsets * 3);
								{
									//Fake yaw
									this->Checkbox("Use this Fake Yaw", "useInAirFakeYawAngles");
									this->Combo("Fake Yaw", "FakeYawInAir", Yaw);
									this->SlideFloat("Angle", "FakeYawAngleInAir", -180, 180);
									this->Checkbox("Use Second Yaw", "secondFakeYawInAir");
									this->SlideFloat("Second Yaw", "secondFakeYawAngleInAir", -180, 180);
									this->SlideFloat("Jitter", "FakeYawJitterInAir", -180, 180);
									this->SlideFloat("Spin", "FakeYawSpinInAir", -180, 180);
									this->SlideFloat("Randomiser", "FakeYawRandomiseInAir", -180, 180);
								}
							}
							break;
							case 5:
							{

								{
									//Used for pitches
									this->Checkbox("Use this Pitch", "useInAirShootingPitchAngles");
									this->Checkbox("Safe Angles", "ClampAnglesInAirShooting");
									this->Combo("Pitch", "PitchInAirShooting", Pitches);
									this->SlideFloat("Angle", "pitchAngleInAirShooting", -180, 180);
									this->SlideFloat("Jitter", "pitchJitterInAirShooting", -180, 180);
									this->SlideFloat("Randomiser", "pitchRandomiserInAirShooting", -180, 180);

								}
								ImGui::NextColumn();
								ImGui::SetColumnOffset(2, Offsets * 2);
								{
									//Real Yaw
									this->Checkbox("Use this Real Yaw", "useInAirShootingRealYawAngles");
									this->Combo("Real Yaw", "RealYawInAirShooting", Yaw);
									this->SlideFloat("Angle", "RealYawAngleInAirShooting", -180, 180);
									this->Checkbox("Use Second Yaw", "secondRealYawInAirShooting");
									this->SlideFloat("Second Yaw", "secondRealYawAngleInAirShooting", -180, 180);
									this->SlideFloat("Jitter", "RealYawJitterInAirShooting", -180, 180);
									this->SlideFloat("Spin", "RealYawSpinInAirShooting", -180, 180);
									this->SlideFloat("Randomiser", "RealYawRandomiseInAirShooting", -180, 180);

								}

								ImGui::NextColumn();
								ImGui::SetColumnOffset(3, Offsets * 3);
								{
									//Fake yaw
									this->Checkbox("Use this Fake Yaw", "useInAirShootingFakeYawAngles");
									this->Combo("Fake Yaw", "FakeYawInAirShooting", Yaw);
									this->SlideFloat("Angle", "FakeYawAngleInAirShooting", -180, 180);
									this->Checkbox("Use Second Yaw", "secondFakeYawInAirShooting");
									this->SlideFloat("Second Yaw", "secondFakeYawAngleInAirShooting", -180, 180);
									this->SlideFloat("Jitter", "FakeYawJitterInAirShooting", -180, 180);
									this->SlideFloat("Spin", "FakeYawSpinInAirShooting", -180, 180);
									this->SlideFloat("Randomiser", "FakeYawRandomiseInAirShooting", -180, 180);
								}
							}
							break;

							default:
							{
								{
									//Used for pitches
									this->Checkbox("Safe Angles", "ClampAnglesdefault");
									this->Combo("Pitch", "PitchDefault", Pitches);
									this->SlideFloat("Angle", "pitchAngleDefault", -180, 180);
									this->SlideFloat("Jitter", "pitchJitterDefault", -180, 180);
									this->SlideFloat("Randomiser", "pitchRandomiserDefault", -180, 180);

								}
								ImGui::NextColumn();
								ImGui::SetColumnOffset(2, Offsets * 2);
								{
									//Real Yaw
									this->Combo("Real Yaw", "RealYawDefault", Yaw);
									this->SlideFloat("Angle", "RealYawAngleDefault", -180, 180);
									this->Checkbox("Use Second Yaw", "secondRealYawDefault");
									this->SlideFloat("Second Yaw", "secondRealYawAngleDefault", -180, 180);
									this->SlideFloat("Jitter", "RealYawJitterDefault", -180, 180);
									this->SlideFloat("Spin", "RealYawSpinDefault", -180, 180);
									this->SlideFloat("Randomiser", "RealYawRandomiseDefault", -180, 180);

								}

								ImGui::NextColumn();
								ImGui::SetColumnOffset(3, Offsets * 3);
								{
									//Fake yaw
									this->Combo("Fake Yaw", "FakeYawDefault", Yaw);
									this->SlideFloat("Angle", "FakeYawAngleDefault", -180, 180);
									this->Checkbox("Use Second Yaw", "secondFakeYawDefault");
									this->SlideFloat("Second Yaw", "secondFakeYawAngleDefault", -180, 180);
									this->SlideFloat("Jitter", "FakeYawJitterDefault", -180, 180);
									this->SlideFloat("Spin", "FakeYawSpinDefault", -180, 180);
									this->SlideFloat("Randomiser", "FakeYawRandomiseDefault", -180, 180);

								}
							}
							break;
							}
							
						}
						break;
						default:
						{
							ImGui::SetColumnOffset(1, Offsets * 3);
							ImGui::SetColumnOffset(2, Offsets * 4);
							ImGui::SetColumnOffset(3, Offsets * 4);
							this->Combo("Anti Aim Pitch", "AAPitch", "None\0Up\0Down\0Jitter Down\0Jitter Up/Down\0Lisp 1 (DETECTED)\0Lisp 2 (DETECTED)\0True Down (DETECTED)\0Fake Down (DETECTED)\0");
							this->Combo("Anti Aim Yaw Real", "AARealYaw", "None\0BackWards\0Sideways\0Legit\0Skate\0Ballerina\0Slow Spin\0Spin\0Experimental\0Experimental 2\0Experimental Forwards\0Experimental Forwards Inverted\0LBY Inverted\0Random Yaw\0Classical\0Classical180\0Classical90\0Inverse Switch\0Sideways Jitter");
							this->Combo("Anti Aim Yaw Fake", "AAFakeYaw", "None\0Automatic\0BackWards\0Sideways\0Legit\0Skate\0Ballerina\0Slow Spin\0Spin\0Experimental\0Experimental 2\0Experimental Forwards\0Experimental Forwards Inverted\0LBY Inverted\0Random Yaw\0Classical\0Classical180\0Classical90\0Inverse Switch\0Sideways Jitter");
							this->SlideInt("Spin Degrees Per Tick", "SpinDegPerTickTxt", -180, 180);
							this->Checkbox("Anti Aim Jitter", "AntiAimJitterChk");
							this->Checkbox("Spin Between Shots", "SpinBetweenShotsChk");
							this->Checkbox("Manual Anti Aim", "ManualAntiAimChk");
							this->Checkbox("Edge Anti Aim", "EdgeAntiAimChk");

						}
						break;

						}
					}
					break;
					case 2:
					{
						ImGui::SetColumnOffset(1, Offsets * 3);
						ImGui::SetColumnOffset(2, Offsets * 4);
						ImGui::SetColumnOffset(3, Offsets * 4);
						this->Checkbox("Anti-Anti Aim Resolver Enabled", "ResolverChk");
						this->Checkbox("Predict Fake Angles Automatically", "PredictFakeAnglesChk");
						this->Checkbox("FaceIt Corrections (DOES NOT MAKE IT FACEIT-PROOF!)", "FaceItChk");
						this->Checkbox("Try Correcting pSilent/FakeLag When Players Shoot", "FiringCorrectionsChk");
						this->Checkbox("Resolve head from body hits and use it for prediction", "BloodResolverChk");
						this->Checkbox("Backtrack to head resolved from body hits", "BacktrackToBloodChk");
						this->SlideInt("No Spread Resolver", "WhaleDongTxt", 64);

					}
						break;

					default:
					{
						
						{
							this->Checkbox("Enable", "aimbotEnable");
							this->Combo("Aimbot Key", "AimbotKeyTxt", Keys, ARRAYSIZE(Keys));
							this->Combo("PSilent Key", "pSilentKeyTxt", Keys, ARRAYSIZE(Keys));
							this->Checkbox("Hold key to aim", "AimbotHoldKeyChk");
							this->Checkbox("Lock onto Hitgroups Only", "LockOnToHitgroupsOnlyChk");
							this->Checkbox("Track X", "AimbotAimXChk");
							this->Checkbox("Track Y", "AimbotAimYChk");
							this->Checkbox("Aim Head + Neck", "AimbotAimHeadNeckChk");
							this->Checkbox("Aim Torso", "AimbotAimTorsoChk");
							this->Checkbox("Aim Arms", "AimbotAimArmsHandsChk");
							this->Checkbox("Aim Legs", "AimbotAimLegsFeetChk");
							this->Checkbox("MultiPoint Head", "AimbotMultiPointHeadNeckChk");
							this->Checkbox("MultiPoint Torso", "AimbotMultiPointChestTorsoPelvisChk");
						}
						ImGui::NextColumn();
						ImGui::SetColumnOffset(2, Offsets * 2);
						{
							this->Checkbox("MultiPoint Arms", "AimbotMultiPointShoulderArmsHandsChk");
							this->Checkbox("MultiPoint Legs", "AimbotMultiPointLegsFeetChk");
							this->Checkbox("Multi-Point Targetting", "AimbotMultiPointChk");
							this->Checkbox("Only MultiPoint Priority", "AimbotMultiPointPrioritiesOnlyChk");
							this->Checkbox("Body Aim if Lethal", "BodyAimIfLethalChk");
							this->Checkbox("Detect Peekers", "DetectPeekersChk");
							this->Checkbox("Headshot only Peekers", "HeadshotOnlyPeekersChk");
							this->Checkbox("Silent Aim", "SilentAimChk");
							this->Checkbox("PSilent", "pSilentChk");
							this->Checkbox("Expensive Arms/Legs/Feet Calcs", "AimbotHitchanceArmsLegsFeetChk");
							this->Checkbox("Legit Aim Step", "LegitAimStepChk");
							this->Checkbox("Rage Aim Step", "RageAimStepChk");
							this->Checkbox("Auto Bone Best Damage", "AutoBoneBestDamageChk");
						}

						ImGui::NextColumn();
						ImGui::SetColumnOffset(3, Offsets * 3);
						{
							this->SlideFloat("Fov", "AimbotFOVTxt", 0, 180);
							this->SlideFloat("Psilent Fov", "pSilentFOVTxt", 0, 180);
							this->SlideFloat("Speed", "AimbotSpeedTxt", 0, 180);
							this->SlideFloat("Baim Below Health", "BodyAimBelowThisHealthTxt", 0, 100);
							this->SlideFloat("Max Targets Per Tick", "MaxTargetsPerTickTxt", 0, 20);
							this->SlideFloat("TargetFPS", "TargetFPSTxt", 0, 400);
						}
					}
					break;

					}
					ImGui::EndChild();
				}
			}
			if (CurrentTab == 2)
			{
				if (ImGui::BeginChild("Visuals", ImVec2(600, 300), true))
				{
					ImGui::Columns(3, "Visuals", true);
					float Offsets = ImGui::GetWindowWidth() / 3;
					ImGui::SetColumnOffset(1, Offsets);
					{
						this->Combo("Third Person Toggle Key", "ThirdPersonToggleKeyTxt", Keys, ARRAYSIZE(Keys));
						this->Checkbox("Third Person Z Offset", "third_person_z_offset");
						this->Checkbox("Show Faked Angles Other Players See", "show_faked_angles_other_players_see");
						this->Checkbox("Show Actual Angles", "show_actual_angles");
						this->Combo("Airstuck Key", "AirstuckKeyTxt", Keys, ARRAYSIZE(Keys));
						this->SlideInt("FOV Changer", "FOVChangerTxt", 179);
						this->SlideInt("View Model FOV Changer", "ViewModelFOVChangerTxt", 179);
						this->Checkbox("Glow", "glow");
						this->Checkbox("ESP", "esp");
					}
					ImGui::NextColumn();
					ImGui::SetColumnOffset(2, Offsets * 2);
					{
						this->Checkbox("Player Box ESP", "player_box_esp");
						this->Checkbox("Minimap Enemies Spotted", "mimimap_enemies_spotted");
						this->Checkbox("Draw Player Spectating You", "drar_player_spectating_you");
						this->Checkbox("Draw Recoil Crosshair", "draw_recoil_crosshair");
						this->Checkbox("Draw Damage Given", "draw_damage_given");
						this->Checkbox("Draw Aimbot Bone", "draw_aimbot_bone");
						this->Checkbox("Disable Post Processing(FPS Boost)", "disable_post_processiong");
						this->Checkbox("Disable Dynamic Lighting(FPS Boost)", "disable_dynamic_lighting");
					}
					ImGui::NextColumn();
					ImGui::SetColumnOffset(3, Offsets * 3);
					{
						this->Checkbox("No Flash", "no_flash");
						this->Checkbox("Gernade Trajectory ESP", "gernade_trajectory_esp");
						this->Checkbox("Hitbox ESP", "hitbox_esp");
						this->Checkbox("Aim ESP", "aim_esp");
						this->Checkbox("Highlight Targets", "highlight_targets");
						this->Checkbox("Weapon / Bomb ESP", "weapon_/_bomb_esp");
						this->Checkbox("Draw Head from Resolved Body Hits", "draw_head_from_resolved_body_hits");
						this->Checkbox("Draw Resolve Mode", "draw_resolved_mode");
						this->Checkbox("No Scope", "no_scope");
						this->Checkbox("Footstep ESP", "footstep_esp");
						this->Checkbox("Arrow ESP", "arrow_esp");
					}
				}
				ImGui::EndChild();

			}
			if (CurrentTab == 3)
			{
				if (ImGui::BeginChild("Misc", ImVec2(600, 300), true))
				{
					ImGui::Columns(3, "Misc", true);
					float Offsets = ImGui::GetWindowWidth() / 3;
					ImGui::SetColumnOffset(1, Offsets);
					{
						this->SlideFloat("No Recoil X Percent", "NoRecoilXPercentTxt",0, 100);
						this->SlideFloat("No Recoil Y Percent", "NoRecoilYPercentTxt",0, 100);
						this->Checkbox("No Recoil", "NoRecoilChk");
						this->Checkbox("No Visual Recoil", "NoVisualRecoilChk");
						this->Checkbox("Rage No Recoil", "NoRecoilRageChk");
						this->Checkbox("Allow No Recoil on Pistols/Shotguns/Sniper", "NoRecoilPistolsShotgunsSnipersChk");
					}
					ImGui::NextColumn();
					ImGui::SetColumnOffset(2, Offsets * 2);
					{
						this->Combo("Fake Lag", "FakeLag", "off\0Static FakeLag\0LagComp\0Step\0Random\0");
						this->SlideFloat("Fake Lag Value", "FakeLagChokedTxt", 0, 16);
						this->Checkbox("Auto Grief", "AutoGriefChk");
						this->Checkbox("Engine Prediction", "EnginePredictChk");
						this->Checkbox("Remove Interpolation", "RemoveInterpChk");


					}
					ImGui::NextColumn();
					ImGui::SetColumnOffset(3, Offsets * 3);
					{
						this->Checkbox("BHop", "BhopChk");
						this->Checkbox("Rage BHop", "RageBhopChk");
						this->Checkbox("Auto Wall", "AutoWallChk");
						this->Checkbox("Penetrate Teammates to hit enemies", "AutoWallPenetrateTeammatesChk");
						this->Checkbox("Target Teammates", "TargetTeammatesChk");

					}
				}
				ImGui::EndChild();

			}

			if (CurrentTab == 5)
			{
				//ImGui::SameLine();
				if (ImGui::BeginChild("Colors", ImVec2(600, 300), true))
				{
					static int Item = 0;
					ImGui::Combo("Feature", &Item, "Team\0Enemy\0Team Vis\0Enemy Vis\0");
					switch (Item)
					{
					case 1:
						this->ColorPicker("Enemy", "esp_r", "esp_g", "esp_b");
						break;
					case 2:
						this->ColorPicker("Team Vis", "esp_team_r_vis", "esp_team_g_vis", "esp_team_b_vis");
						break;
					case 3:
						this->ColorPicker("Enemy Vis", "esp_r_vis", "esp_g_vis", "esp_b_vis");
						break;
					default:
						this->ColorPicker("Team", "esp_team_r", "esp_team_g", "esp_team_b");
						break;
					}
					ImGui::EndChild();
				}
			}
			if (CurrentTab == 6)
			{
				//	ImGui::SameLine();
				if (ImGui::BeginChild("Configs", ImVec2(600, 300), true))
				{
					

					ImGui::SameLine();

					


					ImGui::EndChild();
				}
			}
			ImGui::Columns(1);
			ImGui::End();
		}
		ImGui::Render();
	}
}

bool MutinyFrame::UserInterface::WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	if (!this)
		return false;
	auto& io = ImGui::GetIO();
	switch (uMsg)
	{

	case WM_LBUTTONDOWN:
		PressedKey[VK_LBUTTON] = true;
		break;
	case WM_LBUTTONUP:
		PressedKey[VK_LBUTTON] = false;
		break;
	case WM_RBUTTONDOWN:
		PressedKey[VK_RBUTTON] = true;
		break;
	case WM_RBUTTONUP:
		PressedKey[VK_RBUTTON] = false;
		break;
	case WM_KEYDOWN:
		PressedKey[wParam] = true;
		break;
	case WM_KEYUP:
		PressedKey[wParam] = false;
		break;
	case WM_MOUSEMOVE:
		io.MousePos.x = static_cast<signed short>(lParam);
		io.MousePos.y = static_cast<signed short>(lParam >> 16);
		break;
	default: break;
	}
	{
		static bool ShouldShow, isPressed = false;
		if (PressedKey[VK_INSERT])
		{
			isPressed = true;
			ShouldShow = false;
		}
		else if (!PressedKey[VK_INSERT] && isPressed)
		{
			ShouldShow = true;
			isPressed = false;
		}
		else
		{
			ShouldShow = false;
			isPressed = false;		
		}

		if (ShouldShow)
		{
			MenuOpen = !MenuOpen;
		}

	}
	if (this->Initialized && MenuOpen && ImGui_ImplDX9_WndProcHandler(hWnd, uMsg, wParam, lParam))
		return true;
	return false;
}

void MutinyFrame::UserInterface::Checkbox(std::string Option, std::string strOption)
{

	if (ImGui::Checkbox(Option.c_str(), 0));
}

void MutinyFrame::UserInterface::InputInt(std::string Option, std::string strOption)
{

	if (ImGui::InputInt(Option.c_str(), 0));
}

void MutinyFrame::UserInterface::SlideInt(std::string Option, std::string strOption, int Max)
{

	if (ImGui::SliderInt(Option.c_str(), 0, 0, Max));
}
void MutinyFrame::UserInterface::SlideInt(std::string Option, std::string strOption, int Min, int Max)
{

	if (ImGui::SliderInt(Option.c_str(), 0, Min, Max));
}
void MutinyFrame::UserInterface::InputFloat(std::string Option, std::string strOption)
{

	if (ImGui::InputFloat(Option.c_str(), 0));
}
void MutinyFrame::UserInterface::SlideFloat(std::string Option, std::string strOption, float Max)
{

	if (ImGui::SliderFloat(Option.c_str(), 0, 0, Max));
}
void MutinyFrame::UserInterface::SlideFloat(std::string Option, std::string strOption, float Min, float Max)
{

	if (ImGui::SliderFloat(Option.c_str(), 0, Min, Max));
}



void MutinyFrame::UserInterface::Combo(std::string Option, std::string strOption, const char* Options)
{

	if (ImGui::Combo(Option.c_str(), 0, Options));
}
void MutinyFrame::UserInterface::Combo(std::string Option, std::string strOption, const char* const* items, int items_count)
{

	if (ImGui::Combo(Option.c_str(), 0, items, items_count));
}

void MutinyFrame::UserInterface::ColorPicker(std::string strOption, std::string r, std::string g, std::string b)
{
	float Color[3] = {
		0,
		120,
		255
	};

}
#endif
#endif // NewMenu
