#pragma once

#include <windows.h>
#include <string>
#include <vector>
#include <cassert>

#include "Public/2D/Vector2D.h"

//------------------------------- Define some colors -----------------------

const int numColors = 15;

const COLORREF colors[numColors] =
{
	RGB(255, 0, 0),
	RGB(0, 0, 255),
	RGB(0, 255, 0),
	RGB(0, 0, 0),
	RGB(255, 200, 200),
	RGB(200, 200, 200),
	RGB(255, 255, 0),
	RGB(255, 170, 0),
	RGB(255, 0, 170),
	RGB(133, 90, 0),
	RGB(255, 255, 255),
	// Dark green
	RGB(0, 100, 0),
	// Light blue
	RGB(0, 255, 255),
	// Light grey
	RGB(200, 200, 200),
	// Light pink
	RGB(255, 230, 230)
};

#define gdi Cgdi::Instance()

class Cgdi
{
public:

	int NumPenColors() const { return numColors; }

	// Enumerate some colors
	enum
	{
		red, 
		blue, 
		green, 
		black, 
		pink, 
		grey, 
		yellow, 
		orange, 
		purple, 
		brown, 
		white, 
		dark_green, 
		light_blue, 
		light_grey, 
		light_pink, 
		hollow
	};

private:

	HPEN m_oldPen;

	// All the pens
	HPEN m_blackPen;
	HPEN m_whitePen;
	HPEN m_redPen;
	HPEN m_greenPen;
	HPEN m_bluePen;
	HPEN m_greyPen;
	HPEN m_pinkPen;
	HPEN m_orangePen;
	HPEN m_yellowPen;
	HPEN m_purplePen;
	HPEN m_brownPen;

	HPEN m_darkGreenPen;
	HPEN m_lightBluePen;
	HPEN m_lightGreyPen;
	HPEN m_lightPinkPen;

	HPEN m_thickBlackPen;
	HPEN m_thickWhitePen;
	HPEN m_thickRedPen;
	HPEN m_thickGreenPen;
	HPEN m_thickBluePen;

	HBRUSH m_oldBrush;

	// All the brushes
	HBRUSH m_redBrush;
	HBRUSH m_greenBrush;
	HBRUSH m_blueBrush;
	HBRUSH m_greyBrush;
	HBRUSH m_brownBrush;
	HBRUSH m_yellowBrush;
	HBRUSH m_orangeBrush;
	 
	HBRUSH m_lightBlueBrush;
	HBRUSH m_darkGreenBrush;

	HDC m_hdc;

	// Constructor is private
	Cgdi();

public:

	~Cgdi();

	// Copy ctor and assignment should be private
	Cgdi(const Cgdi&) = delete;
	Cgdi& operator=(const Cgdi&) = delete;

	static Cgdi* Instance();

	// Always make sure to call this before drawing
	void StartDrawing(HDC hdc);

	// Always make sure to call this after drawing
	void StopDrawing(HDC hdc);

	//------------------------------- Text Drawing -----------------------

	void TextAtPos(int x, int y, const std::string& s);
	void TextAtPos(double x, double y, const std::string& s);
	void TextAtPos(Vector2D pos, const std::string& s);
	
	void TransparentText();
	void OpaqueText();

	void TextColor(int color);
	void TextColor(int r, int g, int b);

	//------------------------------- Pixels Drawing -----------------------

	void DrawDot(Vector2D pos, COLORREF color);
	void DrawDot(int x, int y, COLORREF color);

	//------------------------------- Line Drawing -----------------------

	void Line(Vector2D from, Vector2D to);
	void Line(int a, int b, int x, int y);
	void Line(double a, double b, double x, double y);

	void PolyLine(const std::vector<Vector2D>& points);
	void LineWithArrow(Vector2D from, Vector2D to, double size);
	void Cross(Vector2D pos, int diameter);

	//------------------------------- Geometry Drawing -----------------------

