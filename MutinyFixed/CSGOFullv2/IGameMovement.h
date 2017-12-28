#pragma once
#include "CreateMove.h"
#include "VPhysics.h"
#include "CBaseHandle.h"
#define MAX_CLIMB_SPEED	200

typedef CBaseEntity CBasePlayer;

//-----------------------------------------------------------------------------
// An entity identifier that works in both game + client dlls
//-----------------------------------------------------------------------------

typedef CBaseHandle EntityHandle_t;

class CMoveData
{
//public:
//    char pad_0x00[183];
public:
    bool            m_bFirstRunOfFunctions : 1;
    bool            m_bGameCodeMovedPlayer : 1;
    bool            m_bNoAirControl : 1;

    EntityHandle_t    m_nPlayerHandle;    // edict index on server, client entity handle on client

    int                m_nImpulseCommand;    // Impulse command issued.
    QAngle            m_vecViewAngles;    // Command view angles (local space)
    QAngle            m_vecAbsViewAngles;    // Command view angles (world space)
    int                m_nButtons;            // Attack buttons.
    int                m_nOldButtons;        // From host_client->oldbuttons;
    float            m_flForwardMove;
    float            m_flSideMove;
    float            m_flUpMove;
    
    float            _m_flMaxSpeed;
    float            m_flClientMaxSpeed;

    // Variables from the player edict (sv_player) or entvars on the client.
    // These are copied in here before calling and copied out after calling.
    Vector            m_vecVelocity_;        // edict::velocity        // Current movement direction.
    Vector            m_vecOldVelocity;
    float            somefloat;
    QAngle            m_vecAngles;        // edict::angles
    QAngle            m_vecOldAngles;
    
// Output only
    float            m_outStepHeight;    // how much you climbed this move
    Vector            m_outWishVel;        // This is where you tried 
    Vector            m_outJumpVel;        // This is your jump velocity

    // Movement constraints    (radius 0 means no constraint)
    Vector            m_vecConstraintCenter;
    float            m_flConstraintRadius;
    float            m_flConstraintWidth;
    float            m_flConstraintSpeedFactor;
    bool            m_bConstraintPastRadius;        ///< If no, do no constraining past Radius.  If yes, cap them to SpeedFactor past radius

    void            SetAbsOrigin( const Vector &vec );
    const Vector    &GetAbsOrigin() const;

private:
    Vector            m_vecAbsOrigin;        // edict::origin

};  

//-----------------------------------------------------------------------------
// Purpose: The basic player movement interface
//-----------------------------------------------------------------------------

abstract_class IGameMovement
{
public:
	virtual			~IGameMovement(void) {}

	// Process the current movement command
	virtual void	ProcessMovement(CBasePlayer *pPlayer, CMoveData *pMove) = 0;
	virtual void	Reset(void) = 0;
	virtual void	StartTrackPredictionErrors(CBasePlayer *pPlayer) = 0;
	virtual void	FinishTrackPredictionErrors(CBasePlayer *pPlayer) = 0;
	virtual void	DiffPrint(char const *fmt, ...) = 0;

	// Allows other parts of the engine to find out the normal and ducked player bbox sizes
	virtual Vector const&	GetPlayerMins(bool ducked) const = 0;
	virtual Vector const&	GetPlayerMaxs(bool ducked) const = 0;
	virtual Vector const&   GetPlayerViewOffset(bool ducked) const = 0;

	virtual bool		IsMovingPlayerStuck(void) const = 0;
	virtual CBasePlayer *GetMovingPlayer(void) const = 0;
	virtual void		UnblockPusher(CBasePlayer *pPlayer, CBaseEntity *pPusher) = 0;

	virtual void SetupMovementBounds(CMoveData *pMove) = 0;
};


class CGameMovement : public IGameMovement
{
public:
	DECLARE_CLASS_NOBASE(CGameMovement);

	CGameMovement(void);
	virtual			~CGameMovement(void);

