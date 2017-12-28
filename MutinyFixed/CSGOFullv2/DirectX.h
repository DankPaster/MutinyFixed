#pragma once
#include "Overlay.h"

typedef long(__stdcall* ResetFn)(IDirect3DDevice9*, D3DPRESENT_PARAMETERS*);
typedef long(__stdcall* EndSceneFn)(IDirect3DDevice9*);
extern ResetFn oReset;
extern EndSceneFn oEndScene;
extern int DRAW_CALL;
extern int DRAW_CALL_MAX;

long __stdcall Hooked_Reset(IDirect3DDevice9* pDevice, D3DPRESENT_PARAMETERS* pPresentationParameters);
long __stdcall Hooked_EndScene(IDirect3DDevice9* pDevice);