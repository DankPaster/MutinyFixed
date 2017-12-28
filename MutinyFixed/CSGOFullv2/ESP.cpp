#include "ESP.h"
#include "CreateMove.h"
#include "EncryptString.h"
#include "BaseCombatWeapon.h"
#include "LocalPlayer.h"
#include "ClassIDS.h"

void DrawGlowESP(TEAMS team, int GlowIndex, bool IsLockedOn)
{
	const float GlowTerroristRed = 1.0f;
	const float GlowTerroristGreen = 0.6f;
	const float GlowTerroristBlue = 0.3f;
	const float GlowTerroristAlpha = 0.6f;

	const float GlowCounterTerroristRed = 0.0f;
	const float GlowCounterTerroristGreen = 0.5f;
	const float GlowCounterTerroristBlue = 0.7f;
	const float GlowCounterTerroristAlpha = 0.7f;

	const float GlowErrorRed = 1.0f;
	const float GlowErrorGreen = 1.0f;
	const float GlowErrorBlue = 1.0f;
	const float GlowErrorAlpha = 0.7f;

	GlowObjectDefinition_t *GlowPointer = (GlowObjectDefinition_t*)GetGlowObject();
	if (GlowPointer)
	{
		GlowObjectDefinition_t *target = &GlowPointer[GlowIndex];
		if (target && target->m_pEntity > 0)
		{
			if (GlowTargetsChk.Checked && IsLockedOn)
			{
				WriteFloat((uintptr_t)&target->m_flGlowRed, 1.0f);
				WriteFloat((uintptr_t)&target->m_flGlowGreen, 0.0f);
				WriteFloat((uintptr_t)&target->m_flGlowBlue, 0.0f);
				WriteFloat((uintptr_t)&target->m_flGlowAlpha, 0.8f);
				WriteByte((uintptr_t)&target->m_bRenderWhenOccluded, true);
				WriteByte((uintptr_t)&target->m_bRenderWhenUnoccluded, false);
			}
			else
			{
				switch (team)	// 1 GoTV; 2 T; 3 CT
				{
				case TEAM_T:
					WriteFloat((uintptr_t)&target->m_flGlowRed, GlowTerroristRed);
					WriteFloat((uintptr_t)&target->m_flGlowGreen, GlowTerroristGreen);
					WriteFloat((uintptr_t)&target->m_flGlowBlue, GlowTerroristBlue);
					//if (!EnableHack)
					//WriteFloat((GlowPointer + ((GlowIndex * 0x38) + 0x10)), 0.0f);
					//else
					WriteFloat((uintptr_t)&target->m_flGlowAlpha, 0.6f);
					WriteByte((uintptr_t)&target->m_bRenderWhenOccluded, true);
					WriteByte((uintptr_t)&target->m_bRenderWhenUnoccluded, false);
					break;
				case TEAM_CT:
					WriteFloat((uintptr_t)&target->m_flGlowRed, GlowCounterTerroristRed);
					WriteFloat((uintptr_t)&target->m_flGlowGreen, GlowCounterTerroristGreen);
					WriteFloat((uintptr_t)&target->m_flGlowBlue, GlowCounterTerroristBlue);
					//if (!EnableHack)
					//WriteFloat((GlowPointer + ((GlowIndex * 0x38) + 0x10)), 0.0f);
					//else
					WriteFloat((uintptr_t)&target->m_flGlowAlpha, GlowCounterTerroristAlpha);
					WriteByte((uintptr_t)&target->m_bRenderWhenOccluded, true);
					WriteByte((uintptr_t)&target->m_bRenderWhenUnoccluded, false);
					break;
				default:
					WriteFloat((uintptr_t)&target->m_flGlowRed, GlowErrorRed);
					WriteFloat((uintptr_t)&target->m_flGlowGreen, GlowErrorGreen);
					WriteFloat((uintptr_t)&target->m_flGlowBlue, GlowErrorBlue);
					//if (!EnableHack)
					//WriteFloat((GlowPointer + ((GlowIndex * 0x38) + 0x10)), 0.0f);
					//else
					WriteFloat((uintptr_t)&target->m_flGlowAlpha, GlowErrorAlpha);
					WriteByte((uintptr_t)&target->m_bRenderWhenOccluded, true);
					WriteByte((uintptr_t)&target->m_bRenderWhenUnoccluded, false);
					break;
				}
			}
		}
	}
}
char *healthstr = new char[46]{ 95, 9, 112, 50, 31, 27, 22, 14, 18, 64, 90, 95, 19, 112, 59, 8, 23, 21, 8, 64, 90, 95, 19, 112, 50, 31, 22, 23, 31, 14, 64, 90, 95, 19, 112, 45, 31, 27, 10, 21, 20, 64, 90, 95, 41, 0 }; /*%s
																																																						  Health: %i
																																																						  Armor: %i
																																																						  Helmet: %i
																																																						  Weapon: %S*/char *healthstr2 = new char[8]{ 50, 31, 27, 22, 14, 18, 64, 0 }; /*%s\nHealth: %i*/
char *formatresolvemodestr = new char[17]{ 40, 31, 9, 21, 22, 12, 31, 90, 55, 21, 30, 31, 64, 90, 95, 9, 0 }; /*Resolve Mode: %s*/

