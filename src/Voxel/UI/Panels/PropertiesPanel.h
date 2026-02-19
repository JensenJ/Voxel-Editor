#pragma once
#include <Voxel/pch.h>
#include <Voxel/Core.h>
#include <Voxel/EditorSettings.h>
#include <Voxel/UI/UIPanel.h>

class PropertiesPanel : public UIPanel {
  public:
    const char* GetPanelName() override { return "Properties"; }

  private:
    void RenderInternal() override {
        ScopedTimer timer(Profiler::ui_properties);
        const float leftWidth = 180.0f;
        float footerHeight = ImGui::GetFrameHeightWithSpacing();

        // Setting tabs
        ImGui::BeginChild("##tabs", ImVec2(leftWidth, -footerHeight), true);
        for (int i = 0; i < IM_ARRAYSIZE(tabs); i++) {
            if (ImGui::Selectable(tabs[i], selectedTab == i))
                selectedTab = i;
        }
        ImGui::EndChild();

        ImGui::SameLine();

        // Content for each tab
        ImGui::BeginChild("##content", ImVec2(0, -footerHeight), true);
        switch (selectedTab) {
        case 0:
            DrawEditorTab();
            break;
        case 1:
            DrawKeybindingsTab();
            break;
        }
        ImGui::EndChild();

        // Footer
        ImGui::BeginDisabled(!EditorSettings::HasPendingChanges());
        if (ImGui::Button("Apply")) {
            EditorSettings::Save();
        }
        ImGui::EndDisabled();
        ImGui::SameLine();
        ImGui::BeginDisabled(!EditorSettings::HasPendingChanges());
        if (ImGui::Button("Revert")) {
            EditorSettings::ClearPendingChanges();
        }
        ImGui::EndDisabled();
    }

    void DrawEditorTab() {
        ImGui::Text("Editor Settings");
        ImGui::Separator();

        const float scales[] = {0.5f, 0.75f, 1.0f, 1.25f, 1.5f, 1.75f, 2.0f, 2.25, 2.5, 2.75, 3.0};
        const char* labels[] = {"50%",  "75%",  "100%", "125%", "150%", "175%",
                                "200%", "225%", "250%", "275%", "300%"};

        int currentIndex = 1;
        float currentScale = EditorSettings::GetPendingFloat("Editor", "UIScale", 1.0f);
        for (int i = 0; i < IM_ARRAYSIZE(scales); i++) {
            if (currentScale == scales[i]) {
                currentIndex = i;
                break;
            }
        }

        if (ImGui::Combo("UI Scale", &currentIndex, labels, IM_ARRAYSIZE(labels))) {
            float newScale = scales[currentIndex];
            EditorSettings::SetFloat("Editor", "UIScale", newScale);
        }
    }

    void DrawKeybindingsTab() { ImGui::Text("Keybindings"); }

    int LoadStyles() override {
        ImGui::SetNextWindowSizeConstraints(ImVec2(800, 600), ImVec2(FLT_MAX, FLT_MAX));
        return 0;
    }

    static inline const char* tabs[] = {"Editor", "Keybindings"};
    static inline int selectedTab = 0;
};