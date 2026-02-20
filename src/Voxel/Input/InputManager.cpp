#include "InputManager.h"
#include <Voxel/pch.h>
#include <Voxel/Core.h>
#include <Voxel/Camera.h>
#include <Voxel/EditorSettings.h>

InputManager* InputManager::instance = nullptr;

InputManager* InputManager::GetInstance() {
    if (instance == nullptr) {
        instance = new InputManager();
        // EditorSettings::OnSettingsAppliedEvent.AddObserver(
        //     [](const SettingsAppliedEvent& event) { instance->LoadBindings(); });
    }
    return instance;
}

void InputManager::BindAction(InputAction action, InputTrigger trigger,
                              std::function<void()> callback) {
    switch (trigger) {
    case InputTrigger::Pressed:
        callbacks[action].pressed.push_back(callback);
        break;
    case InputTrigger::Released:
        callbacks[action].released.push_back(callback);
        break;
    case InputTrigger::Held:
        callbacks[action].held.push_back(callback);
        break;
    }
}

bool InputManager::AddBinding(InputAction action, InputDevice device, int key, int mods) {
    KeyChord chord;
    chord.device = device;
    chord.key = key;
    chord.mods = mods;

    // Prevent conflicts
    if (FindMatchingAction(chord) != InputAction::None) {
        LOG_WARN("Key conflict found, unable to bind key {} with mods {} to action {}.", key, mods,
                 ActionToString(action));
        return false;
    }

    actionBindings[action].push_back(chord);
    SaveBindings();
    return true;
}

bool InputManager::RemoveBinding(InputAction action, InputDevice device, int key, int mods) {
    KeyChord chord;
    chord.device = device;
    chord.key = key;
    chord.mods = mods;

    auto it = actionBindings.find(action);
    if (it == actionBindings.end())
        return false;

    auto& vec = it->second;
    auto found = std::find(vec.begin(), vec.end(), chord);
    if (found != vec.end())
        vec.erase(found);
    else
        return false;

    SaveBindings();
    return true;
}

void InputManager::Update() {
    for (auto& [action, isHeld] : actionStates) {
        if (isHeld)
            TriggerAction(action, InputTrigger::Held);
    }
}

InputAction InputManager::FindMatchingAction(const KeyChord& current) {
    for (const auto& [action, chords] : actionBindings) {
        for (const KeyChord& binding : chords) {
            if (binding.device != current.device)
                continue;

            if (binding.key != current.key)
                continue;

            if ((binding.mods & current.mods) == binding.mods)
                return action;
        }
    }

    return InputAction::None;
}

void InputManager::TriggerAction(InputAction action, InputTrigger trigger) {
    auto it = callbacks.find(action);
    if (it == callbacks.end())
        return;

    auto& actionCallbacks = it->second;

    switch (trigger) {
    case InputTrigger::Pressed:
        for (auto& func : actionCallbacks.pressed)
            func();
        break;

    case InputTrigger::Released:
        for (auto& func : actionCallbacks.released)
            func();
        break;

    case InputTrigger::Held:
        for (auto& func : actionCallbacks.held)
            func();
        break;
    }
}

// Runtime input
void InputManager::OnKey(int key, int glfwAction, int mods) {
    // Ignore repeat events — held is handled in Update()
    if (glfwAction == GLFW_REPEAT)
        return;

    KeyChord chord{};
    chord.device = InputDevice::Keyboard;
    chord.key = key;
    chord.mods = mods;

    InputAction inputAction = FindMatchingAction(chord);
    if (inputAction == InputAction::None)
        return;

    if (glfwAction == GLFW_PRESS) {
        // Prevent duplicate press if already held
        if (!actionStates[inputAction]) {
            actionStates[inputAction] = true;
            TriggerAction(inputAction, InputTrigger::Pressed);
        }
    } else if (glfwAction == GLFW_RELEASE) {
        actionStates[inputAction] = false;
        TriggerAction(inputAction, InputTrigger::Released);
    }
}

