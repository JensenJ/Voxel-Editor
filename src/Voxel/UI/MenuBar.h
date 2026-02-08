#pragma once
#include <Voxel/pch.h>
#include <Voxel/Core.h>

class MenuBar {
  public:
    static void Render() {
        ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(10, 6));
        if (ImGui::BeginMenuBar()) {
            // File menu
            if (ImGui::BeginMenu("File")) {
                ImGui::MenuItem("New", "Ctrl+N");
                ImGui::MenuItem("Open", "Ctrl+N");
                ImGui::Separator();
                ImGui::MenuItem("Save", "Ctrl+S");
                ImGui::MenuItem("Save As", "Ctrl+Shift+S");
                ImGui::EndMenu();
            }

            // Edit menu
            if (ImGui::BeginMenu("Edit")) {
                ImGui::MenuItem("Undo", "Ctrl+Z");
                ImGui::MenuItem("Redo", "Ctrl+Shift+Z");
                ImGui::EndMenu();
            }

            ImGui::EndMenuBar();
        }
        ImGui::PopStyleVar();
    }
};