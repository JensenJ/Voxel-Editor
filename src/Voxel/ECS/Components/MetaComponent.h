#pragma once

#include <Voxel/pch.h>
#include <Voxel/Core.h>
#include <Voxel/ECS/Systems/VisibilitySystem.h>

struct MetaComponent {
  public:
    static constexpr const char* ComponentName = "Information";
    Entity entity;

    std::string name{"Entity"};
    bool visibility{true};
    bool effectiveVisibility{true};

    MetaComponent() = default;
    MetaComponent(Entity entity, const std::string& n, bool vis = true)
        : entity(entity), name(n), visibility(vis) {}

    void RenderComponentPanel() {
        // ImGui needs a mutable char buffer
        char buffer[64];
        std::strncpy(buffer, name.c_str(), sizeof(buffer));
        buffer[sizeof(buffer) - 1] = '\0';

        if (ImGui::BeginTable("MetaProps", 2, ImGuiTableFlags_SizingStretchSame)) {

            ImGui::TableSetupColumn("Label", ImGuiTableColumnFlags_WidthFixed, 100.0f);
            ImGui::TableSetupColumn("Value", ImGuiTableColumnFlags_WidthStretch);

            // Entity number
            ImGui::TableNextRow();
            ImGui::TableSetColumnIndex(0);
            ImGui::TextUnformatted("Entity ID");

            ImGui::TableSetColumnIndex(1);
            ImGui::Text("%d", entity);

            // Entity name
            ImGui::TableNextRow();
            ImGui::TableSetColumnIndex(0);
            ImGui::TextUnformatted("Name");

            ImGui::TableSetColumnIndex(1);
            ImGui::SetNextItemWidth(-1);
            if (ImGui::InputText("##Name", buffer, sizeof(buffer))) {
                name = buffer;
            }

            // Entity visibility
            ImGui::TableNextRow();
            ImGui::TableSetColumnIndex(0);
            ImGui::TextUnformatted("Visible");

            ImGui::TableSetColumnIndex(1);
            if (ImGui::Checkbox("##Visible", &visibility)) {
                VisibilitySystem::onEntityChangedVisibility.Notify({entity, visibility});
            }

            ImGui::EndTable();
        }
    }
};
