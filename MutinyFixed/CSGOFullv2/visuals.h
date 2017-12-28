#pragma once

#include <d3d9.h>
#include <d3dx9.h>
#pragma comment(lib, "d3d9.lib")
#pragma comment(lib, "d3dx9.lib")
#include "VTHook.h"

class Renderer
{
public:
	void Initialize(HWND g_hWindow);

	void EndScene();
	void BeginScene();
	void DrawScene();
	void DrawESP();
	void DrawEntityBox(ImColor Colour, CBaseEntity* TempTarget, CustomPlayer *pCPlayer);
	void SkeletonEsp(skeleton SkeletonPoints, ImColor color);
	void SkeltonEsp(CustomPlayer* player);
	void EntityLoop();
	bool showRenderESP(CBaseEntity* TempTarget, CBaseEntity* LocalEntity);
	bool isVisible(CBaseEntity* TempTarget, CBaseEntity* LocalEntity, Vector LocalEyePos, QAngle LocalAngles);
	ImColor getColour(std::string setting, bool teamMate, bool visible, int entityHealth);
	static Renderer* GetInstance();

	float RenderText(ImFont* pFont, const std::string& text, const ImVec2& position, float size, ImColor color, bool center = false);
	void  DrawLine(const ImVec2& from, const ImVec2& to, ImColor color, float thickness = 1.0f);
	void  DrawCircle(const ImVec2& position, float radius, ImColor color, float thickness = 1.0f);
	void  DrawCircleFilled(const ImVec2& position, float radius, ImColor color);
	void  r2DrawBox(float x, float y, float x1, float y1, ImColor Color, float thickNess, float pad);

	void DrawLinePadded(const Vector& from, const Vector& to, ImColor color, float thickness, float pad);

	void rDrawFilledBox(float x, float y, float x1, float y1, ImColor Color);
	void rDrawBox(float x, float y, float x1, float y1, ImColor Color, float thickness);
	void rDrawLine(float x, float y, float x1, float y1, ImColor Color);
	void rRenderText(std::string Text, float x, float y, float size, ImColor Color, bool center);
	void rDrawCircle(float x, float y, float radius, ImColor Color, float thickness);

	ImFont* m_pFont;

	static Renderer* m_pInstance;

private:
	void Rainbow();
	int counter = 0;
	int state = 0;
	int r = 255;
	int g = 0;
	int b = 0;
	int healthb = 0;
};

extern Renderer renderra;