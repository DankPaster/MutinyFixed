#pragma once
#include "GameEventListener.h"
void CreateEventListeners();
void GameEvent_PlayerHurt(IGameEvent* gameEvent);
void GameEvent_BulletImpact(IGameEvent* gameEvent);
void GameEvent_PlayerDeath(IGameEvent* gameEvent);
void DecayPlayerIsShot();