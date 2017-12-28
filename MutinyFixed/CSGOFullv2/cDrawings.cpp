#include "hDrawings.h"
#include "GameMemory.h"
#include <string>

void DrawHitbox(ColorRGBA color, Vector* hitBoxVectors)
{
	Vector screen1, screen2;

	//top of hit box
	for (int i = 0; i < 3; i++)
	{
		if (WorldToScreenCapped(hitBoxVectors[i], screen1))
		if (WorldToScreenCapped(hitBoxVectors[i + 1], screen2))
		DrawLine(Vector2D(screen1.x, screen1.y), Vector2D(screen2.x, screen2.y), color.r, color.g, color.b, color.a);
	}
	if (WorldToScreenCapped(hitBoxVectors[0], screen1))
	if (WorldToScreenCapped(hitBoxVectors[3], screen2))
	DrawLine(Vector2D(screen1.x, screen1.y), Vector2D(screen2.x, screen2.y), color.r, color.g, color.b, color.a);

	//bottom of hit box
	for (int i = 4; i < 7; i++)
	{
		if (WorldToScreenCapped(hitBoxVectors[i], screen1))
		if (WorldToScreenCapped(hitBoxVectors[i + 1], screen2))
		DrawLine(Vector2D(screen1.x, screen1.y), Vector2D(screen2.x, screen2.y), color.r, color.g, color.b, color.a);
	}
	if (WorldToScreenCapped(hitBoxVectors[4], screen1))
	if (WorldToScreenCapped(hitBoxVectors[7], screen2))
	DrawLine(Vector2D(screen1.x, screen1.y), Vector2D(screen2.x, screen2.y), color.r, color.g, color.b, color.a);

	//sides of hit box
	if (WorldToScreenCapped(hitBoxVectors[0], screen1))
	if (WorldToScreenCapped(hitBoxVectors[6], screen2))
	DrawLine(Vector2D(screen1.x, screen1.y), Vector2D(screen2.x, screen2.y), color.r, color.g, color.b, color.a);

	if (WorldToScreenCapped(hitBoxVectors[1], screen1))
	if (WorldToScreenCapped(hitBoxVectors[5], screen2))
	DrawLine(Vector2D(screen1.x, screen1.y), Vector2D(screen2.x, screen2.y), color.r, color.g, color.b, color.a);

	if (WorldToScreenCapped(hitBoxVectors[2], screen1))
	if (WorldToScreenCapped(hitBoxVectors[4], screen2))
	DrawLine(Vector2D(screen1.x, screen1.y), Vector2D(screen2.x, screen2.y), color.r, color.g, color.b, color.a);

	if (WorldToScreenCapped(hitBoxVectors[3], screen1))
	if (WorldToScreenCapped(hitBoxVectors[7], screen2))
	DrawLine(Vector2D(screen1.x, screen1.y), Vector2D(screen2.x, screen2.y), color.r, color.g, color.b, color.a);

}

void DrawString(std::string String, Vector2D coords, ColorRGBA color, ID3DXFont* ifont)
{
	if (!RunningFrame)
		return;
	//int len = String.length();
	RECT ShadowPos;
	ShadowPos.left = coords.x + 1;
	ShadowPos.top = coords.y + 1;
	ShadowPos.bottom = rc.bottom;
	ShadowPos.right = rc.right;
	RECT FontPos;
	FontPos.left = coords.x;
	FontPos.top = coords.y;
	FontPos.bottom = rc.bottom;
	FontPos.right = rc.right;
	ifont->DrawTextA(pSprite, String.c_str(), -1, &ShadowPos, DT_NOCLIP | DT_TOP | DT_LEFT, D3DCOLOR_ARGB(color.a, color.r / 6, color.g / 6, color.b / 6));
	ifont->DrawTextA(pSprite, String.c_str(), -1, &FontPos, DT_NOCLIP | DT_TOP | DT_LEFT, D3DCOLOR_ARGB(color.a, color.r, color.g, color.b));
}

