#include "VTHook.h"
#include "CreateMove.h"
#include "ConVar.h"
#include "ThirdPerson.h"
#include "LocalPlayer.h"

ShouldDrawLocalPlayerFn oShouldDrawLocalPlayer;

//NOT USED
bool __fastcall Hooks::ShouldDrawLocalPlayer(void* ecx, void* edx, CBaseEntity *pPlayer)
{
	return oShouldDrawLocalPlayer(ecx, edx, pPlayer);
}

#define CAM_HULL_OFFSET		14.0    // the size of the bounding hull used for collision checking
#define CAM_MIN_DIST 30.0
#define CAM_ANGLE_MOVE .5
#define CAMERA_UP_OFFSET	25.0f
#define CAMERA_OFFSET_LERP_TIME 0.5f
bool ThirdPerson = false; //Global bool that determines whether or not third person is enabled or disabled
QAngle camAngles = angZero;
Vector camOrigin = vecZero;
static Vector CAM_HULL_MIN(-CAM_HULL_OFFSET, -CAM_HULL_OFFSET, -CAM_HULL_OFFSET);
static Vector CAM_HULL_MAX(CAM_HULL_OFFSET, CAM_HULL_OFFSET, CAM_HULL_OFFSET);
// What is the current camera offset from the view origin?
Vector		m_vecCameraOffset{ 0,0,0 };
static float	m_flFraction = 0;
static float	m_flUpFraction = 0;
static float	m_flTargetFraction = 0;
static float	m_flTargetUpFraction = 0;
static float	m_flLerpTime = 0;
static float	m_flUpLerpTime = 0;
static float idealpitch = 0.0f;
static float idealyaw = 0.0f;
static float idealdist = 150.0f;
//ConVar* cam_idealdist = nullptr;
const bool cl_thirdperson = false;
bool m_bOverrideThirdPerson = false;
ConVar* cam_idealdistright = nullptr;
ConVar* cam_idealdistup = nullptr;
ConVar* cam_ideallag = nullptr;
//ConVar* cam_idealyaw = nullptr;
//ConVar* cam_idealpitch = nullptr;
ConVar* cam_collision = nullptr;
ConVar* cam_snapto = nullptr;
ConVar* cam_idealdelta = nullptr;
ConVar* c_minyaw = nullptr;
ConVar* c_maxyaw = nullptr;
ConVar* c_minpitch = nullptr;
ConVar* c_maxpitch = nullptr;
ConVar* c_mindistance = nullptr;
ConVar* c_maxdistance = nullptr;
char *idealdiststr = new char[14]{ 25, 27, 23, 37, 19, 30, 31, 27, 22, 30, 19, 9, 14, 0 }; /*cam_idealdist*/
char *idealdistrightstr = new char[19]{ 25, 27, 23, 37, 19, 30, 31, 27, 22, 30, 19, 9, 14, 8, 19, 29, 18, 14, 0 }; /*cam_idealdistright*/
char *idealdistupstr = new char[16]{ 25, 27, 23, 37, 19, 30, 31, 27, 22, 30, 19, 9, 14, 15, 10, 0 }; /*cam_idealdistup*/
char *ideallagstr = new char[13]{ 25, 27, 23, 37, 19, 30, 31, 27, 22, 22, 27, 29, 0 }; /*cam_ideallag*/
																					   //char *idealyawstr = new char[13]{ 25, 27, 23, 37, 19, 30, 31, 27, 22, 3, 27, 13, 0 }; /*cam_idealyaw*/
																					   //char *idealpitchstr = new char[15]{ 25, 27, 23, 37, 19, 30, 31, 27, 22, 10, 19, 14, 25, 18, 0 }; /*cam_idealpitch*/
