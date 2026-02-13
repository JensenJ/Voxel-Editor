#pragma once

#include <Voxel/pch.h>
#include <Voxel/Core.h>

class VisibilitySystem {
  public:
    static void Init(EntityRegistry* registry) {
        entityRegistry = registry;
        LOG_INFO("Initialised VisibilitySystem");
    }

    static void Run();
    static void UpdateVisibilityRecursive(Entity entity, bool parentVisible);
    static void MarkEntityDirty(Entity entity) { dirtyEntities.push_back(entity); }

  private:
    static inline EntityRegistry* entityRegistry = nullptr;
    static inline std::vector<Entity> dirtyEntities = std::vector<Entity>();
};