void DrawStringUnencrypted(char* String, Vector2D coords, ColorRGBA color, ID3DXFont* ifont)
{
	if (!RunningFrame)
		return;
	//int len = strlen(String);
	RECT ShadowPos;
	ShadowPos.left = coords.x + 1;
	ShadowPos.top = coords.y + 1;
	ShadowPos.bottom = rc.bottom;
	ShadowPos.right = rc.right;
	RECT FontPos;
	FontPos.left = coords.x;
	FontPos.top = coords.y;
	FontPos.bottom = rc.bottom;
	FontPos.right = rc.right;
	ifont->DrawTextA(pSprite, String, -1, &ShadowPos, DT_NOCLIP | DT_TOP | DT_LEFT, D3DCOLOR_ARGB(color.a, color.r / 6, color.g / 6, color.b / 6));
	ifont->DrawTextA(pSprite, String, -1, &FontPos, DT_NOCLIP | DT_TOP | DT_LEFT, D3DCOLOR_ARGB(color.a, color.r, color.g, color.b));
}

void DrawStringUnencryptedW(wchar_t* String, Vector2D coords, ColorRGBA color, ID3DXFont* ifont)
{
	if (!RunningFrame)
		return;
	//int len = wcslen(String);
	RECT ShadowPos;
	ShadowPos.left = coords.x + 1;
	ShadowPos.top = coords.y + 1;
	RECT FontPos;
	FontPos.left = coords.x;
	FontPos.top = coords.y;
	ifont->DrawTextW(pSprite, String, -1, &ShadowPos, DT_NOCLIP | DT_TOP | DT_LEFT, D3DCOLOR_ARGB(color.a, color.r / 6, color.g / 6, color.b / 6));
	ifont->DrawTextW(pSprite, String, -1, &FontPos, DT_NOCLIP | DT_TOP | DT_LEFT, D3DCOLOR_ARGB(color.a, color.r, color.g, color.b));
}

#if 0
void DrawStringNew(std::string String, Vector2D coords, ColorRGBA color, CD3DFont* ifont)
{
	if (!RunningFrame || !bNewFontCanDraw || !bNewFontSmallCanDraw)
		return;
	ifont->DrawTextA(coords.x, coords.y, D3DCOLOR_ARGB(color.a, color.r, color.g, color.b), String.c_str(), D3DFONT_SHADOW, 0UL);
}

void DrawStringUnencryptedNew(char* String, Vector2D coords, ColorRGBA color, CD3DFont* ifont)
{
	if (!RunningFrame || !bNewFontCanDraw || !bNewFontSmallCanDraw)
		return;
	ifont->DrawTextA(coords.x, coords.y, D3DCOLOR_ARGB(color.a, color.r, color.g, color.b), String, D3DFONT_SHADOW, 0UL);
}

void DrawStringUnencryptedWNew(wchar_t* String, Vector2D coords, ColorRGBA color, CD3DFont* ifont)
{
	if (!RunningFrame || !bNewFontCanDraw || !bNewFontSmallCanDraw)
		return;
	ifont->DrawTextW(coords.x, coords.y, D3DCOLOR_ARGB(color.a, color.r, color.g, color.b), String, D3DFONT_SHADOW, 0UL);
}
#endif

