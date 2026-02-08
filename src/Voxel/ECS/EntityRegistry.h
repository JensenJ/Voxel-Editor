#pragma once
#include <Voxel/pch.h>
#include <Voxel/Core.h>
#include <Voxel/ECS/ComponentStorage.h>
#include <Voxel/ECS/Entity.h>
#include <Voxel/ECS/View.h>

class EntityRegistry {
  public:
    static class EntityRegistry* GetInstance();

    Entity CreateEntity() { return ++nextEntity; }

    void DestroyEntity(Entity e) {
        for (auto& remover : componentRemovers)
            remover(e);
    }

    template <typename T, typename... Args> T& AddComponent(Entity e, Args&&... args) {
        T comp(std::forward<Args>(args)...);
        comp.entity = e;
        return GetStorage<T>().Add(e, std::move(comp));
    }

    template <typename T> T* GetComponent(Entity e) { return GetStorage<T>().Get(e); }

    template <typename T> bool HasComponent(Entity e) const { return GetStorage<T>().Has(e); }

    template <typename T> bool RemoveComponent(Entity e) { return GetStorage<T>().Remove(e); }

    template <typename... Ts> View<Ts...> MakeView() {
        return View<Ts...>(GetStorage<std::remove_const_t<Ts>>()...);
    }

    void Cleanup() {
        for (auto& clear : storageClearers)
            clear();
        nextEntity = InvalidEntity;
    }

  private:
    static EntityRegistry* instance;
    Entity nextEntity = InvalidEntity;

    template <typename T> ComponentStorage<T>& GetStorage() {
        static ComponentStorage<T> storage;
        static bool registered = false;

        if (!registered) {
            componentRemovers.emplace_back([](Entity e) { storage.Remove(e); });
            storageClearers.emplace_back([]() { storage.Clear(); });

            registered = true;
        }
        return storage;
    }

    std::vector<std::function<void(Entity)>> componentRemovers;
    std::vector<std::function<void()>> storageClearers;
};
