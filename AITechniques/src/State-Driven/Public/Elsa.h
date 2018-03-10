#pragma once

#include <string>

#include "Common/Public/FSM/State.h"
#include "Common/Public/FSM/StateMachine.h"
#include "Common/Public/Entities/BaseGameEntity.h"
#include "Common/Public/Misc/ConsoleUtils.h"
#include "Common/Public/Misc/Utils.h"
#include "Locations.h"
#include "Miner.h"
#include "ElsaStates.h"

class Elsa : public BaseGameEntity
{
private:

	// An instance of the state machine class
	StateMachine<Elsa>* m_pStateMachine;

	ELocationType m_Location;

	// Is Elsa cooking?
	bool m_bCooking;

public:

	Elsa(int id)
		:BaseGameEntity(id),
		m_Location(ELocationType::EL_Shack),
		m_bCooking(false)
	{
		m_pStateMachine = new StateMachine<Elsa>(this);

		m_pStateMachine->SetCurrentState(DoHouseWorkState::Instance());

		m_pStateMachine->SetGlobalState(WifeGlobalState::Instance());
	}

	~Elsa() { delete m_pStateMachine; }

	void Update();

	virtual bool HandleMessage(const Telegram& msg);

	StateMachine<Elsa>* GetFSM() const { return m_pStateMachine; }

	// Accessors
	ELocationType Location() const { return m_Location; }
	void ChangeLocation(const ELocationType& newLocation) { m_Location = newLocation; }

	bool IsCooking() const { return m_bCooking; }
	void SetCooking(bool val) { m_bCooking = val; }
};