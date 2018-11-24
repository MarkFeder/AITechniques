#include "State-Driven/Public/Miner.h"
#include "State-Driven/Public/MinerStates.h"
#include "Common/Public/FSM/State.h"

#include <cassert>
#include <iostream>
#include <string>


Miner::Miner(int ID)
	:BaseGameEntity{ID},
	m_iGoldCarried{0},
	m_iMoneyInBank{0},
	m_iThirst{0},
	m_iFatigue{0}
{
	// Setup Miner's state machine
	m_pStateMachine = new StateMachine<Miner>(this);

	m_pStateMachine->SetCurrentState(GoHomeAndSleepTilRestedState::Instance());
	
	// A global state has not been implemented for the Miner
}

void Miner::Update(double timeElapsed)
{
	++m_iThirst;

	m_pStateMachine->Update();
}

bool Miner::HandleMessage(const Telegram & msg)
{
	return m_pStateMachine->HandleMessage(msg);
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
