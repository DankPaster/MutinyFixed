#pragma once
#include "CreateMove.h"

enum FakeLagValues : int
{
	FAKELAG_OFF = 0,
	FAKELAG_STATIC,
	FAKELAG_LAGCOMP,
	FAKELAG_STEP,
	FAKELAG_RANDOM
};

void FakeLag(int& buttons, uintptr_t FramePointer);
