#ifdef NewMenu
#include "framework.h"
#endif
#include "Menu.h"
#include "DirectX.h"
#include "hDirectX.h"
#include "PlayerList.h"
#include "Interfaces.h"
#include "GrenadePrediction.h"
#include "LocalPlayer.h"
#include <intrin.h> //VS2017 requires this for _ReturnAddress

ResetFn oReset;
EndSceneFn oEndScene;
int DRAW_CALL = 0;
int DRAW_CALL_MAX = 1;
extern HWND hWnd;
#ifdef SERVER_CRASHER
char *crashingserverstr = new char[17]{ 57, 40, 59, 41, 50, 51, 52, 61, 90, 41, 63, 40, 44, 63, 40, 91, 0 }; /*CRASHING SERVER!*/
#endif

void RunDrawingFrame()
{
	DRAW_CALL++;
	if (NoFlickerChk.Checked || DRAW_CALL == DRAW_CALL_MAX)
	{
		RunningFrame = TRUE;
		pSprite->Begin(D3DXSPRITE_ALPHABLEND | D3DXSPRITE_DO_NOT_ADDREF_TEXTURE);
		LocalPlayer.Get(&LocalPlayer);

		bool InGame = Interfaces::Engine->IsInGame() && LocalPlayer.Entity;

		if (InGame)
		{
			if (!DisableAllChk.Checked)
			{
				//pSprite->Begin(D3DXSPRITE_ALPHABLEND | D3DXSPRITE_DO_NOT_ADDREF_TEXTURE);
				RunVisualHacks_DXHook();
				if (GrenadeTrajectoryChk.Checked && LocalPlayer.Entity && LocalPlayer.Entity->GetAlive())
				{
					PaintGrenadeTrajectory();
				}
				//pSprite->End();
			}
		}
		
		if (MENU_IS_OPEN)
		{
			DrawMenu();
		}

#ifdef SERVER_CRASHER
		if (CrashingServer)
		{
			DecStr(crashingserverstr, 16);
			DrawStringUnencrypted(crashingserverstr, Vector2D(CenterOfScreen.x - 50, CenterOfScreen.y - 30), ColorRGBA(255, 0, 0, 255), pFont);
			EncStr(crashingserverstr, 16);
		}
#endif

		pSprite->End();
		RunningFrame = FALSE;
	}
}

#ifdef NewMenu
WNDPROC g_pOldWindowProc;
LRESULT __stdcall hkdWndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	if (Framework->Menu->WndProc(hWnd, uMsg, wParam, lParam))
		return true;
	return CallWindowProc(g_pOldWindowProc, hWnd, uMsg, wParam, lParam);
}
#endif

long __stdcall Hooked_EndScene(IDirect3DDevice9* pDevice)
{
	auto stat = START_PROFILING("EndScene");
	if (!p_Device)
	{
		p_Device = (IDirect3DDevice9Ex*)pDevice;
	}
	if (!p_Line)
	{
		if (!CreateRenderObjects())
		{
			END_PROFILING(stat);
			return oEndScene(pDevice);
		}
	}

	HRESULT hState = p_Device->TestCooperativeLevel();
	if (hState != D3D_OK)
	{
		END_PROFILING(stat);
		return oEndScene(pDevice);
	}

	DWORD returnAdd = reinterpret_cast<DWORD>(_ReturnAddress());
	static DWORD firstAdd = reinterpret_cast<DWORD>(_ReturnAddress());
	if (returnAdd != firstAdd)
	{
		END_PROFILING(stat);
		return oEndScene(pDevice);
	}

	IDirect3DPixelShader9* pixelshader;
	pDevice->GetPixelShader(&pixelshader);
	IDirect3DVertexShader9* vertexshader;
	pDevice->GetVertexShader(&vertexshader);
	DWORD cullmode;
	pDevice->GetRenderState(D3DRS_CULLMODE, &cullmode);
	DWORD lighting;
	pDevice->GetRenderState(D3DRS_LIGHTING, &lighting);
	DWORD zenable;
	pDevice->GetRenderState(D3DRS_ZENABLE, &zenable);
	DWORD alphablend;
	pDevice->GetRenderState(D3DRS_ALPHABLENDENABLE, &alphablend);
	DWORD blendop;
	pDevice->GetRenderState(D3DRS_BLENDOP, &blendop);
	DWORD alphatest;
	pDevice->GetRenderState(D3DRS_ALPHATESTENABLE, &alphatest);
	DWORD srcblend;
	pDevice->GetRenderState(D3DRS_SRCBLEND, &srcblend);
	DWORD destblend;
	pDevice->GetRenderState(D3DRS_DESTBLEND, &destblend);
	DWORD scissor;
	pDevice->GetRenderState(D3DRS_SCISSORTESTENABLE, &scissor);
	DWORD colorwrite;
	pDevice->GetRenderState(D3DRS_COLORWRITEENABLE, &colorwrite);

	DWORD colorop;
	pDevice->GetTextureStageState(0, D3DTSS_COLOROP, &colorop);
	DWORD colorarg;
	pDevice->GetTextureStageState(0, D3DTSS_COLORARG1, &colorarg);
	DWORD alphaop;
	pDevice->GetTextureStageState(0, D3DTSS_ALPHAOP, &alphaop);
	DWORD alphaarg1;
	pDevice->GetTextureStageState(0, D3DTSS_ALPHAARG1, &alphaarg1);
	DWORD alphaarg2;
	pDevice->GetTextureStageState(0, D3DTSS_ALPHAARG2, &alphaarg2);

	DWORD minfilter;
	pDevice->GetSamplerState(0, D3DSAMP_MINFILTER, &minfilter);
	DWORD magfilter;
	pDevice->GetSamplerState(0, D3DSAMP_MAGFILTER, &magfilter);

	pDevice->SetPixelShader(NULL);
	pDevice->SetVertexShader(NULL);
	pDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
	pDevice->SetRenderState(D3DRS_LIGHTING, false);
	pDevice->SetRenderState(D3DRS_ZENABLE, false);
	pDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, true);
	pDevice->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD);
	pDevice->SetRenderState(D3DRS_ALPHATESTENABLE, false);
	pDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	pDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
	pDevice->SetRenderState(D3DRS_SCISSORTESTENABLE, true);
	pDevice->SetRenderState(D3DRS_COLORWRITEENABLE, 0xFFFFFFFF);
	pDevice->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_SELECTARG1);
	pDevice->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_DIFFUSE);
	pDevice->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_MODULATE);
	pDevice->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
	pDevice->SetTextureStageState(0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE);
	pDevice->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_POINT);
	pDevice->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_POINT);


