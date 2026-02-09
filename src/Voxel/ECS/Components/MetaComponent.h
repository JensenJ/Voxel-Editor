#pragma once
#include <Voxel/pch.h>
#include <Voxel/Core.h>

struct MetaComponent {
    static constexpr const char* ComponentName = "Information";
    Entity entity;

    std::string name{"Entity"};
    bool visible{true};

    MetaComponent() = default;
    MetaComponent(const std::string& n, bool vis = true) : name(n), visible(vis) {}

    void RenderComponentPanel() {
        // ImGui needs a mutable char buffer
        char buffer[64];
        std::strncpy(buffer, name.c_str(), sizeof(buffer));
        buffer[sizeof(buffer) - 1] = '\0';

        if (ImGui::BeginTable("MetaProps", 2, ImGuiTableFlags_SizingStretchSame)) {

            ImGui::TableSetupColumn("Label", ImGuiTableColumnFlags_WidthFixed, 100.0f);
            ImGui::TableSetupColumn("Value", ImGuiTableColumnFlags_WidthStretch);

            ImGui::TableNextRow();
            ImGui::TableSetColumnIndex(0);
            ImGui::TextUnformatted("Name");

            ImGui::TableSetColumnIndex(1);
            ImGui::SetNextItemWidth(-1);
            if (ImGui::InputText("##Name", buffer, sizeof(buffer))) {
                name = buffer;
            }

            ImGui::TableNextRow();
            ImGui::TableSetColumnIndex(0);
            ImGui::TextUnformatted("Visible");

            ImGui::TableSetColumnIndex(1);
            ImGui::Checkbox("##Visible", &visible);

            ImGui::EndTable();
        }
    }
};
