#pragma once
#include <Voxel/pch.h>
#include <Voxel/Core.h>
#include <Voxel/Rendering/FrameBuffer.h>
#include <Voxel/UI/UIPanel.h>

class ViewportPanel : public UIPanel {
  public:
    const char* GetPanelName() override { return "Viewport"; }

  private:
    void RenderInternal() override {
        ScopedTimer timer(Profiler::uiViewport);
        Application* application = Application::GetInstance();
        if (application == nullptr) {
            return;
        }
        ImGui::BeginChild("View");
        ImVec2 size = ImGui::GetContentRegionAvail();
        application->SetSceneViewportWidth((int)size.x);
        application->SetSceneViewportHeight((int)size.y);
        isHovered = ImGui::IsWindowHovered(ImGuiHoveredFlags_RootAndChildWindows);
        ImGui::Image(ImTextureID(application->GetSceneBuffer()->GetFrameTexture()), size,
                     ImVec2(0, 1), ImVec2(1, 0));
        ImGui::EndChild();
    }

    int LoadStyles() override {
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
        return 1;
    }
};