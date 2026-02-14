#pragma once
#include <Voxel/pch.h>
#include <Voxel/Core.h>
#include <typeindex>
#include <Voxel/ECS/ComponentStorage.h>
#include <Voxel/ECS/Entity.h>
#include <Voxel/ECS/View.h>
#include <Voxel/Event/Event.h>

struct EntityAddComponentEvent {
    Entity entity;
    std::type_index componentType;
};

struct EntityRemoveComponentEvent {
    Entity entity;
    std::type_index componentType;
};

struct EntityAddEvent {
    Entity entity;
};

struct EntityRemoveEvent {
    Entity entity;
};

struct EntityClearEvent {};

class EntityRegistry {
  public:
    static class EntityRegistry* GetInstance();

    static inline Subject<EntityAddComponentEvent> onAddComponent;
    static inline Subject<EntityRemoveComponentEvent> onRemoveComponent;
    static inline Subject<EntityAddEvent> onAddEntity;
    static inline Subject<EntityRemoveEvent> onRemoveEntity;
    static inline Subject<EntityClearEvent> onClearEntities;

    Entity CreateEntity() {
        Entity e = ++nextEntity;
        onAddEntity.Notify({e});
        return e;
    }

    void DestroyEntity(Entity e) {
        for (auto& remover : componentRemovers)
            remover(e);
        if (selectedEntity == e) {
            selectedEntity = InvalidEntity;
        }
        onRemoveEntity.Notify({e});
    }

    template <typename T, typename... Args> T& AddComponent(Entity e, Args&&... args) {
        T& comp = GetStorage<T>().Add(e, std::forward<Args>(args)...);
        comp.entity = e;
        onAddComponent.Notify({e, std::type_index(typeid(T))});
        return comp;
    }

    template <typename T> T* GetComponent(Entity e) { return GetStorage<T>().Get(e); }

    template <typename T> bool HasComponent(Entity e) { return GetStorage<T>().Has(e); }

    template <typename T> bool RemoveComponent(Entity e) {
        onRemoveComponent.Notify({e, std::type_index(typeid(T))});
        return GetStorage<T>().Remove(e);
    }

    template <typename... Ts> View<Ts...> MakeView() {
        return View<Ts...>(GetStorage<std::remove_const_t<Ts>>()...);
    }

    void Cleanup() {
        for (auto& clear : storageClearers)
            clear();
        nextEntity = InvalidEntity;
        selectedEntity = InvalidEntity;
        onClearEntities.Notify({});
    }

    void SelectEntity(Entity entity) { selectedEntity = entity; }
    Entity GetSelectedEntity() { return selectedEntity; }

  private:
    static EntityRegistry* instance;
    Entity nextEntity = InvalidEntity;

    Entity selectedEntity = InvalidEntity;

    template <typename T> ComponentStorage<T>& GetStorage() {
        static ComponentStorage<T> storage;
        static bool registered = false;

        if (!registered) {
            componentRemovers.emplace_back([](Entity e) {
                onRemoveComponent.Notify({e, std::type_index(typeid(T))});
                storage.Remove(e);
            });
            storageClearers.emplace_back([]() { storage.Clear(); });

            registered = true;
        }
        return storage;
    }

    std::vector<std::function<void(Entity)>> componentRemovers;
    std::vector<std::function<void()>> storageClearers;
};