char *automaticyawstr2 = new char[14]{ 59, 15, 14, 21, 23, 27, 14, 19, 25, 90, 35, 27, 13, 0 }; /*Automatic Yaw*/
char *fakespinstr2 = new char[20]{ 60, 27, 17, 31, 90, 41, 22, 21, 13, 85, 60, 27, 9, 14, 90, 41, 10, 19, 20, 0 }; /*Fake Slow/Fast Spin*/
char *linearfakeyawstr2 = new char[12]{ 54, 19, 20, 31, 27, 8, 90, 60, 27, 17, 31, 0 }; /*Linear Fake*/
char *randomfakeyawstr2 = new char[12]{ 40, 27, 20, 30, 21, 23, 90, 60, 27, 17, 31, 0 }; /*Random Fake*/
char *staticantiresolveyawstr2 = new char[18]{ 59, 20, 14, 19, 87, 40, 31, 9, 21, 22, 12, 31, 90, 60, 27, 17, 31, 0 }; /*Anti-Resolve Fake*/
char *bodyrealdeltastr2 = new char[16]{ 56, 21, 30, 3, 90, 40, 31, 27, 22, 90, 62, 31, 22, 14, 27, 0 }; /*Body Real Delta*/
char *eyerealdeltastr2 = new char[15]{ 63, 3, 31, 90, 40, 31, 27, 22, 90, 62, 31, 22, 14, 27, 0 }; /*Eye Real Delta*/
char *lastrealyawstr2 = new char[14]{ 54, 27, 9, 14, 90, 40, 31, 27, 22, 90, 35, 27, 13, 0 }; /*Last Real Yaw*/
char *attargetstr2 = new char[10]{ 59, 14, 90, 46, 27, 8, 29, 31, 14, 0 }; /*At Target*/
char *inverseattargetstr2 = new char[18]{ 51, 20, 12, 31, 8, 9, 31, 90, 59, 14, 90, 46, 27, 8, 29, 31, 14, 0 }; /*Inverse At Target*/
char *avglowerbodydeltastr2 = new char[21]{ 59, 12, 29, 90, 54, 21, 13, 31, 8, 90, 56, 21, 30, 3, 90, 62, 31, 22, 14, 27, 0 }; /*Avg Lower Body Delta*/
char *staticdynamicfakestr2 = new char[20]{ 41, 14, 27, 14, 19, 25, 90, 60, 27, 17, 31, 90, 62, 3, 20, 27, 23, 19, 25, 0 }; /*Static Fake Dynamic*/
char *fakewalkstr1 = new char[10]{ 60, 27, 17, 31, 90, 45, 27, 22, 17, 0 }; /*Fake Walk*/
char *staticfakeyawstr2 = new char[12]{ 41, 14, 27, 14, 19, 25, 90, 60, 27, 17, 31, 0 }; /*Static Fake*/
char *inverseyawstr2 = new char[8]{ 51, 20, 12, 31, 8, 9, 31, 0 }; /*Inverse*/
char *yawforceback2 = new char[11]{ 60, 21, 8, 25, 31, 90, 56, 27, 25, 17, 0 }; /*Force Back*/
char *yawleftstr2 = new char[11]{ 60, 21, 8, 25, 31, 90, 54, 31, 28, 14, 0 }; /*Force Left*/
char *yawrightstr2 = new char[12]{ 60, 21, 8, 25, 31, 90, 40, 19, 29, 18, 14, 0 }; /*Force Right*/
char *whaeldongstr = new char[11]{ 45, 18, 27, 22, 31, 90, 62, 21, 20, 29, 0 }; /*Whale Dong*/
char *BackTrackFirestr = new char[14]{ 56, 27, 25, 17, 46, 8, 27, 25, 17, 60, 19, 8, 31, 0 }; /*BackTrackFire*/
char *BackTrackHitstr = new char[13]{ 56, 27, 25, 17, 46, 8, 27, 25, 17, 50, 19, 14, 0 }; /*BackTrackHit*/
char *BackTrackRealstr = new char[14]{ 56, 27, 25, 17, 46, 8, 27, 25, 17, 40, 31, 27, 22, 0 }; /*BackTrackReal*/
char *BackTrackLbystr = new char[13]{ 56, 27, 25, 17, 46, 8, 27, 25, 17, 54, 24, 3, 0 }; /*BackTrackLby*/
char *BackTrackUpstr = new char[12]{ 56, 27, 25, 17, 46, 8, 27, 25, 17, 47, 10, 0 }; /*BackTrackUp*/
char *NotBackTrackedstr = new char[15]{ 52, 21, 14, 56, 27, 25, 17, 46, 8, 27, 25, 17, 31, 30, 0 }; /*NotBackTracked*/
char *WD_StaticFakestr = new char[14]{ 45, 62, 37, 41, 14, 27, 14, 19, 25, 60, 27, 17, 31, 0 }; /*WD_StaticFake*/
char *WD_Velocitystr = new char[12]{ 45, 62, 37, 44, 31, 22, 21, 25, 19, 14, 3, 0 }; /*WD_Velocity*/
char *WD_Targetstr = new char[10]{ 45, 62, 37, 46, 27, 8, 29, 31, 14, 0 }; /*WD_Target*/
char *WD_Spinstr = new char[8]{ 45, 62, 37, 41, 10, 19, 20, 0 }; /*WD_Spin*/
char *WD_Jitterstr = new char[10]{ 45, 62, 37, 48, 19, 14, 14, 31, 8, 0 }; /*WD_Jitter*/
char *WD_Staticstr = new char[10]{ 45, 62, 37, 41, 14, 27, 14, 19, 25, 0 }; /*WD_Static*/
char *WD_FuckItstr = new char[10]{ 45, 62, 37, 60, 15, 25, 17, 51, 14, 0 }; /*WD_FuckIt*/
char *notfakedstr = new char[10]{ 52, 21, 14, 90, 60, 27, 17, 31, 30, 0 }; /*Not Faked*/

