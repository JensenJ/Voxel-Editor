#pragma once
#include <Voxel/pch.h>
#include <Voxel/Core.h>
#include <Voxel/Input/InputActions.h>

enum class InputDevice { Keyboard = 0, MouseButton = 1, MouseScroll = 2 };
enum ScrollCode { SCROLL_UP = 1, SCROLL_DOWN = -1 };
enum class InputTrigger { Pressed, Released, Held };

struct KeyChord {
    InputDevice device;
    int key;  // GLFW_KEY_* or GLFW_MOUSE_BUTTON_* or ScrollCode
    int mods; // GLFW_MOD_* bitmask

    bool operator==(const KeyChord& other) const {
        return device == other.device && key == other.key && mods == other.mods;
    }
};

struct ActionCallbacks {
    std::vector<std::function<void()>> pressed;
    std::vector<std::function<void()>> released;
    std::vector<std::function<void()>> held;
};

class InputManager {
  public:
    static class InputManager* GetInstance();

    void BindAction(InputAction action, InputTrigger trigger, std::function<void()> callback);

    bool AddBinding(InputAction action, InputDevice device, int key, int mods);
    bool RemoveBinding(InputAction action, InputDevice device, int key, int mods);

    void Update();

    void OnKey(int key, int action, int mods);
    void OnMouseButton(int button, int action, int mods);
    void OnScroll(int xOffset, int yOffset, int mods);

    void SaveBindings();
    void LoadBindings();

    // GLFW
    static void RawKeyInput(GLFWwindow* window, int key, int scancode, int action, int mods);
    static void RawMouseInput(GLFWwindow* window, double xpos, double ypos);
    static void RawScrollInput(GLFWwindow* window, double xoffset, double yoffset);
    static void RawMouseButtonInput(GLFWwindow* window, int button, int action, int mods);
    static int NormaliseMods(int mods);

  private:
    void TriggerAction(InputAction action, InputTrigger trigger);
    InputAction FindMatchingAction(const KeyChord& current);

  private:
    static InputManager* instance;

    std::unordered_map<InputAction, std::vector<KeyChord>> actionBindings;
    std::unordered_map<InputAction, ActionCallbacks> callbacks;
    std::unordered_map<InputAction, bool> actionStates; // For glfw
};