	virtual void	ProcessMovement(CBasePlayer *pPlayer, CMoveData *pMove);
	virtual void	Reset(void);
	virtual void	StartTrackPredictionErrors(CBasePlayer *pPlayer);
	virtual void	FinishTrackPredictionErrors(CBasePlayer *pPlayer);
	virtual void	DiffPrint(char const *fmt, ...);
	virtual const Vector&	GetPlayerMins(bool ducked) const;
	virtual const Vector&	GetPlayerMaxs(bool ducked) const;
	virtual const Vector&	GetPlayerViewOffset(bool ducked) const;
	virtual void SetupMovementBounds(CMoveData *pMove);

	virtual bool		IsMovingPlayerStuck(void) const;
	virtual CBasePlayer *GetMovingPlayer(void) const;
	virtual void		UnblockPusher(CBasePlayer *pPlayer, CBaseEntity *pPusher);

	// For sanity checking getting stuck on CMoveData::SetAbsOrigin
	virtual void			TracePlayerBBox(const Vector& start, const Vector& end, unsigned int fMask, int collisionGroup, trace_t& pm);

	// wrapper around tracehull to allow tracelistdata optimizations
	void			GameMovementTraceHull(const Vector& start, const Vector& end, const Vector &mins, const Vector &maxs, unsigned int fMask, ITraceFilter *pFilter, trace_t *pTrace);

#define BRUSH_ONLY true
	virtual unsigned int PlayerSolidMask(bool brushOnly = false, CBasePlayer *testPlayer = NULL) const;	///< returns the solid mask for the given player, so bots can have a more-restrictive set
	CBasePlayer		*player;
	CMoveData *GetMoveData() { return mv; }
//protected: //dylan removed
	// Input/Output for this movement
	CMoveData		*mv;

	int				m_nOldWaterLevel;
	float			m_flWaterEntryTime;
	int				m_nOnLadder;

	Vector			m_vecForward;
	Vector			m_vecRight;
	Vector			m_vecUp;


	// Does most of the player movement logic.
	// Returns with origin, angles, and velocity modified in place.
	// were contacted during the move.
	virtual void	PlayerMove(void);

	// Set ground data, etc.
	void			FinishMove(void);

	virtual float	CalcRoll(const QAngle &angles, const Vector &velocity, float rollangle, float rollspeed);

	virtual	void	DecayPunchAngle(void);

	virtual void	CheckWaterJump(void);

	virtual void	WaterMove(void);

	virtual void	WaterJump(void);

	// Handles both ground friction and water friction
	virtual void	Friction(void);

	virtual void	AirAccelerate(Vector& wishdir, float wishspeed, float accel);

	virtual void	AirMove(void);

	virtual bool	CanAccelerate();
	virtual void	Accelerate(Vector& wishdir, float wishspeed, float accel);

	// Only used by players.  Moves along the ground when player is a MOVETYPE_WALK.
	virtual void	WalkMove(void);

	// Try to keep a walking player on the ground when running down slopes etc
	virtual void	StayOnGround(void);

	// Handle MOVETYPE_WALK.
	virtual void	FullWalkMove();

	// Implement this if you want to know when the player collides during OnPlayerMove
	virtual void	OnTryPlayerMoveCollision(trace_t &tr) {}

	virtual const Vector&	GetPlayerMins(void) const; // uses local player
	virtual const Vector&	GetPlayerMaxs(void) const; // uses local player

	typedef enum
	{
		GROUND = 0,
		STUCK,
		LADDER,
		LADDER_WEDGE
	} IntervalType_t;

	virtual int		GetCheckInterval(IntervalType_t type);

	// Useful for things that happen periodically. This lets things happen on the specified interval, but
	// spaces the events onto different frames for different players so they don't all hit their spikes
	// simultaneously.
	bool			CheckInterval(IntervalType_t type);


	// Decompoosed gravity
	virtual void	StartGravity(void);
	virtual void	FinishGravity(void);

	// Apply normal ( undecomposed ) gravity
	virtual void	AddGravity(void);

