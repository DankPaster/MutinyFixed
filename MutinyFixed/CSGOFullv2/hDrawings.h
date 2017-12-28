#ifndef HDRAWFUNC_H
#define HDRAWFUNC_H

#include "hDirectX.h"
#include "Overlay.h"

#pragma warning( disable : 4244 ) //remove the incoming warns
#pragma warning( disable : 4996 ) //remove the incoming warns

#include <d3dx9.h>
#include <d3d9.h>
#pragma comment(lib, "d3d9.lib")
#pragma comment(lib, "d3dx9.lib")
#include "Misc.h"
#include <string>
//#include "D3DFont.h"

void GradientFunc(Vector2D coords, int width, int height, int r, int g, int b, int a, int startfadeheight);
//void DrawCenterLine(Vector2D coords, int r, int g, int b, int a);
void DrawLine(Vector2D coords, Vector2D coords2, int r, int g, int b, int a);
void FillRGB(Vector2D coords, float width, float height, int r, int g, int b, int a);
void DrawBox(Vector2D coords, float width, float height, float px, int r, int g, int b, int a); 
void DrawGUIBox(Vector2D coords, float width, float height, int r, int g, int b, int a, int FillR, int FillG, int FillB, int FillA);
void DrawCircle(Vector2D &Center, float Radius, ColorRGBA color, int Sides);
//void DrawHealthBar(Vector2D coords, float width, float height, int r, int g);
//void DrawHealthBarBack(Vector2D coords, float width, float height); 
struct skeleton
{
	Vector2D head;
	Vector2D neck;
	Vector2D chest;
	Vector2D leftShoulder;
	Vector2D rightShoulder;
	Vector2D leftElbow;
	Vector2D rightElbow;
	Vector2D leftHand;
	Vector2D rightHand;
	Vector2D pelvis;
	Vector2D leftKnee;
	Vector2D rightKnee;
	Vector2D leftFoot;
	Vector2D rightFoot;

};

void SkeletonEsp(skeleton SkeletonPoints, int r, int g, int b, int a);

void DrawHitbox(ColorRGBA color, Vector* hitBoxVectors);
void DrawString(std::string String, Vector2D coords, ColorRGBA color, ID3DXFont* ifont);
void DrawStringUnencrypted(char* String, Vector2D coords, ColorRGBA color, ID3DXFont* ifont);
void DrawStringUnencryptedW(wchar_t* String, Vector2D coords, ColorRGBA color, ID3DXFont* ifont);
//void DrawStringNew(std::string String, Vector2D coords, ColorRGBA color, CD3DFont* ifont);
//void DrawStringUnencryptedNew(char* String, Vector2D coords, ColorRGBA color, CD3DFont* ifont);
//void DrawStringUnencryptedWNew(wchar_t* String, Vector2D coords, ColorRGBA color, CD3DFont* ifont);
//int DrawShadowString(char* String, Vector2D coords, int r, int g, int b, ID3DXFont* ifont);

#endif