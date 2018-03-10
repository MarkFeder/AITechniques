#pragma once

#include <cassert>
#include <string>

#include "State.h"
#include "Common/Public/Messaging/Telegram.h"

template<class entity_type>
class StateMachine
{
private:

	// A pointer to the agent that owns this instance
	entity_type* m_pOwner;

	State<entity_type>* m_pCurrentState;

	// A record of the last state the agent was in
	State<entity_type>* m_pPreviousState;

	// This is called every time the FSM is updated
	State<entity_type>* m_pGlobalState;

public:

	StateMachine(entity_type* pOwner)
		: m_pOwner(pOwner),
		m_pCurrentState(nullptr),
		m_pPreviousState(nullptr),
		m_pGlobalState(nullptr)
	{}

	virtual ~StateMachine() = default;

	// Getters
	void CurrentState() const { return m_pCurrentState; }
	void PreviousState() const { return m_pPreviousState; }
	void GlobalState() const { return m_pGlobalState; }

	// Use these methods to initialize the FSM
	void SetCurrentState(State<entity_type>* state) { m_pCurrentState = state; }
	void SetPreviousState(State<entity_type>* state) { m_pPreviousState = state; }
	void SetGlobalState(State<entity_type>* state) { m_pGlobalState = state; }

	// Call this method to update the FSM
	void Update() const
	{
		// If a global state exists, call its execute method, else do nothing
		if (m_pGlobalState)
		{
			m_pGlobalState->Execute(m_pOwner);
		}

		// Same for the current state
		if (m_pCurrentState)
		{
			m_pCurrentState->Execute(m_pOwner);
		}
	}

	// Call this method to handle received message depending on the state
	bool HandleMessage(const Telegram& msg) const
	{
		// First see if the current state is valid and that it can
		// handle the message
		if (m_pCurrentState && m_pCurrentState->OnMessage(m_pOwner, msg))
		{
			return true;
		}

		// If not, and if a global state ha been implemented, send
		// the message to the global state
		if (m_pGlobalState && m_pGlobalState->OnMessage(m_pOwner, msg))
		{
			return true;
		}

		return false;
	}

	// Call this method to change to a new State
	void ChangeState(State<entity_type>* pNewState)
	{
		assert(pNewState && "<StateMachine::ChangeState>: trying to assign null state to current");

		// Keep a record of the previous state
		m_pPreviousState = m_pCurrentState;

		// Call the exit method of the existing state
		m_pCurrentState->Exit(m_pOwner);

		// Change state to the new State
		m_pCurrentState = pNewState;

		// Call the entry method of the new State
		m_pCurrentState->Enter(m_pOwner);
	}

	void RevertToPreviousState()
	{
		ChangeState(m_pPreviousState);
	}

	// Call this method to check whether current state is equal to the 
	// type of the class passed as a parameter
	bool IsInState(const State<entity_type>& state) const
	{
		return (typeid(*m_pCurrentState) == typeid(state));
	}

	// Call this method to retrieve the name of the current state
	std::string GetNameOfCurrentState() const
	{
		std::string s(typeid(*m_pCurrentState).name());

		// Remove the 'class ' part from the front of the string
		if (s.size() > 5)
		{
			s.erase(0, 6);
		}

		return s;
	}
};
