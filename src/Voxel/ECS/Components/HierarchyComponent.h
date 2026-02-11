#pragma once
#include <Voxel/pch.h>
#include <Voxel/Core.h>

struct HierarchyComponent {
    Entity entity;
    Entity parent = InvalidEntity;
    std::set<Entity> children = std::set<Entity>();

    HierarchyComponent() = default;
    explicit HierarchyComponent(Entity parent) : parent(parent) {
        EntityRegistry* registry = EntityRegistry::GetInstance();
        HierarchyComponent* parentHierarchy = registry->GetComponent<HierarchyComponent>(parent);
        parentHierarchy->AddChild(entity);
    }

  public:
    void AddChild(Entity entity) { children.insert(entity); }
    void RemoveChild(Entity entity) { children.erase(entity); }
    bool HasParent() const { return parent != InvalidEntity; }
};
