#pragma once
#include "VTHook.h"
#include "Interfaces.h"
#include "Math.h"
#include "IGameMovement.h"
extern Vector2D RecoilCrosshairScreen; //MOVEME
extern int iDamageGivenToTarget;
extern int iLastHitgroupHit;
extern int iLastHealthTarget;
extern CBaseEntity* LastPlayerDamageGiven;
extern int iHitsToTarget;
extern float flTimeDamageWasGiven;
extern bool WaitForRePress;
extern ModifiableUserCmd CurrentUserCmd;
extern int buttons;
extern bool IsReadyForFiring;
extern bool SetLastAngleFirstTime;

#if 1
extern bool FIXED_NETVARS;
extern float Fixed_LocalSpread;
extern float Fixed_LocalCone;
extern QAngle Fixed_LocalEyeAngles;;
extern QAngle Fixed_LocalAimPunch;
extern QAngle Fixed_LocalViewPunch;
extern Vector Fixed_LocalAimPunchVel;
extern Vector Fixed_LocalEyePos;
extern Vector Fixed_LocalOrigin;
extern Vector Fixed_LocalVelocity;
extern Vector Fixed_LocalBaseVelocity;
extern QAngle Fixed_LocalAngRotation;
extern int Fixed_LocalMoveType;
extern int Fixed_LocalMoveCollide;
extern Vector Fixed_LocalMins;
extern Vector Fixed_LocalMaxs;
extern int Fixed_LocalSequence;
extern float Fixed_LocalPoseParameter;
extern unsigned char Fixed_LocalClientSideAnimation;
extern float Fixed_LocalCycle;
extern float Fixed_LocalNextAttack;
extern bool Fixed_LocalLocalDucked;
extern bool Fixed_LocalLocalDucking;
extern bool Fixed_LocalInDuckJump;
extern int Fixed_LocalDuckTimeMsecs;
extern int Fixed_LocalJumpTimeMsecs;
extern float Fixed_LocalFallVelocity;
extern Vector Fixed_LocalViewOffset;
extern int Fixed_LocalTickBase;
extern int Fixed_LocalNextThinkTick;
extern int Fixed_LocalFlags;
extern float Fixed_LocalDuckAmount;
extern float Fixed_LocalDuckSpeed;
extern float Fixed_LocalVelocityModifier;
extern int Fixed_LocalActiveWeapon;
extern void* Fixed_LocalMyWeapons;
extern int Fixed_LocalHitboxSet;
extern float Fixed_LocalPlaybackRate;
extern float Fixed_LocalAnimTime;
extern float Fixed_LocalSimulationTime;
extern int Fixed_LocalModelIndex;
extern float Fixed_LocalLaggedMovement;
extern CBaseEntity* Fixed_LocalGroundEntity;
extern Vector Fixed_VecLadderNormal;
#endif