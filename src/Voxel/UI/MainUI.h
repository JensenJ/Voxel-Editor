#pragma once

class MainUI {
  public:
    static void RenderUI();

    static bool IsSceneViewportHovered();

  private:
    static inline bool viewportHovered = false;
};