char *cam_collisionstr = new char[14]{ 25, 27, 23, 37, 25, 21, 22, 22, 19, 9, 19, 21, 20, 0 }; /*cam_collision*/
char *snaptostr = new char[11]{ 25, 27, 23, 37, 9, 20, 27, 10, 14, 21, 0 }; /*cam_snapto*/
char *minyawstr = new char[9]{ 25, 37, 23, 19, 20, 3, 27, 13, 0 }; /*c_minyaw*/
char *maxyawstr = new char[9]{ 25, 37, 23, 27, 2, 3, 27, 13, 0 }; /*c_maxyaw*/
char *minpitchstr = new char[11]{ 25, 37, 23, 19, 20, 10, 19, 14, 25, 18, 0 }; /*c_minpitch*/
char *maxpitchstr = new char[11]{ 25, 37, 23, 27, 2, 10, 19, 14, 25, 18, 0 }; /*c_maxpitch*/
char *mindistancestr = new char[14]{ 25, 37, 23, 19, 20, 30, 19, 9, 14, 27, 20, 25, 31, 0 }; /*c_mindistance*/
char *maxdistancestr = new char[14]{ 25, 37, 23, 27, 2, 30, 19, 9, 14, 27, 20, 25, 31, 0 }; /*c_maxdistance*/
char *idealdeltastr = new char[15]{ 25, 27, 23, 37, 19, 30, 31, 27, 22, 30, 31, 22, 14, 27, 0 }; /*cam_idealdelta*/

Vector m_vecDesiredCameraOffset;

float MoveToward(float cur, float goal, float lag)
{
	if (cur != goal)
	{
		if (abs(cur - goal) > 180.0)
		{
			if (cur < goal)
				cur += 360.0;
			else
				cur -= 360.0;
		}

		if (cur < goal)
		{
			if (cur < goal - 1.0)
				cur += (goal - cur) / lag;
			else
				cur = goal;
		}
		else
		{
			if (cur > goal + 1.0)
				cur -= (cur - goal) / lag;
			else
				cur = goal;
		}
	}


	// bring cur back into range
	if (cur < 0)
		cur += 360.0;
	else if (cur >= 360)
		cur -= 360;

	return cur;
}

void PositionCamera(CBasePlayer *pPlayer, const QAngle& angles)
{
	if (pPlayer)
	{
		trace_t trace;

		Vector camForward, camRight, camUp;

		// find our player's origin, and from there, the eye position
		Vector origin = pPlayer->GetOrigin();
		origin += pPlayer->GetViewOffset();

		AngleVectors((QAngle)angles, &camForward, &camRight, &camUp); //+ punchangle

		Vector endPos = origin;

		Vector vecCamOffset = endPos + (camForward * -GetDesiredCameraOffset()[0]) + (camRight * GetDesiredCameraOffset()[1]) + (camUp * GetDesiredCameraOffset()[2]);

		// use our previously #defined hull to collision trace
		CTraceFilter traceFilter;
		traceFilter.pSkip = (IHandleEntity*)pPlayer;
		traceFilter.m_icollisionGroup = COLLISION_GROUP_NONE;
		UTIL_TraceHull(endPos, vecCamOffset, CAM_HULL_MIN, CAM_HULL_MAX, MASK_SOLID & ~CONTENTS_MONSTER, (ITraceFilter*)&traceFilter, trace);

		if (trace.fraction != m_flTargetFraction)
		{
			m_flLerpTime = Interfaces::Globals->curtime;
		}

		m_flTargetFraction = trace.fraction;
		m_flTargetUpFraction = 1.0f;

		//If we're getting closer to a wall snap the fraction right away.
		if (m_flTargetFraction < m_flFraction)
		{
			m_flFraction = m_flTargetFraction;
			m_flLerpTime = Interfaces::Globals->curtime;
		}

		// move the camera closer if it hit something
		if (trace.fraction < 1.0)
		{
			m_vecCameraOffset[2] *= trace.fraction;

			UTIL_TraceHull(endPos, endPos + (camForward * (-GetDesiredCameraOffset()[0])), CAM_HULL_MIN, CAM_HULL_MAX, MASK_SOLID & ~CONTENTS_MONSTER, &traceFilter, trace);

			if (trace.fraction != 1.0f)
			{
				if (trace.fraction != m_flTargetUpFraction)
				{
					m_flUpLerpTime = Interfaces::Globals->curtime;
				}

				m_flTargetUpFraction = trace.fraction;

				if (m_flTargetUpFraction < m_flUpFraction)
				{
					m_flUpFraction = trace.fraction;
					m_flUpLerpTime = Interfaces::Globals->curtime;
				}
			}
		}
	}
}

