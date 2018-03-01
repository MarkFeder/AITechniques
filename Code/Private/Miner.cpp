#include "../Public/Miner.h"
#include "../Public/State.h"
#include <cassert>
#include <iostream>
#include <string>


Miner::Miner(int ID)
	:BaseGameEntity{ID},
	m_iGoldCarried{0},
	m_iMoneyInBank{0},
	m_iThirst{0},
	m_iFatigue{0},
	m_pCurrentState{nullptr}
{}

void Miner::Update()
{
	m_iThirst += 1;

	if (m_pCurrentState)
	{
		m_pCurrentState->Execute(this);
	}
}

void Miner::ChangeState(State * pNewState)
{
	// make sure both states are both valid before attempting to call
	// their methods
	assert(m_pCurrentState && pNewState);

	// call the exit method of the existing state
	m_pCurrentState->Exit(this);

	// change state to the new state
	m_pCurrentState = pNewState;

	// call the entry method of the new state
	m_pCurrentState->Enter(this);
}

void Miner::AddToGoldCarried(const int val)
{
	m_iGoldCarried += val;

	if (m_iGoldCarried < 0)
	{
		m_iGoldCarried = 0;
		std::cout << "Miner::AddToGoldCarried: minimum gold carried reached!" << std::endl;
	}
}

bool Miner::Fatigued() const
{
	return m_iFatigue > TirednessThreshold;
}

void Miner::AddToWealth(const int val)
{
	m_iMoneyInBank += val;

	if (m_iMoneyInBank < 0)
	{
		m_iMoneyInBank = 0;
		std::cout << "Miner::AddToWealth: minimum wealth carried reached!" << std::endl;
	}
}

bool Miner::Thirsty() const
{
	return m_iThirst >= ThirstLevel;
}
