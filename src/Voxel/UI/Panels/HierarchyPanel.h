#pragma once
#include <Voxel/pch.h>
#include <Voxel/Core.h>
#include <Voxel/ECS/Components/MetaComponent.h>
#include <Voxel/UI/UIPanel.h>

class HierarchyPanel : public UIPanel {
  public:
    const char* GetPanelName() override { return "Hierarchy"; }

  private:
    void RenderInternal() override {
        EntityRegistry* registry = EntityRegistry::GetInstance();
        if (registry == nullptr) {
            return;
        }

        static ImGuiTableFlags flags =
            ImGuiTableFlags_SizingFixedFit | ImGuiTableFlags_RowBg | ImGuiTableFlags_Resizable;

        if (ImGui::BeginTable("HierarchyTable", 2, flags)) {
            ImGui::TableSetupColumn("ID", ImGuiTableColumnFlags_WidthFixed);
            ImGui::TableSetupColumn("Name", ImGuiTableColumnFlags_WidthStretch);
            ImGui::TableHeadersRow();

            for (auto&& [entity, meta] : registry->MakeView<const MetaComponent>()) {
                bool selected = (registry->GetSelectedEntity() == entity);

                ImGui::TableNextRow();
                ImGui::TableNextColumn();
                if (ImGui::Selectable(std::to_string(entity).c_str(), selected,
                                      ImGuiSelectableFlags_SpanAllColumns)) {
                    registry->SelectEntity(entity);
                }

                ImGui::TableNextColumn();
                ImGui::Text("%s", meta.name.c_str());
            }

            ImGui::EndTable();
        }
    }

    int LoadStyles() override {
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(4, 4));
        ImGui::PushStyleVar(ImGuiStyleVar_CellPadding, ImVec2(4, 2));
        return 2;
    }
};