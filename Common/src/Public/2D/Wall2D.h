#pragma once

#include <fstream>

#include "Public/Misc/Cgdi.h"
#include "Public/2D/Vector2D.h"

class Wall2D
{
protected:

	Vector2D m_vA;
	Vector2D m_vB;
	Vector2D m_vN;

	void CalculateNormal()
	{
		Vector2D temp = Vec2DNormalize(m_vB - m_vA);

		m_vN.x = -temp.y;
		m_vN.y = temp.x;
	}

public:

	Wall2D() = default;
	Wall2D(Vector2D& a, Vector2D& b) : m_vA(a), m_vB(b) { CalculateNormal(); }
	Wall2D(Vector2D& a, Vector2D& b, Vector2D& n) : m_vA(a), m_vB(b), m_vN(n) {}
	Wall2D(std::ifstream& in) { Read(in); }

	// Getters
	const Vector2D& From() const { return m_vA; }
	const Vector2D& To() const { return m_vB; }
	const Vector2D& Normal() const { return m_vN; }
	const Vector2D& Center() const { (m_vA + m_vB) / 2.0; }

	// Setters
	void SetFrom(const Vector2D& v) { m_vA = v; CalculateNormal(); }
	void SetTo(const Vector2D& v) { m_vB = v; CalculateNormal(); }
	void SetNormal(const Vector2D& v) { m_vN = v; }

	// Render wall along with its info
	virtual void Render(bool renderNormals = false) const
	{
		gdi->Line(m_vA, m_vB);

		// Render the normals if required
		if (renderNormals)
		{
			int midX = (int)((m_vA.x + m_vB.x) / 2);
			int midY = (int)((m_vA.y + m_vB.y) / 2);

			gdi->Line(midX, midY, (int)(midX + (m_vN.x * 5)), (int)(midY + (m_vN.y * 5)));
		}
	}

	// Take info from an ifstream
	void Read(std::ifstream& in)
	{
		double x, y;

		in >> x >> y;
		SetFrom(Vector2D(x, y));

		in >> x >> y;
		SetTo(Vector2D(x, y));

		in >> x >> y;
		SetNormal(Vector2D(x, y));
	}

	// Write info to an ostream
	std::ostream& Write(std::ostream& os) const
	{
		os << std::endl;
		os << From() << ",";
		os << To() << ",";
		os << Normal();

		return os;
	}
};