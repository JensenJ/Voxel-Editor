#pragma once
#include <Voxel/pch.h>

class UIPanel {
  public:
    void Render() {
        if (!isOpen) {
            PanelIsClosed();
            return;
        }

        int styleCount = LoadStyles();

        if (ImGui::Begin(GetPanelName(), &isOpen)) {
            RenderInternal();
        }
        ImGui::End();
        ImGui::PopStyleVar(styleCount);
    }

    virtual const char* GetPanelName() = 0;
    bool* GetOpen() { return &isOpen; }
    void SetOpen(bool open) { isOpen = open; }
    bool IsHovered() const { return isHovered; }

  private:
    virtual void RenderInternal() = 0;
    virtual void PanelIsClosed() = 0;
    virtual int LoadStyles() = 0;

  protected:
    // This is set within the RenderInternal implementation
    bool isHovered = false;

    bool isOpen = true;
};