void CAM_Think()
{
	Vector idealAngles = Vector(0.0f, 0.0f, idealdist);
#if 0
	Vector idealAngles = Vector(idealpitch, idealyaw, idealdist);
	POINT cp;
	//get windows cursor position
	GetCursorPos(&cp);

	const int m_nCameraX = cp.x;
	const int m_nCameraY = cp.y;

	const int x = CenterOfScreen.x;
	const int y = CenterOfScreen.y;

	//keep the camera within certain limits around the player (ie avoid certain bad viewing angles)  
	if (m_nCameraX>x)
	{
		//if ((idealAngles[YAW]>=225.0)||(idealAngles[YAW]<135.0))
		if (idealAngles[1]<c_maxyaw->GetFloat())
		{
			idealAngles[1] += CAM_ANGLE_MOVE *((m_nCameraX - x) / 2);
		}
		if (idealAngles[1] > c_maxyaw->GetFloat())
		{
			idealAngles[1] = c_maxyaw->GetFloat();
		}
	}
	else if (m_nCameraX<x)
	{
		//if ((idealAngles[YAW]<=135.0)||(idealAngles[YAW]>225.0))
		if (idealAngles[1]>c_minyaw->GetFloat())
		{
			idealAngles[1] -= CAM_ANGLE_MOVE * ((x - m_nCameraX) / 2);
		}
		if (idealAngles[1]<c_minyaw->GetFloat())
		{
			idealAngles[1] = c_minyaw->GetFloat();
		}
	}

	//check for y delta values and adjust accordingly
	//eventually adjust PITCH based on amount of movement
	//also make sure camera is within bounds
	if (m_nCameraY > y)
	{
		if (idealAngles[0]<c_maxpitch->GetFloat())
		{
			idealAngles[0] += (CAM_ANGLE_MOVE)* ((m_nCameraY - y) / 2);
		}
		if (idealAngles[0]>c_maxpitch->GetFloat())
		{
			idealAngles[0] = c_maxpitch->GetFloat();
		}
	}
	else if (m_nCameraY < y)
	{
		if (idealAngles[0]>c_minpitch->GetFloat())
		{
			idealAngles[0] -= (CAM_ANGLE_MOVE)*((y - m_nCameraY) / 2);
		}
		if (idealAngles[0]<c_minpitch->GetFloat())
		{
			idealAngles[0] = c_minpitch->GetFloat();
		}
	}

	//Nathan code here
	if ((GetAsyncKeyState(VK_NUMPAD8) & (1 << 16)))
		idealAngles[0] += cam_idealdelta->GetFloat();
	else if ((GetAsyncKeyState(VK_NUMPAD2) & (1 << 16)))
		idealAngles[0] -= cam_idealdelta->GetFloat();

	if ((GetAsyncKeyState(VK_NUMPAD4) & (1 << 16)))
		idealAngles[1] -= cam_idealdelta->GetFloat();
	else if ((GetAsyncKeyState(VK_NUMPAD6) & (1 << 16)))
		idealAngles[1] += cam_idealdelta->GetFloat();

	if ((GetAsyncKeyState(VK_ADD) & (1 << 16)))
	{
		idealAngles[2] -= 2 * cam_idealdelta->GetFloat();
		if (idealAngles[2] < CAM_MIN_DIST)
		{
			// If we go back into first person, reset the angle
			idealAngles[0] = 0;
			idealAngles[1] = 0;
			idealAngles[2] = CAM_MIN_DIST;
		}

	}
	else if ((GetAsyncKeyState(VK_SUBTRACT) & (1 << 16)))
		idealAngles[2] += 2 * cam_idealdelta->GetFloat();
#endif

	QAngle viewangles;
	Interfaces::Engine->GetViewAngles(viewangles);


#if 0
	// bring the pitch values back into a range that MoveToward can handle
	if (idealAngles[0] > 180)
		idealAngles[0] -= 360;
	else if (idealAngles[0] < -180)
		idealAngles[0] += 360;

	// bring the yaw values back into a range that MoveToward can handle
	// --
	// Vitaliy: going with >= 180 and <= -180.
	// This introduces a potential discontinuity when looking directly at model face
	// as camera yaw will be jumping from +180 to -180 and back, but when working with
	// the camera allows smooth rotational transitions from left to right and back.
	// Otherwise one of the transitions that has ">"-comparison will be locked.
	// --
	if (idealAngles[1] >= 180)
		idealAngles[1] -= 360;
	else if (idealAngles[1] <= -180)
		idealAngles[1] += 360;
#endif
	// clamp pitch, yaw and dist...
	idealAngles[0] = clamp(idealAngles[0], c_minpitch->GetFloat(), c_maxpitch->GetFloat());
	idealAngles[1] = clamp(idealAngles[1], c_minyaw->GetFloat(), c_maxyaw->GetFloat());
	idealAngles[2] = clamp(idealAngles[2], c_mindistance->GetFloat(), c_maxdistance->GetFloat());

	// update ideal angles
	idealpitch = idealAngles[0];
	idealyaw = idealAngles[1];
	idealdist = idealAngles[2];

	// Move the CameraOffset "towards" the idealAngles
	// Note: CameraOffset = viewangle + idealAngle
	Vector camOffset;

	if (cam_snapto->GetInt())
	{
		camOffset[1] = idealyaw + viewangles[1];
		camOffset[0] = idealpitch + viewangles[0];
		camOffset[2] = idealdist;
	}
	else
	{
		float lag = max(1, 1 + cam_ideallag->GetFloat());
		camOffset = GetCameraOffsetAngles();

		if (camOffset[1] - viewangles[1] != idealyaw)
			camOffset[1] = MoveToward(camOffset[1], idealyaw + viewangles[1], lag);

		if (camOffset[0] - viewangles[0] != idealpitch)
			camOffset[0] = MoveToward(camOffset[0], idealpitch + viewangles[0], lag);

		if (abs(camOffset[2] - idealdist) < 2.0)
			camOffset[2] = idealdist;
		else
			camOffset[2] += (idealdist - camOffset[2]) / lag;
	}

	if (cam_collision->GetInt())
	{
		QAngle desiredCamAngles;
		if (m_bOverrideThirdPerson == false)
		{
			desiredCamAngles = viewangles;
		}
		else
		{
			desiredCamAngles = QAngle(camOffset[0], camOffset[1], camOffset[2]);
		}
		CBaseEntity *pCameraTarget = LocalPlayer.Entity;
		HANDLE hObserverTarget = LocalPlayer.Entity->GetObserverTarget();
		if (hObserverTarget)
		{
			CBaseEntity* t = Interfaces::ClientEntList->GetClientEntityFromHandle((DWORD)hObserverTarget);
			if (t)
				pCameraTarget = t;
		}
		PositionCamera(pCameraTarget, desiredCamAngles);
	}

	SetCameraOffsetAngles(camOffset);
}