/*
int DrawShadowString(char *String, Vector2D coords, int r, int g, int b, ID3DXFont* ifont)
{
	RECT Font;
	Font.left = coords.x;
	Font.top = coords.y;
	RECT Fonts;
	Fonts.left = coords.x + 1;
	Fonts.top = coords.y;
	RECT Fonts1;
	Fonts1.left = coords.x - 1;
	Fonts1.top = coords.y;
	RECT Fonts2;
	Fonts2.left = coords.x;
	Fonts2.top = coords.y + 1;
	RECT Fonts3;
	Fonts3.left = coords.x;
	Fonts3.top = coords.y - 1;
	ifont->DrawTextA(0, String, strlen(String), &Fonts3, DT_NOCLIP, D3DCOLOR_ARGB(255, 1, 1, 1));
	ifont->DrawTextA(0, String, strlen(String), &Fonts2, DT_NOCLIP, D3DCOLOR_ARGB(255, 1, 1, 1));
	ifont->DrawTextA(0, String, strlen(String), &Fonts1, DT_NOCLIP, D3DCOLOR_ARGB(255, 1, 1, 1));
	ifont->DrawTextA(0, String, strlen(String), &Fonts, DT_NOCLIP, D3DCOLOR_ARGB(255, 1, 1, 1));
	ifont->DrawTextA(0, String, strlen(String), &Font, DT_NOCLIP, D3DCOLOR_ARGB(255, r, g, b));
	return 0;
}
*/
//#define TRUIX_GRADIENT

#ifdef TRUIX_GRADIENT
typedef struct {
	double r;       // a fraction between 0 and 1
	double g;       // a fraction between 0 and 1
	double b;       // a fraction between 0 and 1
} rgb;

typedef struct {
	double h;       // angle in degrees
	double s;       // a fraction between 0 and 1
	double v;       // a fraction between 0 and 1
} hsv;

static hsv   rgb2hsv(rgb in);
static rgb   hsv2rgb(hsv in);

hsv rgb2hsv(rgb in)
{
	hsv         out;
	double      min, max, delta;

	min = in.r < in.g ? in.r : in.g;
	min = min  < in.b ? min : in.b;

	max = in.r > in.g ? in.r : in.g;
	max = max  > in.b ? max : in.b;

	out.v = max;                                // v
	delta = max - min;
	if (delta < 0.00001)
	{
		out.s = 0;
		out.h = 0; // undefined, maybe nan?
		return out;
	}
	if (max > 0.0) { // NOTE: if Max is == 0, this divide would cause a crash
		out.s = (delta / max);                  // s
	}
	else {
		// if max is 0, then r = g = b = 0              
		// s = 0, v is undefined
		out.s = 0.0;
		out.h = NAN;                            // its now undefined
		return out;
	}
	if (in.r >= max)                           // > is bogus, just keeps compilor happy
		out.h = (in.g - in.b) / delta;        // between yellow & magenta
	else
		if (in.g >= max)
			out.h = 2.0 + (in.b - in.r) / delta;  // between cyan & yellow
		else
			out.h = 4.0 + (in.r - in.g) / delta;  // between magenta & cyan

	out.h *= 60.0;                              // degrees

	if (out.h < 0.0)
		out.h += 360.0;

	return out;
}


rgb hsv2rgb(hsv in)
{
	double      hh, p, q, t, ff;
	long        i;
	rgb         out;

	if (in.s <= 0.0) {       // < is bogus, just shuts up warnings
		out.r = in.v;
		out.g = in.v;
		out.b = in.v;
		return out;
	}
	hh = in.h;
	if (hh >= 360.0) hh = 0.0;
	hh /= 60.0;
	i = (long)hh;
	ff = hh - i;
	p = in.v * (1.0 - in.s);
	q = in.v * (1.0 - (in.s * ff));
	t = in.v * (1.0 - (in.s * (1.0 - ff)));

	switch (i) {
	case 0:
		out.r = in.v;
		out.g = t;
		out.b = p;
		break;
	case 1:
		out.r = q;
		out.g = in.v;
		out.b = p;
		break;
	case 2:
		out.r = p;
		out.g = in.v;
		out.b = t;
		break;

	case 3:
		out.r = p;
		out.g = q;
		out.b = in.v;
		break;
	case 4:
		out.r = t;
		out.g = p;
		out.b = in.v;
		break;
	case 5:
	default:
		out.r = in.v;
		out.g = p;
		out.b = q;
		break;
	}
	return out;
}

