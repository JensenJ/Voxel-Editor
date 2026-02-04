#pragma once

class MainUI {
  public:
    static void Initialise();
    static void RenderUI();
    static bool IsSceneViewportHovered();

  private:
    static void SetupFrame();
    static void SetStyle();
    static void RenderMenuBar();
    static void RenderHierarchyPanel();
    static void RenderObjectPropertiesPanel();
    static void RenderViewport();
    static void RenderLogPanel();

  private:
    static inline bool viewportHovered = false;
};
