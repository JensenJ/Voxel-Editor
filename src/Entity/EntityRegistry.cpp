#include "EntityRegistry.h"
#include "Entity.h"

Entity* EntityRegistry::CreateEntity(std::string entityName) {
    Entity* entity = new Entity(entityCount, this, entityName);
    entities.insert(std::pair<unsigned int, Entity*>(entityCount, entity));
    entityCount += 1;
    return entity;
}

void EntityRegistry::Cleanup() {
    componentRegistry.clear();
    for (std::map<unsigned int, Entity*>::iterator iter = entities.begin(); iter != entities.end();
         ++iter) {
        delete iter->second;
    }

    entities.clear();
}

std::map<unsigned int, class Entity*> EntityRegistry::GetAllEntities() { return entities; }

Entity* EntityRegistry::GetEntityFromID(unsigned int id) {
    auto foundEntity = entities.find(id);

    if (foundEntity != entities.end()) {
        return foundEntity->second;
    } else {
        return nullptr;
    }
}