void GradientFunc(Vector2D coords, int width, int height, int r, int g, int b, int a, int startfadeheight)
{
	rgb Color;
	Color.r = r;
	Color.g = g;
	Color.b = b;
	hsv Shifted = rgb2hsv(Color);
	//Shifted.v = 255;
	for (int i = 1; i < height; i++)
	{
		if (i >= startfadeheight)
		{
			float heightsub = (float)(height - startfadeheight);
			float newi = (float)i - heightsub;
			float Val = heightsub / 100;
			float inc = (Val / 2);
			Shifted.v -= inc;
			Color = hsv2rgb(Shifted);
		}
		FillRGB(Vector2D(coords.x, coords.y + i), width, 1, Color.r, Color.g, Color.b, a);
	}
}

#else
void GradientFunc(Vector2D coords, int width, int height, int r, int g, int b, int a, int startfadeheight)
{
	int iColorr = 0;
	int iColorb = 0;
	int iColorg = 0;
	for(int i = 1; i < height; i++)
	{
		if (i >= startfadeheight)
		{
			float heightsub = (float)(height - startfadeheight);
			float newi = (float)i - heightsub;
			iColorr = (int)(newi  / height*r);
			iColorg = (int)(newi / height*g);
			iColorb = (int)(newi / height*b);
		}
		DrawLine(Vector2D(coords.x, coords.y + i), Vector2D(coords.x + width, coords.y + i), r - iColorr, g - iColorg, b - iColorb, a);
		//FillRGB(Vector2D(coords.x , coords.y + i), width, 1.0f , r - iColorr, g - iColorg, b - iColorb, a);
	}
}
#endif

void DrawLine(Vector2D coords, Vector2D coords2, int r, int g, int b, int a)
{
	if (!RunningFrame)
		return;
	D3DXVECTOR2 dLine[2];

	dLine[1].x = coords2.x;
	dLine[1].y = coords2.y;

	dLine[0].x = coords.x;
	dLine[0].y = coords.y;

	p_Line->SetWidth(1);

	p_Line->Draw(dLine, 2, D3DCOLOR_ARGB(a, r, g, b));

} 

void FillRGB(Vector2D coords, float width, float height, int r, int g, int b, int a) 
{ 
	if (!RunningFrame)
		return;

    D3DXVECTOR2 vLine[2]; 

	p_Line->SetWidth(width);
	
	vLine[0].y = coords.y;
	vLine[0].x = coords.x + width / 2;
 

	vLine[1].y = coords.y + height;
	vLine[1].x = coords.x + width / 2;

	p_Line->Draw(vLine, 2, D3DCOLOR_RGBA(r, g, b, a));
} 
void SkeletonEsp(skeleton SkeletonPoints, int r, int g, int b, int a)
{
	DrawLine(SkeletonPoints.head, SkeletonPoints.neck, r, g, b, a);
	DrawLine(SkeletonPoints.neck, SkeletonPoints.chest, r, g, b, a);
	DrawLine(SkeletonPoints.chest, SkeletonPoints.leftShoulder, r, g, b, a);
	DrawLine(SkeletonPoints.chest, SkeletonPoints.rightShoulder, r, g, b, a);
	DrawLine(SkeletonPoints.leftShoulder, SkeletonPoints.leftElbow, r, g, b, a);
	DrawLine(SkeletonPoints.rightShoulder, SkeletonPoints.rightElbow, r, g, b, a);
	DrawLine(SkeletonPoints.leftShoulder, SkeletonPoints.leftElbow, r, g, b, a);
	DrawLine(SkeletonPoints.rightShoulder, SkeletonPoints.rightElbow, r, g, b, a);
	DrawLine(SkeletonPoints.leftElbow, SkeletonPoints.leftHand, r, g, b, a);
	DrawLine(SkeletonPoints.rightElbow, SkeletonPoints.rightHand, r, g, b, a);
	DrawLine(SkeletonPoints.chest, SkeletonPoints.pelvis, r, g, b, a);
	DrawLine(SkeletonPoints.pelvis, SkeletonPoints.leftKnee, r, g, b, a);
	DrawLine(SkeletonPoints.pelvis, SkeletonPoints.rightKnee, r, g, b, a);
	DrawLine(SkeletonPoints.leftKnee, SkeletonPoints.leftFoot, r, g, b, a);
	DrawLine(SkeletonPoints.rightKnee, SkeletonPoints.rightFoot, r, g, b, a);
}
void DrawBox(Vector2D coords, float width, float height, float px, int r, int g, int b, int a) 
{ 
	if (!RunningFrame)
		return;
	D3DXVECTOR2 points[5];  
	points[2] = D3DXVECTOR2(coords.x + width, coords.y + height);
	points[3] = D3DXVECTOR2(coords.x, coords.y + height);
	points[0] = D3DXVECTOR2(coords.x,		coords.y);  
	points[1] = D3DXVECTOR2(coords.x+width,	coords.y);  
	points[4] = D3DXVECTOR2(coords.x,		coords.y);  
	p_Line->SetWidth(px);
	p_Line->Draw(points, 5, D3DCOLOR_RGBA(r, g, b, a));
} 

