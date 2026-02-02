#pragma once

#include <map>
#include <vector>
#include <iostream>
#include <utility>
#include "Components/Component.h"

class EntityRegistry
{
public:

	//Map of all components, regardless of type
	//std::map<Component*, std::map<unsigned int, Component*>> componentRegistry;

	//map which contains
	//	1. type of component, component* can be used with std::is_same with T
	//	2. list/vectorof components of this type, we can check if they belong to this entity using component->GetOwningEntityId()
	// Problem is linear searching to get component, can get around using map but then not easy to batch call

	std::map<Component*, std::vector<Component*>> componentRegistry;

public:
	class Entity* CreateEntity(std::string entityName = "Entity");

	void Cleanup();

	template<typename T>
	bool RegistryHasComponentType()
	{
		//Check if type is a subclass of component
		static_assert(!std::is_same<Component, T>::value, "Component must be subclass of Component");
		static_assert(std::is_base_of<Component, T>::value, "Component must be subclass of Component");

		//For every component type in component registry
		for (std::map<Component*, std::vector<Component*>>::iterator iter = componentRegistry.begin(); iter != componentRegistry.end(); ++iter)
		{
			//If this component is of type T
			Component& comp = *iter->first;
			if (typeid(comp).name() == typeid(T).name())
			{
				return true;
			}
		}
		return false;
	}

	void UpdateAllComponents(float deltaTime)
	{
		for (std::map<Component*, std::vector<Component*>>::iterator iter = componentRegistry.begin(); iter != componentRegistry.end(); ++iter)
		{
			iter->first->OnUpdate(deltaTime);
		}
	}

	template<typename T>
	std::vector<T*> GetAllComponentsOfType()
	{
		//Check if type is a subclass of component
		static_assert(!std::is_same<Component, T>::value, "Component must be subclass of Component");
		static_assert(std::is_base_of<Component, T>::value, "Component must be subclass of Component");

		//For every type of component in registry
		for (std::map<Component*, std::vector<Component*>>::iterator iter = componentRegistry.begin(); iter != componentRegistry.end(); ++iter)
		{
			//if this is the component type we want
			Component& comp = *iter->first;
			if(typeid(comp).name() == typeid(T).name())
			{
				//Cast all elements of this array to type T and return it
				return reinterpret_cast<std::vector<T*>&>(iter->second);
			}
		}
		//Return empty component array
		return std::vector<T*>();
	}

	template<typename T, class... Args>
	Component* AddComponentOfTypeToRegistry(unsigned int entity, Args... args)
	{
		//Create a component of type T
		Component* comp = static_cast<Component*>(new T(args...));
		comp->SetOwningEntityID(entity);

		//For every type of component
		for (std::map<Component*, std::vector<Component*>>::iterator iter = componentRegistry.begin(); iter != componentRegistry.end(); ++iter)
		{
			//If this component is of type T
			Component& comp = *iter->first;
			if (typeid(comp).name() == typeid(T).name())
			{
				//Add component to this vector and return the component
				iter->second.push_back(comp);
				return comp;
			}
		}

		//This type of component does not exist in the registry if we have not added it yet, so create a new vector of this type and add to registry
		std::vector<Component*> newCompVector = std::vector<Component*>();
		newCompVector.push_back(comp);
		componentRegistry.emplace(comp, newCompVector);
		return comp;
	}

	class Entity* GetEntityFromID(unsigned int id);
	std::map<unsigned int, class Entity*> GetAllEntities();

private:
	unsigned int entityCount = 0;
	std::map<unsigned int, class Entity*> entities;
};