Vector GetFinalCameraOffset(void)
{
	Vector desired = GetDesiredCameraOffset();

	if (ThirdPersonOffsetChk.Checked && m_flUpFraction != 1.0f)
	{
		desired.z += CAMERA_UP_OFFSET;
	}

	return desired;

}

float inline RemapValClamped(float val, float A, float B, float C, float D)
{
	if (A == B)
		return val >= B ? D : C;
	float cVal = (val - A) / (B - A);
	cVal = clamp(cVal, 0.0f, 1.0f);

	return C + (D - C) * cVal;
}

Vector GetDistanceFraction(void)
{
	if (m_bOverrideThirdPerson == true)
	{
		return Vector(m_flTargetFraction, m_flTargetFraction, m_flTargetFraction);
	}

	float flFraction = m_flFraction;
	float flUpFraction = m_flUpFraction;

	float flFrac = RemapValClamped(Interfaces::Globals->curtime - m_flLerpTime, 0, CAMERA_OFFSET_LERP_TIME, 0, 1);

	flFraction = Lerp(flFrac, m_flFraction, m_flTargetFraction);

	if (flFrac == 1.0f)
	{
		m_flFraction = m_flTargetFraction;
	}

	flFrac = RemapValClamped(Interfaces::Globals->curtime - m_flUpLerpTime, 0, CAMERA_OFFSET_LERP_TIME, 0, 1);

	flUpFraction = 1.0f - Lerp(flFrac, m_flUpFraction, m_flTargetUpFraction);

	if (flFrac == 1.0f)
	{
		m_flUpFraction = m_flTargetUpFraction;
	}

	return Vector(flFraction, flFraction, flUpFraction);
}

