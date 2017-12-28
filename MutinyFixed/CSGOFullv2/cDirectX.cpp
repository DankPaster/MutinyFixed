#include "hDirectX.h"
#include "GameMemory.h"
#include "CSGO_HX.h"
//#include "hClasses.h"

IDirect3D9Ex* p_Object = 0;
IDirect3DDevice9Ex* p_Device = 0;
D3DPRESENT_PARAMETERS p_Params;

ID3DXLine* p_Line;
ID3DXFont* pFontSmall = 0;
ID3DXFont* pFont = 0;
ID3DXSprite* pSprite = 0;
//CD3DFont* pNewFontSmall = 0;
//CD3DFont* pNewFont = 0;
bool bNewFontCanDraw = false;
bool bNewFontSmallCanDraw = false;

bool DirectXInit(HWND hWnd)
{
	if(FAILED(Direct3DCreate9Ex(D3D_SDK_VERSION, &p_Object)))
		return false;

	ZeroMemory(&p_Params, sizeof(p_Params));    
	p_Params.EnableAutoDepthStencil = TRUE;
	p_Params.AutoDepthStencilFormat = D3DFMT_D16;
    p_Params.hDeviceWindow = hWnd;    
	p_Params.MultiSampleQuality   = D3DMULTISAMPLE_NONE;
    p_Params.BackBufferFormat = D3DFMT_A8R8G8B8 ;     
	p_Params.Windowed = TRUE;
	p_Params.SwapEffect = D3DSWAPEFFECT_DISCARD;
    p_Params.BackBufferWidth = Width;    
    p_Params.BackBufferHeight = Height;    
	p_Params.PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;

	if (FAILED(p_Object->CreateDeviceEx(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hWnd, D3DCREATE_HARDWARE_VERTEXPROCESSING, &p_Params, 0, &p_Device)))
		return false;

	if (!CreateRenderObjects())
		return false;

	return true;
}

/*
typedef struct
{
	float flMatrix[4][4];
}WorldToScreenMatrix_t;
*/

/*
float Get3dDistance(float * myCoords, float * enemyCoords)
{
	return sqrt(
		pow(double(enemyCoords[0] - myCoords[0]), 2.0) +
		pow(double(enemyCoords[1] - myCoords[1]), 2.0) +
		pow(double(enemyCoords[2] - myCoords[2]), 2.0));

}
*/

BOOL RunningFrame = 0;
static float TimeToZClear = 0.0f;

bool CreateRenderObjects()
{
	if (!p_Device)
		return false;

	if (!p_Line || !pFont || !pFontSmall || !pSprite /*|| !pNewFont || !pNewFontSmall*/)
	{
		//if (p_Device->Reset(&p_Params) == D3D_OK)
		{
			if (!p_Line)
			{
				if (FAILED(D3DXCreateLine(p_Device, &p_Line)))
					return false;
			}

			if (!pFont)
			{
				if (FAILED(D3DXCreateFont(p_Device, 18, 0, 0, 0, false, DEFAULT_CHARSET, OUT_CHARACTER_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH, "Arial", &pFont)))
					return false;
			}

			if (!pFontSmall)
			{
				if (FAILED(D3DXCreateFont(p_Device, 12, 0, 0, 0, false, DEFAULT_CHARSET, OUT_CHARACTER_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH, "Arial", &pFontSmall)))
					return false;
			}

			if (!pSprite)
			{
				if (FAILED(D3DXCreateSprite(p_Device, &pSprite)))
					return false;
			}
#if 0
			if (!pNewFont)
			{
				pNewFont = new CD3DFont("Arial", 12, p_Device, D3DFONT_BACKGROUND);
				if (!pNewFont) return false;
			}

			if (!pNewFontSmall)
			{
				pNewFontSmall = new CD3DFont("Arial", 7, p_Device, D3DFONT_BACKGROUND);
				if (!pNewFontSmall) return false;
			}
#endif
		}
	}

	return true;
}

void RenderObjectsOnDeviceLost()
{
	if (pFont)
	{
		pFont->OnLostDevice();
	}
	if (pFontSmall)
	{
		pFontSmall->OnLostDevice();
	}
	if (p_Line)
	{
		p_Line->OnLostDevice();
	}
	if (pSprite)
	{
		pSprite->OnLostDevice();
	}
}