#if 0
	DWORD v_state = D3DZB_TRUE; //var used to obtain the CURRENT renderstate (usefull if you want to set a texture if target is visible and not visible)
								//your set texture/pixelshader if target not visible, optional
	pDevice->GetRenderState(D3DRS_ZENABLE, &v_state);
	pDevice->SetRenderState(D3DRS_ZENABLE, D3DZB_FALSE); //Disable depth buffering and draw the model
	DWORD old_color_write_enable;
	pDevice->GetRenderState(D3DRS_COLORWRITEENABLE, &old_color_write_enable);
	pDevice->SetRenderState(D3DRS_COLORWRITEENABLE, 0xFFFFFFFF);
	DWORD old_src_blend;
	pDevice->GetRenderState(D3DRS_SRCBLEND, &old_src_blend);
	pDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCCOLOR);
	DWORD old_dest_blend;
	pDevice->GetRenderState(D3DRS_DESTBLEND, &old_dest_blend);
	pDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCCOLOR);
#endif

	RunDrawingFrame();

	pDevice->SetPixelShader(pixelshader);
	pDevice->SetVertexShader(vertexshader);
	pDevice->SetRenderState(D3DRS_CULLMODE, cullmode);
	pDevice->SetRenderState(D3DRS_LIGHTING, lighting);
	pDevice->SetRenderState(D3DRS_ZENABLE, zenable);
	pDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, alphablend);
	pDevice->SetRenderState(D3DRS_BLENDOP, blendop);
	pDevice->SetRenderState(D3DRS_ALPHATESTENABLE, alphatest);
	pDevice->SetRenderState(D3DRS_SRCBLEND, srcblend);
	pDevice->SetRenderState(D3DRS_DESTBLEND, destblend);
	pDevice->SetRenderState(D3DRS_SCISSORTESTENABLE, scissor);
	pDevice->SetRenderState(D3DRS_COLORWRITEENABLE, colorwrite);
	pDevice->SetTextureStageState(0, D3DTSS_COLOROP, colorop);
	pDevice->SetTextureStageState(0, D3DTSS_COLORARG1, colorarg);
	pDevice->SetTextureStageState(0, D3DTSS_ALPHAOP, alphaop);
	pDevice->SetTextureStageState(0, D3DTSS_ALPHAARG1, alphaarg1);
	pDevice->SetTextureStageState(0, D3DTSS_ALPHAARG1, alphaarg2);
	pDevice->SetSamplerState(0, D3DSAMP_MINFILTER, minfilter);
	pDevice->SetSamplerState(0, D3DSAMP_MAGFILTER, magfilter);

#if 0
	pDevice->SetRenderState(D3DRS_ZENABLE, v_state);//Restore original depth buffer and set pixelshader or texture if target is really visible
	pDevice->SetRenderState(D3DRS_COLORWRITEENABLE, old_color_write_enable);
	pDevice->SetRenderState(D3DRS_SRCBLEND, old_src_blend);
	pDevice->SetRenderState(D3DRS_DESTBLEND, old_dest_blend);
#endif


#ifdef NewMenu
	static HWND g_hWindow = FindWindowA(("Valve001"), ("Counter-Strike: Global Offensive"));
	if (!g_pOldWindowProc && g_hWindow)
	{
		g_pOldWindowProc = reinterpret_cast<WNDPROC>(SetWindowLongPtr(g_hWindow, GWL_WNDPROC, reinterpret_cast<LONG_PTR>(&hkdWndProc)));
	}

	if (!Framework->Menu)
	{
		Framework->Menu = new MutinyFrame::UserInterface(g_hWindow, pDevice);

		Framework->Menu->Initialized = true;

	}
	if (!Framework->Menu->Initialized)
	{

		Framework->Menu = new MutinyFrame::UserInterface(g_hWindow, pDevice);

	}

	Framework->Menu->Draw();

#endif//
	END_PROFILING(stat);
	return oEndScene(pDevice);
}
long __stdcall Hooked_Reset(IDirect3DDevice9* pDevice, D3DPRESENT_PARAMETERS* pPresentationParameters)
{
	if (!p_Device)
	{
		p_Device = (IDirect3DDevice9Ex*)pDevice;
	}
	//reset objects
#ifdef NewMenu
	ImGui_ImplDX9_InvalidateDeviceObjects();
#endif
	RenderObjectsOnDeviceLost();

	//reset device
	long result = oReset(pDevice, pPresentationParameters);
	if (result == D3D_OK)
	{
		RenderObjectsOnDeviceReset();
		GetCSGORect();
		//Recreate them
#ifdef NewMenu
		ImGui_ImplDX9_CreateDeviceObjects();
#endif
	}
	InitialRenderThread = -1;
	ResetRenderThreadTime = QPCTime();

	return result;
}