__forceinline void VectorMA(const Vector& start, float scale, const Vector& direction, Vector& dest)
{
	dest.x = start.x + direction.x*scale;
	dest.y = start.y + direction.y*scale;
	dest.z = start.z + direction.z*scale;
}

void GetCameraCommands()
{
#if 0
	DecStr(idealdiststr, 13);
	cam_idealdist = Interfaces::Cvar->FindVar(idealdiststr);
	EncStr(idealdiststr, 13);
#endif
	DecStr(idealdistrightstr, 18);
	cam_idealdistright = Interfaces::Cvar->FindVar(idealdistrightstr);
	EncStr(idealdistrightstr, 18);
	DecStr(idealdistupstr, 15);
	cam_idealdistup = Interfaces::Cvar->FindVar(idealdistupstr);
	EncStr(idealdistupstr, 15);
	DecStr(ideallagstr, 12);
	cam_ideallag = Interfaces::Cvar->FindVar(ideallagstr);
	EncStr(ideallagstr, 12);
#if 0
	DecStr(idealpitchstr, 14);
	cam_idealpitch = Interfaces::Cvar->FindVar(idealpitchstr);
	EncStr(idealpitchstr, 14);
	DecStr(idealyawstr, 12);
	cam_idealyaw = Interfaces::Cvar->FindVar(idealyawstr);
	EncStr(idealyawstr, 12);
#endif
	DecStr(cam_collisionstr, 13);
	cam_collision = Interfaces::Cvar->FindVar(cam_collisionstr);
	EncStr(cam_collisionstr, 13);
	DecStr(snaptostr, 10);
	cam_snapto = Interfaces::Cvar->FindVar(snaptostr);
	EncStr(snaptostr, 10);
	DecStr(minyawstr, 8);
	c_minyaw = Interfaces::Cvar->FindVar(minyawstr);
	EncStr(minyawstr, 8);
	DecStr(maxyawstr, 8);
	c_maxyaw = Interfaces::Cvar->FindVar(maxyawstr);
	EncStr(maxyawstr, 8);
	DecStr(minpitchstr, 10);
	c_minpitch = Interfaces::Cvar->FindVar(minpitchstr);
	EncStr(minpitchstr, 10);
	DecStr(maxpitchstr, 10);
	c_maxpitch = Interfaces::Cvar->FindVar(maxpitchstr);
	EncStr(maxpitchstr, 10);
	DecStr(mindistancestr, 13);
	c_mindistance = Interfaces::Cvar->FindVar(mindistancestr);
	EncStr(mindistancestr, 13);
	DecStr(maxdistancestr, 13);
	c_maxdistance = Interfaces::Cvar->FindVar(maxdistancestr);
	EncStr(maxdistancestr, 13);
	DecStr(idealdeltastr, 14);
	cam_idealdelta = Interfaces::Cvar->FindVar(idealdeltastr);
	EncStr(idealdeltastr, 14);
	//delete[] idealdiststr;
	//delete[] idealpitchstr;
	//delete[] idealyawstr;
	delete[] idealdistrightstr;
	delete[] idealdistupstr;
	delete[] ideallagstr;
	delete[] cam_collisionstr;
	delete[] snaptostr;
	delete[] minyawstr;
	delete[] maxyawstr;
	delete[] minpitchstr;
	delete[] maxpitchstr;
	delete[] mindistancestr;
	delete[] maxdistancestr;
	delete[] idealdeltastr;
}

