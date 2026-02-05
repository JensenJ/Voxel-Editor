#pragma once
#include <Voxel/pch.h>

class UIPanel {
  public:
    void Render() {
        int styleCount = LoadStyles();
        ImGui::Begin(GetPanelName());
        RenderInternal();
        ImGui::End();
        ImGui::PopStyleVar(styleCount);
    }

    virtual const char* GetPanelName() = 0;
    bool IsHovered() const { return isHovered; }

  private:
    virtual void RenderInternal() = 0;
    virtual int LoadStyles() = 0;

  protected:
    // This is set within the RenderInternal implementation
    bool isHovered = false;
};