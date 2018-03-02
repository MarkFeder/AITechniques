#pragma once

#include "Public/Common/Telegram.h"

class BaseGameEntity
{
private:
	// Every entity has an unique identifier number
	int m_ID;

	// This is the next valid ID. Each time a BaseGameEntity is instantiated
	// this value is always updated
	static int m_iNextvalidID;

	// This is called within the constructor to make sure the ID is set
	// properly. It verifies that the value passed to the method is greater or 
	// equal to the next valid ID, before setting the ID and incrementing
	// the next valid ID
	void SetID(int val);

public:
	// Default constructor
	BaseGameEntity() = default;
	
	// Set ID on constructor
	BaseGameEntity(int id)
	{
		SetID(id);
	}

	// Default virtual destructor
	virtual ~BaseGameEntity() = default;

	// All entities must implement an update function
	virtual void Update() = 0;

	// All entities can communicate using messages. They are sent
	// using the MessageDispatcher singleton class
	virtual bool HandleMessage(const Telegram& msg) = 0;

	// Get the current ID of this entity
	int ID() const { return m_ID; }
};

