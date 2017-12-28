#include "BunnyHop.h"
#include "Overlay.h"
#include "LocalPlayer.h"
#include "ConVar.h"

static bool TimeToJump = false;

void Bunnyhop(int& buttons, int flags)
{
#ifdef NO_PREDICTION
	static bool bLastJumped = false;
	static bool bShouldFake = false;
	if (!bLastJumped && bShouldFake)
	{
		bShouldFake = false;
		buttons |= IN_JUMP;
	}
	else if (buttons & IN_JUMP)
	{
		if (flags & FL_ONGROUND)
		{
			bLastJumped = true;
			bShouldFake = true;
		}
		else
		{
			buttons &= ~IN_JUMP;
			bLastJumped = false;
		}
	}
	else
	{
		bLastJumped = false;
		bShouldFake = false;
	}
#else

		if (!(flags & FL_ONGROUND))
		{
			buttons &= ~IN_JUMP;
		}
#endif
}

#define M_PI2	1.57079632679489661923
#define M_PI4	0.785398163397448309616
const float M_U_DEG = 360.0 / 65536;
const float M_U_RAD = M_PI / 32768;

	double anglemod_deg(double a)
	{
		return M_U_DEG * ((int)(a / M_U_DEG) & 0xffff);
	}

	double anglemod_rad(double a)
	{
		return M_U_RAD * ((int)(a / M_U_RAD) & 0xffff);
	}

	double point2line_distsq(const double pos[2],
		const double line_origin[2],
		const double line_dir[2])
	{
		double tmp[2] = { line_origin[0] - pos[0], line_origin[1] - pos[1] };
		double dotprod = line_dir[0] * tmp[0] + line_dir[1] * tmp[1];
		tmp[0] -= line_dir[0] * dotprod;
		tmp[1] -= line_dir[1] * dotprod;
		return tmp[0] * tmp[0] + tmp[1] * tmp[1];
	}

	double strafe_theta_opt(double speed, double L, double tauMA)
	{
		double tmp = L - tauMA;
		if (tmp <= 0)
			return M_PI2;
		if (tmp < speed)
			return std::acos(tmp / speed);
		return 0;
	}

	double strafe_theta_const(double speed, double nofric_speed, double L,
		double tauMA)
	{
		double sqdiff = nofric_speed * nofric_speed - speed * speed;
		double tmp = sqdiff / tauMA;
		if (tmp + tauMA < 2 * L && 2 * speed >= std::fabs(tmp - tauMA))
			return std::acos((tmp - tauMA) / (2 * speed));
		tmp = std::sqrt(L * L - sqdiff);
		if (tauMA - L > tmp && speed >= tmp)
			return std::acos(-tmp / speed);
		return strafe_theta_opt(speed, L, tauMA);
	}

	void strafe_fme_vec(double vel[2], const double avec[2], double L,
		double tauMA)
	{
		double tmp = L - vel[0] * avec[0] - vel[1] * avec[1];
		if (tmp < 0)
			return;
		if (tauMA < tmp)
			tmp = tauMA;
		vel[0] += avec[0] * tmp;
		vel[1] += avec[1] * tmp;
	}

	void strafe_fric(double vel[2], double E, double ktau, double cspeed)
	{
		if (cspeed >= E) {
			vel[0] *= 1 - ktau;
			vel[1] *= 1 - ktau;
			return;
		}

		double tmp = E * ktau;
		if (cspeed > tmp) {
			tmp /= cspeed;
			vel[0] -= tmp * vel[0];
			vel[1] -= tmp * vel[1];
			return;
		}

		vel[0] = 0;
		vel[1] = 0;
	}

	double strafe_fric_spd(double spd, double E, double ktau)
	{
		if (spd >= E)
			return spd * (1 - ktau);
		double tmp = E * ktau;
		if (spd > tmp)
			return spd - tmp;
		return 0;
	}

	void strafe_side(double &yaw, int &Sdir, int &Fdir, double vel[2],
		double theta, double L, double tauMA, int dir)
	{
		double phi;
		// This is to reduce the overall shaking.
		if (theta >= M_PI2 * 0.75) {
			Sdir = dir;
			Fdir = 0;
			phi = std::copysign(M_PI2, dir);
		}
		else if (M_PI2 * 0.25 <= theta && theta <= M_PI2 * 0.75) {
			Sdir = dir;
			Fdir = 1;
			phi = std::copysign(M_PI4, dir);
		}
		else {
			Sdir = 0;
			Fdir = 1;
			phi = 0;
		}

		if (std::fabs(vel[0]) > 0.1 || std::fabs(vel[1]) > 0.1)
			yaw = std::atan2(vel[1], vel[0]);
		yaw += phi - std::copysign(theta, dir);
		double yawcand[2] = {
			anglemod_rad(yaw), anglemod_rad(yaw + std::copysign(M_U_RAD, yaw))
		};
		double avec[2] = { std::cos(yawcand[0] - phi), std::sin(yawcand[0] - phi) };
		double tmpvel[2] = { vel[0], vel[1] };
		strafe_fme_vec(vel, avec, L, tauMA);
		avec[0] = std::cos(yawcand[1] - phi);
		avec[1] = std::sin(yawcand[1] - phi);
		strafe_fme_vec(tmpvel, avec, L, tauMA);

		if (tmpvel[0] * tmpvel[0] + tmpvel[1] * tmpvel[1] >
			vel[0] * vel[0] + vel[1] * vel[1]) {
			vel[0] = tmpvel[0];
			vel[1] = tmpvel[1];
			yaw = yawcand[1];
		}
		else
			yaw = yawcand[0];
	}
	void strafe_side_opt(double &yaw, int &Sdir, int &Fdir, double vel[2], double L, double tauMA, int dir, double cspeed)
	{
		double theta = strafe_theta_opt(cspeed, L, tauMA);
		strafe_side(yaw, Sdir, Fdir, vel, theta, L, tauMA, dir);
	}

	void strafe_side_const(double &yaw, int &Sdir, int &Fdir, double vel[2], double nofricspd, double L, double tauMA, int dir, double cspeed)
	{
		double theta = strafe_theta_const(cspeed, nofricspd, L, tauMA);
		strafe_side(yaw, Sdir, Fdir, vel, theta, L, tauMA, dir);
	}

	void strafe_line_opt(double &yaw, int &Sdir, int &Fdir, double vel[2], double pos[2], double L, double tau, double MA, double line_origin[2], double line_dir[2], double cspeed)
	{
		double tauMA = tau * MA;
		double theta = strafe_theta_opt(cspeed, L, tauMA);
		double ct = std::cos(theta);
		double tmp = L - cspeed * ct;
		if (tmp < 0) {
			strafe_side(yaw, Sdir, Fdir, vel, theta, L, tauMA, 1);
			return;
		}

		if (tauMA < tmp)
			tmp = tauMA;
		tmp /= cspeed;
		double st = std::sin(theta);
		double newpos_right[2], newpos_left[2];
		double avec[2];

		avec[0] = (vel[0] * ct + vel[1] * st) * tmp;
		avec[1] = (-vel[0] * st + vel[1] * ct) * tmp;
		newpos_right[0] = pos[0] + tau * (vel[0] + avec[0]);
		newpos_right[1] = pos[1] + tau * (vel[1] + avec[1]);

		avec[0] = (vel[0] * ct - vel[1] * st) * tmp;
		avec[1] = (vel[0] * st + vel[1] * ct) * tmp;
		newpos_left[0] = pos[0] + tau * (vel[0] + avec[0]);
		newpos_left[1] = pos[1] + tau * (vel[1] + avec[1]);

		bool rightgt = point2line_distsq(newpos_right, line_origin, line_dir) < point2line_distsq(newpos_left, line_origin, line_dir);
		strafe_side(yaw, Sdir, Fdir, vel, theta, L, tauMA, rightgt ? 1 : -1);
	}

	void strafe_back(double &yaw, int &Sdir, int &Fdir, double vel[2], double tauMA)
	{
		Sdir = 0;
		Fdir = -1;

		yaw = std::atan2(vel[1], vel[0]);
		float frac = yaw / M_U_RAD;
		frac -= std::trunc(frac);
		if (frac > 0.5)
			yaw += M_U_RAD;
		else if (frac < -0.5)
			yaw -= M_U_RAD;
		yaw = anglemod_rad(yaw);

		double avec[2] = { std::cos(yaw), std::sin(yaw) };
		vel[0] -= tauMA * avec[0];
		vel[1] -= tauMA * avec[1];
	}

	double strafe_opt_spd(double spd, double L, double tauMA)
	{
		double tmp = L - tauMA;
		if (tmp < 0)
			return std::sqrt(spd * spd + L * L);
		if (tmp < spd)
			return std::sqrt(spd * spd + tauMA * (L + tmp));
		return spd + tauMA;
	}

	float NormaliseYaw(float one)
	{
		while (one > 180)
			one -= 360;
		while (one < -180)
			one += 360;

		return one;

	}