void RenderObjectsOnDeviceReset()
{
	if (pFont)
	{
		pFont->OnResetDevice();
	}
	if (pFontSmall)
	{
		pFontSmall->OnResetDevice();
	}
	if (p_Line)
	{
		p_Line->OnResetDevice();
	}
	if (pSprite)
	{
		pSprite->OnResetDevice();
	}
}

void FreeRenderObjects()
{
	if (pFont)
	{
		pFont->Release();
		pFont = nullptr;
	}
	if (pFontSmall)
	{
		pFontSmall->Release();
		pFontSmall = nullptr;
	}
	if (p_Line)
	{
		p_Line->Release();
		p_Line = nullptr;
	}
	if (pSprite)
	{
		pSprite->Release();
		pSprite = nullptr;
	}
#if 0
	if (pNewFont)
	{
		delete pNewFont;
		pNewFont = nullptr;
	}

	if (pNewFontSmall)
	{
		delete pNewFontSmall;
		pNewFontSmall = nullptr;
	}
#endif
}

static bool DEVICE_LOST = false;
bool MESSAGE_PUMP_EMPTY = false;

bool DeviceIsNormal()
{
	if (DEVICE_LOST)
	{
		HRESULT hState = p_Device->TestCooperativeLevel();

		if (hState == D3DERR_DEVICELOST)
		{
			return false;
		}
		else if (hState == D3DERR_DEVICENOTRESET)
		{
			RenderObjectsOnDeviceLost();
			p_Device->Reset(&p_Params);
			return false;
		}
		else if (hState == D3D_OK)
		{
			RenderObjectsOnDeviceReset();
			DEVICE_LOST = false;
		}
	}
	return true;
}

bool CanRender()
{
	if (p_Device == nullptr)
		return false;

	if (!DeviceIsNormal())
		return false;

	//if (!MESSAGE_PUMP_EMPTY)
		//return false;

	return true;
	//return CreateRenderObjects();
}

HDC layhdc;
static float LastTimeRenderedFrame = 0.0f;
float FRAME_TIME = DEFAULT_FRAME_TIME;
//static float NextFrameRenderTime = 0.0f;
static bool EndingFrame = false; //Don't allow starting new frames if we haven't finished presenting
bool DirectXBeginFrame()
{
#ifdef NO_DIRECTX
	return true;
#endif
	if (EndingFrame || !CanRender() /*|| (Time - LastTimeRenderedFrame) < FRAME_TIME*/)
		return false;

	if (!RunningFrame)
	{
		//	layhdc = GetDC(hWnd);
		//InvalidateRect(hWnd, NULL, 0);
		// Re-enable window updates
		//SendMessage(hWnd, WM_SETREDRAW, TRUE, 0);
		p_Device->Clear(0, 0, D3DCLEAR_TARGET, 0, 1.0f, 0);
		if (!FAILED(p_Device->BeginScene()))
		{

#if 0
			pNewFontSmall->BeginDrawing();
			pNewFont->BeginDrawing();
			bNewFontCanDraw = true;
			bNewFontSmallCanDraw = true;
#endif

			////pSprite->Begin(D3DXSPRITE_ALPHABLEND | D3DXSPRITE_DO_NOT_ADDREF_TEXTURE);
			//TimeToZClear = Time + 0.2f;
			RunningFrame = 1;
		}
		else
		{
			return false;
		}
	}

	return true;
}

void DirectXClearScreen()
{
	//if (RunningFrame || EndingFrame || !CanRender())
		//return;
	//layhdc = GetDC(hWnd);

	if (!DeviceIsNormal())
		return;

	p_Device->Clear(0, 0, D3DCLEAR_TARGET, 0, 1.0f, 0);
	if (!RunningFrame)
	{
		p_Device->BeginScene();
	}
	p_Device->EndScene();
	DEVICE_LOST = p_Device->PresentEx(0, 0, 0, 0, 0) == D3DERR_DEVICELOST;
	//ReleaseDC(hWnd, layhdc);
	//InvalidateRect(hWnd, NULL, 0);
}