void CAM_ToThirdPerson(void)
{
	if (cl_thirdperson == false)
	{
		SetDesiredCameraOffset(Vector(idealdist, cam_idealdistright->GetFloat(), cam_idealdistup->GetFloat()));
		m_bOverrideThirdPerson = true;
	}

	QAngle viewangles;
	Interfaces::Engine->GetViewAngles(viewangles);
	SetCameraOffsetAngles(Vector(viewangles[1], viewangles[0], CAM_MIN_DIST));
	LocalPlayer.Entity->SetObserverMode(1);
}

void CAM_ToFirstPerson(void)
{
	if (cl_thirdperson == false)
		m_bOverrideThirdPerson = false;

	SetDesiredCameraOffset(Vector(0, 0, 0));
	LocalPlayer.Entity->SetObserverMode(LocalPlayer.OriginalObserverMode);
}


char *sv_cheatsstrc = new char[10]{ 9, 12, 37, 25, 18, 31, 27, 14, 9, 0 }; /*sv_cheats*/
char *firstpersonstr = new char[12]{ 28, 19, 8, 9, 14, 10, 31, 8, 9, 21, 20, 0 }; /*firstperson*/
char *thirdpersonstr = new char[12]{ 14, 18, 19, 8, 30, 10, 31, 8, 9, 21, 20, 0 }; /*thirdperson*/