	void Rect(int left, int top, int right, int bot);
	void Rect(double left, double top, double right, double bot);

	void ClosedShape(const std::vector<Vector2D>& points);
	void Circle(Vector2D pos, double radius);
	void Circle(double x, double y, double radius);
	void Circle(int x, int y, double radius);

	//------------------------------- Pen Color -----------------------
	// Returns the proper color pen

	void SetPenColor(int color);

	void BlackPen() { if (m_hdc) { SelectObject(m_hdc, m_blackPen); } }
	void WhitePen() { if (m_hdc) { SelectObject(m_hdc, m_whitePen); } }
	void RedPen() { if (m_hdc) { SelectObject(m_hdc, m_redPen); } }
	void GreenPen() { if (m_hdc) { SelectObject(m_hdc, m_greenPen); } }
	void BluePen() { if (m_hdc) { SelectObject(m_hdc, m_bluePen); } }
	void GreyPen() { if (m_hdc) { SelectObject(m_hdc, m_greyPen); } }
	void PinkPen() { if (m_hdc) { SelectObject(m_hdc, m_pinkPen); } }
	void YellowPen() { if (m_hdc) { SelectObject(m_hdc, m_yellowPen); } }
	void OrangePen() { if (m_hdc) { SelectObject(m_hdc, m_orangePen); } }
	void PurplePen() { if (m_hdc) { SelectObject(m_hdc, m_purplePen); } }
	void BrownPen() { if (m_hdc) { SelectObject(m_hdc, m_brownPen); } }

	void DarkGreenPen() { if (m_hdc) { SelectObject(m_hdc, m_darkGreenPen); } }
	void LightBluePen() { if (m_hdc) { SelectObject(m_hdc, m_lightBluePen); } }
	void LightGreyPen() { if (m_hdc) { SelectObject(m_hdc, m_lightGreyPen); } }
	void LightPinkPen() { if (m_hdc) { SelectObject(m_hdc, m_lightPinkPen); } }

	void ThickBlackPen() { if (m_hdc) { SelectObject(m_hdc, m_thickBlackPen); } }
	void ThickWhitePen() { if (m_hdc) { SelectObject(m_hdc, m_thickWhitePen); } }
	void ThickRedPen() { if (m_hdc) { SelectObject(m_hdc, m_thickRedPen); } }
	void ThickGreenPen() { if (m_hdc) { SelectObject(m_hdc, m_thickGreenPen); } }
	void ThickBluePen() { if (m_hdc) { SelectObject(m_hdc, m_thickBluePen); } }


	//------------------------------- Brush Color -----------------------
	// Returns the proper brush

	void BlackBrush() { if (m_hdc)SelectObject(m_hdc, GetStockObject(BLACK_BRUSH)); }
	void WhiteBrush() { if (m_hdc)SelectObject(m_hdc, GetStockObject(WHITE_BRUSH)); }
	void HollowBrush() { if (m_hdc)SelectObject(m_hdc, GetStockObject(HOLLOW_BRUSH)); }
	void GreenBrush() { if (m_hdc)SelectObject(m_hdc, m_greenBrush); }
	void RedBrush() { if (m_hdc)SelectObject(m_hdc, m_redBrush); }
	void BlueBrush() { if (m_hdc)SelectObject(m_hdc, m_blueBrush); }
	void GreyBrush() { if (m_hdc)SelectObject(m_hdc, m_greyBrush); }
	void BrownBrush() { if (m_hdc)SelectObject(m_hdc, m_brownBrush); }
	void YellowBrush() { if (m_hdc)SelectObject(m_hdc, m_yellowBrush); }
	void LightBlueBrush() { if (m_hdc)SelectObject(m_hdc, m_lightBlueBrush); }
	void DarkGreenBrush() { if (m_hdc)SelectObject(m_hdc, m_darkGreenBrush); }
	void OrangeBrush() { if (m_hdc)SelectObject(m_hdc, m_orangeBrush); }
};
