#ifndef OFFSETS_H
#define OFFSETS_H
#pragma once

extern unsigned long RadarBaseAdr;
extern unsigned long GlowObjectManagerAdr;

// Updated Using OLLYBG

#define AllowThirdPersonOffset (0x0A4 / 4)
#define dw_m_BoneAccessor 0x2694
#define standardblendingrules_offset (0x320 / 4)
#define buildtransformations_offset (0x2E0 / 4)
#define unknownsetupbonesfn_offset (0x260 / 4)
#define unknownsetupbonesfn2_offset (0x300 / 4)
#define LastBoneChangedTime_Offset (956 / 4)
#define IsViewModel_index (912 / 4)
#define LastSetupBonesFrameCount 0xA5C
#define m_iLastOcclusionCheckFrameCount 0xA30
#define m_iLastOcclusionCheckFlags 0xA28
#define m_BoneSetupLock 9892
#define PlayFootstepSoundOffset (0x548 / 4)
#define next_message_time 0x110
#define m_pStudioHdr 0x293C
#define m_pStudioHdr2 0x2938
#define m_Local 0x2FAC
#define m_Collision 0x0318
#define m_pRagdoll 0x9F4
#define m_bClientSideRagdoll 0x275
#define m_CachedBoneData 0x2900
#define m_iPrevBoneMask 0x268C
#define m_iAccumulatedBoneMask 0x2690
#define m_flLastBoneSetupTime 0x2914
#define m_pIK_offset 9824
#define m_EntClientFlags 104
#define m_bIsToolRecording 0x28D
#define m_dwGetPredictable 0x2EA
#define m_iMostRecentModelBoneCounter 0x2680
#define m_hNetworkMoveParent 0x144
#define m_pMoveParent 0x300
#define ShouldInterpolateIndex (0x2B4 / 4)
#define EyeAnglesIndex (0x290 / 4)
#define m_dwWorldSpaceCenterOffset 0x138
#define m_dwGetAbsAnglesOffset 0x2C
#define m_dwEntIndexOffset 0x28
#define dwClientTickCount 0x4C94
#define dwServerTickCount 0x16C
#define m_flMaxSpeed 0x322C
#define m_flStepSize 0x300C
#define m_flFriction 0x0140
#define m_iBurstShotsRemaining 0x3334
#define dwm_nAnimOverlay 0x2970
#define deadflag 0x31C4
#define dwRenderAngles (deadflag + 0x4)
#define m_iObserverMode 0x334C
#define m_flPostponeFireReadyTime 0x32C8
#define SetHost_CurrentHost 0x18
#define m_dwGetCollisionAngles 0x24
#define m_flFriction 0x0140
#define m_flLowerBodyYawTarget 0x39DC
#define m_dwShouldCollide 0x29C
#define m_dwGetSolid 0x144
#define chokedcommands 0x4CB0
#define m_pCurrentCommand 0x3314
#define lastoutgoingcommand 0x4CA8
#define m_OffsetEyePos 0x28C
#define m_OffsetEyeAngles 0x290
#define m_dwGetBaseAnimating 0x0B0
#define m_dwGetLastUserCommand 0x60
#define m_dwGetAbsOrigin 0xA0
#define m_dwIsPlayer 0x260
#define m_dwGetNetChannel 0x50
#define m_fEffects 0x00EC
#define m_dwGetNetChannel2 0
#define m_dwIsConnected 0x84
#define m_dwIsSpawned 0x88
#define m_bReloadVisuallyComplete  0x32CC
#define m_dwIsActive 0x8C
#define m_dwIsFakeClient 0x90
#define m_nMaxClients 0x310 //ClientState
#define isHLTV 0x4CC8 //ClientState
#define m_nSolidType 0x033A
#define m_bInSimulation 0x4C90 //ClientState
#define m_bPinPulled 0x3342
#define m_fThrowTime 0x3344
#define m_bRedraw 0x334C
#define m_nForceBone 0x267C
#define m_hObserverTarget 0x3360
#define m_dwSetLocalViewAngles 0x5A4
#define m_dwMaxPlayer 0x00000308
#define m_iTeamNum 0x000000F0
#define m_bDormant 0x000000E9
#define m_iGlowIndex 0xA310
#define m_dwLocalPlayerIndex 0x180
#define m_dwViewAngles 0x4D10
#define m_iCrossHairID 0xB2A4
#define m_iHealth 0x000000FC
#define m_vecOldOrigin (0x3A0 + 8)
#define m_vecOrigin 0x00000134 //network origin
#define m_vecOriginReal 0xAC //same as local origin
#define m_vecLocalOrigin m_vecOriginReal
#define m_dwBoneMatrix 0x2698
#define m_lifeState 0x25B
#define m_iCompetitiveRanking 0x1A44
#define m_iCompetitiveWins 0x00001B48
#define m_bSpotted 0x939
#define m_dwRadarBasePointer 0x00000054
#define m_bIsScoped 0x387E
#define m_hPlayerAnimState 0x3870
#define m_hPlayerAnimStateServer 0x25D4
#define m_flGoalFeetYawServer 0x70
#define m_flCurrentFeetYawServer 0x68
#define m_flNextLowerBodyYawUpdateTimeServer 0x0FC
#define m_iFOV 0x31D4
#define m_fFlags 0x100
#define m_nTickBase 0x3404
#define m_iShotsFired 0xA2A0
#define m_nMoveType 0x00000258
#define m_nModelIndex 0x0254
#define m_nHitboxSet 0x09FC
#define m_ArmorValue 0xB238
#define m_hOwnerEntity 0x148
#define m_flC4Blow 0x297C
#define m_flFlashDuration 0xA2F8
#define m_bGunGameImmunity 0x3894
#define m_bHasHelmet 0xB22C
#define m_bHasDefuser 0xB248
#define m_bIsDefusing 0x3888
#define m_bInReload (m_bIsDefusing - 0x65F)
#define m_bIsBroken 0x0A04
#define m_viewPunchAngle 0x3010//0x300C
#define m_aimPunchAngle 0x301C//0x3018
#define m_aimPunchAngleVel 0x3028
#define m_angEyeAngles 0xB23C
#define m_angEyeAnglesServer 0x29B0
#define m_vecViewOffset 0x0104
#define m_vecVelocity 0x0110 //network velocity
#define m_vecVelocityReal 0xD0
#define m_szLastPlaceName 0x3588
#define m_hActiveWeapon 0x2EE8
#define m_CollisionGroup 0x470
#define m_iWeaponID 0x32EC
#define m_flNextPrimaryAttack 0x31D8
#define m_flNextSecondaryAttack 0x31DC
#define m_fAccuracyPenalty 0x32B0
#define m_OriginalOwnerXuidLow 0x3168
#define m_OriginalOwnerXuidHigh 0x316C
#define m_iEntityQuality 0x2F8C
#define m_iAccountID 0x2FA8
#define m_iItemIDHigh 0x2FA0
#define m_iItemDefinitionIndex 0x2F88
#define m_nFallbackPaintKit 0x3170
#define m_nFallbackStatTrak 0x317C
#define m_flFallbackWear 0x3178
#define m_iClip1 0x3204
#define m_vecBaseVelocity 0x011C
#define m_angRotation 0x0128
#define m_vecLocalAngles (0xC8 + 8)
#define m_vecOldAngRotation (0x3AC + 8)
#define m_flAnimTime 0x025C
#define m_flAnimTimeOld (m_flAnimTime + 4)
#define m_flSimulationTime 0x264
#define m_flOldSimulationTime (0x260 + 8)
#define m_flSimulationTimeServer 0x6C
#define m_nCreationTick (0x998 + 8)
#define m_flProxyRandomValue (0x0D8 + 8)
#define m_vecMins 0x0320
#define m_vecMaxs 0x032C
#define m_nSequence 0x28AC
#define m_vecForce 0x2670
#define m_nBody 0x0A20
#define m_flPoseParameter 0x2764
#define m_flPlaybackRate 0x0A18
#define m_bClientSideAnimation 0x288C
#define m_flCycle 0x0A14
#define m_flNextAttack 0x2D60
#define m_bDucked 0x3034
#define m_bDucking 0x3035
#define m_bInDuckJump 0x303C
#define m_nDuckTimeMsecs 0x2FF8
#define m_nDuckJumpTimeMsecs 0x2FFC
#define m_nJumpTimeMsecs 0x3000
#define m_flFallVelocity 0x3004
#define m_nNextThinkTick 0x00F8
#define m_flLaggedMovementValue 0x3568
#define m_hGroundEntity 0x014C
#define m_flDuckAmount 0x2F9C
#define m_flDuckSpeed 0x2FA0
#define m_vecLadderNormal 0x3214
#define m_flVelocityModifier 0xA2AC

