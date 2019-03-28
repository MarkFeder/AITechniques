#pragma once

#include <string>

#include "Public/FSM/State.h"
#include "Public/FSM/StateMachine.h"
#include "Public/Entities/BaseGameEntity.h"
#include "Public/Misc/ConsoleUtils.h"
#include "Public/Misc/Utils.h"
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

	void Update(double timeElapsed) override;

	bool HandleMessage(const Telegram& msg) override;

	StateMachine<Elsa>* GetFSM() const { return m_pStateMachine; }

	// Accessors
	ELocationType Location() const { return m_Location; }
	void ChangeLocation(const ELocationType& newLocation) { m_Location = newLocation; }

	bool IsCooking() const { return m_bCooking; }
	void SetCooking(bool val) { m_bCooking = val; }
};