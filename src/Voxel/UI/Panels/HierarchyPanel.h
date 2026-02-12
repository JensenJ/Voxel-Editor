#pragma once

#include <Voxel/pch.h>
#include <Voxel/Core.h>
#include <Voxel/ECS/Components/HierarchyComponent.h>
#include <Voxel/ECS/Components/MetaComponent.h>
#include <Voxel/ECS/Systems/TransformSystem.h>
#include <Voxel/UI/UIPanel.h>

class HierarchyPanel : public UIPanel {
  public:
    const char* GetPanelName() override { return "Hierarchy"; }

  private:
    void DrawEntityNode(EntityRegistry* registry, Entity entity) {
        MetaComponent* meta = registry->GetComponent<MetaComponent>(entity);
        HierarchyComponent* hierarchy = registry->GetComponent<HierarchyComponent>(entity);

        if (!meta || !hierarchy)
            return;

        ImGuiTreeNodeFlags flags =
            ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_SpanAvailWidth;

        if (hierarchy->children.empty())
            flags |= ImGuiTreeNodeFlags_Leaf;

        if (entity == registry->GetSelectedEntity())
            flags |= ImGuiTreeNodeFlags_Selected;

        if (!meta->effectiveVisibility) {
            ImVec4 col = ImGui::GetStyleColorVec4(ImGuiCol_Text);
            col.w *= 0.4f;
            ImGui::PushStyleColor(ImGuiCol_Text, col);
        }

        bool opened =
            ImGui::TreeNodeEx((void*)(uint64_t)(uint32_t)entity, flags, "%s", meta->name.c_str());

        if (!meta->effectiveVisibility)
            ImGui::PopStyleColor();

        if (ImGui::IsItemClicked()) {
            registry->SelectEntity(entity);
        }

        if (ImGui::BeginDragDropSource()) {
            ImGui::SetDragDropPayload("HIERARCHY_ENTITY", &entity, sizeof(Entity));
            ImGui::Text("Reparent %s", meta->name.c_str());
            ImGui::EndDragDropSource();
        }

        if (ImGui::BeginDragDropTarget()) {
            if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("HIERARCHY_ENTITY")) {
                Entity dropped = *(Entity*)payload->Data;

                if (dropped != entity &&
                    !TransformSystem::IsDescendant(dropped, entity)) // prevent cycles
                {
                    TransformSystem::Reparent(dropped, entity);
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
        ScopedTimer timer(Profiler::uiHierarchy);
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

        if (ImGui::BeginDragDropTargetCustom(ImGui::GetCurrentWindow()->Rect(),
                                             ImGui::GetID("HierarchyRootDrop"))) {
            if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("HIERARCHY_ENTITY")) {
                Entity dropped = *(Entity*)payload->Data;
                TransformSystem::Reparent(dropped, InvalidEntity);
            }
            ImGui::EndDragDropTarget();
        }
    }

    int LoadStyles() override {
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(4, 4));
        ImGui::PushStyleVar(ImGuiStyleVar_CellPadding, ImVec2(4, 2));
        return 2;
    }
};