char *nonestr = new char[5]{ 52, 21, 20, 31, 0 }; /*None*/
char *otherstr2 = new char[6]{ 53, 14, 18, 31, 8, 0 }; /*Other*/
char *backtrackedstr = new char[12]{ 56, 27, 25, 17, 14, 8, 27, 25, 17, 31, 30, 0 }; /*Backtracked*/
void SkeltonEsp(CustomPlayer* player)
{
	Vector W2S;
	skeleton data;

	float time = Interfaces::Globals->curtime;
	for (int i = 0; i < 10; i++)
	{

		if (WorldToScreenCapped(player->BaseEntity->GetBonePosition(HITBOX_HEAD, Interfaces::Globals->curtime, false, false), W2S) == TRUE)
			data.head = Vector2D(W2S.x, W2S.y);
		else return;

		if (WorldToScreenCapped(player->BaseEntity->GetBonePosition(HITBOX_NECK, time, false, false), W2S) == TRUE)
			data.neck = Vector2D(W2S.x, W2S.y);
		if (WorldToScreenCapped(player->BaseEntity->GetBonePosition(HITBOX_CHEST, time, false, false), W2S) == TRUE)
			data.chest = Vector2D(W2S.x, W2S.y);
		if (WorldToScreenCapped(player->BaseEntity->GetBonePosition(HITBOX_PELVIS, time, false, false), W2S) == TRUE)
			data.pelvis = Vector2D(W2S.x, W2S.y);
		if (WorldToScreenCapped(player->BaseEntity->GetBonePosition(HITBOX_LEFT_UPPER_ARM, time, false, false), W2S) == TRUE)
			data.leftShoulder = Vector2D(W2S.x, W2S.y);
		if (WorldToScreenCapped(player->BaseEntity->GetBonePosition(HITBOX_RIGHT_UPPER_ARM, time, false, false), W2S) == TRUE)
			data.rightShoulder = Vector2D(W2S.x, W2S.y);
		if (WorldToScreenCapped(player->BaseEntity->GetBonePosition(HITBOX_LEFT_FOREARM, time, false, false), W2S) == TRUE)
			data.leftElbow = Vector2D(W2S.x, W2S.y);
		if (WorldToScreenCapped(player->BaseEntity->GetBonePosition(HITBOX_RIGHT_FOREARM, time, false, false), W2S) == TRUE)
			data.rightElbow = Vector2D(W2S.x, W2S.y);
		if (WorldToScreenCapped(player->BaseEntity->GetBonePosition(HITBOX_LEFT_HAND, time, false, false), W2S) == TRUE)
			data.leftHand = Vector2D(W2S.x, W2S.y);
		if (WorldToScreenCapped(player->BaseEntity->GetBonePosition(HITBOX_RIGHT_HAND, time, false, false), W2S) == TRUE)
			data.rightHand = Vector2D(W2S.x, W2S.y);
		if (WorldToScreenCapped(player->BaseEntity->GetBonePosition(HITBOX_LEFT_CALF, time, false, false), W2S) == TRUE)
			data.leftKnee = Vector2D(W2S.x, W2S.y);
		if (WorldToScreenCapped(player->BaseEntity->GetBonePosition(HITBOX_RIGHT_CALF, time, false, false), W2S) == TRUE)
			data.rightKnee = Vector2D(W2S.x, W2S.y);
		if (WorldToScreenCapped(player->BaseEntity->GetBonePosition(HITBOX_LEFT_FOOT, time, false, false), W2S) == TRUE)
			data.leftFoot = Vector2D(W2S.x, W2S.y);
		if (WorldToScreenCapped(player->BaseEntity->GetBonePosition(HITBOX_RIGHT_FOOT, time, false, false), W2S) == TRUE)
			data.rightFoot = Vector2D(W2S.x, W2S.y);

		SkeletonEsp(data, 0, 255, 0, 255);
		int temptime = TIME_TO_TICKS(Interfaces::Globals->curtime);
		temptime -= i;
		time = TICKS_TO_TIME(temptime);
	}
}

char *revolverstr = new char[9]{ 40, 31, 12, 21, 22, 12, 31, 8, 0 }; /*Revolver*/


