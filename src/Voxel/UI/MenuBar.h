#pragma once
#include <Voxel/pch.h>
#include <Voxel/Core.h>
#include <Voxel/UI/Panels/ComponentPanel.h>
#include <Voxel/UI/Panels/HierarchyPanel.h>
#include <Voxel/UI/Panels/LogPanel.h>
#include <Voxel/UI/Panels/ProfilingPanel.h>
#include <Voxel/UI/Panels/ViewportPanel.h>

class MenuBar {
  public:
    static void Render() {
        ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(10, 6));
        if (ImGui::BeginMenuBar()) {
            RenderFileMenu();
            RenderEditMenu();
            RenderWindowMenu();
            ImGui::EndMenuBar();
        }
        ImGui::PopStyleVar();
    }

  private:
    static void RenderFileMenu() {
        if (ImGui::BeginMenu("File")) {
            ImGui::MenuItem("New", "Ctrl+N");
            ImGui::MenuItem("Open", "Ctrl+N");
            ImGui::Separator();
            ImGui::MenuItem("Save", "Ctrl+S");
            ImGui::MenuItem("Save As", "Ctrl+Shift+S");
            ImGui::EndMenu();
        }
    }
    static void RenderEditMenu() {
        if (ImGui::BeginMenu("Edit")) {
            ImGui::MenuItem("Undo", "Ctrl+Z");
            ImGui::MenuItem("Redo", "Ctrl+Shift+Z");
            ImGui::EndMenu();
        }
    }
    static void RenderWindowMenu() {
        if (ImGui::BeginMenu("Window")) {
            ImGui::MenuItem("Viewport", (const char*)0, MainUI::GetViewportPanel()->GetOpen());
            ImGui::MenuItem("Hierarchy", (const char*)0, MainUI::GetHierarchyPanel()->GetOpen());
            ImGui::MenuItem("Component", (const char*)0, MainUI::GetComponentPanel()->GetOpen());
            ImGui::Separator();
            ImGui::MenuItem("Log", (const char*)0, MainUI::GetLogPanel()->GetOpen());
            ImGui::MenuItem("Profiler", (const char*)0, MainUI::GetProfilingPanel()->GetOpen());
            ImGui::EndMenu();
        }
    }
};