#include <iostream>
QAngle m_angStrafeAngle = QAngle(0, 0, 0);



bool AutoStrafe(int &buttons, CUserCmd* cmd, float airaccfloat, float maxspeedfloat, QAngle &StrafeAngles, Vector &movements)
{
	//FUCKING ANUE POSTED THIS ON UC 2 DAYS AFTER I ADDED IT I SENT HIM BY ACCIDENT A PASTEBIN! FUCK MY LIFE... THIS ISN'T PASTE (Except a little from aimware)

	bool returnvalue = false;

	if (!(LocalPlayer.Entity->GetFlags() & FL_ONGROUND) || LocalPlayer.PressingJumpButton)
	{
		if (LocalPlayer.Entity->GetVelocity().Length2D() == 0.0f && (cmd->forwardmove == 0 && cmd->sidemove == 0))
		{
			cmd->forwardmove = 450.f;
			//m_angStrafeAngle = CurrentUserCmd.viewangles;
		}
		else if (cmd->mousedx > 1 || cmd->mousedx < -1)
		{
			cmd->sidemove = cmd->mousedx < 0.f ? -450.f : 450.f;
			//m_angStrafeAngle = CurrentUserCmd.viewangles;
		}
		else
		{
			m_angStrafeAngle = LocalPlayer.TargetEyeAngles;

			//Needs to work on LocalPlayer.currentAngles
			//Rather then angle formed from velocity
			double yawrad = NormaliseYaw(DEG2RAD(m_angStrafeAngle.y));
			float speed = maxspeedfloat;
			if (buttons & IN_DUCK) 
				speed *= (1.0f / 3.0f);
			double tau = Interfaces::Globals->interval_per_tick, MA = speed * airaccfloat;
			int Sdir = 0, Fdir = 0;
			Vector velocity = LocalPlayer.Entity->GetVelocity();
			double cspeed = std::hypot(velocity[0], velocity[1]);
			double vel[3] = { velocity[0], velocity[1], velocity[2] };
			double pos[2] = { 0, 0 };
			double dir[2] = { std::cos(DEG2RAD(m_angStrafeAngle.y)), std::sin(DEG2RAD(m_angStrafeAngle.y)) };
			strafe_line_opt(yawrad, Sdir, Fdir, vel, pos, 30.0, tau, MA, pos, dir, cspeed);
			m_angStrafeAngle.y = NormaliseYaw(RAD2DEG(yawrad));
			cmd->sidemove = Sdir * 450.0f;
			returnvalue = true;
			StrafeAngles = m_angStrafeAngle;
			//LocalPlayer.TargetEyeAngles = m_angStrafeAngle;
		}
	}
	m_angStrafeAngle.x = LocalPlayer.TargetEyeAngles.x;
	
	// TODO: move this to a "verify usercmd" section at the end of cm.
	if (cmd->forwardmove > 0.f)
		buttons |= IN_FORWARD;
	else if (cmd->forwardmove < 0.f) 
		buttons |= IN_BACK;
	else
	{
		buttons &= ~IN_FORWARD;
		buttons &= ~IN_BACK;
	}

	if (cmd->sidemove > 0.f)
		buttons |= IN_MOVERIGHT;
	else if (cmd->sidemove < 0.f)
		buttons |= IN_MOVELEFT;
	else
	{
		buttons &= ~IN_MOVERIGHT;
		buttons &= ~IN_MOVELEFT;
	}

	return returnvalue;
}

