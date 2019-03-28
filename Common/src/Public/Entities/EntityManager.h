#pragma once

#include <map>
#include <cassert>
#include <string>

class BaseGameEntity;

// Provide easy access to EntityManager
#define EntityMgr EntityManager::Instance()

class EntityManager
{
private:

	typedef std::map<int, BaseGameEntity*> EntityMap;
	
	// To facilitate quick lookup the entites are stored in a std::map, in which
	// pointers to entites are cross referenced by their identifying number
	EntityMap m_EntityMap;

	EntityManager() = default;

public:

	// Copy ctor and assignment are deleted
	EntityManager(const EntityManager&) = delete;
	EntityManager& operator=(const EntityManager&) = delete;

	static EntityManager* Instance();

	// This method stores a pointer to the entity in the std::map
	// m_Entities at the index position indicated by the entity's 
	// ID (for faster access)
	void RegisterEntity(BaseGameEntity* pNewEntity);

	// Returns a pointer to the entity with the ID given as a parameter
	BaseGameEntity* GetEntityFromID(int id) const;

	// This method removes the entity from the list
	void RemoveEntity(BaseGameEntity* pEntity);
};