	// Handle movement in noclip mode.
	void			FullNoClipMove(float factor, float maxacceleration);

	// Returns true if he started a jump (ie: should he play the jump animation)?
	virtual bool	CheckJumpButton(void);	// Overridden by each game.

											// Dead player flying through air., e.g.
	virtual void    FullTossMove(void);

	// Player is a Observer chasing another player
	void			FullObserverMove(void);

	// Handle movement when in MOVETYPE_LADDER mode.
	virtual void	FullLadderMove();

	// The basic solid body movement clip that slides along multiple planes
	virtual int		TryPlayerMove(Vector *pFirstDest = NULL, trace_t *pFirstTrace = NULL);

	virtual bool	LadderMove(void);
	virtual bool	OnLadder(trace_t &trace);
	virtual float	LadderDistance(void) const { return 2.0f; }	///< Returns the distance a player can be from a ladder and still attach to it
	virtual unsigned int LadderMask(void) const { return MASK_PLAYERSOLID; }
	virtual float	ClimbSpeed(void) const { return MAX_CLIMB_SPEED; }
	virtual float	LadderLateralMultiplier(void) const { return 1.0f; }

	// See if the player has a bogus velocity value.
	void			CheckVelocity(void);

	// Does not change the entities velocity at all
	void			PushEntity(Vector& push, trace_t *pTrace);

	// Slide off of the impacting object
	// returns the blocked flags:
	// 0x01 == floor
	// 0x02 == step / wall
	virtual int		ClipVelocity(Vector& in, Vector& normal, Vector& out, float overbounce);

	// If pmove.origin is in a solid position,
	// try nudging slightly on all axis to
	// allow for the cut precision of the net coordinates

	int				CheckStuck(void);

	// Check if the point is in water.
	// Sets refWaterLevel and refWaterType appropriately.
	// If in water, applies current to baseVelocity, and returns true.
	virtual bool	CheckWater(void);
	virtual void	GetWaterCheckPosition(int waterLevel, Vector *pos);

	// Determine if player is in water, on ground, etc.
	virtual void CategorizePosition(void);

	virtual void	CheckParameters(void);

	virtual	void	ReduceTimers(void);

	virtual void	CheckFalling(void);

	virtual void	PlayerRoughLandingEffects(float fvol);

	void			PlayerWaterSounds(void);

	void ResetGetWaterContentsForPointCache();
	int GetWaterContentsForPointCached(const Vector &point, int slot);

	// Ducking
	virtual void	Duck(void);
	virtual void	HandleDuckingSpeedCrop();
	virtual void	FinishUnDuck(void);
	virtual void	FinishDuck(void);
	virtual bool	CanUnduck();
	virtual void	UpdateDuckJumpEyeOffset(void);
	virtual bool	CanUnDuckJump(trace_t &trace);
	virtual void	StartUnDuckJump(void);
	virtual void	FinishUnDuckJump(trace_t &trace);
	virtual void	SetDuckedEyeOffset(float duckFraction);
	virtual void	FixPlayerCrouchStuck(bool moveup);

	float			SplineFraction(float value, float scale);

	virtual void	CategorizeGroundSurface(trace_t &pm);

	virtual bool	InWater(void);

	// Commander view movement
	void			IsometricMove(void);

	// Traces the player bbox as it is swept from start to end
	virtual CBaseHandle		TestPlayerPosition(const Vector& pos, int collisionGroup, trace_t& pm);

	// Checks to see if we should actually jump 
	void			PlaySwimSound();

	bool			IsDead(void) const;

	// Figures out how the constraint should slow us down
	float			ComputeConstraintSpeedFactor(void);

	virtual void	SetGroundEntity(trace_t *pm);

	virtual void	StepMove(Vector &vecDestination, trace_t &trace);

protected:
	virtual ITraceFilter *LockTraceFilter(int collisionGroup);
	virtual void UnlockTraceFilter(ITraceFilter *&pFilter);

