#pragma once
#include <Voxel/pch.h>

class InputManager {
  public:
    static InputManager* GetInstance();

    // For action events (1 time key presses or releases)
    void BindNewKey(int key, int action, int mods, class std::function<void()> actionToExecute);

    // Callback to link to GLFW key callback function
    void KeyCallback(struct GLFWwindow* window, int key, int scancode, int action, int mods);

    static bool IsKeyDown(struct GLFWwindow* window, int key);
    static bool IsMouseButtonDown(struct GLFWwindow* window, int button);

    void Cleanup();

    static void RawMouseInput(struct GLFWwindow* window, double xpos, double ypos);
    static void RawScrollInput(struct GLFWwindow* window, double xoffset, double yoffset);
    static void RawKeyInput(struct GLFWwindow* window, int key, int scancode, int action, int mods);

  private:
    InputManager() = default;

  private:
    // key id pointing to vector of pairs of int vector (action, mods) and vector of functions
    std::map<int, std::vector<std::pair<std::vector<int>, std::vector<std::function<void()>>>>>
        actionKeys;

    static InputManager* instance;

    inline static float lastMouseX = 0;
    inline static float lastMouseY = 0;
    inline static bool firstMouse = true;
};