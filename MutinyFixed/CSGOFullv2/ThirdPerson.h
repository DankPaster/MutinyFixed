#pragma once
#include "VTHook.h"

extern QAngle camAngles;
extern Vector camOrigin;
extern bool ThirdPerson;
extern Vector m_vecDesiredCameraOffset;
extern Vector m_vecCameraOffset;
Vector GetFinalCameraOffset(void);
void UpdateThirdPerson(CViewSetup *pSetup);
inline void SetDesiredCameraOffset(const Vector& vecOffset) { m_vecDesiredCameraOffset = vecOffset; }
inline const Vector& GetDesiredCameraOffset(void) { return m_vecDesiredCameraOffset; }
inline void SetCameraOffsetAngles(const Vector& vecOffset) { m_vecCameraOffset = vecOffset; }
inline const Vector&	GetCameraOffsetAngles(void) { return m_vecCameraOffset; }