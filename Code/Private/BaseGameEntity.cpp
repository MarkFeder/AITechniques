#include "Public/BaseGameEntity.h"
#include <cassert>

int BaseGameEntity::m_iNextvalidID = 0;

void BaseGameEntity::SetID(int val)
{
	assert((val >= m_iNextvalidID) && "<BaseGameEntity::SetID>: invalid ID");

	m_ID = val;
	m_iNextvalidID = m_ID + 1;
}