void DrawESP(CBaseEntity* LocalEntity, CBaseEntity *Entity, CustomPlayer* player, bool IsLockedOn)
{
	Vector W2S;
	Vector W2S_TargetPos;
	Vector HeadOrigin;

	//FIXME: SOMETHING IS CAUSING MEMORY CORRUPTION IN THIS FUNCTION WHEN VIEWING DEMO AFTER A GAME, CAUSING BASEENTITY TO BECOME NULL
	CBaseEntity* LocalEntityT = Interfaces::ClientEntList->GetClientEntity(Interfaces::Engine->GetLocalPlayer());
	if (!LocalEntityT)
		return;

	if (player->BaseEntity == nullptr)
		return;

	HeadOrigin = Entity->GetEyePosition(); //Randomly crashes: player->BaseEntity->GetBonePosition(HITBOX_HEAD, player->BaseEntity->GetSimulationTime());
	HeadOrigin.z += 8.0f;
	Vector Origin = Entity->GetOrigin();
	Origin.z -= 5.0f;

	if (WorldToScreenCapped(Origin, W2S) == TRUE && WorldToScreenCapped(HeadOrigin, W2S_TargetPos) == TRUE)
	{
		if (Entity->GetAlive())
		{
			int R, G, B;
			if (GlowTargetsChk.Checked && IsLockedOn)
			{
				R = 255; G = 0; B = 0;
			}
			else if (Entity->GetTeam() == TEAM_CT)
			{
				R = 0; G = 162; B = 232;
			}
			else
			{
				R = 255; G = 128; B = 0;
			}

			float height = fabsf(W2S[1] - W2S_TargetPos[1]);
			float width = height * 0.5f;

			if (BoxESPChk.Checked)
			{
				//SkeltonEsp(player);
				//Too laggy may hook paintraverse it seems to not hit fps so hard :)
				DrawBox(Vector2D(W2S.x - (width * 0.5f), W2S.y - height), width, height, 2, R, G, B, 255);
			}

			if (FootstepESPChk.Checked)
			{
				if (Interfaces::Globals->tickcount - player->iLastFootStepTick <= 128)
				{
					Vector footstepscrn;
					if (WorldToScreenCapped(player->vecLastFootStepOrigin, footstepscrn))
					{
						DrawBox(Vector2D(footstepscrn.x - 3, footstepscrn.y - 3), 6, 6, 2, R, G, B, 255);
					}
				}
			}

			if (ArrowESPChk.Checked)
			{
				QAngle angs = Entity->GetEyeAngles();
				Vector vecForward, vecRight, vecUp;
				AngleVectors(angs, &vecForward, &vecRight, &vecUp);

				Vector ForwardSide = vecForward * 25.0f;
				Vector Right = vecRight * 8.0f;

				Vector HeadOriginUp = Vector(HeadOrigin.x, HeadOrigin.y, HeadOrigin.z + 15.0f);

				Vector WorldLineForward = HeadOriginUp + (vecForward * 50.0f);
				Vector WorldLineLeft = HeadOriginUp + ForwardSide - Right;
				Vector WorldLineRight = HeadOriginUp + ForwardSide + Right;

				Vector ScreenHeadUp, ScreenLineForward, ScreenLineLeft, ScreenLineRight;

				WorldToScreenCapped(Vector(HeadOriginUp.x, HeadOriginUp.y, HeadOriginUp.z + 5.0f), ScreenHeadUp);
				WorldToScreenCapped(WorldLineForward, ScreenLineForward);
				WorldToScreenCapped(WorldLineLeft, ScreenLineLeft);
				WorldToScreenCapped(WorldLineRight, ScreenLineRight);

				DrawLine(Vector2D(ScreenHeadUp.x, ScreenHeadUp.y), Vector2D(ScreenLineForward.x, ScreenLineForward.y), 221, 179, 221, 255);
				DrawLine(Vector2D(ScreenHeadUp.x, ScreenHeadUp.y + 1), Vector2D(ScreenLineForward.x, ScreenLineForward.y + 1), 221, 179, 221, 255);
				DrawLine(Vector2D(ScreenHeadUp.x, ScreenHeadUp.y + 2), Vector2D(ScreenLineForward.x, ScreenLineForward.y + 2), 221, 179, 221, 255);

				DrawLine(Vector2D(ScreenLineForward.x, ScreenLineForward.y), Vector2D(ScreenLineLeft.x, ScreenLineLeft.y), 241, 224, 241, 255);
				DrawLine(Vector2D(ScreenLineForward.x, ScreenLineForward.y + 1), Vector2D(ScreenLineLeft.x, ScreenLineLeft.y + 1), 241, 224, 241, 255);
				DrawLine(Vector2D(ScreenLineForward.x, ScreenLineForward.y + 2), Vector2D(ScreenLineLeft.x, ScreenLineLeft.y + 2), 241, 224, 241, 255);

				DrawLine(Vector2D(ScreenLineForward.x, ScreenLineForward.y), Vector2D(ScreenLineRight.x, ScreenLineRight.y), 241, 224, 241, 255);
				DrawLine(Vector2D(ScreenLineForward.x, ScreenLineForward.y + 1), Vector2D(ScreenLineRight.x, ScreenLineRight.y + 1), 241, 224, 241, 255);
				DrawLine(Vector2D(ScreenLineForward.x, ScreenLineForward.y + 2), Vector2D(ScreenLineRight.x, ScreenLineRight.y + 2), 241, 224, 241, 255);
			}

			if (DrawResolveModeChk.Checked)
			{
				Vector resolvescreen;
				WorldToScreenCapped(Vector(HeadOrigin.x, HeadOrigin.y, HeadOrigin.z + 15.0f), resolvescreen);

				char resolvestring[72];
				char *destinationstring;

				switch (player->Personalize.LastResolveModeUsed)
				{
				case NoYaw:
					destinationstring = nonestr;
					break;
				case FakeSpins:
					destinationstring = fakespinstr2;
					break;
				case LinearFake:
					destinationstring = linearfakeyawstr2;
					break;
				case RandomFake:
					destinationstring = randomfakeyawstr2;
					break;
				case CloseFake:
					destinationstring = staticantiresolveyawstr2;
					break;
				case BloodBodyRealDelta:
					destinationstring = bodyrealdeltastr2;
					break;
				case BloodEyeRealDelta:
					destinationstring = eyerealdeltastr2;
					break;
				case BloodReal:
					destinationstring = lastrealyawstr2;
					break;
				case AtTarget:
					destinationstring = attargetstr2;
					break;
				case InverseAtTarget:
					destinationstring = inverseattargetstr2;
					break;
				case AverageLBYDelta:
					destinationstring = avglowerbodydeltastr2;
					break;
				case StaticFakeDynamic:
					destinationstring = staticdynamicfakestr2;
					break;
				case FakeWalk:
					destinationstring = fakewalkstr1;
					break;
				case StaticFake:
					destinationstring = staticfakeyawstr2;
					break;
				case Inverse:
					destinationstring = inverseyawstr2;
					break;
				case Back:
					destinationstring = yawforceback2;
					break;
				case Left:
					destinationstring = yawleftstr2;
					break;
				case Right:
					destinationstring = yawrightstr2;
					break;
				case WD:
					destinationstring = whaeldongstr;
					break;
				case BackTrackFire:
					destinationstring = BackTrackFirestr;
					break;
				case BackTrackUp:
					destinationstring = BackTrackUpstr;
					break;
				case BackTrackHit:
					destinationstring = BackTrackHitstr;
					break;
				case BackTrackReal:
					destinationstring = BackTrackRealstr;
					break;
				case BackTrackLby:
					destinationstring = BackTrackLbystr;
					break;
				case Backtracked:
					destinationstring = backtrackedstr;
					break;
				case NotBackTracked:
					destinationstring = NotBackTrackedstr;
					break;
				case WD_StaticFake:
					destinationstring = WD_StaticFakestr;
					break;
				case WD_Velocity:
					destinationstring = WD_Velocitystr;
					break;
				case WD_Target:
					destinationstring = WD_Targetstr;
					break;
				case WD_Spin:
					destinationstring = WD_Spinstr;
					break;
				case WD_Jitter:
					destinationstring = WD_Jitterstr;
					break;
				case WD_Static:
					destinationstring = WD_Staticstr;
					break;
				case WD_FuckIt:
					destinationstring = WD_FuckItstr;
					break;
				//case NotFaked:
				//	destinationstring = notfakedstr;
					break;
				default:
					destinationstring = otherstr2;
					break;
				}

				DecStr(formatresolvemodestr, 16);
				int len = strlen(destinationstring);
				DecStr(destinationstring, len);
				sprintf(resolvestring, formatresolvemodestr, destinationstring);
				EncStr(destinationstring, len);
				EncStr(formatresolvemodestr, 16);
				ColorRGBA colour = ColorRGBA(0, 255, 0, 255);
				if (player->IsBreakingLagCompensation)
					colour = ColorRGBA(255, 0, 0, 255);

				DrawStringUnencrypted(resolvestring, Vector2D(resolvescreen.x - 50, resolvescreen.y), colour, pFontSmall);

			}

			if (ESPChk.Checked)
			{
				//DWORD CSPlayerResource = GetCSPlayerResource();

				wchar_t PlayerName[32];
				DWORD Radar = GetRadar();
				if (Radar)
					GetPlayerName(Radar, Entity->index, PlayerName);
				else
					wcscpy(PlayerName, L"Error");


				/*---- Bone ESP ----*/
				//Crotch to neck
				/*
				DrawBone(0, 5, player->BoneMatrix);

				//Left Arm
				DrawBone(5, 8, player->BoneMatrix);
				DrawBone(8, 9, player->BoneMatrix);
				DrawBone(9, 2, player->BoneMatrix);
				DrawBone(2, 10, player->BoneMatrix);

				//Right Arm
				DrawBone(5, 36, player->BoneMatrix);
				DrawBone(36, 37, player->BoneMatrix);
				DrawBone(37, 60, player->BoneMatrix);
				DrawBone(60, 38, player->BoneMatrix);

				//Left Leg
				DrawBone(0, 64, player->BoneMatrix);
				DrawBone(64, 65, player->BoneMatrix);
				DrawBone(65, 66, player->BoneMatrix);

				//Right Leg
				DrawBone(0, 70, player->BoneMatrix);
				DrawBone(70, 71, player->BoneMatrix);
				DrawBone(71, 72, player->BoneMatrix);
				*/
				float DistanceFromPlayer = fabsf(LocalEntity->GetNetworkOrigin().Length2D() - Origin.Length2D());
				wchar_t wstr[1024];

				//char Rankstr[128];
				//GetCompetitiveRankString(Rankstr, 128, player->CompetitiveRank);

				CBaseCombatWeapon *pWeapon = Entity->GetWeapon();
				char *weaponname = nullptr;
				bool IsRevolver = false;
				if (pWeapon)
				{
					WeaponInfo_t *pWeaponInfo = pWeapon->GetCSWpnData();
					if (pWeaponInfo)
					{
						if (pWeapon->GetItemDefinitionIndex() != WEAPON_REVOLVER)
						{
							if (pWeaponInfo->szHudName[0] == '#' && pWeaponInfo->szHudName[12] == '_')
								weaponname = pWeaponInfo->szHudName + 13;
						}
						else
						{
							IsRevolver = true;
							weaponname = revolverstr;
							DecStr(revolverstr, 8);
						}
					}
				}

				wchar_t wformat[92];
				//char format[46];
				DecStr(healthstr, 45);
				//strcpy_s(format, 35, healthstr2);//\nWins: %i\nRank: %S"));
				mbstowcs(wformat, healthstr, 46);
				EncStr(healthstr, 45);
				//wcscpy(format, wstrenc("%s\nHealth: %i\nWins: %i\nRank: %S").c_str()); //fuck this crashing bullshit
				//const wchar_t *format = wcharenc("%s\nHealth: %i\nWins: %i\nRank: %S"); //instant crash
				_snwprintf(wstr, 1024, wformat, PlayerName, Entity->GetHealth(), Entity->GetArmor(), (int)Entity->HasHelmet(), weaponname ? weaponname : " ");
				DrawStringUnencryptedW(wstr, Vector2D(W2S.x - (width * 0.5f), W2S.y), ColorRGBA(R, G, B, 255), DistanceFromPlayer <= 500.0f ? pFont : pFontSmall);
				if (IsRevolver)
					EncStr(revolverstr, 8);
#if 0
				char str2[384];
				char *Rankstr = new char[129];
				GetCompetitiveRankString(Rankstr, 129, player->CompetitiveRank);
				char *HealthString = new char[8];
				strcpy(HealthString, charenc("Health:"));
				char *WinsString = new char[6];
				strcpy(WinsString, charenc("Wins:"));
				char *RankString = new char[6];
				strcpy(RankString, charenc("Rank:"));
				snprintf(str, 384, "%s\n%s %i\n%s %i\n%s %s", PlayerName, HealthString, player->Health, WinsString, player->CompetitiveWins, RankString, Rankstr);//Ranks[player->CompetitiveRank].c_str());
#endif
			}
		}
	}
}

