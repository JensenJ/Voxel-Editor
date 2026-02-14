#pragma once

#include <Voxel/pch.h>
#include <Voxel/Core.h>

struct EntityVisibilityChangedEvent {
    Entity entity;
    bool visibility;
};

class VisibilitySystem {
  public:
    static void Init(EntityRegistry* registry) {
        entityRegistry = registry;

        onEntityChangedVisibility.AddObserver([](const EntityVisibilityChangedEvent& event) {
            dirtyEntities.push_back(event.entity);
        });

        LOG_INFO("Initialised VisibilitySystem");
    }

    static inline Subject<EntityVisibilityChangedEvent> onEntityChangedVisibility;
    static inline Subject<EntityVisibilityChangedEvent> onEntityChangedEffectiveVisibility;

    static void Run();
    static void UpdateVisibilityRecursive(Entity entity, bool parentVisible);

  private:
    static inline EntityRegistry* entityRegistry = nullptr;
    static inline std::vector<Entity> dirtyEntities = std::vector<Entity>();
};