void AngleVectorsCS(const Vector &angles, Vector forward)
{
	float sp, sy, cp, cy;

	SinCos(DEG2RAD(angles.y), &sy, &cy);
	SinCos(DEG2RAD(angles.x), &sp, &cp);

	forward.x = cp*cy;
	forward.y = cp*sy;
	forward.z = -sp;
}
float moveangle(Vector angles)
{
	if (angles.x == 0 || angles.y == 0)
		return 0;
	float rise = angles.x;
	float run = angles.y;
	float value = rise / run;
	float theta = atan(value);
	theta = RAD2DEG(theta) + 90;
	if (angles.y < 0 && angles.x > 0 || angles.y < 0 && angles.x < 0)
		theta *= -1;
	else
		theta = 180 - theta;

	return theta;
}
bool advancedCStrafe(int& buttons, CUserCmd* cmd, float airaccfloat, float maxspeedfloat, double &AngleAdd, Vector &movements)
{
	//TODO trace a circle to follow then do it.
	if (!(GetAsyncKeyState(CStraffKeyTxt.iValue) & (1 << 16)))
		return false;
	//Let us get some basic info
	Vector Velocity = LocalPlayer.Entity->GetVelocity();
	Velocity.z = 0;
	Vector pos = *LocalPlayer.Entity->GetAbsOrigin();
	float speed = abs(Velocity.Length2D());
	float speedTrue = abs(Velocity.Length2D()*Interfaces::Globals->interval_per_tick);

	float MintoAdd =(-1 * speed / 1200 + 2.1);
	CTraceFilter filter;
	filter.pSkip = (IHandleEntity*)LocalPlayer.Entity;
	filter.m_icollisionGroup = COLLISION_GROUP_NONE;
	Vector mins = LocalPlayer.Entity->GetMins();
	Vector maxs = LocalPlayer.Entity->GetMaxs();
	trace_t trfront, trleft, trright;
	Ray_t ray;

	Vector TempVelocity = Velocity;

	bool hitobject = false;
	for (int i = 0; i < 5; i++)
	{
		int futureTics = 10;
		//Let us rotate the velocity vector
		float cs = cos(DEG2RAD(MintoAdd*i));
		float sn = sin(DEG2RAD(MintoAdd*i));

		TempVelocity.x = Velocity.x * cs - Velocity.y * sn;
		TempVelocity.y = Velocity.x * sn + Velocity.y * cs;

		Vector add = TempVelocity * futureTics* Interfaces::Globals->interval_per_tick;
		Vector endPos = pos + add;

		//Lets trace
		ray.Init(pos, endPos, mins, maxs);
		Interfaces::EngineTrace->TraceRay(ray, MASK_SOLID, &filter, &trfront);
		
		if (trfront.fraction < 1)
		{
			MintoAdd *= 2 - trfront.fraction;
		}

	}

	if (MintoAdd < 0.5)
		MintoAdd = 0.5;

	static float addition = 0;
	addition += MintoAdd;

	addition = NormaliseYaw(addition);
	AngleAdd = NormaliseYaw(AngleAdd + addition);

	//Set up values for parametric equations
	cmd->forwardmove = sin(DEG2RAD(AngleAdd + 90)) * 450;
	cmd->sidemove = cos(DEG2RAD(AngleAdd + 90)) * 450;

	return true;
}
bool CStrafe(int& buttons, CUserCmd* cmd, float airaccfloat, float maxspeedfloat, double &AngleAdd, Vector &movements)
{
	//TODO trace a circle to follow then do it.
	if (!(GetAsyncKeyState(CStraffKeyTxt.iValue) & (1 << 16)))
		return false;
	//Let us get some basic info
	Vector Velocity = LocalPlayer.Entity->GetVelocity();
	Velocity.z = 0;
	Vector pos = *LocalPlayer.Entity->GetAbsOrigin();
	float speed = abs(Velocity.Length2D());
	float speedTrue = abs(Velocity.Length2D()*Interfaces::Globals->interval_per_tick);

	
	int futureTics = 10;
	Vector add = Velocity * futureTics* Interfaces::Globals->interval_per_tick;

	Vector endPos = pos + add;

	//Lets set up values for tracing
	CTraceFilter filter;
	filter.pSkip = (IHandleEntity*)LocalPlayer.Entity;
	filter.m_icollisionGroup = COLLISION_GROUP_NONE;

	Vector mins = LocalPlayer.Entity->GetMins();
	Vector maxs = LocalPlayer.Entity->GetMaxs();
	trace_t trfront, trleft, trright;
	Ray_t ray;

	//Lets trace
	ray.Init(pos, endPos, mins, maxs);
	Interfaces::EngineTrace->TraceRay(ray, MASK_SOLID, &filter, &trfront);
	
	static float addition = 0;

	float tracing = (2 / (20 * trfront.fraction + 1.1)) + 0.9;
	float traceadd = (1 - trfront.fraction)*2;
	float toAdd = (tracing)*(-1 * speed / 1200 + 2.1)+ traceadd;
	
	if (toAdd < 0.5)
		toAdd = 0.5;

	addition += toAdd;

	addition = NormaliseYaw(addition);
	AngleAdd = NormaliseYaw(AngleAdd + addition);

	//Set up values for parametric equations
	cmd->forwardmove = sin(DEG2RAD(AngleAdd + 90)) * 450;
	cmd->sidemove = cos(DEG2RAD(AngleAdd + 90)) * 450;

	return true;
}