void DrawTextOverlay(int x, int y, char *text, COLORREF color, HFONT font)
{
	int oldleft, oldtop;
	oldleft = rc.left;
	oldtop = rc.top;

	rc.left = x - rc.left;
	rc.top = y - rc.top;
	LPRECT prect = &rc;
	HWND hwnd = GetForegroundWindow();
	HDC layhdc = GetWindowDC(hwnd);
	HFONT OLD_HFONT = (HFONT)SelectObject(layhdc, font);
	SetTextColor(layhdc, color);
	SetBkMode(layhdc, TRANSPARENT);
	DrawTextA(layhdc, (LPCSTR)text, -1, prect, 0);
	//UpdateWindow(hwnd);
	SelectObject(layhdc, OLD_HFONT);
	ReleaseDC(hwnd, layhdc);
	rc.left = oldleft;
	rc.top = oldtop;
}

char *bombstr = new char[5]{ 56, 21, 23, 24, 0 }; /*Bomb*/
char *explodetimestr = new char[21]{ 46, 90, 23, 19, 20, 15, 9, 90, 95, 84, 75, 28, 90, 9, 31, 25, 21, 20, 30, 9, 0 }; /*T minus %.1f seconds*/
char *flashbangstr = new char[21]{ 60, 22, 27, 9, 18, 24, 27, 20, 29, 90, 42, 8, 21, 16, 31, 25, 14, 19, 22, 31, 0 }; /*Flashbang Projectile*/

