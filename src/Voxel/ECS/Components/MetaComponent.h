#pragma once
#include <Voxel/pch.h>
#include <Voxel/Core.h>

struct MetaComponent {
  private:
    static inline bool visibilityDirty = true;

  public:
    static constexpr const char* ComponentName = "Information";
    Entity entity;

    std::string name{"Entity"};
    bool visibility{true};
    bool effectiveVisibility{true};

    MetaComponent() = default;
    MetaComponent(const std::string& n, bool vis = true) : name(n), visibility(vis) {}

    static void MarkVisibilityDirty() { visibilityDirty = true; }

    static void MarkVisibilityClean() { visibilityDirty = false; }

    static bool IsVisibilityDirty() { return visibilityDirty; }

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
            if (ImGui::Checkbox("##Visible", &visibility)) {
                MetaComponent::MarkVisibilityDirty();
            }

            ImGui::EndTable();
        }
    }
};
