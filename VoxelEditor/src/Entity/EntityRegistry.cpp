#include "EntityRegistry.h"
#include "Entity.h"

Entity* EntityRegistry::CreateEntity()
{
	Entity* entity = new Entity(entityCount, this);
	//entities.emplace(entityCount, &entity);
	entities.insert(std::pair<unsigned int, Entity*>(entityCount, entity));
	entityCount += 1;
	return entity;
}

void EntityRegistry::Cleanup()
{
	componentRegistry.clear();
	for (std::map<unsigned int, Entity*>::iterator iter = entities.begin(); iter != entities.end(); ++iter)
	{
		delete iter->second;
	}

	entities.clear();
}

Entity* EntityRegistry::GetEntityFromID(unsigned int id)
{
	auto foundEntity = entities.find(id);

	//std::cout << foundEntity->first << ", " << foundEntity->second << std::endl;

	if (foundEntity != entities.end())
	{
		return foundEntity->second;
	}
	else
	{
		return nullptr;
	}
}