// entity effects
enum
{
	EF_BONEMERGE = 0x001,	// Performs bone merge on client side
	EF_BRIGHTLIGHT = 0x002,	// DLIGHT centered at entity origin
	EF_DIMLIGHT = 0x004,	// player flashlight
	EF_NOINTERP = 0x008,	// don't interpolate the next frame
	EF_NOSHADOW = 0x010,	// Don't cast no shadow
	EF_NODRAW = 0x020,	// don't draw entity
	EF_NORECEIVESHADOW = 0x040,	// Don't receive no shadow
	EF_BONEMERGE_FASTCULL = 0x080,	// For use with EF_BONEMERGE. If this is set, then it places this ent's origin at its
									// parent and uses the parent's bbox + the max extents of the aiment.
									// Otherwise, it sets up the parent's bones every frame to figure out where to place
									// the aiment, which is inefficient because it'll setup the parent's bones even if
									// the parent is not in the PVS.
									EF_ITEM_BLINK = 0x100,	// blink an item so that the user notices it.
									EF_PARENT_ANIMATES = 0x200,	// always assume that the parent entity is animating
#ifdef NFS_DLL
									EF_POWERUP_SPEED = 0x400,
									EF_POWERUP_DAMAGE = 0x800,
									EF_POWERUP_AMMO = 0x1000,
									EF_POWERUP_ARMOR = 0x2000,
									EF_POWERUP_BATTERY = 0x4000,
									EF_ROCKET = 0x8000, //dlight that glows orange-red
									EF_MAX_BITS = 15
#else
									EF_MAX_BITS = 10
#endif
};

#endif