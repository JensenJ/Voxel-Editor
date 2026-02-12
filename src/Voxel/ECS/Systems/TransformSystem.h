#pragma once

#include <Voxel/pch.h>
#include <Voxel/Core.h>
#include <glm/gtx/matrix_decompose.hpp>
#include <Voxel/ECS/Components/HierarchyComponent.h>
#include <Voxel/ECS/Components/MetaComponent.h>
#include <Voxel/ECS/Components/TransformComponent.h>
#include <Voxel/ECS/EntityRegistry.h>

class TransformSystem {
  public:
    static void Init(EntityRegistry* registry) {
        entityRegistry = registry;
        LOG_INFO("Initialised TransformSystem");
    }

    static void Run() {
        if (!TransformComponent::IsAnyDirty())
            return;

        // Update components with a hierarchy
        for (auto [entity, transform, hierarchy] :
             entityRegistry->MakeView<const TransformComponent, const HierarchyComponent>()) {
            if (!hierarchy.HasParent()) { // If this entity does not have a parent, then we should
                                          // update all children recursively
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

        TransformComponent::MarkAnyClean();
    }

    static void Reparent(Entity child, Entity newParent) {
        HierarchyComponent* childHierarchy =
            entityRegistry->GetComponent<HierarchyComponent>(child);
        TransformComponent* transform = entityRegistry->GetComponent<TransformComponent>(child);

        if (childHierarchy->parent == newParent)
            return;

        // Save old world matrix
        glm::mat4 world = transform->worldMatrix;

        // Remove from old parent
        if (childHierarchy->parent != InvalidEntity) {
            HierarchyComponent* oldParentHierarchy =
                entityRegistry->GetComponent<HierarchyComponent>(childHierarchy->parent);

            oldParentHierarchy->RemoveChild(child);
        }

        childHierarchy->parent = newParent;
        // Setup new relationship if the new parent is not invalid
        if (newParent != InvalidEntity &&
            entityRegistry->HasComponent<HierarchyComponent>(newParent)) {
            HierarchyComponent* newParentHierarchy =
                entityRegistry->GetComponent<HierarchyComponent>(newParent);
            newParentHierarchy->AddChild(child);

            // Apply new transform to child component
            TransformComponent* newParentTransform =
                entityRegistry->GetComponent<TransformComponent>(newParent);
            if (newParentTransform) {

                glm::mat4 parentWorld = newParentTransform->worldMatrix;
                glm::mat4 newLocal = glm::inverse(parentWorld) * world;

                DecomposeTransform(newLocal, transform->position, transform->rotation,
                                   transform->scale);
            }
        } else {
            DecomposeTransform(world, transform->position, transform->rotation, transform->scale);
        }

        transform->UpdateTransform();
        MetaComponent::MarkVisibilityDirty();
    }

    static bool IsDescendant(Entity possibleParent, Entity entity) {
        HierarchyComponent* hierarchy =
            entityRegistry->GetComponent<HierarchyComponent>(possibleParent);

        for (Entity child : hierarchy->children) {
            if (child == entity)
                return true;

            if (IsDescendant(child, entity))
                return true;
        }

        return false;
    }

    static void DecomposeTransform(const glm::mat4& mat, glm::vec3& position, glm::quat& rotation,
                                   glm::vec3& scale) {
        glm::vec3 skew;
        glm::vec4 perspective;
        glm::decompose(mat, scale, rotation, position, skew, perspective);
        rotation = glm::normalize(rotation);
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
