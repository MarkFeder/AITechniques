#include "Steering-Behaviours/Public/Path.h"
#include "Common/Public/2D/Transformations.h"
#include "Common/Public/Misc/Utils.h"
#include "Common/Public/Misc/Cgdi.h"

//------------------------------- CreateRandomPath -----------------------
//------------------------------------------------------------------------

void Path::CreateRandomPath(int numWaypoints, double minX, double minY, double maxX, double maxY)
{
	m_wayPoints.clear();

	double midX = (maxX + minX) / 2.0;
	double midY = (maxY + minY) / 2.0;

	double smaller = min(midX, midY);
	double spacing = TwoPi / (double)numWaypoints;

	for (int i = 0; i < numWaypoints; ++i)
	{
		double radialDist = RandInRange(smaller * 0.2f, smaller);

		Vector2D temp(radialDist, 0.0f);

		Vec2DRotateAroundOrigin(temp, i * spacing);

		temp.x += midX;
		temp.y += midY;

		m_wayPoints.push_back(temp);
	}

	m_curWaypoint = m_wayPoints.begin();
}

//------------------------------- SetNextWaypoint -----------------------
//-----------------------------------------------------------------------

void Path::SetNextWaypoint()
{
	assert(m_wayPoints.size() > 0);

	if (++m_curWaypoint == m_wayPoints.end())
	{
		if (m_bLooped)
		{
			m_curWaypoint = m_wayPoints.begin();
		}
	}
}

//------------------------------- Render -----------------------
//--------------------------------------------------------------

void Path::Render() const
{
	gdi->OrangePen();

	std::list<Vector2D>::const_iterator it = m_wayPoints.begin();

	Vector2D wp = *it++;

	while (it != m_wayPoints.end())
	{
		gdi->Line(wp, *it);

		wp = *it++;
	}

	if (m_bLooped)
	{
		gdi->Line(*(--it), *m_wayPoints.begin());
	}
}
