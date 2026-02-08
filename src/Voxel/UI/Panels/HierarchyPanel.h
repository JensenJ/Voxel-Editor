#pragma once
#include <Voxel/pch.h>
#include <Voxel/Core.h>
#include <Voxel/UI/UIPanel.h>

class HierarchyPanel : public UIPanel {
  public:
    const char* GetPanelName() override { return "Hierarchy"; }

  private:
    void RenderInternal() override {
        // EntityRegistry* entityRegistry = EntityRegistry::GetInstance();
        // if (entityRegistry == nullptr) {
        //     return;
        // }

        //// Get all entities and list them
        // std::map<unsigned int, Entity*> entities = entityRegistry->GetAllEntities();

        // static ImGuiTableFlags entityHierarchyFlags =
        //     ImGuiTableFlags_SizingFixedFit | ImGuiTableFlags_RowBg | ImGuiTableFlags_Resizable |
        //     ImGuiTableFlags_Reorderable | ImGuiTableFlags_Hideable;

        // if (ImGui::BeginTable("Hierarchy Entity Table", 2, entityHierarchyFlags)) {
        //     // Print table headers
        //     ImGui::TableSetupColumn("Entity ID", ImGuiTableColumnFlags_WidthFixed);
        //     ImGui::TableSetupColumn("Entity Name", ImGuiTableColumnFlags_WidthStretch);
        //     ImGui::TableHeadersRow();

        //    // Print Entities in panel and make them selectable
        //    for (std::map<unsigned int, Entity*>::iterator iter = entities.begin();
        //         iter != entities.end(); ++iter) {
        //        bool selected = false;
        //        bool found = false;

        //        // If this entity is part of selected entities already, then set selected to true
        //        auto selectedEntity = entityRegistry->selectedEntities.find(iter->second);

        //        if (selectedEntity != entityRegistry->selectedEntities.end()) {
        //            selected = true;
        //            found = true;
        //        }

        //        ImGui::TableNextRow();
        //        ImGui::TableNextColumn();
        //        if (ImGui::Selectable(std::to_string(iter->second->GetEntityID()).c_str(),
        //                              &selected, ImGuiSelectableFlags_SpanAllColumns)) {

        //            if (!ImGui::GetIO().KeyCtrl) // If ctrl is not held
        //            {
        //                // Clear the existing entities
        //                entityRegistry->selectedEntities.clear();

        //                // Check if its false as clicking on a selected item would deselect it
        //                if (selected == false) {
        //                    if (found == true) // If this entity was previously selected
        //                    {
        //                        // Add it back to selected entities
        //                        entityRegistry->selectedEntities.insert(iter->second);
        //                    }
        //                }
        //                // If this entity is a different selection from on the list
        //                else {
        //                    if (found == true) // If this entity was found in selected entities
        //                    {
        //                        entityRegistry->selectedEntities.erase(iter->second);
        //                    } else // If this entity was not found in selected entities
        //                    {
        //                        entityRegistry->selectedEntities.insert(iter->second);
        //                    }
        //                }
        //            } else // If CTRL was held
        //            {
        //                // If this entity is selected
        //                if (selected == true) {
        //                    if (found == false) // If entity is not on the list already
        //                    {
        //                        entityRegistry->selectedEntities.insert(iter->second);
        //                    }
        //                } else {
        //                    if (found == true) // If entity is on the list already
        //                    {
        //                        entityRegistry->selectedEntities.erase(iter->second);
        //                    }
        //                }
        //            }
        //        }
        //        ImGui::TableNextColumn();
        //        ImGui::Text("%s", iter->second->GetEntityName().c_str());
        //    }
        //    ImGui::EndTable();
        //}
    }

    int LoadStyles() override {
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(4, 4));
        ImGui::PushStyleVar(ImGuiStyleVar_CellPadding, ImVec2(4, 2));
        return 2;
    }
};