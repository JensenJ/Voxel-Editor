#pragma once

#include <Voxel/pch.h>
#include <Voxel/Core.h>
#include <Voxel/ECS/Components/HierarchyComponent.h>
#include <Voxel/ECS/Components/MetaComponent.h>

class VisibilitySystem {
  public:
    static void Init(EntityRegistry* registry) {
        entityRegistry = registry;
        LOG_INFO("Initialised VisibilitySystem");
    }

    static void Run() {
        if (!MetaComponent::IsVisibilityDirty())
            return;

        for (auto [e, meta, hierarchy] :
             entityRegistry->MakeView<MetaComponent, const HierarchyComponent>()) {
            if (hierarchy.parent == InvalidEntity) { // Only check root entities
                UpdateVisibilityRecursive(e, true);
            }
        }
        MetaComponent::MarkVisibilityClean();
    }

    static void UpdateVisibilityRecursive(Entity entity, bool parentVisible) {
        MetaComponent* meta = entityRegistry->GetComponent<MetaComponent>(entity);
        HierarchyComponent* hierarchy = entityRegistry->GetComponent<HierarchyComponent>(entity);

        if (!meta || !hierarchy)
            return;

        meta->effectiveVisibility = parentVisible && meta->visibility;

        for (Entity child : hierarchy->children) {
            UpdateVisibilityRecursive(child, meta->effectiveVisibility);
        }
    }

  private:
    static inline EntityRegistry* entityRegistry = nullptr;
};