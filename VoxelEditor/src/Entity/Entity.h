#pragma once
#include "Components/Component.h"
#include "Components/TransformComponent.h"
#include <iostream>
#include <type_traits>
#include "EntityRegistry.h"

class Entity
{
public:
	Entity(unsigned int entityID, class EntityRegistry* eRegistry);

	unsigned int GetEntityID() { return entityID; }

	template<typename T, class... Args>
	T* AddComponent(Args... args)
	{
		//Check if type is a subclass of component
		static_assert(!std::is_same<Component, T>::value, "Added component must be subclass of Component");
		static_assert(std::is_base_of<Component, T>::value, "Added component must be subclass of Component");

		//If this entity has this component already, return that
		T* hasComp = GetComponent<T>();
		if (hasComp != nullptr) { return hasComp; }

		//Otherwise create new one in registry
		T* comp = static_cast<T*>(eRegistry->AddComponentOfTypeToRegistry<T>(entityID, args...));

		//Add to local copy of components
		components.push_back(comp);
		return comp;
	}
	
	//Get the component of type T from this entity
	template<typename T>
	T* GetComponent()
	{
		//Check if type is a subclass of component
		static_assert(!std::is_same<Component, T>::value, "Component must be subclass of Component");
		static_assert(std::is_base_of<Component, T>::value, "Component must be subclass of Component");

		//loop over local components, find component of type T
		for (int i = 0; i < components.size(); i++)
		{
			if (typeid(*components[i]).name() == typeid(T).name())
			{
				return static_cast<T*>(components[i]);
			}
		}

		//Component not found on this entity
		return nullptr;
	}

	//Check if this entity has a component of this type
	template<typename T>
	bool HasComponent()
	{
		//Check if type is a subclass of component
		static_assert(!std::is_same<Component, T>::value, "Component must be subclass of Component");
		static_assert(std::is_base_of<Component, T>::value, "Component must be subclass of Component");

		for (int i = 0; i < components.size(); i++)
		{
			if (typeid(*components[i]).name() == typeid(T).name())
			{
				return true;
			}
		}

		return false;
	}

private:
	//The id associated with this entity, should be unique for each instance
	unsigned int entityID;
	//Registry of entities, faster accessing for bulk data, e.g. updating components/rendering
	class EntityRegistry* eRegistry;

	//Local copy of components addresses for this entity, faster accessing for single calls
	std::vector<Component*> components;
};