void DrawEntityESP(CBaseEntity *LocalEntity)
{
	for (int i = (MAX_PLAYERS + 1); i <= Interfaces::ClientEntList->GetHighestEntityIndex(); i++)
	{
		CBaseEntity *pEnt = Interfaces::ClientEntList->GetClientEntity(i);
		if (pEnt && !pEnt->GetDormant() && !pEnt->IsPlayer())
		{
			bool bIsWeapon = pEnt->IsWeapon();
			bool bForceDraw = pEnt->IsProjectile() || pEnt->IsChicken();
			if (bIsWeapon || bForceDraw)
				//TODO: MORE CLEANUP HERE AND GET RID OF ELSE STATEMENT
			{
				CBaseEntity* owner = pEnt->GetOwner();
				if (owner && !owner->IsPlayer())
					owner = nullptr;
				bool IsLockedOn = owner && LastTargetIndex != INVALID_PLAYER && AllPlayers[LastTargetIndex].Index == ReadInt((uintptr_t)&owner->index);

				GlowObjectManager *manager = (GlowObjectManager*)GetGlowObjectManager();
				if (manager)
				{
					for (unsigned i = 0; i < manager->Count; i++)
					{
						GlowObjectDefinition_t *object = &manager->DataPtr[i];
						if (object && (CBaseEntity*)ReadInt((uintptr_t)&object->m_pEntity) == pEnt)
						{
							TEAMS team = owner ? (TEAMS)owner->GetTeam() : TEAM_NONE;
							DrawGlowESP(team, i, IsLockedOn);
							//pEnt->DrawHitboxes(team == TEAM_CT ? ColorRGBA(0, 162, 232, 255) : team == TEAM_T ? ColorRGBA(255, 127, 39, 255) : ColorRGBA(255, 255, 255, 255));
							if (!owner || bForceDraw)
							{
								Vector origin = pEnt->GetOrigin();
								if (origin != vecZero)
								{
									WeaponInfo_t *pWeaponInfo = bIsWeapon ? ((CBaseCombatWeapon*)pEnt)->GetCSWpnData() : nullptr;
									if (pWeaponInfo || bForceDraw)
									{
										Vector screen;
										if (WorldToScreenCapped(pEnt->GetOrigin(), screen))
										{
											if (bIsWeapon)
											{
												if (pWeaponInfo && ReadByte((uintptr_t)&pWeaponInfo->szHudName[0]) == '#' && ReadByte((uintptr_t)&pWeaponInfo->szHudName[12]) == '_')
													DrawStringUnencrypted(pWeaponInfo->szHudName + 13, Vector2D(screen.x, screen.y), ColorRGBA(255, 127, 39, 255), pFontSmall);
											}
											else
											{
												ClientClass* clClass = pEnt->GetClientClass();
												if (clClass)
												{
													if (clClass->m_ClassID != _CBaseCSGrenadeProjectile)
													{
														DrawStringUnencrypted((char*)((DWORD)clClass->m_pNetworkName + 1), Vector2D(screen.x, screen.y), ColorRGBA(255, 127, 39, 255), pFontSmall);
													}
													else
													{
														DecStr(flashbangstr, 20);
														DrawStringUnencrypted(flashbangstr, Vector2D(screen.x, screen.y), ColorRGBA(255, 127, 39, 255), pFontSmall);
														EncStr(flashbangstr, 20);
													}
												}
											}
										}
									}
								}
							}
							break;
						}
					}
				}
			}
			else
			{
				ClientClass *clclass = pEnt->GetClientClass();
				if (clclass)
				{
					int classid = ReadInt((uintptr_t)&clclass->m_ClassID);
					Vector screen;
					Vector bomborigin;
					char timestr[64];
					float flC4Blow;
					float flTimeToExplosion;
					switch (classid)
					{
					case _CPlantedC4:
						flC4Blow = ReadFloat((DWORD)pEnt + m_flC4Blow);
						flTimeToExplosion = (flC4Blow - ReadFloat((uintptr_t)&Interfaces::Globals->curtime));
						DefuseTimeRemaining = flTimeToExplosion;
						if (flTimeToExplosion >= 0.0f)
						{
							if (WorldToScreenCapped(pEnt->GetOrigin(), screen))
							{
								DecStr(bombstr, 4);
								DrawBox(Vector2D(screen.x - 15, screen.y - 20), 30, 30, 1, 255, 255, 0, 255);
								DrawStringUnencrypted(bombstr, Vector2D(screen.x - 18, screen.y + 15), ColorRGBA(255, 255, 0, 255), pFont);
								EncStr(bombstr, 4);
							}
							HANDLE LocalPlayerObserveTarget = LocalEntity->GetObserverTarget();
							CBaseEntity *pEntityToCheckDefuseKit = LocalPlayerObserveTarget ? Interfaces::ClientEntList->GetClientEntityFromHandle((DWORD)LocalPlayerObserveTarget) : LocalEntity;
							if (!pEntityToCheckDefuseKit)
								pEntityToCheckDefuseKit = LocalEntity;
							DecStr(explodetimestr, 20);
							sprintf(timestr, explodetimestr, flTimeToExplosion);
							DrawStringUnencrypted(timestr, Vector2D(CenterOfScreen.x - 70, CenterOfScreen.y - 50), (flTimeToExplosion > 10.0f || (pEntityToCheckDefuseKit->HasDefuseKit() && flTimeToExplosion > 5.0f)) ? greencolor : redcolor, pFont);
							EncStr(explodetimestr, 20);
						}
						if (flTimeToExplosion > -5.0f)
						{
							float flDistance = LocalEntity->GetEyePosition().Dist(pEnt->GetOrigin());
							const float a = 450.7f;
							const float b = 75.68f;
							const float c = 789.2f;
							float d = ((flDistance - b) / c);
							float flDamage = a*exp(-d * d);
							HANDLE LocalPlayerObserveTarget = LocalEntity->GetObserverTarget();
							//NOTE: RANDOM CRASH HERE pEntityToDrawHealth is fucked
							CBaseEntity *pEntityToDrawHealth = LocalPlayerObserveTarget ? Interfaces::ClientEntList->GetClientEntityFromHandle((DWORD)LocalPlayerObserveTarget) : LocalEntity;
							if (!pEntityToDrawHealth)
								pEntityToDrawHealth = LocalEntity;
							int healthafterexplosion = (int)(max((int)pEntityToDrawHealth->GetHealth() - max((int)ceilf(CSGO_Armor(flDamage, pEntityToDrawHealth->GetArmor())), 0), 0));
							sprintf(timestr, "%i", healthafterexplosion);
							DrawStringUnencrypted(timestr, Vector2D(healthafterexplosion >= 100 ? (rc.left + 14) : healthafterexplosion < 10 ? (rc.left + 22) : (rc.left + 18), rc.bottom - 37), healthafterexplosion > 0 ? greencolor : redcolor, pFont);
						}
					case _CC4:
						bomborigin = pEnt->GetOrigin();
						if (bomborigin != vecZero && WorldToScreenCapped(bomborigin, screen))
						{
							DecStr(bombstr, 4);
							DrawBox(Vector2D(screen.x - 15, screen.y - 20), 30, 30, 1, 255, 255, 0, 255);
							DrawStringUnencrypted(bombstr, Vector2D(screen.x - 18, screen.y + 15), ColorRGBA(255, 255, 0, 255), pFont);
							EncStr(bombstr, 4);
							//pEnt->DrawHitboxes(ColorRGBA(255, 255, 0, 255));
						}
					case _CHostage:
					case _CHostageCarriableProp:
						//case _CChicken:
					case _CFish:
						//case _CBaseCSGrenadeProjectile:
						GlowObjectManager *manager = (GlowObjectManager*)GetGlowObjectManager();
						if (manager)
						{
							for (unsigned i = 0; i < manager->Count; i++)
							{
								GlowObjectDefinition_t *object = &manager->DataPtr[i];
								if (object && (CBaseEntity*)ReadInt((uintptr_t)&object->m_pEntity) == pEnt)
								{
									DrawGlowESP(TEAM_T, i, false);
									break;
								}
							}
						}
						break;
					}
				}
			}
		}
	}
}