char *airacceleratestr = new char[17]{ 9, 12, 37, 27, 19, 8, 27, 25, 25, 31, 22, 31, 8, 27, 14, 31, 0 }; /*sv_airaccelerate*/
char *maxspeedstr = new char[12]{ 9, 12, 37, 23, 27, 2, 9, 10, 31, 31, 30, 0 }; /*sv_maxspeed*/

ConVar* airaccel = nullptr;
ConVar* maxspeed = nullptr;

void RunBunnyHopping(CUserCmd *cmd, int& flags, bool& StrafeModifiedAngles, QAngle& StrafeAngles)
{
	if (BhopChk.Checked)
	{
		Bunnyhop(buttons, flags);
		if (!RageBhopChk.Checked)
		{
			if (LocalPlayer.PressingJumpButton)
			{
				if ((cmd->mousedx > 1 || cmd->mousedx < -1))
					cmd->sidemove = cmd->mousedx < 0.0f ? -450.0f : 450.0f;
			}
		}
	}
	if (RageBhopChk.Checked && LocalPlayer.PressingJumpButton)
	{
		if (!airaccel)
		{
			DecStr(airacceleratestr, 16);
			airaccel = Interfaces::Cvar->FindVar(airacceleratestr);
			EncStr(airacceleratestr, 16);
			delete[] airacceleratestr;

			DecStr(maxspeedstr, 11);
			maxspeed = Interfaces::Cvar->FindVar(maxspeedstr);
			EncStr(maxspeedstr, 11);
			delete[] maxspeedstr;
		}
		float airaccfloat = airaccel->GetFloat();
		float maxspeedfloat = maxspeed->GetFloat();
		double cstraffdif = StrafeAngles.y;
		//JAKE FIX THIS PLEASE, IT'S COMPLETELY BROKEN
		if (advancedCStrafe(buttons, cmd, airaccfloat, maxspeedfloat, cstraffdif, Vector(0, 0, 0)))
		{
			StrafeModifiedAngles = true;
			StrafeAngles.y = cstraffdif;
			StrafeAngles.ClampY();
			LocalPlayer.TargetEyeAngles = StrafeAngles;
		}
		else

		{
			if (AutoStrafe(buttons, cmd, airaccfloat, maxspeedfloat, StrafeAngles, Vector(0, 0, 0)))
			{
				LocalPlayer.TargetEyeAngles = StrafeAngles;
				StrafeModifiedAngles = true;
			}
		}
	}
}