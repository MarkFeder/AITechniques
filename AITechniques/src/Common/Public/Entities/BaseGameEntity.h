#pragma once

#include <vector>
#include <string>
#include "Common/Public/2D/Vector2D.h"
#include "Common/Public/Messaging/Telegram.h"
#include "Common/Public/Misc/Utils.h"

class BaseGameEntity
{
public:

	// Enum type which represents the type of `BaseGameEntity` class
	enum EntityType
	{
		ET_Default = -1
	};

private:

	// Every entity has an unique identifier number
	int m_ID;

	// Each entity has a type associated with it (health, ammo, etc)
	int m_EntityType;

	// Each entity has a generic flag
	bool m_bTag;

	// This is the next valid ID. Each time a BaseGameEntity is instantiated
	// this value is always updated
	static int m_iNextvalidID;

	// This is called within the constructor to make sure the ID is set
	// properly. It verifies that the value passed to the method is greater or 
	// equal to the next valid ID, before setting the ID and incrementing
	// the next valid ID
	void SetID(int val);

protected:

	// Its location in the environment
	Vector2D m_vPos;

	// Its scale in the environment
	Vector2D m_vScale;

	// The length of this object's bounding radius
	double m_dBoundingRadius;

public:

	// Default constructor
	BaseGameEntity() = default;
	
	// Default virtual destructor
	virtual ~BaseGameEntity() = default;

	// Set ID on constructor
	BaseGameEntity(int id)
		:m_ID(-1),
		m_EntityType(ET_Default),
		m_bTag(false),
		m_dBoundingRadius(0)
	{ 
		SetID(id); 
	}

	// Another constructor with more additional parameters
	BaseGameEntity(int entityType, Vector2D pos, double radius)
		:m_ID(-1),
		m_EntityType(entityType),
		m_bTag(false),
		m_vPos(pos),
		m_vScale(Vector2D(1.0f, 1.0f)),
		m_dBoundingRadius(radius)
	{
		SetID(m_iNextvalidID);
	}

	// Another constructor with more additional parameters
	BaseGameEntity(int entityType, Vector2D pos, double radius, Vector2D scale)
		:m_ID(-1),
		m_EntityType(entityType),
		m_bTag(false),
		m_vPos(pos),
		m_vScale(scale),
		m_dBoundingRadius(radius)
	{
		SetID(m_iNextvalidID);
	}

	// Get the current ID of this entity
	int ID() const { return m_ID; }

	// This is used to grab the next valid ID
	static int GetNextValidID() { return m_iNextvalidID; }

	// This is used to reset the next ID
	static void ResetNextValidID() { m_iNextvalidID = 0; }

	// All entities must implement an update function
	virtual void Update() = 0;

	// All entities are rendered
	virtual void Render() {};

	// All entities must be capable of write their data to a stream
	virtual void Write(std::ostream& os) const {}

	// All entities must be able to read their data from a steam
	virtual void Read(std::ifstream& is) {}

	// All entities can communicate using messages. They are sent
	// using the MessageDispatcher singleton class
	virtual bool HandleMessage(const Telegram& msg) = 0;

	// Get/Set the current pos of/to this entity
	Vector2D Pos() const { return m_vPos; }
	void SetPos(Vector2D newPos) { m_vPos = newPos; }

	// Get/Set bounding radius of/to this entity
	double BRadius() const { return m_dBoundingRadius; }
	void SetBRadius(double radius) { m_dBoundingRadius = radius; }

	// Get/Set the scale of/to this entity
	Vector2D Scale() const { return m_vScale; }
	void SetScale(Vector2D newScale) { m_vScale = newScale; }

	// Get the current tag of this entity
	bool IsTagged() { return m_bTag; }
	void Tag() { m_bTag = true; }
	void UnTag() { m_bTag = false; }
};