void DirectXEndFrame()
{
#ifndef NO_DIRECTX
	// Re-enable window updates
	//SendMessage(hWnd, WM_SETREDRAW, TRUE, 0);

	if (RunningFrame)
	{
		EndingFrame = true;
		RunningFrame = 0;
#if 0
		bNewFontCanDraw = false;
		bNewFontSmallCanDraw = false;
		pNewFontSmall->EndDrawing();
		pNewFont->EndDrawing();
#endif
		////pSprite->End();
		p_Device->EndScene();
		DEVICE_LOST = p_Device->PresentEx(0, 0, 0, 0, 0) == D3DERR_DEVICELOST;
		EndingFrame = false;
		LastTimeRenderedFrame = Time;
		//NextFrameRenderTime = Time + (1.0f / 5.0f);// -min(max(0.0f, Time - NextFrameRenderTime), 0.2f);
	//	ReleaseDC(hWnd, layhdc);
		//SET_FRAME_TIME(true, DEFAULT_FRAME_TIME);
	}

#if 0
	if (Time >= TimeToZClear)
	{
		DirectXClearScreen();
		TimeToZClear = Time + 0.2f;
	}
#endif
	//ValidateRect(hWnd, NULL);
	// Disable window updates
	//SendMessage(hWnd, WM_SETREDRAW, FALSE, 0);
#endif
}

#if 0
void Render(bool IsActiveWindow)
{
	p_Device->Clear(0, 0, D3DCLEAR_TARGET, 0, 1.0f, 0);
	p_Device->BeginScene();

	//Code inside this if statement will only be called if CS:GO is our current window
	if (tWnd == GetForegroundWindow())
	{
		char * value = "Counter-Strike: Global Offensive";


		HWND newhwnd = FindWindow(NULL, value);
		if (newhwnd != NULL){
			GetWindowRect(newhwnd, &rc);
		}
		else{
			ExitProcess(0);
		}

		DrawShadowString("DYLAN OVERLAY TEST", 10, 10, 255, 255, 255, pFont);
		
#if 0
		CLocalPlayer* pLocal = new CLocalPlayer();//Define our local player
		pLocal->ReadData();//Read our local data

		for (int i = 0; i < 64; i++)//2Lazy4RealPlayercount
		{
			CPlayer* pEntity = new CPlayer();//Define pEntity for every player
			pEntity->ReadData(i);

			//Decrale 2 float arrays 0 = x, 1 = y, 2 = z
			float W2S[3];
			float W2S_Head[3];
			if (WorldToScreen(pEntity->Pos, W2S) && WorldToScreen(pEntity->HeadPos, W2S_Head));
			{
				//Get the height of our box by taking the W2S'd feet position minus the W2S'd head position
				int height = abs(W2S[1] - W2S_Head[1]);//abs = absolute http://www.cplusplus.com/reference/cstdlib/abs/
				int width = height / 2; //You can change this value to anything you would liek

				//Check if the player is alive and if it's NOT our player. Why would we need a box on our own player?
				if (pEntity->isAlive && pLocal->Player != pEntity->Player)
				{
					//Check if the player is in our team
					if (pEntity->Team == pLocal->Team)
						DrawBox(W2S[0] - (width / 2), W2S[1] - height, width, height, 1, 0, 0, 255, 255);
					else//Check if the player is not in our team
						DrawBox(W2S[0] - (width / 2), W2S[1] - height, width, height, 1, 255, 0, 0, 255);

					//Outline and inline boxes
					DrawBox(W2S[0] - (width / 2) - 1, W2S[1] - height - 1, width + 2, height + 2, 1, 0, 0, 0, 255);
					DrawBox(W2S[0] - (width / 2) + 1, W2S[1] - height + 1, width - 2, height - 2, 1, 0, 0, 0, 255);
					
					/*---- Bone ESP ----*/
					//Crotch to neck
					DrawBone(0, 5, pEntity->BoneBase);

					//Left Arm
					DrawBone(5, 6, pEntity->BoneBase);
					DrawBone(6, 36, pEntity->BoneBase);
					DrawBone(36, 31, pEntity->BoneBase);
					DrawBone(31, 21, pEntity->BoneBase);

					//Right Arm
					DrawBone(5, 12, pEntity->BoneBase);
					DrawBone(12, 38, pEntity->BoneBase);
					DrawBone(38, 15, pEntity->BoneBase);

					//Left Leg
					DrawBone(0, 32, pEntity->BoneBase);
					DrawBone(27, 28, pEntity->BoneBase);

					//Right Leg
					DrawBone(0, 33, pEntity->BoneBase);
					DrawBone(33, 25, pEntity->BoneBase);
				}
			}
		}
#endif
	}

	p_Device->EndScene();
	p_Device->PresentEx(0, 0, 0, 0, 0);
}
#endif