	// Performs the collision resolution for fliers.
	void			PerformFlyCollisionResolution(trace_t &pm, Vector &move);

	virtual bool	GameHasLadders() const;

	enum
	{
		// eyes, waist, feet points (since they are all deterministic
		MAX_PC_CACHE_SLOTS = 3,
	};

	// Cache used to remove redundant calls to GetPointContents() for water.
	int m_CachedGetPointContents[MAX_PLAYERS][MAX_PC_CACHE_SLOTS];
	Vector m_CachedGetPointContentsPoint[MAX_PLAYERS][MAX_PC_CACHE_SLOTS];

	//private:
	bool			m_bSpeedCropped;

	bool			m_bProcessingMovement;
	bool			m_bInStuckTest;

	float			m_flStuckCheckTime[MAX_PLAYERS + 1][2]; // Last time we did a full test

															// special function for teleport-with-duck for episodic
#ifdef HL2_EPISODIC
public:
	void			ForceDuck(void);

#endif
	ITraceListData	*m_pTraceListData;

	int				m_nTraceCount;
};


#define INVALID_ENTITY_HANDLE INVALID_EHANDLE_INDEX
#define PRINTF_FORMAT_STRING _Printf_format_string_
enum PLAYER_ANIM;

enum soundlevel_t
{
	SNDLVL_NONE = 0,

	SNDLVL_20dB = 20,			// rustling leaves
	SNDLVL_25dB = 25,			// whispering
	SNDLVL_30dB = 30,			// library
	SNDLVL_35dB = 35,
	SNDLVL_40dB = 40,
	SNDLVL_45dB = 45,			// refrigerator

	SNDLVL_50dB = 50,	// 3.9	// average home
	SNDLVL_55dB = 55,	// 3.0

	SNDLVL_IDLE = 60,	// 2.0	
	SNDLVL_60dB = 60,	// 2.0	// normal conversation, clothes dryer

	SNDLVL_65dB = 65,	// 1.5	// washing machine, dishwasher
	SNDLVL_STATIC = 66,	// 1.25

	SNDLVL_70dB = 70,	// 1.0	// car, vacuum cleaner, mixer, electric sewing machine

	SNDLVL_NORM = 75,
	SNDLVL_75dB = 75,	// 0.8	// busy traffic

	SNDLVL_80dB = 80,	// 0.7	// mini-bike, alarm clock, noisy restaurant, office tabulator, outboard motor, passing snowmobile
	SNDLVL_TALKING = 80,	// 0.7
	SNDLVL_85dB = 85,	// 0.6	// average factory, electric shaver
	SNDLVL_90dB = 90,	// 0.5	// screaming child, passing motorcycle, convertible ride on frw
	SNDLVL_95dB = 95,
	SNDLVL_100dB = 100,	// 0.4	// subway train, diesel truck, woodworking shop, pneumatic drill, boiler shop, jackhammer
	SNDLVL_105dB = 105,			// helicopter, power mower
	SNDLVL_110dB = 110,			// snowmobile drvrs seat, inboard motorboat, sandblasting
	SNDLVL_120dB = 120,			// auto horn, propeller aircraft
	SNDLVL_130dB = 130,			// air raid siren

	SNDLVL_GUNFIRE = 140,	// 0.27	// THRESHOLD OF PAIN, gunshot, jet engine
	SNDLVL_140dB = 140,	// 0.2

	SNDLVL_150dB = 150,	// 0.2

	SNDLVL_180dB = 180,			// rocket launching

								// NOTE: Valid soundlevel_t values are 0-255.
								//       256-511 are reserved for sounds using goldsrc compatibility attenuation.
};

//-----------------------------------------------------------------------------
// Functions the engine provides to IGameMovement to assist in its movement.
//-----------------------------------------------------------------------------

