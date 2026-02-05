#pragma once
#include <Voxel/UI/UIPanel.h>

class MainUI {
  public:
    static void Initialise();
    static void RenderUI();

    static class MenuBar* GetMenuBar();
    static class ViewportPanel* GetViewportPanel();
    static class LogPanel* GetLogPanel();
    static class HierarchyPanel* GetHierarchyPanel();
    static class ComponentPanel* GetComponentPanel();

  private:
    static void SetupFrame();
    static bool ShouldBuildDefaultDockLayout();
    static void BuildDefaultDockLayout(ImGuiID dockspaceID);
    static void RegisterPanels();

  private:
    static inline class MenuBar* menuBar = nullptr;
    static inline class ViewportPanel* viewportPanel = nullptr;
    static inline class LogPanel* logPanel = nullptr;
    static inline class HierarchyPanel* hierarchyPanel = nullptr;
    static inline class ComponentPanel* componentPanel = nullptr;

    static inline bool dockLayoutBuilt = false;
    static inline std::vector<std::unique_ptr<UIPanel>> panels =
        std::vector<std::unique_ptr<UIPanel>>();
};