void DrawGUIBox(Vector2D coords, float width, float height, int r, int g, int b, int a, int FillR, int FillG, int FillB, int FillA)
{  
	DrawBox(coords, width, height, 1, r, g, b, a);
	//FillRGB(Vector2D(coords.x + 1, coords.y + 1), width - 1, height - 1, FillR, FillG, FillB, FillA);
	FillRGB(coords, width, height, FillR, FillG, FillB, FillA);
}

void DrawCircle(Vector2D &Center, float Radius, ColorRGBA color, int Sides)
{
	float Angle = DEG2RAD(360.0f / (float)Sides);

	float Cos = cos(Angle);
	float Sin = sin(Angle);

	Vector2D vec(Radius, 0);

	for (unsigned short i = 0; i < Sides; ++i)
	{
		Vector2D rot(Cos*vec.x - Sin*vec.y, Sin*vec.x + Cos*vec.y);
		rot.x = rot.x + Center.x;
		rot.y = rot.y + Center.y;
		vec.x = vec.x + Center.x;
		vec.y = vec.y + Center.y;
		DrawLine(vec, rot, color.r, color.g, color.b, color.a);
		vec.x = rot.x - Center.x;
		vec.y = rot.y - Center.y;
	}
}

/*
void DrawHealthBar(Vector2D coords, float w, float h, int r, int g) 
{     
    FillRGB(coords.x,coords.y,w,h,r,g,0,255);      
} 
void DrawHealthBarBack(Vector2D coords, float w, float h) 
{     
    FillRGB(coords.x,coords.y,w,h,0,0,0,255);      
} 
void DrawCenterLine(Vector2D coords, int width, int r, int g, int b)
{
	D3DXVECTOR2 dPoints[2];
	dPoints[0] = D3DXVECTOR2(coords.x, coords.y);
	dPoints[1] = D3DXVECTOR2(Width / 2, Height);
	p_Line->SetWidth(width);
	p_Line->Draw(dPoints, 2, D3DCOLOR_RGBA(r, g, b, 255));
} 
*/
void DrawBone(int bone1, int bone2, DWORD BoneMatrix)
{
	Vector Bone1;
	Vector W2S_Bone1;

	GetBonePos(BoneMatrix, bone1, Bone1);

	Vector Bone2;
	Vector W2S_Bone2;
	GetBonePos(BoneMatrix, bone2, Bone2);

	if (WorldToScreen(Bone2, W2S_Bone2) && WorldToScreen(Bone1, W2S_Bone1))
		DrawLine(Vector2D(W2S_Bone1[0], W2S_Bone1[1]), Vector2D(W2S_Bone2[0], W2S_Bone2[1]), 255, 255, 255, 255);
}