abstract_class IMoveHelper
{
public:
	// Call this to set the singleton
	static IMoveHelper* GetSingleton() { return sm_pSingleton; }

	// Methods associated with a particular entity
	virtual	char const*		GetName(EntityHandle_t handle) const = 0;

	virtual void SetHost(CBasePlayer *host) = 0; //dylan moved

	// Adds the trace result to touch list, if contact is not already in list.
	virtual void	ResetTouchList(void) = 0;
	virtual bool	AddToTouched(const CGameTrace& tr, const Vector& impactvelocity) = 0;
	virtual void	ProcessImpacts(void) = 0;

	// Numbered line printf
	virtual void	Con_NPrintf(int idx, PRINTF_FORMAT_STRING char const* fmt, ...) = 0;

	// These have separate server vs client impementations
	virtual void	StartSound(const Vector& origin, int channel, char const* sample, float volume, soundlevel_t soundlevel, int fFlags, int pitch) = 0;
	virtual void	StartSound(const Vector& origin, const char *soundname) = 0;
	virtual void	PlaybackEventFull(int flags, int clientindex, unsigned short eventindex, float delay, Vector& origin, Vector& angles, float fparam1, float fparam2, int iparam1, int iparam2, int bparam1, int bparam2) = 0;

	// Apply falling damage to m_pHostPlayer based on m_pHostPlayer->m_flFallVelocity.
	virtual bool	PlayerFallingDamage(void) = 0;

	// Apply falling damage to m_pHostPlayer based on m_pHostPlayer->m_flFallVelocity.
	virtual void	PlayerSetAnimation(PLAYER_ANIM playerAnim) = 0;

	virtual IPhysicsSurfaceProps *GetSurfaceProps(void) = 0;

	virtual bool IsWorldEntity(const CBaseHandle &handle) = 0;

protected:
	// Inherited classes can call this to set the singleton
	static void SetSingleton(IMoveHelper* pMoveHelper) { sm_pSingleton = pMoveHelper; }

	// Clients shouldn't call delete directly
	virtual			~IMoveHelper() {}

	// The global instance
	static IMoveHelper* sm_pSingleton;
};

//-----------------------------------------------------------------------------
// Implementation of the movehelper on the server
//-----------------------------------------------------------------------------

abstract_class IMoveHelperServer : public IMoveHelper
{
//public:
	//virtual void SetHost(CBasePlayer *host) = 0; //dylan removed
};

class CMoveHelperServer : public IMoveHelperServer
{
public:
	CMoveHelperServer(void);
	virtual ~CMoveHelperServer();

	// Methods associated with a particular entity
	virtual	char const*		GetName(EntityHandle_t handle) const;

	// Touch list...
	virtual void	ResetTouchList(void);
	virtual bool	AddToTouched(const trace_t &tr, const Vector& impactvelocity);
	virtual void	ProcessImpacts(void);

	virtual bool	PlayerFallingDamage(void);
	virtual void	PlayerSetAnimation(PLAYER_ANIM eAnim);

	// Numbered line printf
	virtual void	Con_NPrintf(int idx, char const* fmt, ...);

	// These have separate server vs client impementations
	virtual void	StartSound(const Vector& origin, int channel, char const* sample, float volume, soundlevel_t soundlevel, int fFlags, int pitch);
	virtual void	StartSound(const Vector& origin, const char *soundname);

	virtual void	PlaybackEventFull(int flags, int clientindex, unsigned short eventindex, float delay, Vector& origin, Vector& angles, float fparam1, float fparam2, int iparam1, int iparam2, int bparam1, int bparam2);
	virtual IPhysicsSurfaceProps *GetSurfaceProps(void);

	//void DylanMissingFunction(void);

	//void			SetHost(CBasePlayer *host);

	virtual bool IsWorldEntity(const CBaseHandle &handle);

private:
	CBasePlayer*	m_pHostPlayer;

	// results, tallied on client and server, but only used by server to run SV_Impact.
	// we store off our velocity in the trace_t structure so that we can determine results
	// of shoving boxes etc. around.
	struct touchlist_t
	{
		Vector	deltavelocity;
		trace_t trace;
	};

	//CUtlVector<touchlist_t>	m_TouchList;
};
