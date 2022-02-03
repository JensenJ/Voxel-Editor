#include "Entity.h"
#include "Components/Component.h"
#include <iostream>

Entity::Entity(unsigned int entityID, EntityRegistry* eRegistry)
{
	this->entityID = entityID;
	this->eRegistry = eRegistry;
}