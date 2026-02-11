#pragma once

#include <Voxel/pch.h>
#include <Voxel/Core.h>
#include <Voxel/ECS/Components/HierarchyComponent.h>
#include <Voxel/ECS/Components/TransformComponent.h>
#include <Voxel/ECS/EntityRegistry.h>

class TransformSystem {
  public:
    static void Init(EntityRegistry* registry) {
        entityRegistry = registry;
        LOG_INFO("Initialised TransformSystem");
    }

    static void Run() {
        // Update components with a hierarchy
        for (auto [entity, transform, hierarchy] :
             entityRegistry->MakeView<const TransformComponent, const HierarchyComponent>()) {
            if (!hierarchy.HasParent()) {
                UpdateRecursive(entity, glm::mat4(1.0f));
            }
        }

        // Update components without a hierarchy
        for (auto [entity, transform] : entityRegistry->MakeView<TransformComponent>()) {
            if (entityRegistry->HasComponent<HierarchyComponent>(entity))
                continue;

            if (transform.IsDirty()) {
                transform.worldMatrix = transform.localMatrix;
                transform.MarkClean();
            }
        }
    }

  private:
    static inline EntityRegistry* entityRegistry = nullptr;

    static void UpdateRecursive(Entity entity, const glm::mat4& parentWorld) {
        TransformComponent* transform = entityRegistry->GetComponent<TransformComponent>(entity);

        if (transform->IsDirty()) {
            transform->worldMatrix = parentWorld * transform->localMatrix;
            transform->MarkClean();
        }

        if (!entityRegistry->HasComponent<HierarchyComponent>(entity))
            return;

        HierarchyComponent* hierarchy = entityRegistry->GetComponent<HierarchyComponent>(entity);
        for (Entity child : hierarchy->children) {
            UpdateRecursive(child, transform->worldMatrix);
        }
    }
};
