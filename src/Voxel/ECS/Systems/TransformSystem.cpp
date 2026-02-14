#include "TransformSystem.h"
#include <Voxel/ECS/Components/HierarchyComponent.h>
#include <Voxel/ECS/Components/MetaComponent.h>
#include <Voxel/ECS/Components/TransformComponent.h>
#include <Voxel/ECS/EntityRegistry.h>
#include <Voxel/ECS/Systems/VisibilitySystem.h>

void TransformSystem::Run() {
    ScopedTimer timer(Profiler::system_transform);

    for (Entity entity : dirtyEntities) {
        HierarchyComponent* hierarchy = entityRegistry->GetComponent<HierarchyComponent>(entity);
        TransformComponent* transform = entityRegistry->GetComponent<TransformComponent>(entity);
        if (!hierarchy || !transform)
            continue;

        if (hierarchy->HasParent()) {
            TransformComponent* parentTransform =
                entityRegistry->GetComponent<TransformComponent>(hierarchy->parent);
            UpdateRecursive(entity, parentTransform->worldMatrix);
        } else {
            UpdateRecursive(entity, glm::mat4(1.0f));
        }
    }
    dirtyEntities.clear();
}

void TransformSystem::Reparent(Entity child, Entity newParent) {
    HierarchyComponent* childHierarchy = entityRegistry->GetComponent<HierarchyComponent>(child);
    TransformComponent* transform = entityRegistry->GetComponent<TransformComponent>(child);

    if (childHierarchy->parent == newParent)
        return;

    Entity oldParent = childHierarchy->parent;

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
    if (newParent != InvalidEntity && entityRegistry->HasComponent<HierarchyComponent>(newParent)) {
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

    onEntityChangedParent.Notify({child, oldParent, newParent});

    if (entityRegistry->HasComponent<MetaComponent>(child)) {
        VisibilitySystem::MarkEntityDirty(child);
    }
}

bool TransformSystem::IsDescendant(Entity possibleParent, Entity entity) {
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

void TransformSystem::UpdateRecursive(Entity entity, const glm::mat4& parentWorld) {
    TransformComponent* transform = entityRegistry->GetComponent<TransformComponent>(entity);
    transform->worldMatrix = parentWorld * transform->localMatrix;

    if (!entityRegistry->HasComponent<HierarchyComponent>(entity))
        return;

    HierarchyComponent* hierarchy = entityRegistry->GetComponent<HierarchyComponent>(entity);
    for (Entity child : hierarchy->children) {
        UpdateRecursive(child, transform->worldMatrix);
    }
}