void InputManager::OnMouseButton(int button, int glfwAction, int mods) {
    // Ignore repeat events — held is handled in Update()
    if (glfwAction == GLFW_REPEAT)
        return;

    KeyChord chord{};
    chord.device = InputDevice::MouseButton;
    chord.key = button;
    chord.mods = mods;

    InputAction inputAction = FindMatchingAction(chord);
    if (inputAction == InputAction::None)
        return;

    if (glfwAction == GLFW_PRESS) {
        // Prevent duplicate press if already held
        if (!actionStates[inputAction]) {
            actionStates[inputAction] = true;
            TriggerAction(inputAction, InputTrigger::Pressed);
        }
    } else if (glfwAction == GLFW_RELEASE) {
        actionStates[inputAction] = false;
        TriggerAction(inputAction, InputTrigger::Released);
    }
}

void InputManager::OnScroll(int xOffset, int yOffset, int mods) {
    int direction = 0;

    if (yOffset > 0.0)
        direction = SCROLL_UP;
    else if (yOffset < 0.0)
        direction = SCROLL_DOWN;

    if (direction == 0)
        return;

    KeyChord chord{};
    chord.device = InputDevice::MouseScroll;
    chord.key = direction;
    chord.mods = mods;

    InputAction inputAction = FindMatchingAction(chord);
    if (inputAction == InputAction::None)
        return;

    // Discrete mouse scroll is a bit weird, ideally we should only use pressed, but I've put the
    // others triggers also just to prevent confusion when we setup inputs and we accidentally
    // select the wrong trigger, causing it to silently fail
    TriggerAction(inputAction, InputTrigger::Pressed);
    TriggerAction(inputAction, InputTrigger::Released);
    TriggerAction(inputAction, InputTrigger::Held);
}

void InputManager::SaveBindings() {}

void InputManager::LoadBindings() {}

int InputManager::NormaliseMods(int mods) {
    return mods & (GLFW_MOD_SHIFT | GLFW_MOD_CONTROL | GLFW_MOD_ALT | GLFW_MOD_SUPER);
}

void InputManager::RawKeyInput(GLFWwindow* window, int key, int scancode, int action, int mods) {
    InputManager::GetInstance()->OnKey(key, action, NormaliseMods(mods));
}

void InputManager::RawMouseButtonInput(GLFWwindow* window, int button, int action, int mods) {
    InputManager::GetInstance()->OnMouseButton(button, action, NormaliseMods(mods));
}

void InputManager::RawScrollInput(GLFWwindow* window, double xoffset, double yoffset) {
    int mods = 0;

    // Manually collect modifier keys
    if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS ||
        glfwGetKey(window, GLFW_KEY_RIGHT_SHIFT) == GLFW_PRESS)
        mods |= GLFW_MOD_SHIFT;

    if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS ||
        glfwGetKey(window, GLFW_KEY_RIGHT_CONTROL) == GLFW_PRESS)
        mods |= GLFW_MOD_CONTROL;

    if (glfwGetKey(window, GLFW_KEY_LEFT_ALT) == GLFW_PRESS ||
        glfwGetKey(window, GLFW_KEY_RIGHT_ALT) == GLFW_PRESS)
        mods |= GLFW_MOD_ALT;

    if (glfwGetKey(window, GLFW_KEY_LEFT_SUPER) == GLFW_PRESS ||
        glfwGetKey(window, GLFW_KEY_RIGHT_SUPER) == GLFW_PRESS)
        mods |= GLFW_MOD_SUPER;

    InputManager::GetInstance()->OnScroll(xoffset, yoffset, NormaliseMods(mods));
}

// For mouse position, for now we just forward to the camera, as axis based input is kind of
// complicated
void InputManager::RawMouseInput(GLFWwindow* window, double xposIn, double yposIn) {
    static bool firstMouse = true;
    static float lastX = 0.0f, lastY = 0.0f;

    float xpos = static_cast<float>(xposIn);
    float ypos = static_cast<float>(yposIn);

    if (firstMouse) {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos; // reversed since y-coordinates go bottom -> top

    lastX = xpos;
    lastY = ypos;

    // Forward to camera if needed
    Application* app = Application::GetInstance();
    if (app) {
        Camera* cam = app->GetCamera();
        if (cam)
            cam->ProcessMouseMovement(xoffset, yoffset);
    }
}