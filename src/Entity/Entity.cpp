#include "Entity.h"
#include <iostream>
#include "Components/Component.h"

Entity::Entity(unsigned int entityID, EntityRegistry* eRegistry, std::string entityName) {
    this->entityID = entityID;
    this->entityName = entityName;
    this->eRegistry = eRegistry;
}

std::vector<Component*> Entity::GetComponents() { return components; }