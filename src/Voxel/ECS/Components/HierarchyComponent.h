#pragma once
#include <Voxel/pch.h>
#include <Voxel/Core.h>
#include <Voxel/ECS/Components/MetaComponent.h>

struct HierarchyComponent {
    static constexpr const char* ComponentName = "Hierarchy";
    Entity entity;
    Entity parent = InvalidEntity;
    std::set<Entity> children = std::set<Entity>();

    HierarchyComponent() = default;
    explicit HierarchyComponent(Entity parent) : parent(parent) {
        EntityRegistry* registry = EntityRegistry::GetInstance();
        HierarchyComponent* parentHierarchy = registry->GetComponent<HierarchyComponent>(parent);
        parentHierarchy->AddChild(entity);
    }

    void AddChild(Entity entity) { children.insert(entity); }
    void RemoveChild(Entity entity) { children.erase(entity); }
    bool HasParent() const { return parent != InvalidEntity; }

    void RenderComponentPanel() {
        EntityRegistry* registry = EntityRegistry::GetInstance();
        if (parent != InvalidEntity) {
            MetaComponent* parentMeta = registry->GetComponent<MetaComponent>(parent);
            ImGui::Text("Parent: %s", parentMeta->name.c_str());
        } else {
            ImGui::Text("Parent: None");
        }
        ImGui::Text("Children: %d", children.size());
        ImGui::Indent();
        for (Entity child : children) {
            MetaComponent* childMeta = registry->GetComponent<MetaComponent>(child);
            ImGui::Text("%s", childMeta->name.c_str());
        }
        ImGui::Unindent();
    }
};
