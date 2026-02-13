#include "VisibilitySystem.h"
#include <Voxel/pch.h>
#include <Voxel/Core.h>
#include <Voxel/ECS/Components/HierarchyComponent.h>
#include <Voxel/ECS/Components/MetaComponent.h>

void VisibilitySystem::Run() {
    ScopedTimer timer(Profiler::system_visibility.lastFrame);

    for (Entity entity : dirtyEntities) {
        MetaComponent* meta = entityRegistry->GetComponent<MetaComponent>(entity);
        HierarchyComponent* hierarchy = entityRegistry->GetComponent<HierarchyComponent>(entity);

        bool parentEffective = true;

        if (hierarchy->HasParent()) {
            MetaComponent* parentMeta =
                entityRegistry->GetComponent<MetaComponent>(hierarchy->parent);
            parentEffective = parentMeta->effectiveVisibility;
        }

        UpdateVisibilityRecursive(entity, parentEffective);
    }
    dirtyEntities.clear();
}

void VisibilitySystem::UpdateVisibilityRecursive(Entity entity, bool parentVisible) {
    MetaComponent* meta = entityRegistry->GetComponent<MetaComponent>(entity);
    HierarchyComponent* hierarchy = entityRegistry->GetComponent<HierarchyComponent>(entity);

    if (!meta || !hierarchy)
        return;

    meta->effectiveVisibility = parentVisible && meta->visibility;

    for (Entity child : hierarchy->children) {
        UpdateVisibilityRecursive(child, meta->effectiveVisibility);
    }
}