#include "Common/Public/Misc/Cgdi.h"

//------------------------------- Instance -----------------------
// This class is a singleton
//----------------------------------------------------------------

Cgdi* Cgdi::Instance()
{
	static Cgdi instance;
	return &instance;
}

//------------------------------- Default Constructor -----------------------
//---------------------------------------------------------------------------

Cgdi::Cgdi()
	:m_oldBrush(nullptr),
	m_oldPen(nullptr),
	m_hdc(nullptr)
{
	m_blackPen = CreatePen(PS_SOLID, 1, colors[black]);
	m_whitePen = CreatePen(PS_SOLID, 1, colors[white]);
	m_redPen = CreatePen(PS_SOLID, 1, colors[red]);
	m_greenPen = CreatePen(PS_SOLID, 1, colors[green]);
	m_bluePen = CreatePen(PS_SOLID, 1, colors[blue]);
	m_greyPen = CreatePen(PS_SOLID, 1, colors[grey]);
	m_pinkPen = CreatePen(PS_SOLID, 1, colors[pink]);
	m_yellowPen = CreatePen(PS_SOLID, 1, colors[yellow]);
	m_orangePen = CreatePen(PS_SOLID, 1, colors[orange]);
	m_purplePen = CreatePen(PS_SOLID, 1, colors[purple]);
	m_brownPen = CreatePen(PS_SOLID, 1, colors[brown]);

	m_darkGreenPen = CreatePen(PS_SOLID, 1, colors[dark_green]);

	m_lightBluePen = CreatePen(PS_SOLID, 1, colors[light_blue]);
	m_lightGreyPen = CreatePen(PS_SOLID, 1, colors[light_grey]);
	m_lightPinkPen = CreatePen(PS_SOLID, 1, colors[light_pink]);

	m_thickBlackPen = CreatePen(PS_SOLID, 2, colors[black]);
	m_thickWhitePen = CreatePen(PS_SOLID, 2, colors[white]);
	m_thickRedPen = CreatePen(PS_SOLID, 2, colors[red]);
	m_thickGreenPen = CreatePen(PS_SOLID, 2, colors[green]);
	m_thickBluePen = CreatePen(PS_SOLID, 2, colors[blue]);

	m_greenBrush = CreateSolidBrush(colors[green]);
	m_redBrush = CreateSolidBrush(colors[red]);
	m_blueBrush = CreateSolidBrush(colors[blue]);
	m_greyBrush = CreateSolidBrush(colors[grey]);
	m_brownBrush = CreateSolidBrush(colors[brown]);
	m_yellowBrush = CreateSolidBrush(colors[yellow]);
	m_lightBlueBrush = CreateSolidBrush(RGB(0, 255, 255));
	m_darkGreenBrush = CreateSolidBrush(colors[dark_green]);
	m_orangeBrush = CreateSolidBrush(colors[orange]);
}

//------------------------------- Default Destructor -----------------------
//--------------------------------------------------------------------------

Cgdi::~Cgdi()
{
	DeleteObject(m_blackPen);
	DeleteObject(m_whitePen);
	DeleteObject(m_redPen);
	DeleteObject(m_greenPen);
	DeleteObject(m_bluePen);
	DeleteObject(m_greyPen);
	DeleteObject(m_pinkPen);
	DeleteObject(m_orangePen);
	DeleteObject(m_yellowPen);
	DeleteObject(m_purplePen);
	DeleteObject(m_brownPen);
	DeleteObject(m_oldPen);

	DeleteObject(m_darkGreenPen);

	DeleteObject(m_lightBluePen);
	DeleteObject(m_lightGreyPen);
	DeleteObject(m_lightPinkPen);

	DeleteObject(m_thickBlackPen);
	DeleteObject(m_thickWhitePen);
	DeleteObject(m_thickRedPen);
	DeleteObject(m_thickGreenPen);
	DeleteObject(m_thickBluePen);

	DeleteObject(m_greenBrush);
	DeleteObject(m_redBrush);
	DeleteObject(m_blueBrush);
	DeleteObject(m_oldBrush);
	DeleteObject(m_greyBrush);
	DeleteObject(m_brownBrush);
	DeleteObject(m_lightBlueBrush);
	DeleteObject(m_yellowBrush);
	DeleteObject(m_darkGreenBrush);
	DeleteObject(m_orangeBrush);

	delete m_oldBrush;
	delete m_oldPen;
	delete m_hdc;
}

void Cgdi::StartDrawing(HDC hdc)
{
	assert(m_hdc != nullptr);

	m_hdc = hdc;

	// Get the current pen
	m_oldPen = static_cast<HPEN>(SelectObject(hdc, m_blackPen));
	// Select it back in
	SelectObject(hdc, m_oldPen);

	m_oldBrush = static_cast<HBRUSH>(SelectObject(hdc, GetStockObject(BLACK_BRUSH)));
	SelectObject(hdc, m_oldBrush);
}

void Cgdi::StopDrawing(HDC hdc)
{
	assert(hdc != nullptr);

	SelectObject(hdc, m_oldPen);
	SelectObject(hdc, m_oldBrush);

	m_hdc = nullptr;
}

//------------------------------- Text Drawing -----------------------
//--------------------------------------------------------------------

void Cgdi::TextAtPos(int x, int y, const std::string& s)
{
	TextOut(m_hdc, x, y, s.c_str(), (int)s.size());
}

void Cgdi::TextAtPos(double x, double y, const std::string& s)
{
	TextOut(m_hdc, (int)x, (int)y, s.c_str(), (int)s.size());
}