void convarSpoofing(bool thirdPerson)
{
	static bool spoofed = false;

	static ConVar* sv_cheat = nullptr;
	if (!sv_cheat)
	{
		DecStr(sv_cheatsstrc, 9);
		sv_cheat = Interfaces::Cvar->FindVar(sv_cheatsstrc);
		EncStr(sv_cheatsstrc, 9);
		delete[] sv_cheatsstrc;
	}
	static SpoofedConvar *sv_cheats_s = nullptr;
	if (sv_cheat->GetInt() == 1)
	{
		CBaseEntity *pCameraTarget = LocalPlayer.Entity;
		HANDLE hObserverTarget = LocalPlayer.Entity->GetObserverTarget();
		if (hObserverTarget && LocalPlayer.Entity->GetHealth() <= 0)
		{
			CBaseEntity* t = Interfaces::ClientEntList->GetClientEntityFromHandle((DWORD)hObserverTarget);
			if (t)
				pCameraTarget = t;
		}
		if (!pCameraTarget)return;

		static bool inFirstPerson;
		if (pCameraTarget->GetHealth() > 0 && (!inFirstPerson && thirdPerson))
		{
			DecStr(thirdpersonstr, 11);
			Interfaces::Engine->ClientCmd_Unrestricted(thirdpersonstr, 0);
			EncStr(thirdpersonstr, 11);
			ThirdPerson = true;
			inFirstPerson = true;
		}
		else if(pCameraTarget->GetHealth() > 0 && inFirstPerson && !thirdPerson)
		{
			DecStr(firstpersonstr, 11);
			Interfaces::Engine->ClientCmd_Unrestricted(firstpersonstr, 0);
			EncStr(firstpersonstr, 11);
			inFirstPerson = false;
			ThirdPerson = false;
		}
	}
	else
	{

		if (!spoofed||!sv_cheats_s)
		{
			sv_cheats_s = new SpoofedConvar(sv_cheat);
			spoofed = true;
		}
		if (!sv_cheats_s)
			return;
		if(!LocalPlayer.Entity)
			return;
		CBaseEntity *pCameraTarget = LocalPlayer.Entity;
		HANDLE hObserverTarget = LocalPlayer.Entity->GetObserverTarget();
		if (hObserverTarget && LocalPlayer.Entity->GetHealth() <= 0)
		{
			CBaseEntity* t = Interfaces::ClientEntList->GetClientEntityFromHandle((DWORD)hObserverTarget);
			if (t)
				pCameraTarget = t;
		}
		if (!pCameraTarget)return;

		static bool inFirstPerson;
		if (pCameraTarget->GetHealth() > 0 && (!inFirstPerson && thirdPerson))
		{
			sv_cheats_s->SetBool(TRUE);
			DecStr(thirdpersonstr, 11);
			Interfaces::Engine->ClientCmd_Unrestricted(thirdpersonstr, 0);
			EncStr(thirdpersonstr, 11);
			ThirdPerson = true;
			inFirstPerson = true;
		}
		else if (pCameraTarget->GetHealth() > 0 && inFirstPerson && !thirdPerson)
		{
			DecStr(firstpersonstr, 11);
			Interfaces::Engine->ClientCmd_Unrestricted(firstpersonstr, 0);
			EncStr(firstpersonstr, 11);
			sv_cheats_s->SetBool(FALSE);
			inFirstPerson = false;
			ThirdPerson = false;
		}
	}
}
void UpdateThirdPerson(CViewSetup *pSetup)
{
	int LocalPlayerIndex = Interfaces::Engine->GetLocalPlayer();
	if (!LocalPlayerIndex)
		return;

	CBaseEntity* pLocalEntity = Interfaces::ClientEntList->GetClientEntity(LocalPlayerIndex);

	if (!pLocalEntity || !Interfaces::Engine->IsInGame())
		return;
	bool useconvarSpoofing = spoofConvars.Checked;

	if (DisableAllChk.Checked)
	{
		if (ThirdPerson)
		{
			if (useconvarSpoofing)
			{
				convarSpoofing(false);
				ThirdPerson = false;
				return;
			}

			CAM_ToFirstPerson();
			ThirdPerson = false;
		}
		return;
	}

	static bool EnableThirdPerson = false;
	static float NextTimeCheckKey = 0.0f;
	if (Time >= NextTimeCheckKey)
	{
		if ((GetAsyncKeyState(ThirdPersonKeyTxt.iValue) & (1 << 16)))
		{
			EnableThirdPerson = !EnableThirdPerson;
			NextTimeCheckKey = Time + 0.3f;
		}
	}
	if (useconvarSpoofing)
	{
		convarSpoofing(EnableThirdPerson);
		return;
	}

	if (!c_maxpitch && !useconvarSpoofing)
	{
		GetCameraCommands();
	}

	if (EnableThirdPerson)
	{
		if (useconvarSpoofing)
		{
			convarSpoofing(true);
			ThirdPerson = true;
			return;
		}

		if (!ThirdPerson)
		{
			CAM_ToThirdPerson();
			ThirdPerson = true;
		}
	}
	else
	{
		if (ThirdPerson)
		{
			if (useconvarSpoofing)
			{
				convarSpoofing(false);
				ThirdPerson = false;
				return;
			}

			CAM_ToFirstPerson();
			ThirdPerson = false;
		}
	}
	if (!ThirdPerson)
	{
		return;
	}

	if (useconvarSpoofing)
		return;
	CBaseEntity *pCameraTarget = LocalPlayer.Entity;

	pCameraTarget->UpdateClientSideAnimation();

	HANDLE hObserverTarget = LocalPlayer.Entity->GetObserverTarget();
	if (hObserverTarget)
	{
		CBaseEntity* t = Interfaces::ClientEntList->GetClientEntityFromHandle((DWORD)hObserverTarget);
		if (t)
			pCameraTarget = t;
	}
	pSetup->origin = pCameraTarget->GetEyePosition();

	CAM_Think();

	const Vector cam_ofs = GetCameraOffsetAngles();
	Vector cam_ofs_distance = GetFinalCameraOffset();

	cam_ofs_distance *= GetDistanceFraction();

	Vector camForward, camRight, camUp;

	if (m_bOverrideThirdPerson == false)
	{
		Interfaces::Engine->GetViewAngles(camAngles);
	}
	else
	{
		camAngles = QAngle(GetCameraOffsetAngles()[0], GetCameraOffsetAngles()[1], 0.0f);
	}

	// get the forward vector
	AngleVectors(camAngles, &camForward, &camRight, &camUp);

	VectorMA(pSetup->origin, -cam_ofs_distance[0], camForward, pSetup->origin);
	VectorMA(pSetup->origin, cam_ofs_distance[1], camRight, pSetup->origin);
	VectorMA(pSetup->origin, cam_ofs_distance[2], camUp, pSetup->origin);

	camOrigin = pSetup->origin;

	// Override angles from third person camera
	pSetup->angles = camAngles;
}