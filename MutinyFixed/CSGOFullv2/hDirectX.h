#ifndef HDIRECTX_H
#define HDIRECTX_H
#pragma once
//#include "hMain.h"
#include "hDrawings.h"

#include <Windows.h>
#include <iostream>
//#include "D3DFont.h"

#include <d3d9.h>
#include <d3dx9.h>
#pragma comment(lib, "d3d9.lib")
#pragma comment(lib, "d3dx9.lib")

#include <dwmapi.h>
#pragma comment(lib, "dwmapi.lib")

extern IDirect3D9Ex* p_Object;
extern IDirect3DDevice9Ex* p_Device;
extern D3DPRESENT_PARAMETERS p_Params;
extern ID3DXLine* p_Line;
extern ID3DXFont* pFontSmall;
extern ID3DXFont* pFont;
extern ID3DXSprite* pSprite;
//extern CD3DFont* pNewFontSmall;
//extern CD3DFont* pNewFont;
extern bool bNewFontCanDraw;
extern bool bNewFontSmallCanDraw;
extern bool MESSAGE_PUMP_EMPTY;
extern BOOL RunningFrame;

bool DirectXInit(HWND hWnd);
bool CreateRenderObjects();
void FreeRenderObjects();
bool DirectXBeginFrame();
void DirectXClearScreen();
void DirectXEndFrame();
void RenderObjectsOnDeviceLost();
void RenderObjectsOnDeviceReset();
void FreeRenderObjects();
extern float FRAME_TIME;
inline void SET_FRAME_TIME(bool force, float ft) { if ( force || (ft < FRAME_TIME) ) FRAME_TIME = ft; }
#define DEFAULT_FRAME_TIME (1.0f / 64.0f)
void DrawBone(int bone1, int bone2, DWORD BoneMatrix);

#endif