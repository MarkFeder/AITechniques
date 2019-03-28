#include "Public/Entities/EntityManager.h"
#include "Public/Entities/BaseGameEntity.h"

EntityManager* EntityManager::Instance()
{
	static EntityManager instance;

	return &instance;
}

void EntityManager::RegisterEntity(BaseGameEntity* pNewEntity)
{
	assert(pNewEntity && "<EntityManager::RegisterEntity>: pNewEntity is null");

	m_EntityMap.insert(std::make_pair(pNewEntity->ID(), pNewEntity));
}

BaseGameEntity * EntityManager::GetEntityFromID(int id) const
{
	// Find the entity
	EntityMap::const_iterator it = m_EntityMap.find(id);

	// Assert that the entity is a member of the map
	assert((it != m_EntityMap.cend()) && "<EntityManager::GetEntityFromID>: invalid ID");

	return it->second;
}

void EntityManager::RemoveEntity(BaseGameEntity* pEntity)
{
	assert(pEntity && "<EntityManager::RemoveEntity>: pEntity is null");

	m_EntityMap.erase(m_EntityMap.find(pEntity->ID()));
}