char *drawdmgformatstr = new char[19]{ 87, 95, 19, 90, 50, 42, 112, 81, 95, 19, 90, 50, 19, 14, 9, 112, 95, 9, 0 }; /*-%i HP+%i Hits%s*/
char *blankstr = new char[2]{ 90, 0 }; /* */
char *headstr = new char[5]{ 50, 31, 27, 30, 0 }; /*Head*/
char *cheststr = new char[6]{ 57, 18, 31, 9, 14, 0 }; /*Chest*/
char *stomachstr = new char[8]{ 41, 14, 21, 23, 27, 25, 18, 0 }; /*Stomach*/
char *leftarmstr = new char[6]{ 54, 90, 59, 8, 23, 0 }; /*L Arm*/
char *rightarmstr = new char[6]{ 40, 90, 59, 8, 23, 0 }; /*R Arm*/
char *leftlegstr = new char[6]{ 54, 90, 54, 31, 29, 0 }; /*L Leg*/
char *rightlegstr = new char[6]{ 40, 90, 54, 31, 29, 0 }; /*R Leg*/
char *otherstr = new char[6]{ 53, 14, 18, 31, 8, 0 }; /*Other*/

void DrawDamageIndicator()
{
	if (ReadFloat((uintptr_t)&Interfaces::Globals->curtime) - flTimeDamageWasGiven < 1.2f)
	{
		//SET_FRAME_TIME(false, (1.0f / 256.0f));
		char dmgstr[42];
		char *hitgrouptodraw;
		switch (iLastHitgroupHit)
		{
		case HITGROUP_GENERIC:
			hitgrouptodraw = blankstr;
			break;
		case HITGROUP_HEAD:
			hitgrouptodraw = headstr;
			break;
		case HITGROUP_CHEST:
			hitgrouptodraw = cheststr;
			break;
		case HITGROUP_STOMACH:
			hitgrouptodraw = stomachstr;
			break;
		case HITGROUP_LEFTARM:
			hitgrouptodraw = leftarmstr;
			break;
		case HITGROUP_RIGHTARM:
			hitgrouptodraw = rightarmstr;
			break;
		case HITGROUP_LEFTLEG:
			hitgrouptodraw = leftlegstr;
			break;
		case HITGROUP_RIGHTLEG:
			hitgrouptodraw = rightlegstr;
			break;
		default:
			hitgrouptodraw = otherstr;
			break;
		}
		int len = strlen(hitgrouptodraw);
		DecStr(hitgrouptodraw, len);
		DecStr(drawdmgformatstr, 18);
		sprintf(dmgstr, drawdmgformatstr, iDamageGivenToTarget, iHitsToTarget, hitgrouptodraw);
		EncStr(drawdmgformatstr, 18);
		DrawStringUnencrypted(dmgstr, Vector2D(CenterOfScreen.x + 15.0f, CenterOfScreen.y), ColorRGBA(255, 0, 0, 245), pFont);
		EncStr(hitgrouptodraw, len);
	}
	else
	{
		iHitsToTarget = 0;
		iLastHitgroupHit = 0;
		LastPlayerDamageGiven = nullptr;
	}
}