void Cgdi::TextAtPos(Vector2D pos, const std::string& s)
{
	TextOut(m_hdc, (int)pos.x, (int)pos.y, s.c_str(), (int)s.size());
}

void Cgdi::TransparentText()
{
	SetBkMode(m_hdc, TRANSPARENT);
}

void Cgdi::OpaqueText()
{
	SetBkMode(m_hdc, OPAQUE);
}

void Cgdi::TextColor(int color)
{
	assert(color < numColors);

	SetTextColor(m_hdc, colors[color]);
}

void Cgdi::TextColor(int r, int g, int b)
{
	SetTextColor(m_hdc, RGB(r, g, b));
}

//------------------------------- Pixels Drawing -----------------------
//----------------------------------------------------------------------

void Cgdi::DrawDot(Vector2D pos, COLORREF color)
{
	SetPixel(m_hdc, (int)pos.x, (int)pos.y, color);
}

void Cgdi::DrawDot(int x, int y, COLORREF color)
{
	SetPixel(m_hdc, x, y, color);
}

//------------------------------- Line Drawing -----------------------
//--------------------------------------------------------------------

void Cgdi::Line(Vector2D from, Vector2D to)
{
	MoveToEx(m_hdc, (int)from.x, (int)from.y, nullptr);
	LineTo(m_hdc, (int)to.x, (int)to.y);
}

void Cgdi::Line(int a, int b, int x, int y)
{
	MoveToEx(m_hdc, a, b, nullptr);
	LineTo(m_hdc, x, y);
}

void Cgdi::Line(double a, double b, double x, double y)
{
	MoveToEx(m_hdc, (int)a, (int)b, nullptr);
	LineTo(m_hdc, (int)x, (int)y);
}

void Cgdi::PolyLine(const std::vector<Vector2D>& points)
{
	// Make sure we have at least 2 points
	if (points.size() < 2) return;

	MoveToEx(m_hdc, (int)points[0].x, (int)points[0].y, nullptr);

	for (uint32_t p = 1; p < points.size(); ++p)
	{
		LineTo(m_hdc, (int)points[p].x, (int)points[p].y);
	}
}

void Cgdi::LineWithArrow(Vector2D from, Vector2D to, double size)
{
	Vector2D norm = Vec2DNormalize(to - from);

	// Calculate where the arrow is attached
	Vector2D crossingPoint = to - (norm * size);

	// Calculate the two extra points required to make the arrowhead
	Vector2D arrowPoint1 = crossingPoint + (norm.Perp() * 0.4f * size);
	Vector2D arrowPoint2 = crossingPoint - (norm.Perp() * 0.4f * size);

	// Draw the line
	MoveToEx(m_hdc, (int)from.x, (int)from.y, nullptr);
	LineTo(m_hdc, (int)crossingPoint.x, (int)crossingPoint.y);

	// Draw the arrowhead (filled with the currently selected brush)
	POINT p[3];

	p[0] = VectorToPOINT(arrowPoint1);
	p[1] = VectorToPOINT(arrowPoint2);
	p[2] = VectorToPOINT(to);

	SetPolyFillMode(m_hdc, WINDING);
	Polygon(m_hdc, p, 3);
}

void Cgdi::Cross(Vector2D pos, int diameter)
{
	Line((int)pos.x - diameter, (int)pos.y - diameter, (int)pos.x + diameter, (int)pos.y + diameter);
	Line((int)pos.x - diameter, (int)pos.y + diameter, (int)pos.x + diameter, (int)pos.y - diameter);
}

//------------------------------- Geometry Drawing -----------------------
//------------------------------------------------------------------------

void Cgdi::Rect(int left, int top, int right, int bot)
{
	Rectangle(m_hdc, left, top, right, bot);
}

void Cgdi::Rect(double left, double top, double right, double bot)
{
	Rectangle(m_hdc, (int)left, (int)top, (int)right, (int)bot);
}

void Cgdi::ClosedShape(const std::vector<Vector2D>& points)
{
	MoveToEx(m_hdc, (int)points[0].x, (int)points[0].y, nullptr);

	for (unsigned int p = 1; p < points.size(); ++p)
	{
		LineTo(m_hdc, (int)points[p].x, (int)points[p].y);
	}

	LineTo(m_hdc, (int)points[0].x, (int)points[0].y);
}

void Cgdi::Circle(Vector2D pos, double radius)
{
	Ellipse(
		m_hdc, 
		(int)(pos.x - radius), 
		(int)(pos.y - radius), 
		(int)(pos.x + radius + 1), 
		(int)(pos.y + radius + 1)
	);
}

void Cgdi::Circle(double x, double y, double radius)
{
	Ellipse(
		m_hdc,
		(int)(x - radius),
		(int)(y - radius),
		(int)(x + radius + 1),
		(int)(y + radius + 1)
	);
}

void Cgdi::Circle(int x, int y, double radius)
{
	Ellipse(
		m_hdc,
		(x - (int)radius),
		(y - (int)radius),
		(x + (int)radius + 1),
		(y + (int)radius + 1)
	);
}

void Cgdi::SetPenColor(int color)
{
	assert(color < numColors);

	switch (color)
	{
		case black:BlackPen(); return;
		case white:WhitePen(); return;
		case red:RedPen(); return;
		case green:GreenPen(); return;
		case blue:BluePen(); return;
		case pink:PinkPen(); return;
		case grey:GreyPen(); return;
		case yellow:YellowPen(); return;
		case orange:OrangePen(); return;
		case purple:PurplePen(); return;
		case brown:WhitePen(); return;
		case light_blue:WhitePen(); return;
		case light_grey:WhitePen(); return;
		case light_pink:WhitePen(); return;
	}
}
