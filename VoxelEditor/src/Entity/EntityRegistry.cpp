#include "EntityRegistry.h"
#include "Entity.h"

Entity EntityRegistry::CreateEntity()
{
	Entity entity(entityCount, this);
	entityCount += 1;
	return entity;
}

void EntityRegistry::Cleanup()
{
	componentRegistry.clear();
}
