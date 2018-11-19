#pragma once

#include "Common/Public/2D/Vector2D.h"
#include "Common/Public/Misc/Cgdi.h"

/*
V simple inverted (y increases down screen) axis 
aligned bounding box class.
*/
class InvertedAABox2D
{
private:

	Vector2D m_vTopLeft;
	Vector2D m_vBottomRight;
	Vector2D m_vCenter;

public:

	InvertedAABox2D(Vector2D t1, Vector2D br)
		:m_vTopLeft(t1),
		m_vBottomRight(br),
		m_vCenter((t1 + br) / 2.0)
	{}

	// Returns true if the bbox described by other intersects with this one
	bool IsOverlappedWith(const InvertedAABox2D& other) const
	{
		return !((other.Top() > this->Bottom()) || 
			(other.Bottom() < this->Top()) || 
			(other.Left() > this->Right()) || 
			(other.Right() < this->Left()));
	}

	void Render(bool renderCenter = false) const
	{
		gdi->Line((int)Left(), (int)Top(), (int)Right(), (int)Top());
		gdi->Line((int)Left(), (int)Bottom(), (int)Right(), (int)Bottom());
		gdi->Line((int)Left(), (int)Top(), (int)Left(), (int)Bottom());
		gdi->Line((int)Right(), (int)Top(), (int)Right(), (int)Bottom());

		if (renderCenter)
		{
			gdi->Circle(m_vCenter, 5);
		}
	}

	const Vector2D& TopLeft() const { return m_vTopLeft; }
	const Vector2D& BottomRight() const { return m_vBottomRight; }
	const Vector2D& Center() const { return m_vCenter; }

	double Top() const { return m_vTopLeft.y; }
	double Left() const { return m_vTopLeft.x; }
	double Bottom() const { return m_vBottomRight.y; }
	double Right() const { return m_vBottomRight.x; }
};

