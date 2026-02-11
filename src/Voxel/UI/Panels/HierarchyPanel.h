#pragma once
#include <Voxel/pch.h>
#include <Voxel/Core.h>
#include <Voxel/ECS/Components/HierarchyComponent.h>
#include <Voxel/ECS/Components/MetaComponent.h>
#include <Voxel/UI/UIPanel.h>

class HierarchyPanel : public UIPanel {
  public:
    const char* GetPanelName() override { return "Hierarchy"; }

  private:
    void Reparent(EntityRegistry* registry, Entity child, Entity newParent) {
        HierarchyComponent* childHierarchy = registry->GetComponent<HierarchyComponent>(child);
        TransformComponent* transform = registry->GetComponent<TransformComponent>(child);

        // Remove from old parent
        if (childHierarchy->parent != InvalidEntity) {
            HierarchyComponent* oldParentHierarchy =
                registry->GetComponent<HierarchyComponent>(childHierarchy->parent);

            oldParentHierarchy->RemoveChild(child);
        }

        // Setup new relationship
        childHierarchy->parent = newParent;
        HierarchyComponent* newParentHierarchy =
            registry->GetComponent<HierarchyComponent>(newParent);
        newParentHierarchy->AddChild(child);

        transform->MarkDirty();
    }

    bool IsDescendant(EntityRegistry* registry, Entity possibleParent, Entity entity) {
        HierarchyComponent* hierarchy = registry->GetComponent<HierarchyComponent>(possibleParent);

        for (Entity child : hierarchy->children) {
            if (child == entity)
                return true;

            if (IsDescendant(registry, child, entity))
                return true;
        }

        return false;
    }

    void DrawEntityNode(EntityRegistry* registry, Entity entity) {
        MetaComponent* meta = registry->GetComponent<MetaComponent>(entity);
        HierarchyComponent* hierarchy = registry->GetComponent<HierarchyComponent>(entity);

        ImGuiTreeNodeFlags flags =
            ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_SpanAvailWidth;

        if (hierarchy->children.empty())
            flags |= ImGuiTreeNodeFlags_Leaf;

        if (entity == registry->GetSelectedEntity())
            flags |= ImGuiTreeNodeFlags_Selected;

        bool opened =
            ImGui::TreeNodeEx((void*)(uint64_t)(uint32_t)entity, flags, "%s", meta->name.c_str());

        if (ImGui::IsItemClicked()) {
            registry->SelectEntity(entity);
        }

        if (ImGui::BeginDragDropSource()) {
            ImGui::SetDragDropPayload("DND_ENTITY", &entity, sizeof(Entity));
            ImGui::Text("%s", meta->name.c_str());
            ImGui::EndDragDropSource();
        }

        if (ImGui::BeginDragDropTarget()) {
            if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("DND_ENTITY")) {
                Entity dropped = *(Entity*)payload->Data;

                if (dropped != entity && !IsDescendant(registry, dropped, entity)) // prevent cycles
                {
                    Reparent(registry, dropped, entity);
                }
            }
            ImGui::EndDragDropTarget();
        }

        if (opened) {
            for (Entity child : hierarchy->children) {
                DrawEntityNode(registry, child);
            }

            ImGui::TreePop();
        }
    }

    void RenderInternal() override {
        EntityRegistry* registry = EntityRegistry::GetInstance();
        if (registry == nullptr) {
            return;
        }

        for (auto&& [entity, meta, hierarchy] :
             registry->MakeView<const MetaComponent, const HierarchyComponent>()) {

            if (!hierarchy.HasParent()) {
                DrawEntityNode(registry, entity);
            }
        }
    }

    int LoadStyles() override {
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(4, 4));
        ImGui::PushStyleVar(ImGuiStyleVar_CellPadding, ImVec2(4, 2));
        return 2;
    }
};