void RunAimESP()
{
	//FIXME: THIS IS TEMPORARY
	for (int i = 0; i < NumStreamedPlayers; i++)
	{
		CustomPlayer *pCPlayer = StreamedPlayers[i];
		if (pCPlayer->Active && !pCPlayer->IsLocalPlayer && !pCPlayer->Dormant && pCPlayer->IsTarget)
		{
			CBaseEntity *pEnt = pCPlayer->BaseEntity;
			if (pEnt->CacheBones())
			{
				StoredNetvars *CurrentRecord = pCPlayer->GetCurrentRecord();
				memcpy(&pCPlayer->CachedBoneMatrixes, &CurrentRecord->CachedBoneMatrixes, sizeof(matrix3x4_t) * MAXSTUDIOBONES);
				pCPlayer->bHasCachedBones = true;
			}

			if (pCPlayer->bHasCachedBones && pCPlayer->LifeState == LIFE_ALIVE)
			{
				if (DrawHitboxesChk.Checked)
				{
					bool IsLockedOn = LastTargetIndex != INVALID_PLAYER && AllPlayers[LastTargetIndex].Index == pCPlayer->Index;
					int R, G, B;
					if (GlowTargetsChk.Checked && IsLockedOn)
					{
						R = 255; G = 0; B = 0;
					}
					else if (pEnt->GetTeam() == TEAM_CT)
					{
						R = 0; G = 162; B = 232;
					}
					else
					{
						R = 255; G = 128; B = 0;
					}
					pEnt->DrawHitboxesFromCache(ColorRGBA(R * 0.7f, G  * 0.7f, B * 0.7f, 100), !EXTERNAL_WINDOW ? Interfaces::Globals->interval_per_tick * 2.0f : 0.001f, pCPlayer->CachedBoneMatrixes);
				}
				Vector eyepos = pEnt->GetEyePosition();
				Vector vecDir;
				trace_t tr;
				AngleVectors(pEnt->GetEyeAngles(), &vecDir);
				VectorNormalizeFast(vecDir);
				UTIL_TraceLine(eyepos, eyepos + (vecDir * 8192.0f), MASK_SHOT, pEnt, &tr);

				if (tr.DidHit())
				{
					Vector actualheadpos = pEnt->GetBonePositionCachedOnly(HITBOX_HEAD, Interfaces::Globals->curtime, pCPlayer->CachedBoneMatrixes);
					Interfaces::DebugOverlay->AddLineOverlay(actualheadpos, tr.endpos, 175, 0, 0, false, Interfaces::Globals->interval_per_tick * 2.0f);
				}
			}
		}
	}
}